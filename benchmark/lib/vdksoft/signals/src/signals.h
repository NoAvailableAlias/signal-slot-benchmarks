/*===================================================================
*   Copyright (c) Vadim Karkhin. All rights reserved.
*   Use, modification, and distribution is subject to license terms.
*   You are welcome to contact the author at: vdksoft@gmail.com
===================================================================*/

#ifndef VDK_SIGNALS_H
#define VDK_SIGNALS_H

#include <new>
#include <tuple>
#include <cstddef>
#include <utility>
#include <type_traits>

namespace vdk
{
template<typename>
class signal;

// Enumeration that specifies slot execution type
enum class exec : unsigned short
{
    sync  = 1, // Execute synchronously
    async = 2, // Execute asynchronously
};

namespace memory::signals
{
using std::size_t;

// Interface for memory resources
class memory_resource
{
public:
    virtual void * allocate(size_t size, size_t align) = 0;
    virtual void deallocate(void*addr, size_t size, size_t align) noexcept = 0;
    virtual ~memory_resource() noexcept = default;
};

// Get | set centralized memory resource for the entire library
memory_resource * memory(memory_resource * r = nullptr) noexcept;

} // namespace memory::signals

// Internal implementation details
namespace internal::signals
{
using std::size_t;
using vdk::exec;

using vdk::memory::signals::memory;

// Helper class for memory management during object construction
class memory_owner
{
public:

    memory_owner(size_t size, size_t align);
    ~memory_owner() noexcept;

    void * get() const noexcept;
    void release() noexcept;

    memory_owner(const memory_owner &) = delete;
    memory_owner & operator=(const memory_owner &) = delete;

private:

    void * addr_;
    size_t size_;
    size_t align_;
};

// Convenience function for memory allocation and object construction
template<typename T, typename ... Ts>
inline T * memory_new(Ts && ... args)
{
    memory_owner mem{ sizeof(T), alignof(T) };
    auto result = ::new (mem.get()) T{ std::forward<Ts>(args)... };
    mem.release();
    return result;
}
// Convenience function for object destruction and memory deallocation
template<typename T>
inline void memory_delete(T * addr) noexcept
{
    addr->~T();
    memory()->deallocate(addr, sizeof(T), alignof(T));
}

// Check whether T is equality comparable
template<typename T, typename = bool>
struct is_comparable : std::false_type {};
template<typename T>
struct is_comparable<T,
    decltype(std::declval<T&>() == std::declval<T&>())>
    : std::true_type {};
template<typename T> inline constexpr bool
is_comparable_v = is_comparable<T>::value;

// Trait to extract class type from pointer to member
template<typename>
struct member_pointer_class { using type = void; };
template<typename F, typename T>
struct member_pointer_class<F T::*> { using type = T; };
template<typename F>
using member_pointer_class_t = typename member_pointer_class<F>::type;

// Trait to extract value type from pointer to member
template<typename>
struct member_pointer_value { using type = void; };
template<typename F, typename T>
struct member_pointer_value<F T::*> { using type = F; };
template<typename F>
using member_pointer_value_t = typename member_pointer_value<F>::type;

// Convenience aliases for argument pack types
template<typename ... Ts>
using local_args = std::tuple<std::add_lvalue_reference_t<Ts>...>;
template<typename ... Ts>
using packaged_args = std::tuple<std::remove_reference_t<Ts>...>;

class target;
struct connection;

// Interface to manage a unit of cross-thread call transferring
// Carries data required to call a target slot from a target thread
class command
{
public:

    // Pointer to the next command in a thread channel
    command * next_;

    ~command() noexcept;

    // Invoke target slot with a tuple of copies of arguments
    void invoke();
    // Destroy the command and deallocate memory
    void destroy() noexcept;

    // Allocate memory and create command for the connection's target
    // Throws an exception if memory allocation fails
    static command * create(connection * conn, void * args);

    command(const command &) = delete;
    command(command &&) = delete;
    command & operator=(const command &) = delete;
    command & operator=(command &&) = delete;

protected:

    using slot_invoker = void(*)(command*, target*);
    using self_deleter = void(*)(command*)noexcept;

    command(connection * conn,
            slot_invoker call,
            self_deleter dtor) noexcept;

private:

    connection * const conn_;
    slot_invoker const call_;
    self_deleter const dtor_;
};

// Interface to manage an object that represents a target slot
class target
{
public:

    ~target() noexcept = default;

