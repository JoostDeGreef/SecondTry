#pragma once

// create a store (memory pool) to serve shared pointers to objects
// T: object type
// Note: this is very similar to std::shared_ptr, but enables 
//       make_shared with a memory pool
template<typename T>
class SmartPtrStore final
{
public:
    static const unsigned int objects_per_block = 128;

    typedef SmartPtrStore<T> store_type;
    typedef T object_type;

    class object_type_wrapper
    {
    public:
        template<typename ...ARGS>
        object_type_wrapper(store_type * store, ARGS... args)
            : m_object(args...)
            , m_store(store)
            , m_hardCount(0)
            , m_softCount(0)
        {}

        object_type_wrapper * AttachOwner()
        {
            m_hardCount++;
            return this;
        }
        object_type_wrapper * AttachViewer()
        {
            assert(m_hardCount>0);
            m_softCount++;
            return this;
        }

        void DetachOwner()
        {
            assert(m_hardCount>0);
            if(--m_hardCount <= 0)
            {
                if(m_softCount>0)
                {
                    m_store->UsedToLimbo(this);
                }
                else
                {
                    m_store->UsedToFree(this);
                }
            }
        }
        void DetachView()
        {
            assert(m_softCount>0);
            if(--m_softCount <= 0 && m_hardCount == 0)
            {
                m_store->LimboToFree(this);
            }
        }

        const object_type & Object() const
        {
            return m_object;
        }
        object_type & Object()
        {
            return m_object;
        }

        int HardCount() const 
        {
            return m_hardCount;
        }
        int SoftCount() const 
        {
            return m_softCount;
        }
    protected:
        object_type m_object;
        store_type * m_store;
        int m_hardCount;
        int m_softCount;
    };

    class ViewedPtr;

    class OwnedPtr
    {
    protected:
        friend ViewedPtr;
        friend store_type;

        typedef OwnedPtr this_type;

        OwnedPtr(object_type_wrapper * ptr)
        {
            m_object = ptr->AttachOwner();
        }
    public:
        OwnedPtr()
            : m_object(nullptr)
        {}
        OwnedPtr(const this_type & other)
        {
            Copy(other);
        }
        OwnedPtr(this_type && other)        
        {
            Swap(std::move(other));
        }
        ~OwnedPtr()
        {
            Release();
        }

        this_type & operator = (const this_type & other)
        {
            Copy(other);
            return *this;
        }
        this_type & operator = (this_type && other)
        {
            Swap(std::move(other));
            return *this;
        }

        void Release() const
        {
            if(m_object)
            {
                m_object->DetachOwner();
                m_object = nullptr;
            }
        }
        bool IsSet() const        
        {
            assert(m_object == nullptr || m_object->HardCount() > 0);
            return m_object != nullptr;
        }
        ViewedPtr CreateView() const;

        const T * operator -> () const
        {
            assert(m_object != nullptr && m_object->HardCount()>0);
            return &m_object->Object();
        }
        T * operator -> ()
        {
            assert(m_object != nullptr && m_object->HardCount()>0);
            return &m_object->Object();
        }
        const T & operator * () const
        {
            assert(m_object != nullptr && m_object->HardCount()>0);
            return m_object->Object();
        }
        T & operator * ()
        {
            assert(m_object != nullptr && m_object->HardCount()>0);
            return m_object->Object();
        }

        bool operator == (const this_type & other) const
        {
            return m_object == other.m_object;
        }
        bool operator != (const this_type & other) const
        {
            return m_object != other.m_object;
        }
        bool operator < (const this_type & other) const
        {
            return m_object < other.m_object;
        }
        bool operator == (const ViewedPtr & other) const
        {
            return m_object == other.m_object;
        }
        bool operator != (const ViewedPtr & other) const
        {
            return m_object != other.m_object;
        }
        bool operator < (const ViewedPtr & other) const
        {
            return m_object < other.m_object;
        }

        // return the first non-null ptr
        const this_type & operator || (const this_type & other) const
        {
            return IsSet() ? *this : other;
        }
    private:
        mutable object_type_wrapper * m_object = nullptr;

        void Copy(const this_type & other)
        {
            Release();
            if(other.IsSet())
            {
                m_object = other.m_object->AttachOwner();
            }
        }
        void Swap(this_type && other)        
        {
            std::swap(m_object,other.m_object);
        }
    };

