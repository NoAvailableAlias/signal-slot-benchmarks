/*===================================================================
*   Copyright (c) Vadim Karkhin. All rights reserved.
*   Use, modification, and distribution is subject to license terms.
*   You are welcome to contact the author at: vdksoft@gmail.com
===================================================================*/

#include "../vdksoft/signals/src/signals.h"

#include <atomic>
#include <thread>
#include <cstdint>
#include <climits>

namespace vdk
{
namespace memory::signals
{
// Default memory resource
// Used if no alternative memory resource is provided
class default_memory_resource : public memory_resource
{
public:
    void * allocate(size_t size, size_t align) override;
    void deallocate(void * addr, size_t size, size_t align) noexcept override;
    ~default_memory_resource() override = default;
};

void * default_memory_resource::
allocate(size_t size, size_t align)
{
    return ::operator new(size, std::align_val_t{ align });
}

void default_memory_resource::
deallocate(void * addr, size_t size, size_t align) noexcept
{
    ::operator delete(addr, size, std::align_val_t{ align });
}

memory_resource * get_default_resource() noexcept
{
    static default_memory_resource resource;
    return &resource;
}

memory_resource * memory(memory_resource * r) noexcept
{
    static memory_resource * const resource =
        r ? r : get_default_resource();
    return resource;
}

} // namespace memory::signals

// Internal implementation details
namespace internal::signals
{
memory_owner::memory_owner(size_t size, size_t align)
    : addr_{ memory()->allocate(size, align) },
      size_{ size },
      align_{ align }
{}

memory_owner::~memory_owner() noexcept
{
    if (addr_) memory()->deallocate(addr_, size_, align_);
}

void * memory_owner::get() const noexcept
{
    return addr_;
}

void memory_owner::release() noexcept
{
    addr_ = nullptr;
}

// Simple synchronization primitive
class slim_lock
{
public:

    slim_lock() noexcept = default;
    ~slim_lock() noexcept = default;

    void lock() noexcept;
    void unlock() noexcept;
    bool try_lock() noexcept;

    slim_lock(const slim_lock &) = delete;
    slim_lock(slim_lock &&) = delete;
    slim_lock & operator=(const slim_lock &) = delete;
    slim_lock & operator=(slim_lock &&) = delete;

private:

    std::atomic_flag lock_ = ATOMIC_FLAG_INIT;
};

void slim_lock::lock() noexcept
{
    while (lock_.test_and_set(std::memory_order_acquire))
        std::this_thread::yield();
}

void slim_lock::unlock() noexcept
{
    lock_.clear(std::memory_order_release);
}

bool slim_lock::try_lock() noexcept
{
    return lock_.test_and_set(std::memory_order_acquire) ? false : true;
}

// Simple slim_lock ownership wrapper
class auto_lock
{
public:

    explicit auto_lock(slim_lock & lock) noexcept;
    ~auto_lock() noexcept;

    auto_lock(const auto_lock &) = delete;
    auto_lock(auto_lock &&) = delete;
    auto_lock & operator=(const auto_lock &) = delete;
    auto_lock & operator=(auto_lock &&) = delete;

private:

    slim_lock & lock_;
};

auto_lock::auto_lock(slim_lock & lock) noexcept
    : lock_{ lock }
{
    lock_.lock();
}

auto_lock::~auto_lock() noexcept
{
    lock_.unlock();
}

// Provides communication channel between threads
// Contains command objects received in the associated thread
class channel
{
public:

    channel() noexcept = default;
    ~channel() noexcept = default;

    // Send command to the thread channel
    // Takes ownership of the given command object
    void send(command * cmnd) noexcept;

    // Extract command received in the thread channel
    // Returns command, if any; otherwise returns null pointer
    // Caller obtains ownership of the returned command object
    // !NOTE! Must be called from target thread only
    command * get() noexcept;

    // Close the channel preventing it from receiving new commands
    // !NOTE! Must be called from target thread only
    void close() noexcept;

    void incr_refs() noexcept;
    void decr_refs() noexcept;

    channel(const channel &) = delete;
    channel(channel &&) = delete;
    channel & operator=(const channel &) = delete;
    channel & operator=(channel &&) = delete;

private:

    // Refresh the channel's state
    // Moves and rearranges received commands from stack to list
    void refresh() noexcept;

    // Get 'closed' pointer
    // Returns pointer that denotes closed channel
    command * closed() noexcept;

