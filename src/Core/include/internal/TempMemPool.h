#pragma once

// simple memory pool implementation for temporary objects
// allocation is performed in blocks. memory is returned 
// when the pool is destroyed. objects are not reused!
template<typename T, size_t BLOCK_SIZE = 128> 
class TempMemPool final
{
public:
    TempMemPool()
    {
        m_mempool.emplace_back(new std::vector<T>);
        m_mempool.back()->reserve(BLOCK_SIZE);
    }
    ~TempMemPool()
    {
        for(auto & block:m_mempool)
        {
            delete block;
        }
        m_mempool.clear();
    }

    template<typename ... ARGS>
    T * Alloc(const ARGS & ... args)
    {
        if(m_mempool.back()->size()>=BLOCK_SIZE)
        {
            m_mempool.emplace_back(new std::vector<T>);
        }
        return &m_mempool.back()->emplace_back(args...);
//        return &m_mempool.back()->back();
    }

private:
    std::vector<std::vector<T>*> m_mempool;
};