    class ViewedPtr
    {
    protected:
        friend OwnedPtr;
        friend store_type;

        typedef ViewedPtr this_type;

        ViewedPtr(object_type_wrapper * ptr)
        {
            m_object = ptr->AttachViewer();
        }
    public:
        ViewedPtr()
            : m_object(nullptr)
        {}
        ViewedPtr(const this_type & other)
        {
            Copy(other);
        }
        ViewedPtr(this_type && other)        
        {
            Swap(std::move(other));
        }
        ViewedPtr(const OwnedPtr & other)
        {
            if(other.IsSet())
            {
                m_object = other.m_object->AttachViewer();
            }
            else
            {
                assert(false); // copying something without object? limbo?
            }
        }
        ~ViewedPtr()
        {
            Release();
        }

        void Release() const
        {
            if(m_object)
            {
                m_object->DetachView();
                m_object = nullptr;
            }
        }
        bool IsSet() const
        {
            if(m_object != nullptr)
            {
                if(m_object->HardCount() <= 0)
                {
                    Release();
                }
                else
                {
                    return true;
                }
            }
            return false;
        }

        OwnedPtr BecomeOwner() const
        {
            if(IsSet())
            {
                return m_object->AttachOwner();
            }
            else
            {
                return OwnedPtr();
            }
        }

        // these are dangerous, but convenient. 
        const T * operator -> () const
        {
            assert(m_object != nullptr && m_object->HardCount()>0);
            return &m_object->Object();
        }
        T * operator -> ()
        {
            assert(m_object != nullptr && m_object->HardCount()>0);
            return &m_object->Object();
        }
        const T & operator * () const
        {
            assert(m_object != nullptr && m_object->HardCount()>0);
            return m_object->Object();
        }
        T & operator * ()
        {
            assert(m_object != nullptr && m_object->HardCount()>0);
            return m_object->Object();
        }

        this_type & operator = (const this_type & other)
        {
            Copy(other);
            return *this;
        }
        this_type & operator = (this_type && other)
        {
            Swap(std::move(other));
            return *this;
        }

        bool operator == (const this_type & other) const
        {
            return m_object == other.m_object;
        }
        bool operator != (const this_type & other) const
        {
            return m_object != other.m_object;
        }
        bool operator == (const OwnedPtr & other) const
        {
            return m_object == other.m_object;
        }
        bool operator != (const OwnedPtr & other) const
        {
            return m_object != other.m_object;
        }
    private:
        mutable object_type_wrapper * m_object = nullptr;

        void Copy(const this_type & other)
        {
            Release();
            if(other.IsSet())
            {
                m_object = other.m_object->AttachViewer();
            }
        }
        void Swap(this_type && other)
        {
            std::swap(m_object,other.m_object);
        }
    };

public:
    SmartPtrStore() = default;
    SmartPtrStore(const store_type & other) = delete;
    SmartPtrStore(store_type && other) = delete;
    ~SmartPtrStore()
    {
        ClearWithoutCallingDestructors();
    }

    // Calls a constructor which accepts the provided arguments
    template<typename ...ARGS>
    OwnedPtr Create(ARGS... args)
    {
        if(!m_free.empty())
        {
            auto res = new (m_free.back()) object_type_wrapper(this, args...);
            m_free.pop_back();
            m_used.emplace(res);
            return OwnedPtr(res);
        }
        else
        {
            m_free.reserve(objects_per_block);
            auto block = AddBlock();
            AddBlockToFree(block);
            return Create(args...);
        }
    }

    // iterator over the used objects in the store.
    // mainly for use by store owners to update all objects
    class UsedPtrIterator
    {
    public:
        UsedPtrIterator(typename std::unordered_set<object_type_wrapper *>::iterator && iter)
            : m_iter(std::move(iter))
        {}

        bool operator != (const UsedPtrIterator & other)
        {
            return m_iter != other.m_iter;
        }

        void operator ++ ()
        {
            m_iter++;
        }

        object_type & operator * ()
        {
            return (*m_iter)->Object();
        }
    private:
        typename std::unordered_set<object_type_wrapper *>::iterator m_iter;
    };