    std::atomic<command*> stack_{};
    command *             list_{};
    std::atomic_size_t    refs_{ 1 };
};

inline command * channel::closed() noexcept
{
    // This pointer is never dereferenced and used as a unique marker
    // to indicate that the channel has been closed
    return reinterpret_cast<command*>(this);
}

void channel::incr_refs() noexcept
{
    ++refs_;
}

void channel::decr_refs() noexcept
{
    if (!--refs_) memory_delete(this);
}

void channel::refresh() noexcept
{
    if (list_) return;

    auto curr = stack_.exchange(nullptr);
    command * prev = nullptr;
    command * next = nullptr;

    while (curr)
    {
        next = curr->next_;
        curr->next_ = prev;
        prev = curr;
        curr = next;
    }

    list_ = prev;
}

void channel::send(command * cmnd) noexcept
{
    auto curr = stack_.load();
    auto exit = closed();

    while (curr != exit)
    {
        cmnd->next_ = curr;
        if (stack_.compare_exchange_weak(curr, cmnd)) return;
    }

    memory_delete(cmnd);
}

command * channel::get() noexcept
{
    refresh();
    if (!list_) return nullptr;

    auto tmp = list_;
    list_ = list_->next_;
    return tmp;
}

void channel::close() noexcept
{
    auto stack = stack_.exchange(closed());

    while (stack)
    {
        auto tmp = stack;
        stack = stack->next_;
        memory_delete(tmp);
    }

    while (list_)
    {
        auto tmp = list_;
        list_ = list_->next_;
        memory_delete(tmp);
    }

    decr_refs();
}

// Obtain channel of the calling thread
// Allocates thread's channel on the first call (per thread)
// Throws an exception if memory allocation fails
inline channel * this_thread_channel()
{
    struct channel_owner
    {
        channel_owner()
            : chnl{ memory_new<channel>() }
        {}
        ~channel_owner() noexcept
        {
            chnl->close();
        }
        channel * const chnl;
    };

    thread_local const channel_owner thread;
    return thread.chnl;
}

// Control block for context
class ctx_ctrl
{
public:

    // Construct control block and associate it with the calling thread
    // If this is the first context object constructed in the thread,
    // also allocates memory and constructs the thread's channel
    // Throws an exception if memory allocation fails
    ctx_ctrl();

    // Destruct the control block
    // Releases ownership of the associated thread's channel
    ~ctx_ctrl() noexcept;

    // Check whether the context object is still active
    bool active() const noexcept;

    // Obtain channel of the thread associated with the context object
    // Returns null pointer if the context object is no longer active
    channel * thread_channel() const noexcept;

    // Release ownership of the control block
    void free() noexcept;

    void incr_refs() noexcept;
    void decr_refs() noexcept;

    ctx_ctrl(const ctx_ctrl &) = delete;
    ctx_ctrl(ctx_ctrl &&) = delete;
    ctx_ctrl & operator=(const ctx_ctrl &) = delete;
    ctx_ctrl & operator=(ctx_ctrl &&) = delete;

private:

    // Bit-flag for reference counter
    using uint = size_t;
    static constexpr auto total = sizeof(uint) * CHAR_BIT;
    static constexpr uint active_flag = uint(1) << (total - 1);

    channel * const     chnl_;
    std::atomic_size_t  refs_;
};

ctx_ctrl::ctx_ctrl()
    : chnl_{ this_thread_channel() },
      refs_{ active_flag }
{
    chnl_->incr_refs();
}

ctx_ctrl::~ctx_ctrl() noexcept
{
    chnl_->decr_refs();
}

inline bool ctx_ctrl::active() const noexcept
{
    return refs_.load() & active_flag;
}

inline channel * ctx_ctrl::thread_channel() const noexcept
{
    if (active())
        return chnl_;
    else
        return nullptr;
}

void ctx_ctrl::incr_refs() noexcept
{
    ++refs_;
}

void ctx_ctrl::decr_refs() noexcept
{
    if (!--refs_) memory_delete(this);
}

void ctx_ctrl::free() noexcept
{
    if (!(refs_ &= ~active_flag)) memory_delete(this);
}

// Structure that forms internal list of connections to slots
struct connection
{
    using ushort = unsigned short;
    static constexpr ushort active = 0;
    static constexpr ushort rem_v1 = 1;
    static constexpr ushort rem_v2 = 2;

    connection(target * slot,
               ctx_ctrl * cntx,
               unsigned int id,
               exec type) noexcept;

    ~connection() noexcept;

    void incr_refs() noexcept;
    void decr_refs() noexcept;

    connection(const connection &) = delete;
    connection(connection &&) = delete;
    connection & operator=(const connection &) = delete;
    connection & operator=(connection &&) = delete;