    // Invoke target slot with a tuple of references to arguments
    void invoke(void * args);

    // Compare targets
    // If targets are of the same static type and comparable,
    // returns the result of comparison; otherwise returns false
    bool compare(const target * other) const noexcept;

    // Allocate dynamic memory and move-construct target there
    // Throws an exception if memory allocation fails
    target * install();

    // Destroy installed target and deallocate memory
    void uninstall() noexcept;

    target(const target &) = delete;
    target(target &&) = delete;
    target & operator=(const target &) = delete;
    target & operator=(target &&) = delete;

protected:

    // Artificial virtual table
    struct vtbl
    {
        auto(*compare)(const target*, const target*)noexcept->bool;
        auto(*install)(target*)->target*;
        auto(*uninstall)(target*)noexcept->void;
    };

    using slot_invoker = void(*)(target*, void*);
    using cmnd_creator = command*(*)(connection*, void*);

    target(slot_invoker call,
           cmnd_creator ctor,
           const vtbl * vptr) noexcept;
    
    template<typename> friend const vtbl * target_vtable()noexcept;
    friend class command;

private:

    slot_invoker const call_;
    cmnd_creator const ctor_;
    const vtbl * const vptr_;
};

// Implementation of 'command' for particular types of arguments
template<typename ... Ts>
struct command_imp : command
{
    command_imp(connection * conn,
                slot_invoker call,
                self_deleter dtor,
                local_args<Ts...> * args);
    ~command_imp() noexcept = default;

    packaged_args<Ts...> pack_;
};

// Implementation of 'target' for a particular type of slot
template<typename Fn, typename ... Ts>
struct target_imp : target
{
    using cmnd_type = command_imp<Ts...>;
    using pack_type = local_args<Ts...>;

    template<typename F>
    explicit target_imp(F && func);
    ~target_imp() noexcept = default;

    Fn slot_;
};

template<typename Trgt>
inline void command_invoke(command * self, target * trgt)
{
    using trgt_t = Trgt;
    using self_t = typename trgt_t::cmnd_type;

    std::apply(static_cast<trgt_t*>(trgt)->slot_,
               static_cast<self_t*>(self)->pack_);
}

template<typename Cmnd>
inline void command_delete(command * self) noexcept
{
    memory_delete(static_cast<Cmnd*>(self));
}

template<typename Trgt>
inline bool target_compare([[maybe_unused]] const target * lhs,
                           [[maybe_unused]] const target * rhs) noexcept
{
    if constexpr (is_comparable_v<decltype(Trgt::slot_)>)
        return static_cast<const Trgt*>(lhs)->slot_ ==
               static_cast<const Trgt*>(rhs)->slot_;
    else
        return false;
}

template<typename Trgt>
inline target * target_install(target * self)
{
    return memory_new<Trgt>(std::move(static_cast<Trgt*>(self)->slot_));
}

template<typename Trgt>
inline void target_uninstall(target * self) noexcept
{
    memory_delete(static_cast<Trgt*>(self));
}

template<typename Trgt>
inline void target_invoke(target * self, void * args)
{
    using self_t = Trgt;
    using pack_t = typename self_t::pack_type;

    std::apply(static_cast<self_t*>(self)->slot_,
              *static_cast<pack_t*>(args));
}

template<typename Trgt>
inline command * target_create(connection * conn, void * args)
{
    using self_t = Trgt;
    using cmnd_t = typename self_t::cmnd_type;
    using pack_t = typename self_t::pack_type;

    return memory_new<cmnd_t>(conn,
                              &command_invoke<self_t>,
                              &command_delete<cmnd_t>,
                              static_cast<pack_t*>(args));
}

template<typename Trgt>
inline const target::vtbl * target_vtable() noexcept
{
    static constexpr target::vtbl tbl
    {
        &target_compare<Trgt>,
        &target_install<Trgt>,
        &target_uninstall<Trgt>,
    };
    return &tbl;
}

template<typename ... Ts>
command_imp<Ts...>::
command_imp(connection * conn,
            slot_invoker call,
            self_deleter dtor,
            local_args<Ts...> * args)
    : command{ conn, call, dtor },
      pack_{ *args }
{}

template<typename Fn, typename ... Ts>
template<typename F>
target_imp<Fn, Ts...>::
target_imp(F && func)
    : target{ &target_invoke<target_imp>,
              &target_create<target_imp>,
               target_vtable<target_imp>() },
      slot_{ std::forward<F>(func) }
{}

// Helper class that is used to bind pointer to class member and
// pointer to class instance together
template<typename Fn, typename Ty>
class binder
{
public:

