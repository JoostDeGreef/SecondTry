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
    // auto CopyMap = []<class T>(T & t_res,const T & t_in)
    // {
    //     using Object = typename std::remove_pointer<typename T::key_type>::type;
    //     while(t_res.size()>t_in.size())
    //     {
    //         delete t_res.begin()->first;
    //         t_res.erase(t_res.begin());
    //     }
    //     while(t_res.size()<t_in.size())
    //     {
    //         t_res.emplace(new Object,0);
    //     }
    //     std::unordered_map<Object*,Object*> res;
    //     auto t_res_iter = t_res.begin();
    //     auto t_in_iter = t_in.begin();
    //     for(size_t i=0;i<t_res.size();++i)
    //     {
    //         res.emplace(t_in_iter->first,t_res_iter->first);
    //         ++t_res_iter;
    //         ++t_in_iter;
    //     }
    //     return res;
    // };

    auto nodeMap = m_nodes.Copy(other.m_nodes);
    
    // auto edgeMap = CopyMap(m_nodes,other.m_nodes);
    // auto faceMap = CopyMap(m_nodes,other.m_nodes);

    // TODO: fill the structures
    
    return *this;
}

Shape & Shape::Swap(Shape && other)
{
    m_nodes.Swap(other.m_nodes);
    m_edges.Swap(other.m_edges);
    m_faces.Swap(other.m_faces);
    return *this;
}

void Shape::Clear()
{
    m_nodes.Clear();
    m_edges.Clear();
    m_faces.Clear();
}