    UsedPtrIterator begin()
    {
        return m_used.begin();
    }
    UsedPtrIterator end()
    {
        return m_used.end();
    }
protected:
    void UsedToFree(object_type_wrapper * otw)
    {
        otw->Object().~T();
        m_free.emplace_back(otw);
        m_used.erase(otw);
    }
    void UsedToLimbo(object_type_wrapper * otw)
    {
        ++m_limbo;
        m_used.erase(otw);
    }
    void LimboToFree(object_type_wrapper * otw)
    {
        --m_limbo;
        m_free.emplace_back(otw);
    }

public:
    // copy the storage from 'other' to 'this', and provide a mapping for the pointers.
    // uses the object copy constructor for the objects.
    // a map with 'other store' to 'this store' pointer mapping is returned to facilitate deep copy.
    // this map (temporarily) owns the pointers. 
    auto InsertCopy(const store_type & other)
    {
        struct PtrHash 
        {
            std::size_t operator()(const ViewedPtr & p) const
            {
                return std::hash<object_type_wrapper *>()(p.m_object);
            }
        };       
        struct PtrEqual 
        {
            bool operator()(const ViewedPtr& lhs, const ViewedPtr& rhs) const
            {
                return lhs == rhs;
            }
        };
        std::unordered_map<ViewedPtr,OwnedPtr,PtrHash,PtrEqual> mapping;
        std::unordered_set<object_type_wrapper *> freeInOther;
        freeInOther.reserve(other.m_free.size());
        for(auto p:other.m_free)
        {
            freeInOther.emplace(p);
        }
        auto other_block_iter = other.m_blocks.cbegin();
        for(;other_block_iter != other.m_blocks.end();++other_block_iter)
        {
            object_type_wrapper * other_block = *other_block_iter;
            for(size_t i = 0;i<objects_per_block;++i)
            {
                object_type_wrapper * other_object = &other_block[i];                
                if(freeInOther.find(other_object) == freeInOther.end())
                {
                    // todo: fix this so it works with limbo objects
                    mapping.emplace(ViewedPtr(other_object),Create(other_object->Object()));
                }
            }
        }
        return mapping;
    }
    // Number of unique objects in the store
    size_t UniqueObjects() const
    {
        return m_blocks.size()*objects_per_block - m_free.size() - m_limbo;
    }
    // Number of objects only referenced by views
    size_t ObjectsInLimbo() const
    {
        return m_limbo;
    }
    // Remove all memory assigned to this store without calling destructors
    void ClearWithoutCallingDestructors()
    {
        SetBlocksSize(0);
        m_free.clear();
        m_limbo = 0;
    }
protected:
    // Add a new block to the store
    auto AddBlock()
    {
        char * raw = new char[objects_per_block*sizeof(object_type_wrapper)];
        auto block = static_cast<object_type_wrapper *>(static_cast<void *>(raw));
        m_blocks.push_back(block);
        return block;
    }
    // Add a full block to the list of free objects
    void AddBlockToFree(object_type_wrapper * block)
    {
        m_free.reserve(((m_free.size()+2*objects_per_block-1)/objects_per_block)*objects_per_block);
        for(int i=objects_per_block-1;i>=0;--i)
        {
            m_free.push_back(&block[i]);
        }
    }
    // Delete all but 'size' blocks
    // Does not delete the objects inside! Objects are supposedly gone when this call is made
    void SetBlocksSize(size_t size)
    {
        for(size_t i=size;i<m_blocks.size();++i)
        {
            delete [] static_cast<char *>(static_cast<void *>(m_blocks[i]));
        }
        m_blocks.resize(size);
    }

private:
    std::vector<object_type_wrapper *> m_blocks;
    std::vector<object_type_wrapper *> m_free;
    std::unordered_set<object_type_wrapper *> m_used;
    size_t m_limbo = 0;
};

template<typename T> 
typename SmartPtrStore<T>::ViewedPtr SmartPtrStore<T>::OwnedPtr::CreateView() const
{
    return ViewedPtr(m_object);
}

template<typename T> 
using OwnedPtr = typename SmartPtrStore<T>::OwnedPtr;
template<typename T> 
using ViewedPtr = typename SmartPtrStore<T>::ViewedPtr;