    target * const              slot_;
    ctx_ctrl * const            cntx_;
    std::atomic<connection*>    next_;
    std::atomic<connection*>*   prev_;
    connection *                rmvd_;
    std::atomic_size_t          refs_;
    const unsigned int          id_;
    const exec                  type_;
    std::atomic_ushort          state_;
};

connection::connection(target * slot,
                       ctx_ctrl * cntx,
                       unsigned int id,
                       exec type) noexcept
    : slot_{ slot },
      cntx_{ cntx },
      next_{ nullptr },
      prev_{ nullptr },
      rmvd_{ nullptr },
      refs_{ 1 },
      id_{ id },
      type_{ type },
      state_{ active }
{
    if (cntx_) cntx_->incr_refs();
}

connection::~connection() noexcept
{
    slot_->uninstall();
    if (cntx_) cntx_->decr_refs();
}

inline void connection::incr_refs() noexcept
{
    ++refs_;
}

inline void connection::decr_refs() noexcept
{
    if (!--refs_) memory_delete(this);
}

// Control block for signal
class sig_ctrl
{
public:

    sig_ctrl() noexcept;
    ~sig_ctrl() noexcept;

    // Connect the signal to the given slot
    // Returns connection id (never 0)
    // Throws an exception if memory allocation fails
    friend unsigned int signal_connect(sig_ctrl * self,
                                       target * slot,
                                       ctx_ctrl * cntx,
                                       exec type);

    // Disconnect the signal from the given slot
    // If the slot is not equality comparable, disconnects nothing
    // Returns true on successful disconnection, false otherwise
    friend bool signal_disconnect(sig_ctrl * self,
                                  target * slot) noexcept;

    // Disconnect connection identified by the given id
    // Returns true on successful disconnection, false otherwise
    friend bool signal_disconnect(sig_ctrl * self,
                                  unsigned int id) noexcept;

    // Disconnect all slots from the signal
    friend void signal_disconnect(sig_ctrl * self) noexcept;

    // Activate the signal with the given arguments
    friend void signal_activate(sig_ctrl * self, void * args);

    // Block the signal
    // Returns true if the signal was blocked prior to the call;
    // otherwise returns false
    friend bool signal_block(sig_ctrl * self, bool yes) noexcept;

    // Check whether the signal is blocked
    // Returns true if the signal is blocked, false otherwise
    friend bool signal_blocked(const sig_ctrl * self) noexcept;

    // Release ownership of the control block
    void free() noexcept;

    sig_ctrl(const sig_ctrl &) = delete;
    sig_ctrl(sig_ctrl &&) = delete;
    sig_ctrl & operator=(const sig_ctrl &) = delete;
    sig_ctrl & operator=(sig_ctrl &&) = delete;

private:

    // Bit-masks and flags for execution counters
    using ulong = std::uint_fast32_t;
    static constexpr auto total = sizeof(ulong) * CHAR_BIT;
    static constexpr auto shift = (total - 2) / 2;
    static constexpr ulong active_flag = ulong(1) << (total - 1);
    static constexpr ulong exe_v1_mask = ulong(-1) >> (total - shift);
    static constexpr ulong exe_v2_mask = exe_v1_mask << shift;
    static constexpr ulong execs_masks = exe_v1_mask | exe_v2_mask;
    static constexpr ulong exe_v1_unit = 1;
    static constexpr ulong exe_v2_unit = ulong(1) << shift;

    // Bit-flags for signal's state
    using uint = unsigned int;
    static constexpr uint cntr_version = 0b0001;
    static constexpr uint cleanup_flag = 0b0010;
    static constexpr uint blocked_flag = 0b0100;

    void incr_execs(ulong unit) noexcept;
    void decr_execs(ulong unit) noexcept;

    // Obtain id for a new connection
    // This method never returns 0
    // !NOTE! Must be called under mutex protection
    unsigned int new_id() noexcept;

    // Remove given connection logically or delete physically
    // Accepts currently incremented execution counter, if any
    // !NOTE! Must be called under mutex protection
    void remove(connection * node, ulong unit = 0) noexcept;
    // Remove all connections logically or delete physically
    // !NOTE! Must be called under mutex protection
    void remove() noexcept;

    // Try to delete all logically removed connections physically
    // Accepts currently incremented execution counter, if any
    // !NOTE! Must be called under mutex protection
    void cleanup(ulong unit = 0) noexcept;

    // Helper method: Physically delete specified removed connections
    // This method considers only connections associated with the
    // given execution counter version
    // !NOTE! Must be called under mutex protection
    void erase(uint ver) noexcept;
    // Helper method: Physically delete all removed connections
    // This method considers all removed connections regardless of
    // their associated execution counter versions
    // !NOTE! Must be called under mutex protection
    void erase() noexcept;