    binder(Fn call, Ty inst) noexcept
        : slot_{ call }, inst_{ inst }
    {}

    binder(const binder &) noexcept = default;
    binder & operator=(const binder &) noexcept = default;

    ~binder() noexcept = default;

    bool operator==(const binder & other) const noexcept
    {
        return slot_ == other.slot_ && inst_ == other.inst_;
    }

    template<typename ... Ts>
    void operator()(Ts && ... args) const
    {
        (inst_->*slot_)(std::forward<Ts>(args)...);
    }

private:

    Fn const slot_;
    Ty const inst_;
};

class sig_ctrl;
class ctx_ctrl;

// Interface to signal's internal control block
auto signal_construct()->sig_ctrl*;
auto signal_destruct(sig_ctrl*)noexcept->void;
auto signal_connect(sig_ctrl*, target*, ctx_ctrl*, exec)->unsigned;
auto signal_disconnect(sig_ctrl*, target*)noexcept->bool;
auto signal_disconnect(sig_ctrl*, unsigned)noexcept->bool;
auto signal_disconnect(sig_ctrl*)noexcept->void;
auto signal_activate(sig_ctrl*, void*)->void;
auto signal_block(sig_ctrl*, bool)noexcept->bool;
auto signal_blocked(const sig_ctrl*)noexcept->bool;

} // namespace internal::signals

// Execute signals received in the current thread
bool signals_execute();
bool signals_execute(unsigned number);

// Context for slot invocations
// Provides thread affinity and automatic lifetime tracking
class context
{
protected:

    context();
    ~context() noexcept;

    context(const context &) = delete;
    context(context &&) = delete;
    context & operator=(const context &) = delete;
    context & operator=(context &&) = delete;

private:

    template<typename> friend class vdk::signal;

    vdk::internal::signals::ctx_ctrl * const ctrl_;
};

// Public interface for signals
template<typename ... ArgTs>
class signal<void(ArgTs...)> final
{
public:

    signal();
    ~signal() noexcept;

    template<typename Ty, typename Fn>
    unsigned connect(Ty * object, Fn slot, exec type = {});
    template<typename Fn>
    unsigned connect(Fn slot);

    template<typename Ty, typename Fn>
    bool disconnect(Ty * object, Fn slot) noexcept;
    template<typename Fn>
    bool disconnect(Fn slot) noexcept;
    bool disconnect(unsigned id) noexcept;
    void disconnect() noexcept;

    void emit(ArgTs ... args) const;
    void operator()(ArgTs ... args) const;

    bool block(bool yes = true) noexcept;
    bool blocked() const noexcept;

    signal(const signal &) = delete;
    signal(signal &&) = delete;
    signal & operator=(const signal &) = delete;
    signal & operator=(signal &&) = delete;

private:

