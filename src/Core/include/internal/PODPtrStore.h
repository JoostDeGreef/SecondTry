#pragma once

// create a store (memory pool) for POD items,
// POD: pod type
// SIZE: number of pods per allocation block
// Note: this store only calls constructors/destructors for 'special' cases,
//       do not use it for anything else but PODs (Plain Old Data type)
template<typename POD,int SIZE = 128>
class PODPtrStore
{
    static_assert(std::is_pod<POD>::value, "PODPtrStore only supports PODs");

public:
    typedef PODPtrStore<POD,SIZE> this_type;
    typedef POD pod_type;
    typedef POD * pod_ptr_type;

    static const unsigned int pods_per_block = SIZE;

private:   
    class alignas(pod_type) pod_type_counted
    {
    public:
        template<typename ...ARGS>
        pod_type_counted(ARGS... args)
            : m_pod(args...)
            , m_count(1)
        {}
        pod_type m_pod;
        int m_count = 0;

    };
    typedef pod_type_counted * pod_ptr_type_counted;

public:
    PODPtrStore()
    {}
    PODPtrStore(const PODPtrStore & other) = delete;
    PODPtrStore(PODPtrStore && other) = delete;
    ~PODPtrStore()
    {
       Clear();
    }

    // Calls a constructor which accepts the provided arguments
    template<typename ...ARGS>
    pod_ptr_type Emplace(ARGS... args)
    {
        auto pc = Convert(Alloc());
        return Convert(new (pc) pod_type_counted(args...));
    }
    // Does not call any constructor!
    // Use Emplace without arguments to call the default constructor
    pod_ptr_type Alloc()
    {
        if(!m_free.empty())
        {
            auto res = m_free.back();
            m_free.pop_back();
            res->m_count = 1;
            return &res->m_pod;
        }
        else
        {
            m_free.reserve(pods_per_block);
            auto block = AddBlock();
            AddBlockToFree(block);
            return Alloc();
        }
    }
    // Attach another user to the pod ptr (increase the reference count)
    pod_ptr_type Attach(pod_ptr_type & pp)
    {
        auto counted = Convert(pp);
        counted->m_count++;
        return pp;
    }
    // Remove a user (decrease the reference count). Does not call a destructor!
    void Release(pod_ptr_type & pp)
    {
        // TODO: assert pp comes from this store
        auto counted = Convert(pp);
        assert(counted->m_count>=0);
        if(--counted->m_count <= 0)
        {
            m_free.push_back(counted);
        }
        pp = nullptr;
    }
    void Clear()
    {
        SetBlocksSize(0);
        m_free.clear();       
    }
    void Swap(this_type & other)
    {
        m_blocks.swap(other.m_blocks);
        m_free.swap(other.m_free);
    }
    // copy the storage from 'other', and provide a mapping for the pointers.
    // this is a shallow copy, item contents are untouched, reference count is duplicated.
    // a map with other to this pointer mapping is returned to facilitate deep copy.
    // note: not even a constructor is called.
    auto ShallowCopy(const this_type & other)
    {
        std::unordered_map<pod_ptr_type,pod_ptr_type> mapping;
        size_t used = other.UsedSlots();
        size_t blocks = std::max(1ul,(used+pods_per_block-1)/pods_per_block);
        for(size_t i=m_blocks.size();i<blocks;++i)
        {
            AddBlock();
        }
        SetBlocksSize(blocks);
        std::unordered_set<pod_ptr_type_counted> freeInOther;
        freeInOther.reserve(other.m_free.size());
        for(auto p:other.m_free)
        {
            freeInOther.emplace(p);
        }
        auto other_block_iter = other.m_blocks.cbegin();
        auto this_block_iter = m_blocks.begin();
        pod_ptr_type_counted this_block = *this_block_iter;
        size_t j = 0;
        for(;other_block_iter != other.m_blocks.end();++other_block_iter)
        {
            pod_ptr_type_counted other_block = *other_block_iter;
            for(size_t i = 0;i<pods_per_block;++i)
            {
                pod_ptr_type_counted other_pod = &other_block[i];                
                if(freeInOther.find(other_pod) == freeInOther.end())
                {
                    pod_ptr_type_counted this_pod = &this_block[j];
                    this_pod->m_count = other_pod->m_count;
                    mapping.emplace(Convert(other_pod),Convert(this_pod));
                    if(++j>=pods_per_block)
                    {
                        ++this_block_iter;
                        this_block = *this_block_iter;
                    }
                }
            }
        }
        m_free.clear();
        m_free.reserve(pods_per_block);
        for(int i=pods_per_block-1;i>=j;--i)
        {
           m_free.push_back(&this_block[i]);
        }
        return mapping;
    }
    // calls the copy constructor for each pod
    auto DeepCopy(const this_type & other)
    {
        auto podMap = ShallowCopy(other);
        for(auto m:podMap)
        {
            new (m.second) pod_type_counted(*(m.first));
        }
        return podMap;
    }

    size_t UsedSlots() const
    {
        return m_blocks.size()*pods_per_block - m_free.size();
    }
protected:
    auto Convert(pod_ptr_type pod_ptr) const
    {
        return static_cast<pod_ptr_type_counted>(static_cast<void *>(pod_ptr));
    }
    auto Convert(pod_ptr_type_counted pod_ptr_counted) const
    {
        return static_cast<pod_ptr_type>(static_cast<void *>(pod_ptr_counted));
    }
    auto AddBlock()
    {
        auto raw = new (std::align_val_t(alignof(pod_type))) char[pods_per_block*sizeof(pod_type_counted)];
        auto block = static_cast<pod_ptr_type_counted>(static_cast<void *>(raw));
        m_blocks.push_back(block);
        return block;
    }
    void AddBlockToFree(pod_ptr_type_counted block)
    {
        for(int i=pods_per_block-1;i>=0;--i)
        {
            m_free.push_back(&block[i]);
        }
    }
    void SetBlocksSize(size_t size)
    {
        for(size_t i=size;i<m_blocks.size();++i)
        {
            delete [] static_cast<char *>(static_cast<void *>(m_blocks[i]));
        }
        m_blocks.resize(size);
    }
private:
    std::vector<pod_ptr_type_counted> m_blocks;
    std::vector<pod_ptr_type_counted> m_free;
};