    std::atomic<connection*>    list_;
    connection *                rmvd_;
    std::atomic_uint_fast32_t   execs_;
    unsigned int                ids_;
    std::atomic_uint            state_;
    slim_lock                   mutex_;
};

sig_ctrl::sig_ctrl() noexcept
    : list_{ nullptr },
      rmvd_{ nullptr },
      execs_{ active_flag },
      ids_{ 0 },
      state_{ 0 },
      mutex_{}
{}

sig_ctrl::~sig_ctrl() noexcept
{
    erase();

    auto curr = list_.load();

    while (curr)
    {
        auto tmp = curr;
        curr = curr->next_.load();
        tmp->decr_refs();
    }
}

void sig_ctrl::free() noexcept
{
    signal_disconnect(this);
    if (!(execs_ &= ~active_flag)) memory_delete(this);
}

inline void sig_ctrl::incr_execs(ulong unit) noexcept
{
    execs_ += unit;
}

inline void sig_ctrl::decr_execs(ulong unit) noexcept
{
    if (!(execs_ -= unit)) memory_delete(this);
}

unsigned int sig_ctrl::new_id() noexcept
{
    auto id = ++ids_;
    return id != 0 ? id : ++ids_;
}

void sig_ctrl::remove(connection * node, ulong unit) noexcept
{
    if (node->state_.load() != connection::active) return;

    auto next = node->next_.load();
    node->prev_->store(next);
    if (next) next->prev_ = node->prev_;

    if (!((execs_.load() & execs_masks) - unit))
    {
        if (rmvd_) erase();
        node->decr_refs();
    }
    else
    {
        if (state_.load() & cntr_version)
            node->state_.store(connection::rem_v1);
        else
            node->state_.store(connection::rem_v2);

        auto curr = rmvd_;
        auto prev = &rmvd_;

        while (curr)
        {
            if (curr->next_.load() == node)
                curr->next_.store(next);
            prev = &curr->rmvd_;
            curr = curr->rmvd_;
        }

        (*prev) = node;
        state_ |= cleanup_flag;
    }
}

void sig_ctrl::remove() noexcept
{
    auto curr = list_.exchange(nullptr);

    if (!(execs_.load() & execs_masks))
    {
        if (rmvd_) erase();

        while (curr)
        {
            auto tmp = curr;
            curr = curr->next_.load();
            tmp->decr_refs();
        }
    }
    else
    {
        auto list = curr;
        auto prev = &rmvd_;

        auto ver = state_.load() & cntr_version ?
            connection::rem_v1 : connection::rem_v2;

        while (curr)
        {
            curr->rmvd_ = curr->next_.exchange(nullptr);
            curr->state_.store(ver);
            curr = curr->rmvd_;
        }

        curr = rmvd_;

        while (curr)
        {
            curr->next_.store(nullptr);
            prev = &curr->rmvd_;
            curr = curr->rmvd_;
        }

        (*prev) = list;
        state_ |= cleanup_flag;
    }
}

void sig_ctrl::cleanup(ulong unit) noexcept
{
    auto execs = (execs_.load() & execs_masks) - unit;

    if (!execs)
    {
        erase();
        return;
    }

    if (state_.load() & cntr_version)
    {
        if (!(execs & exe_v2_mask))
        {
            erase(connection::rem_v2);
            state_ &= ~cntr_version;
        }
        return;
    }
    else
    {
        if (!(execs & exe_v1_mask))
        {
            erase(connection::rem_v1);
            state_ |= cntr_version;
        }
        return;
    }
}

void sig_ctrl::erase(uint ver) noexcept
{
    auto curr = rmvd_;
    auto prev = &rmvd_;

    while (curr)
    {
        if (curr->state_.load() == ver)
        {
            auto tmp = curr;
            (*prev) = curr->rmvd_;
            curr = curr->rmvd_;
            tmp->decr_refs();
            continue;
        }
        prev = &curr->rmvd_;
        curr = curr->rmvd_;
    }

    if (!rmvd_) state_ &= ~cleanup_flag;
}

void sig_ctrl::erase() noexcept
{
    auto curr = rmvd_;

    while (curr)
    {
        auto tmp = curr;
        curr = curr->rmvd_;
        tmp->decr_refs();
    }

    rmvd_ = nullptr;
    state_ &= ~cleanup_flag;
}

unsigned int signal_connect(sig_ctrl * self,
                            target * slot,
                            ctx_ctrl * cntx,
                            exec type)
{
    auto_lock lock{ self->mutex_ };

    auto curr = self->list_.load();
    auto prev = &self->list_;

    while (curr)
    {
        if (curr->cntx_ && !curr->cntx_->active())
        {
            auto tmp = curr;
            curr = curr->next_.load();
            self->remove(tmp);
            continue;
        }
        prev = &curr->next_;
        curr = curr->next_.load();
    }

    struct target_guard
    {
        target_guard(target * t) noexcept
            : ptr{ t }
        {}
        ~target_guard() noexcept
        {
            if (ptr) ptr->uninstall();
        }
        target * ptr;
    };

    target_guard trgt{ slot->install() };

    auto conn = memory_new<connection>(trgt.ptr,
                                       cntx,
                                       self->new_id(),
                                       type);

    prev->store(conn);
    conn->prev_ = prev;
    trgt.ptr = nullptr;

    if (self->rmvd_) self->cleanup();

    return conn->id_;
}

bool signal_disconnect(sig_ctrl * self, target * slot) noexcept
{
    auto_lock guard{ self->mutex_ };

    auto curr = self->list_.load();
    auto done = false;

    while (curr)
    {
        if (curr->cntx_ && !curr->cntx_->active())
        {
            auto tmp = curr;
            curr = curr->next_.load();
            self->remove(tmp);
            continue;
        }
        if (!done && curr->slot_->compare(slot))
        {
            auto tmp = curr;
            curr = curr->next_.load();
            self->remove(tmp);
            done = true;
            continue;
        }
        curr = curr->next_.load();
    }

    if (self->rmvd_) self->cleanup();
    return done;
}

bool signal_disconnect(sig_ctrl * self, unsigned int id) noexcept
{
    auto_lock guard{ self->mutex_ };

    auto curr = self->list_.load();
    auto done = false;

    while (curr)
    {
        if (curr->cntx_ && !curr->cntx_->active())
        {
            auto tmp = curr;
            curr = curr->next_.load();
            self->remove(tmp);
            continue;
        }
        if (curr->id_ == id)
        {
            auto tmp = curr;
            curr = curr->next_.load();
            self->remove(tmp);
            done = true;
            continue;
        }
        curr = curr->next_.load();
    }

    if (self->rmvd_) self->cleanup();
    return done;
}

void signal_disconnect(sig_ctrl * self) noexcept
{
    auto_lock lock{ self->mutex_ };
    self->remove();
}

void signal_activate(sig_ctrl * self, void * args)
{
    struct exe_guard
    {
        exe_guard(sig_ctrl * c, sig_ctrl::ulong u) noexcept
            : ctrl{ c }, unit{ u }
        {
            ctrl->incr_execs(unit);
        }
        ~exe_guard() noexcept
        {
            ctrl->decr_execs(unit);
        }
        sig_ctrl * const      ctrl;
        const sig_ctrl::ulong unit;
    };

    auto const state = self->state_.load();
    if (state & sig_ctrl::blocked_flag) return;

    auto const this_chnl = this_thread_channel();

    const exe_guard guard{ self, state &
        sig_ctrl::cntr_version ?
        sig_ctrl::exe_v1_unit :
        sig_ctrl::exe_v2_unit };

    auto curr = self->list_.load();

    while (curr)
    {
        if (curr->cntx_)
        {
            if (auto const chnl = curr->cntx_->thread_channel())
            {
                switch (curr->type_)
                {
                case exec::sync:
                {
                    curr->slot_->invoke(args);
                    break;
                }
                case exec::async:
                {
                    auto cmnd = command::create(curr, args);
                    chnl->send(cmnd);
                    break;
                }
                default:
                {
                    if (this_chnl == chnl)
                    {
                        curr->slot_->invoke(args);
                    }
                    else
                    {
                        auto cmnd = command::create(curr, args);
                        chnl->send(cmnd);
                    }
                }
                }
            }
            else
            {
                if (self->mutex_.try_lock())
                {
                    auto tmp = curr;
                    curr = curr->next_.load();
                    self->remove(tmp, guard.unit);
                    self->mutex_.unlock();
                    continue;
                }
            }
        }
        else
        {
            curr->slot_->invoke(args);
        }
        curr = curr->next_.load();
    }

    if (self->state_.load() & sig_ctrl::cleanup_flag &&
        self->mutex_.try_lock())
    {
        self->cleanup(guard.unit);
        self->mutex_.unlock();
    }
}

bool signal_block(sig_ctrl * self, bool yes) noexcept
{
    if (yes)
        return self->state_.fetch_or(sig_ctrl::blocked_flag) ==
        sig_ctrl::blocked_flag;
    else
        return self->state_.fetch_and(~sig_ctrl::blocked_flag) ==
        sig_ctrl::blocked_flag;
}

bool signal_blocked(const sig_ctrl * self) noexcept
{
    return self->state_.load() & sig_ctrl::blocked_flag;
}

sig_ctrl* signal_construct()
{
    return memory_new<sig_ctrl>();
}

void signal_destruct(sig_ctrl * self) noexcept
{
    self->free();
}

command::command(connection * conn,
                 slot_invoker call,
                 self_deleter dtor) noexcept
    : next_{ nullptr }, conn_{ conn }, call_{ call }, dtor_{ dtor }
{
    conn_->incr_refs();
}

command::~command() noexcept
{
    conn_->decr_refs();
}

void command::destroy() noexcept
{
    dtor_(this);
}

void command::invoke()
{
    if (conn_->state_.load() == connection::active)
    {
        if (conn_->cntx_->active())
        {
            call_(this, conn_->slot_);
        }
    }
}

command * command::create(connection * conn, void * args)
{
    return conn->slot_->ctor_(conn, args);
}

target::target(slot_invoker call,
               cmnd_creator ctor,
               const vtbl * vptr) noexcept
    : call_{ call }, ctor_{ ctor }, vptr_{ vptr }
{}

void target::invoke(void * args)
{
    call_(this, args);
}

bool target::compare(const target * other) const noexcept
{
    if (vptr_ != other->vptr_)
        return false;
    return vptr_->compare(this, other);
}

target * target::install()
{
    return vptr_->install(this);
}

void target::uninstall() noexcept
{
    vptr_->uninstall(this);
}

// Helper class for safe execution of command objects
struct command_owner
{
    command_owner(command * c) noexcept
        : cmnd{ c }
    {}
    ~command_owner() noexcept
    {
        cmnd->destroy();
    }