    vdk::internal::signals::sig_ctrl * const ctrl_;
};

template<typename ... ArgTs>
signal<void(ArgTs...)>::signal()
    : ctrl_{ vdk::internal::signals::signal_construct() }
{}

template<typename ... ArgTs>
signal<void(ArgTs...)>::~signal() noexcept
{
    vdk::internal::signals::signal_destruct(ctrl_);
}

template<typename ... ArgTs>
template<typename Ty, typename Fn>
unsigned signal<void(ArgTs...)>::
connect(Ty * object, Fn slot, exec type)
{
    namespace ns = vdk::internal::signals;

    if constexpr (std::is_member_pointer_v<Fn>)
    {
        static_assert(std::is_base_of_v<
            ns::member_pointer_class_t<Fn>, Ty>,
            "signal::connect: signal-slot type mismatch");

        if constexpr (std::is_member_function_pointer_v<Fn>)
        {
            static_assert(std::is_invocable_v<Fn, Ty*, ArgTs...>,
                "signal::connect: signal-slot type mismatch");
        }
        else
        {
            static_assert(std::is_invocable_v<
                ns::member_pointer_value_t<Fn>, ArgTs...>,
                "signal::connect: signal-slot type mismatch");
        }

        if (!object || !slot) return 0;

        using slot_t = ns::binder<Fn, Ty*>;
        ns::target_imp<slot_t, ArgTs...> trgt{ slot_t{ slot, object } };

        if constexpr(std::is_base_of_v<context, Ty>)
            return ns::signal_connect(ctrl_, &trgt, object->ctrl_, type);
        else
            return ns::signal_connect(ctrl_, &trgt, nullptr, type);
    }
    else
    {
        static_assert(std::is_base_of_v<context, Ty>,
            "signal::connect: class does not inherit from 'context'");
        static_assert(std::is_invocable_v<Fn, ArgTs...>,
            "signal::connect: signal-slot type mismatch");
        if constexpr(std::is_pointer_v<Fn>)
            if (!slot) return 0;
        if (!object) return 0;
        ns::target_imp<Fn, ArgTs...> trgt{ std::move(slot) };
        return ns::signal_connect(ctrl_, &trgt, object->ctrl_, type);
    }
}

template<typename ... ArgTs>
template<typename Fn>
unsigned signal<void(ArgTs...)>::
connect(Fn slot)
{
    namespace ns = vdk::internal::signals;
    static_assert(std::is_invocable_v<Fn, ArgTs...>,
        "signal::connect: signal-slot type mismatch");
    if constexpr(std::is_pointer_v<Fn>)
        if (!slot) return 0;
    ns::target_imp<Fn, ArgTs...> trgt{ std::move(slot) };
    return ns::signal_connect(ctrl_, &trgt, nullptr, exec{});
}

template<typename ... ArgTs>
template<typename Ty, typename Fn>
bool signal<void(ArgTs...)>::
disconnect(Ty * object, Fn slot) noexcept
{
    namespace ns = vdk::internal::signals;

    static_assert(std::is_member_pointer_v<Fn>,
        "signal::disconnect: slot is not a member pointer");

    static_assert(std::is_base_of_v<ns::member_pointer_class_t<Fn>, Ty>,
        "signal::disconnect: signal-slot type mismatch");

    if constexpr (std::is_member_function_pointer_v<Fn>)
    {
        static_assert(std::is_invocable_v<Fn, Ty*, ArgTs...>,
            "signal::disconnect: signal-slot type mismatch");
    }
    else
    {
        static_assert(std::is_invocable_v<
            ns::member_pointer_value_t<Fn>, ArgTs...>,
            "signal::disconnect: signal-slot type mismatch");
    }

    if (!object || !slot) return false;

    using slot_t = ns::binder<Fn, Ty*>;
    ns::target_imp<slot_t, ArgTs...> trgt{ slot_t{ slot, object } };
    return ns::signal_disconnect(ctrl_, &trgt);
}

template<typename ... ArgTs>
template<typename Fn>
bool signal<void(ArgTs...)>::
disconnect(Fn slot) noexcept
{
    namespace ns = vdk::internal::signals;
    static_assert(std::is_invocable_v<Fn, ArgTs...>,
        "signal::disconnect: signal-slot type mismatch");
    if constexpr(std::is_pointer_v<Fn>)
        if (!slot) return false;
    ns::target_imp<Fn, ArgTs...> trgt{ std::move(slot) };
    return ns::signal_disconnect(ctrl_, &trgt);
}

template<typename ... ArgTs>
bool signal<void(ArgTs...)>::
disconnect(unsigned id) noexcept
{
    if (!id) return false;
    return vdk::internal::signals::signal_disconnect(ctrl_, id);
}

template<typename ... ArgTs>
void signal<void(ArgTs...)>::
disconnect() noexcept
{
    vdk::internal::signals::signal_disconnect(ctrl_);
}

template<typename ... ArgTs>
void signal<void(ArgTs...)>::operator()(ArgTs ... args) const
{
    namespace ns = vdk::internal::signals;
    ns::local_args<ArgTs...> pack{ args... };
    ns::signal_activate(ctrl_, &pack);
}

template<typename ... ArgTs>
void signal<void(ArgTs...)>::emit(ArgTs ... args) const
{
    namespace ns = vdk::internal::signals;
    ns::local_args<ArgTs...> pack{ args... };
    ns::signal_activate(ctrl_, &pack);
}

template<typename ... ArgTs>
bool signal<void(ArgTs...)>::block(bool yes) noexcept
{
    return vdk::internal::signals::signal_block(ctrl_, yes);
}

template<typename ... ArgTs>
bool signal<void(ArgTs...)>::blocked() const noexcept
{
    return vdk::internal::signals::signal_blocked(ctrl_);
}

} // namespace vdk



//========================= lite version =========================//



