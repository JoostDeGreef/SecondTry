#include "Geometry.h"

using namespace Geometry;

Shape::Shape()
{}

Shape::~Shape()
{
    Clear();
}

Shape::Shape(Shape && other)
{
    Swap(std::move(other));
}

Shape::Shape(const Shape & other)
{
    Copy(other);
}

Shape & Shape::Copy(const Shape & other)
{
    auto CopyMap = []<class T>(T & t_res,const T & t_in)
    {
        using Object = typename std::remove_pointer<typename T::key_type>::type;
        while(t_res.size()>t_in.size())
        {
            delete t_res.begin()->first;
            t_res.erase(t_res.begin());
        }
        while(t_res.size()<t_in.size())
        {
            t_res.emplace(new Object,0);
        }
        std::unordered_map<Object*,Object*> res;
        auto t_res_iter = t_res.begin();
        auto t_in_iter = t_in.begin();
        for(size_t i=0;i<t_res.size();++i)
        {
            res.emplace(t_in_iter->first,t_res_iter->first);
            ++t_res_iter;
            ++t_in_iter;
        }
        return res;
    };

    auto nodeMap = CopyMap(m_nodes,other.m_nodes);
    auto edgeMap = CopyMap(m_nodes,other.m_nodes);
    auto faceMap = CopyMap(m_nodes,other.m_nodes);

    // TODO: fill the structures
    
    return *this;
}

Shape & Shape::Swap(Shape && other)
{
    m_nodes.swap(other.m_nodes);
    m_edges.swap(other.m_edges);
    m_faces.swap(other.m_faces);
    m_surface.swap(other.m_surface);
    return *this;
}

void Shape::Clear()
{
    auto ClearMap = []<class T>(T & t)
    {
        for(auto item:t)
        {
            delete item.first;
        }
        t.clear();
    };

    ClearMap(m_nodes);
    ClearMap(m_edges);
    ClearMap(m_faces);

    m_surface.clear();
}