    command_owner(const command_owner &) = delete;
    command_owner & operator=(const command_owner &) = delete;

    command * const cmnd;
};

} // namespace internal::signals

bool signals_execute(unsigned number)
{
    namespace ns = vdk::internal::signals;

    auto chnl = ns::this_thread_channel();
    auto done = false;

    while (number)
    {
        if (auto cmnd = chnl->get())
        {
            const ns::command_owner owner{ cmnd };
            owner.cmnd->invoke();
            done = true;
        }
        else
        {
            break;
        }
        --number;
    }

    return done;
}

bool signals_execute()
{
    namespace ns = vdk::internal::signals;

    auto chnl = ns::this_thread_channel();
    auto done = false;

    if (auto cmnd = chnl->get())
    {
        const ns::command_owner owner{ cmnd };
        owner.cmnd->invoke();
        done = true;
    }

    return done;
}

context::context()
    : ctrl_{ vdk::internal::signals::memory_new<
             vdk::internal::signals::ctx_ctrl>() }
{}

context::~context() noexcept
{
    ctrl_->free();
}

} // namespace vdk



//========================= lite version =========================//



namespace vdk
{
// Internal implementation details
namespace internal::signals::lite
{
// Control block for context
class ctx_ctrl
{
public:

    ctx_ctrl() noexcept = default;
    ~ctx_ctrl() noexcept = default;