namespace vdk
{
namespace lite
{
template<typename>
class signal;
class context;
} // namespace lite

// Internal implementation details
namespace internal::signals::lite
{
// Implementation of 'target' for a particular type of slot
template<typename Fn, typename ... Ts>
struct target_imp : target
{
    using pack_type = local_args<Ts...>;

    template<typename F>
    explicit target_imp(F && func);
    ~target_imp() noexcept = default;

    Fn slot_;
};

template<typename Fn, typename ... Ts>
template<typename F>
target_imp<Fn, Ts...>::
target_imp(F && func)
    : target{ &target_invoke<target_imp>,
               nullptr,
               target_vtable<target_imp>() },
      slot_{ std::forward<F>(func) }
{}

class sig_ctrl;
class ctx_ctrl;

// Interface to signal's internal control block
auto signal_construct()->sig_ctrl*;
auto signal_destruct(sig_ctrl*)noexcept->void;
auto signal_connect(sig_ctrl*, target*, ctx_ctrl*)->unsigned;
auto signal_disconnect(sig_ctrl*, target*)noexcept->bool;
auto signal_disconnect(sig_ctrl*, unsigned)noexcept->bool;
auto signal_disconnect(sig_ctrl*)noexcept->void;
auto signal_activate(sig_ctrl*, void*)->void;
auto signal_block(sig_ctrl*, bool)noexcept->bool;
auto signal_blocked(const sig_ctrl*)noexcept->bool;

} // namespace internal::signals::lite

namespace lite
{
// Context for slot invocations
// Provides automatic lifetime tracking
class context
{
protected:

    context();
    ~context() noexcept;

    context(const context &) = delete;
    context(context &&) = delete;
    context & operator=(const context &) = delete;
    context & operator=(context &&) = delete;

private:

    template<typename> friend class vdk::lite::signal;

    vdk::internal::signals::lite::ctx_ctrl * const ctrl_;
};

// Public interface for signals
template<typename ... ArgTs>
class signal<void(ArgTs...)> final
{
public:

    signal();
    ~signal() noexcept;

    template<typename Ty, typename Fn>
    unsigned connect(Ty * object, Fn slot);
    template<typename Fn>
    unsigned connect(Fn slot);

    template<typename Ty, typename Fn>
    bool disconnect(Ty * object, Fn slot) noexcept;
    template<typename Fn>
    bool disconnect(Fn slot) noexcept;
    bool disconnect(unsigned id) noexcept;
    void disconnect() noexcept;

    void emit(ArgTs ... args) const;
    void operator()(ArgTs ... args) const;

    bool block(bool yes = true) noexcept;
    bool blocked() const noexcept;

    signal(const signal &) = delete;
    signal(signal &&) = delete;
    signal & operator=(const signal &) = delete;
    signal & operator=(signal &&) = delete;

private:

