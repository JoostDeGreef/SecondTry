#include "Geometry.h"

using namespace Core;
using namespace Geometry;

double Face::CalcSurface()
{
    if(m_surface < 0)
    {
        // Use Herons formula to calculate the surface
        auto p0 = *m_edges[0]->Start();
        auto p1 = *m_edges[1]->Start();
        auto p2 = *m_edges[2]->Start();
        double a = (p1-p0).LengthSquared();
        double b = (p2-p1).LengthSquared();
        double c = (p0-p2).LengthSquared();
        m_surface = sqrt(4*a*b - pow(a+b-c,2))/4;
    }
    return m_surface;
}

const Core::Vector3d & Face::CalcNormal()
{
    if(!m_normal.IsSet())
    {
        // calculate normal for the plane and normalize
        auto p0 = *m_edges[0]->Start();
        auto p1 = *m_edges[1]->Start();
        auto p2 = *m_edges[2]->Start();
        auto a = (p1-p0);
        auto b = (p2-p1);
        auto c = (p0-p2);
        auto n = a.Cross(b) + b.Cross(c) + c.Cross(a);
        m_normal = m_shape->AddNormal(n.Normalized());
    }
    return *m_normal;
}

const Core::Vector3d & Face::GetNormal() const
{
    assert(m_normal.IsSet());
    return *m_normal;
}

void Face::SetShape(Shape * shape) 
{ 
    m_shape = shape; 
}

void Face::SetEdges(const std::array<Core::OwnedPtr<Edge>,3> & edges) 
{ 
    m_surface = -1.0;
    
    m_normal.Release();

    m_normals[0].Release();
    m_normals[1].Release();
    m_normals[2].Release();

    m_edges[0] = edges[0]; 
    m_edges[1] = edges[1]; 
    m_edges[2] = edges[2]; 
}

void Face::SetVertexNormals(const std::array<Core::OwnedPtr<Core::Vector3d>,3> & normals)
{
    m_normals[0] = normals[0];
    m_normals[0] = normals[0];
    m_normals[0] = normals[0];
}

void Face::SetFacegroup(const uint64_t facegroup) 
{ 
    m_facegroup = facegroup; 
}

void Face::SetNormal(const Core::OwnedPtr<Core::Vector3d> & normal)
{
    m_normal = normal;
}

const Core::Vector3d & Face::GetVertexNormal(const int i) const
{
    return *(m_normals[i] || m_normal);
}

const Core::OwnedPtr<Edge> & Face::GetEdge(const int i) const
{
    return m_edges[i];
}