    // Check whether the context object is still active
    bool active() const noexcept;

    // Release ownership of the control block
    void free() noexcept;

    void incr_refs() noexcept;
    void decr_refs() noexcept;

    ctx_ctrl(const ctx_ctrl &) = delete;
    ctx_ctrl(ctx_ctrl &&) = delete;
    ctx_ctrl & operator=(const ctx_ctrl &) = delete;
    ctx_ctrl & operator=(ctx_ctrl &&) = delete;

private:

    // Bit-flag for reference counter
    using uint = size_t;
    static constexpr auto total = sizeof(uint) * CHAR_BIT;
    static constexpr uint active_flag = uint(1) << (total - 1);

    size_t refs_{ active_flag };
};

inline bool ctx_ctrl::active() const noexcept
{
    return refs_ & active_flag;
}

void ctx_ctrl::incr_refs() noexcept
{
    ++refs_;
}

void ctx_ctrl::decr_refs() noexcept
{
    if (!--refs_) memory_delete(this);
}

void ctx_ctrl::free() noexcept
{
    if (!(refs_ &= ~active_flag)) memory_delete(this);
}

// Structure that forms internal list of connections to slots
struct connection
{
    connection(target * slot,
               ctx_ctrl * cntx,
               unsigned int id) noexcept;

    ~connection() noexcept;

    connection(const connection &) = delete;
    connection(connection &&) = delete;
    connection & operator=(const connection &) = delete;
    connection & operator=(connection &&) = delete;