    vdk::internal::signals::lite::sig_ctrl * const ctrl_;
};

template<typename ... ArgTs>
signal<void(ArgTs...)>::signal()
    : ctrl_{ vdk::internal::signals::lite::signal_construct() }
{}

template<typename ... ArgTs>
signal<void(ArgTs...)>::~signal() noexcept
{
    vdk::internal::signals::lite::signal_destruct(ctrl_);
}

template<typename ... ArgTs>
template<typename Ty, typename Fn>
unsigned signal<void(ArgTs...)>::
connect(Ty * object, Fn slot)
{
    namespace ns = vdk::internal::signals;

    if constexpr (std::is_member_pointer_v<Fn>)
    {
        static_assert(std::is_base_of_v<
            ns::member_pointer_class_t<Fn>, Ty>,
            "signal::connect: signal-slot type mismatch");

        if constexpr (std::is_member_function_pointer_v<Fn>)
        {
            static_assert(std::is_invocable_v<Fn, Ty*, ArgTs...>,
                "signal::connect: signal-slot type mismatch");
        }
        else
        {
            static_assert(std::is_invocable_v<
                ns::member_pointer_value_t<Fn>, ArgTs...>,
                "signal::connect: signal-slot type mismatch");
        }

        if (!object || !slot) return 0;

        using slot_t = ns::binder<Fn, Ty*>;
        ns::lite::target_imp<slot_t, ArgTs...> trgt{ slot_t{ slot, object } };

        if constexpr(std::is_base_of_v<context, Ty>)
            return ns::lite::signal_connect(ctrl_, &trgt, object->ctrl_);
        else
            return ns::lite::signal_connect(ctrl_, &trgt, nullptr);
    }
    else
    {
        static_assert(std::is_base_of_v<context, Ty>,
            "signal::connect: class does not inherit from 'context'");
        static_assert(std::is_invocable_v<Fn, ArgTs...>,
            "signal::connect: signal-slot type mismatch");
        if constexpr(std::is_pointer_v<Fn>)
            if (!slot) return 0;
        if (!object) return 0;
        ns::lite::target_imp<Fn, ArgTs...> trgt{ std::move(slot) };
        return ns::lite::signal_connect(ctrl_, &trgt, object->ctrl_);
    }
}

template<typename ... ArgTs>
template<typename Fn>
unsigned signal<void(ArgTs...)>::
connect(Fn slot)
{
    namespace ns = vdk::internal::signals;
    static_assert(std::is_invocable_v<Fn, ArgTs...>,
        "signal::connect: signal-slot type mismatch");
    if constexpr(std::is_pointer_v<Fn>)
        if (!slot) return 0;
    ns::lite::target_imp<Fn, ArgTs...> trgt{ std::move(slot) };
    return ns::lite::signal_connect(ctrl_, &trgt, nullptr);
}

template<typename ... ArgTs>
template<typename Ty, typename Fn>
bool signal<void(ArgTs...)>::
disconnect(Ty * object, Fn slot) noexcept
{
    namespace ns = vdk::internal::signals;

    static_assert(std::is_member_pointer_v<Fn>,
        "signal::disconnect: slot is not a member pointer");

    static_assert(std::is_base_of_v<ns::member_pointer_class_t<Fn>, Ty>,
        "signal::disconnect: signal-slot type mismatch");

    if constexpr (std::is_member_function_pointer_v<Fn>)
    {
        static_assert(std::is_invocable_v<Fn, Ty*, ArgTs...>,
            "signal::disconnect: signal-slot type mismatch");
    }
    else
    {
        static_assert(std::is_invocable_v<
            ns::member_pointer_value_t<Fn>, ArgTs...>,
            "signal::disconnect: signal-slot type mismatch");
    }

    if (!object || !slot) return false;

    using slot_t = ns::binder<Fn, Ty*>;
    ns::lite::target_imp<slot_t, ArgTs...> trgt{ slot_t{ slot, object } };
    return ns::lite::signal_disconnect(ctrl_, &trgt);
}

template<typename ... ArgTs>
template<typename Fn>
bool signal<void(ArgTs...)>::
disconnect(Fn slot) noexcept
{
    namespace ns = vdk::internal::signals;
    static_assert(std::is_invocable_v<Fn, ArgTs...>,
        "signal::disconnect: signal-slot type mismatch");
    if constexpr(std::is_pointer_v<Fn>)
        if (!slot) return false;
    ns::lite::target_imp<Fn, ArgTs...> trgt{ std::move(slot) };
    return ns::lite::signal_disconnect(ctrl_, &trgt);
}

template<typename ... ArgTs>
bool signal<void(ArgTs...)>::
disconnect(unsigned id) noexcept
{
    if (!id) return false;
    return vdk::internal::signals::lite::signal_disconnect(ctrl_, id);
}

template<typename ... ArgTs>
void signal<void(ArgTs...)>::
disconnect() noexcept
{
    vdk::internal::signals::lite::signal_disconnect(ctrl_);
}

template<typename ... ArgTs>
void signal<void(ArgTs...)>::operator()(ArgTs ... args) const
{
    namespace ns = vdk::internal::signals;
    ns::local_args<ArgTs...> pack{ args... };
    ns::lite::signal_activate(ctrl_, &pack);
}

template<typename ... ArgTs>
void signal<void(ArgTs...)>::emit(ArgTs ... args) const
{
    namespace ns = vdk::internal::signals;
    ns::local_args<ArgTs...> pack{ args... };
    ns::lite::signal_activate(ctrl_, &pack);
}

template<typename ... ArgTs>
bool signal<void(ArgTs...)>::block(bool yes) noexcept
{
    return vdk::internal::signals::lite::signal_block(ctrl_, yes);
}

template<typename ... ArgTs>
bool signal<void(ArgTs...)>::blocked() const noexcept
{
    return vdk::internal::signals::lite::signal_blocked(ctrl_);
}

} // namespace lite
} // namespace vdk

#endif // VDK_SIGNALS_H