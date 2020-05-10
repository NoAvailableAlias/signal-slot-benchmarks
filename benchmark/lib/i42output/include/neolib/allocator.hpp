// allocator.hpp
/*
 *  Copyright (c) 2018 Leigh Johnston.
 *
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 *     * Neither the name of Leigh Johnston nor the names of any
 *       other contributors to this software may be used to endorse or
 *       promote products derived from this software without specific prior
 *       written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include <neolib/neolib.hpp>
#include <memory>
#include <type_traits>
#include <ostream>
#include <boost/pool/pool_alloc.hpp>
#include <boost/pool/singleton_pool.hpp>
#include <neolib/memory.hpp>

namespace neolib
{

    template <typename T, std::size_t ChunkSize = 4096, bool Omega = false, std::size_t Instance = 0>
    class neo_pool_allocator
    {
    public:
        typedef T value_type;
        typedef T* pointer;
        typedef T& reference;
        typedef const T* const_pointer;
        typedef const T& const_reference;
        typedef std::size_t size_type;
        typedef ptrdiff_t difference_type;
    private:
        typedef std::allocator<T> backup_allocator_t;

        // implementation
    private:
        struct link { link* iNext; };

        static constexpr std::size_t chunk_size()
        {
            if constexpr (ChunkSize > sizeof(T))
                return ChunkSize;
            else
                return sizeof(T);
        }

        static constexpr std::size_t element_size()
        {
            return sizeof(value_type) < sizeof(link) ? sizeof(link) : sizeof(value_type);
        }

        struct chunk
        {
            alignas(T) char iMem[chunk_size()];
            chunk* iNext;
        };
        class pool
        {
        public:
            pool() : iChunks(nullptr), iHead(nullptr)
            {
            }
            ~pool()
            {
                chunk* n = iChunks;
                while (n)
                {
                    chunk* p = n;
                    n = n->iNext;
                    delete p;
                }
            }
        public:
            void* allocate()
            {
                if (iHead == nullptr)
                    grow();
                link* p = iHead;
                if constexpr (!Omega)
                    iHead = p->iNext;
                else if (reinterpret_cast<intptr_t>(iHead->iNext) == intptr_t{ -1 })
                    iHead = reinterpret_cast<link*>(reinterpret_cast<char*>(p) + element_size());
                else
                    iHead = p->iNext;
                return p;
            }
            void deallocate(void* aObject)
            {
                if constexpr (!Omega)
                {
                    link* p = reinterpret_cast<link*>(aObject);
                    p->iNext = iHead;
                    iHead = p;
                }
            }
        public:
            void omega_recycle()
            {
                if constexpr (Omega)
                {
                    for (chunk* n = iChunks; n != nullptr; n = n->iNext)
                    {
                        constexpr std::size_t nelem = chunk_size() / element_size();
                        char* start = n->iMem;
                        char* last = start + nelem * element_size();
                        std::memset(start, 0xFF, last - start);
                        reinterpret_cast<link*>(last - element_size())->iNext = (n->iNext != nullptr ? reinterpret_cast<link*>(n->iNext->iMem) : nullptr);
                    }
                    iHead = reinterpret_cast<link*>(iChunks->iMem);
                }
            }
            template <typename CharT, typename Traits>
            void info(std::basic_ostream<CharT, Traits&>* aOutput)
            {
                uint32_t total = 0;
                uint32_t pct = 0;
                for (chunk* n = iChunks; n != nullptr; n = n->iNext)
                {
                    ++total;
                    char* start = n->iMem;
                    char* last = start + chunk_size();
                    if (reinterpret_cast<char*>(iHead) >= start && reinterpret_cast<char*>(iHead) < last)
                        pct = static_cast<uint32_t>((reinterpret_cast<char*>(iHead) - start) * 100 / (last - start));
                }
                aOutput << "Number of chunks: " << total << std::endl;
                if constexpr (Omega)
                    aOutput << "% utilization of last used chunk: " << pct << "%" << std::endl;
            }
        private:
            void grow()
            {
                chunk* n = new chunk;
                n->iNext = iChunks;
                iChunks = n;

                constexpr std::size_t nelem = chunk_size() / element_size();
                char* start = n->iMem;
                char* last = start + nelem * element_size();
                if constexpr (!Omega)
                    for (char* p = start; p < last; p += element_size())
                        reinterpret_cast<link*>(p)->iNext = reinterpret_cast<link*>(p + element_size());
                else
                    std::memset(start, 0xFF, last - start);

                reinterpret_cast<link*>(last - element_size())->iNext = nullptr;
                iHead = reinterpret_cast<link*>(start);
            }
        private:
            chunk * iChunks;
            link* iHead;
        };

        // construction
    public:
        neo_pool_allocator()
        {
        }

        neo_pool_allocator(const neo_pool_allocator&)
        {
        }

        template <typename U>
        neo_pool_allocator(const neo_pool_allocator<U, ChunkSize, Omega, Instance>& /*rhs*/)
        {
        }

        ~neo_pool_allocator()
        {
        }

        // operations
    public:
        static pointer allocate(size_type aCount = 1)
        {
            if (aCount == 1)
                return reinterpret_cast<pointer>(get_pool().allocate());
            else
                return backup_allocator().allocate(aCount);
        }

        static void deallocate(pointer aObject, size_type aCount = 1)
        {
            if constexpr (!Omega)
            {
                if (aCount == 1)
                    get_pool().deallocate(aObject);
                else
                    backup_allocator().deallocate(aObject, aCount);
            }
        }

        static void construct(pointer aObject, const_reference val)
        {
            new (aObject) T(val);
        }

        template <typename... Args>
        static void construct(pointer aObject, Args&&... aArguments)
        {
            new (aObject) T(std::forward<Args>(aArguments)...);
        }

        static void destroy(pointer aObject)
        {
            if constexpr (!Omega)
                aObject->~T();
        }

        static void omega_recycle()
        {
            get_pool().omega_recycle();
        }

        static void info()
        {
            get_pool().info();
        }

        template <typename U>
        struct rebind
        {
            typedef neo_pool_allocator<U, ChunkSize, Omega, Instance> other;
        };

        // this should really return 1 but popular implementations assume otherwise
        size_type max_size() const { return std::allocator<T>().max_size(); }

        bool operator==(const neo_pool_allocator&) const { return true; }
        bool operator!=(const neo_pool_allocator&) const { return false; }

    private:
        static backup_allocator_t& backup_allocator()
        {
            static backup_allocator_t sBackupAllocator;
            return sBackupAllocator;
        }
        static pool& get_pool()
        {
            static pool sPool;
            return sPool;
        }
    };

    template <typename T, std::size_t N, std::size_t Instance = 0>
    class reserve_allocator
    {
    public:
        typedef T value_type;
        typedef T* pointer;
        typedef T& reference;
        typedef const T* const_pointer;
        typedef const T& const_reference;
        typedef std::size_t size_type;
        typedef ptrdiff_t difference_type;

        // implementation
    private:
        struct link { link* iNext; };
        struct block
        {
            size_type iElementSize;
            union
            {
                union
                {
                    alignas(T) char a[sizeof(T)];
                    alignas(link) char b[sizeof(link)];
                } iBuffer[N];
            } iBuffer;
            char* iMem;
            link* iHead;
            block() : iElementSize(sizeof(T) < sizeof(link) ? sizeof(link) : sizeof(T)), iMem(reinterpret_cast<char*>(iBuffer.iBuffer)), iHead(reinterpret_cast<link*>(iMem)) { init(); }
            void init()
            {
                char* start = iMem;
                char* last = &start[(N - 1) * iElementSize];
                for (char* p = start; p < last; p += iElementSize)
                    reinterpret_cast<link*>(p)->iNext = reinterpret_cast<link*>(p + iElementSize);
                reinterpret_cast<link*>(last)->iNext = nullptr;
            }
            void* allocate()
            {
                if (iHead == nullptr)
                    throw std::bad_alloc("neolib::reserve_allocator::allocate() when full");
                link* p = iHead;
                iHead = p->iNext;
                return p;
            }
            void deallocate(void* aObject)
            {
                link* p = reinterpret_cast<link*>(aObject);
                p->iNext = iHead;
                iHead = p;
            }
        };

        // construction
    public:
        reserve_allocator() {}
        reserve_allocator(const reserve_allocator& rhs) {}
        template <typename U>
        reserve_allocator(const reserve_allocator<U, N, Instance>& rhs) {}
        ~reserve_allocator() {}

        // operations
    public:
        T* allocate(size_type aCount = 1)
        {
            if (aCount != 1)
                throw std::bad_alloc();
            return reinterpret_cast<T*>(get_block().allocate());
        }

        void deallocate(T* aObject, size_type aCount = 1)
        {
            if (aCount != 1)
                throw std::logic_error("neolib::reserve_allocator::deallocate");
            get_block().deallocate(aObject);
        }

        void construct(pointer aObject, const_reference val)
        {
            new (aObject) T(val);
        }

        void destroy(pointer aObject)
        {
            aObject->~T();
        }

        template <typename U>
        struct rebind
        {
            typedef reserve_allocator<U, N, Instance> other;
        };

        // this should really return 1 but popular implementations assume otherwise
        size_type max_size() const { return std::allocator<T>().max_size(); }

        bool operator==(const reserve_allocator&) const { return true; }
        bool operator!=(const reserve_allocator&) const { return false; }

    private:
        static block& get_block()
        {
            static block sBlock;
            return sBlock;
        }
    };

    template <typename T, std::size_t SmallBufferSize = 8u>
    class basic_small_buffer_allocator;

    template <typename T, typename R>
    struct small_buffer_allocator_types
    {
        typedef T controlled_value_type;
        typedef R rebound_value_type;
    };

    template <typename T, std::size_t SmallBufferSize>
    struct small_buffer
    {
        typedef T value_type;
        typedef std::aligned_storage_t<sizeof(value_type)* SmallBufferSize> buffer_storage_t;
        buffer_storage_t storage;
        bool allocated;
        small_buffer() : allocated{ false } {}
        small_buffer(const small_buffer&) : allocated{ false } {}
        small_buffer& operator=(const small_buffer&) { return *this; }
    };

    template <typename T, typename R, std::size_t SmallBufferSize>
    class basic_small_buffer_allocator<small_buffer_allocator_types<T, R>, SmallBufferSize> : public std::allocator<R>
    {
        typedef basic_small_buffer_allocator<small_buffer_allocator_types<T, R>, SmallBufferSize> self_type;
        template <typename, std::size_t>
        friend class basic_small_buffer_allocator;
    public:
        struct no_small_buffer : std::logic_error { no_small_buffer() : std::logic_error("neolib::basic_small_buffer_allocator::no_small_buffer") {} };
    public:
        typedef small_buffer_allocator_types<T, R> types;
        typedef std::false_type propagate_on_container_move_assignment;
        typedef std::false_type is_always_equal;
        template<class U> struct rebind { typedef basic_small_buffer_allocator<small_buffer_allocator_types<T, U>, SmallBufferSize> other; };
    public:
        typedef T controlled_value_type;
        typedef R value_type;
        typedef std::allocator<value_type> default_allocator_type;
        typedef typename std::allocator_traits<default_allocator_type>::pointer pointer;
        typedef small_buffer<controlled_value_type, SmallBufferSize> small_buffer_type;
    public:
        basic_small_buffer_allocator() :
            default_allocator_type{},
            iBuffer{ nullptr }
        {
        }
        basic_small_buffer_allocator(small_buffer_type& aBuffer) :
            default_allocator_type{},
            iBuffer{ &aBuffer }
        {
        }
        basic_small_buffer_allocator(const basic_small_buffer_allocator& aOther) :
            default_allocator_type{ aOther },
            iBuffer{ aOther.iBuffer }
        {
        }
        basic_small_buffer_allocator(basic_small_buffer_allocator&& aOther) :
            default_allocator_type(std::move(aOther)),
            iBuffer{ nullptr }
        {
        }
        template <typename U>
        basic_small_buffer_allocator(const basic_small_buffer_allocator<U, SmallBufferSize>& aOther) :
            default_allocator_type{ aOther },
            iBuffer{ nullptr }
        {
        }
        template <typename U>
        basic_small_buffer_allocator(const basic_small_buffer_allocator<U, SmallBufferSize>&& aOther) :
            default_allocator_type(std::move(aOther)),
            iBuffer{ nullptr }
        {
        }
    public:
        basic_small_buffer_allocator& operator=(const basic_small_buffer_allocator& aOther)
        {
            iBuffer = aOther.iBuffer;
            return *this;
        }
        basic_small_buffer_allocator& operator=(basic_small_buffer_allocator&& aOther)
        {
            iBuffer = nullptr;
            return *this;
        }
        template <typename U>
        basic_small_buffer_allocator& operator=(const basic_small_buffer_allocator<U, SmallBufferSize>& aOther)
        {
            iBuffer = nullptr;
            return *this;
        }
        template <typename U>
        basic_small_buffer_allocator& operator=(basic_small_buffer_allocator<U, SmallBufferSize>&& aOther)
        {
            iBuffer = nullptr;
            return *this;
        }
    public:
        bool operator==(const basic_small_buffer_allocator& aOther) const
        {
            return false;
        }
        bool operator!=(const basic_small_buffer_allocator& aOther) const
        {
            return true;
        }
    public:
        pointer allocate(std::size_t n)
        {
            return allocate(n, nullptr);
        }
        pointer allocate(std::size_t n, const void*)
        {
            if constexpr (std::is_same_v<value_type, controlled_value_type>)
            {
                if (n <= SmallBufferSize && is_buffer_available())
                {
                    buffer().allocated = true;
                    return reinterpret_cast<pointer>(&buffer().storage);
                }
                else
                    return default_allocator_type::allocate(n);
            }
            else
                return default_allocator_type::allocate(n);
        }
        void deallocate(pointer p, std::size_t n)
        {
            if constexpr (std::is_same_v<value_type, controlled_value_type>)
            {
                if (is_buffer_used() && p == reinterpret_cast<pointer>(&buffer().storage))
                    buffer().allocated = false;
                else
                    default_allocator_type::deallocate(p, n);
            }
            else
                default_allocator_type::deallocate(p, n);
        }
    public:
        bool has_buffer() const
        {
            return iBuffer != nullptr;
        }
        bool is_buffer_available() const
        {
            return has_buffer() && !buffer().allocated;
        }
        bool is_buffer_used() const
        {
            return has_buffer() && buffer().allocated;
        }
        const small_buffer_type& buffer() const
        {
            if (has_buffer())
                return *iBuffer;
            throw no_small_buffer();
        }
        small_buffer_type& buffer()
        {
            return const_cast<small_buffer_type&>(to_const(*this).buffer());
        }
    private:
        small_buffer_type* iBuffer;
    };

    template <typename T, typename U, std::size_t SmallBufferSize>
    inline bool operator==(const basic_small_buffer_allocator<T, SmallBufferSize>&, const basic_small_buffer_allocator<U, SmallBufferSize>&)
    {
        return false;
    }

    template <typename T, typename U, std::size_t SmallBufferSize>
    inline bool operator!=(const basic_small_buffer_allocator<T, SmallBufferSize>&, const basic_small_buffer_allocator<U, SmallBufferSize>&)
    {
        return true;
    }

    template <typename T, std::size_t SmallBufferSize = 8u>
    using small_buffer_allocator = basic_small_buffer_allocator<small_buffer_allocator_types<T, T>, SmallBufferSize>;

    // WARNING: Omega allocator doesn't free chunks and doesn't call element destructors on deallocation; use only when pathological performance is required.
    template <typename T, std::size_t ChunkSize = 1 * 1024 * 1024>
    using omega_pool_allocator = neo_pool_allocator<T, ChunkSize, true>;

    template <typename T, unsigned NextSize = 32>
    using thread_safe_pool_allocator = boost::pool_allocator<T, boost::default_user_allocator_new_delete, boost::details::pool::default_mutex, NextSize>;
    template <typename T, unsigned NextSize = 32>
    using pool_allocator = boost::pool_allocator<T, boost::default_user_allocator_new_delete, boost::details::pool::null_mutex, NextSize>;

    template <typename T, unsigned NextSize = 32>
    using thread_safe_fast_pool_allocator = boost::fast_pool_allocator<T, boost::default_user_allocator_new_delete, boost::details::pool::default_mutex, NextSize>;
    template <typename T, unsigned NextSize = 32>
    using fast_pool_allocator = boost::fast_pool_allocator<T, boost::default_user_allocator_new_delete, boost::details::pool::null_mutex, NextSize>;
}