    target * const      slot_;
    ctx_ctrl * const    cntx_;
    connection *        next_;
    connection **       prev_;
    connection *        rmvd_;
    const unsigned int  id_;
};

connection::connection(target * slot,
                       ctx_ctrl * cntx,
                       unsigned int id) noexcept
    : slot_{ slot },
      cntx_{ cntx },
      next_{ nullptr },
      prev_{ nullptr },
      rmvd_{ nullptr },
      id_{ id }
{
    if (cntx_) cntx_->incr_refs();
}

connection::~connection() noexcept
{
    slot_->uninstall();
    if (cntx_) cntx_->decr_refs();
}

// Control block for signal
class sig_ctrl
{
public:

    sig_ctrl() noexcept;
    ~sig_ctrl() noexcept;

    // Connect the signal to the given slot
    // Returns connection id (never 0)
    // Throws an exception if memory allocation fails
    friend unsigned int signal_connect(sig_ctrl * self,
                                       target * slot,
                                       ctx_ctrl * cntx);

    // Disconnect the signal from the given slot
    // If the slot is not equality comparable, disconnects nothing
    // Returns true on successful disconnection, false otherwise
    friend bool signal_disconnect(sig_ctrl * self,
                                  target * slot) noexcept;

    // Disconnect connection identified by the given id
    // Returns true on successful disconnection, false otherwise
    friend bool signal_disconnect(sig_ctrl * self,
                                  unsigned int id) noexcept;

    // Disconnect all slots from the signal
    friend void signal_disconnect(sig_ctrl * self) noexcept;

    // Activate the signal with the given arguments
    friend void signal_activate(sig_ctrl * self, void * args);

    // Block the signal
    // Returns true if the signal was blocked prior to the call;
    // otherwise returns false
    friend bool signal_block(sig_ctrl * self, bool yes) noexcept;

    // Check whether the signal is blocked
    // Returns true if the signal is blocked, false otherwise
    friend bool signal_blocked(const sig_ctrl * self) noexcept;

    // Release ownership of the control block
    void free() noexcept;

    sig_ctrl(const sig_ctrl &) = delete;
    sig_ctrl(sig_ctrl &&) = delete;
    sig_ctrl & operator=(const sig_ctrl &) = delete;
    sig_ctrl & operator=(sig_ctrl &&) = delete;

private:

    // Bit-flags for signal's state
    using uint = unsigned int;
    static constexpr uint deleted_flag = 0b0001;
    static constexpr uint cleanup_flag = 0b0010;
    static constexpr uint blocked_flag = 0b0100;

    void incr_execs() noexcept;
    void decr_execs() noexcept;

    // Obtain id for a new connection
    // This method never returns 0
    unsigned int new_id() noexcept;

    // Remove given connection logically or delete physically
    // Accepts currently incremented execution counter, if any
    void remove(connection * node, size_t unit = 0) noexcept;
    // Remove all connections logically or delete physically
    void remove() noexcept;

    // Try to delete all logically removed connections physically
    void cleanup() noexcept;

    connection *    list_;
    connection *    rmvd_;
    size_t          execs_;
    unsigned int    ids_;
    uint            state_;
};

sig_ctrl::sig_ctrl() noexcept
    : list_{ nullptr },
      rmvd_{ nullptr },
      execs_{ 0 },
      ids_{ 0 },
      state_{ 0 }
{}

sig_ctrl::~sig_ctrl() noexcept
{
    cleanup();
}

void sig_ctrl::free() noexcept
{
    remove();

    if (!execs_)
        memory_delete(this);
    else
        state_ |= deleted_flag;
}

inline void sig_ctrl::incr_execs() noexcept
{
    ++execs_;
}

inline void sig_ctrl::decr_execs() noexcept
{
    if (!--execs_)
    {
        if (state_ & cleanup_flag)
            cleanup();
        if (state_ & deleted_flag)
            memory_delete(this);
    }
}

unsigned int sig_ctrl::new_id() noexcept
{
    auto id = ++ids_;
    return id != 0 ? id : ++ids_;
}

void sig_ctrl::remove(connection * node, size_t cntr) noexcept
{
    (*node->prev_) = node->next_;
    if (node->next_) node->next_->prev_ = node->prev_;

    if (!(execs_ - cntr))
    {
        memory_delete(node);
    }
    else
    {
        auto curr = rmvd_;
        auto prev = &rmvd_;

        while (curr)
        {
            if (curr->next_ == node)
                curr->next_ = node->next_;

            prev = &curr->rmvd_;
            curr = curr->rmvd_;
        }
        (*prev) = node;
        state_ |= cleanup_flag;
    }
}

void sig_ctrl::remove() noexcept
{
    if (!execs_)
    {
        auto curr = list_;

        while (curr)
        {
            auto tmp = curr;
            curr = curr->next_;
            memory_delete(tmp);
        }
    }
    else
    {
        auto curr = list_;
        auto prev = &rmvd_;

        while (curr)
        {
            curr->rmvd_ = curr->next_;
            curr->next_ = nullptr;
            curr = curr->rmvd_;
        }

        curr = rmvd_;

        while (curr)
        {
            curr->next_ = nullptr;
            prev = &curr->rmvd_;
            curr = curr->rmvd_;
        }

        (*prev) = list_;
        state_ |= cleanup_flag;
    }
    list_ = nullptr;
}

void sig_ctrl::cleanup() noexcept
{
    while (rmvd_)
    {
        auto tmp = rmvd_;
        rmvd_ = rmvd_->rmvd_;
        memory_delete(tmp);
    }
    state_ &= ~cleanup_flag;
}

unsigned int signal_connect(sig_ctrl * self,
                            target * slot,
                            ctx_ctrl * cntx)
{
    auto curr = self->list_;
    auto prev = &self->list_;

    while (curr)
    {
        if (curr->cntx_ && !curr->cntx_->active())
        {
            auto tmp = curr;
            curr = curr->next_;
            self->remove(tmp);
            continue;
        }
        prev = &curr->next_;
        curr = curr->next_;
    }

    struct target_guard
    {
        target_guard(target * t) noexcept
            : ptr{ t }
        {}
        ~target_guard() noexcept
        {
            if (ptr) ptr->uninstall();
        }
        target * ptr;
    };

    target_guard trgt{ slot->install() };

    auto conn = memory_new<connection>(trgt.ptr,
                                       cntx,
                                       self->new_id());

    (*prev) = conn;
    conn->prev_ = prev;
    trgt.ptr = nullptr;

    return conn->id_;
}

bool signal_disconnect(sig_ctrl * self, target * slot) noexcept
{
    auto curr = self->list_;
    auto done = false;

    while (curr)
    {
        if (curr->cntx_ && !curr->cntx_->active())
        {
            auto tmp = curr;
            curr = curr->next_;
            self->remove(tmp);
            continue;
        }
        if (!done && curr->slot_->compare(slot))
        {
            auto tmp = curr;
            curr = curr->next_;
            self->remove(tmp);
            done = true;
            continue;
        }
        curr = curr->next_;
    }

    return done;
}

bool signal_disconnect(sig_ctrl * self, unsigned int id) noexcept
{
    auto curr = self->list_;
    auto done = false;

    while (curr)
    {
        if (curr->cntx_ && !curr->cntx_->active())
        {
            auto tmp = curr;
            curr = curr->next_;
            self->remove(tmp);
            continue;
        }
        if (curr->id_ == id)
        {
            auto tmp = curr;
            curr = curr->next_;
            self->remove(tmp);
            done = true;
            continue;
        }
        curr = curr->next_;
    }

    return done;
}

void signal_disconnect(sig_ctrl * self) noexcept
{
    self->remove();
}

void signal_activate(sig_ctrl * self, void * args)
{
    struct exe_guard
    {
        exe_guard(sig_ctrl * c) noexcept
            : ctrl{ c }
        {
            ctrl->incr_execs();
        }
        ~exe_guard() noexcept
        {
            ctrl->decr_execs();
        }
        sig_ctrl * const ctrl;
    };

    if (self->state_ & sig_ctrl::blocked_flag) return;

    const exe_guard guard{ self };

    auto curr = self->list_;

    while (curr)
    {
        if (curr->cntx_)
        {
            if (curr->cntx_->active())
            {
                curr->slot_->invoke(args);
            }
            else
            {
                auto tmp = curr;
                curr = curr->next_;
                self->remove(tmp, 1);
                continue;
            }
        }
        else
        {
            curr->slot_->invoke(args);
        }
        curr = curr->next_;
    }
}

bool signal_block(sig_ctrl * self, bool yes) noexcept
{
    bool prev = self->state_ & sig_ctrl::blocked_flag;

    if (yes)
        self->state_ |= sig_ctrl::blocked_flag;
    else
        self->state_ &= ~sig_ctrl::blocked_flag;

    return prev;
}

bool signal_blocked(const sig_ctrl * self) noexcept
{
    return self->state_ & sig_ctrl::blocked_flag;
}

sig_ctrl* signal_construct()
{
    return memory_new<sig_ctrl>();
}

void signal_destruct(sig_ctrl * self) noexcept
{
    self->free();
}

} // namespace internal::signals::lite

namespace lite
{
context::context()
    : ctrl_{ vdk::internal::signals::memory_new<
             vdk::internal::signals::lite::ctx_ctrl>() }
{}

context::~context() noexcept
{
    ctrl_->free();
}

} // namespace lite
} // namespace vdk