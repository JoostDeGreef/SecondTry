#pragma once

class Shape;

class Face
{
public:
    Face()
    {}

    template<typename EDGEMAP,typename NORMALSMAP>
    void MapPtrsAfterStoreCopy(
        Shape * shape,
        EDGEMAP & edgeMap,
        NORMALSMAP & normalsMap)
    {
        for(int i=0;i<3;++i)
        {
            m_edges[i] = edgeMap.at(m_edges[i]);
            if(m_normals[i].IsSet())
            {
                m_normals[i] = normalsMap.at(m_normals[i]);
            }
        }
        m_shape = shape;
        if(m_normal.IsSet())
        {
            m_normal = normalsMap.at(m_normal);
        }
    }

    // calculate or return the cached surface
    double CalcSurface();

    // calculate or return the cached face normal
    const Core::Vector3d & CalcNormal();

    // return the cached face normal
    const Core::Vector3d & GetNormal() const;

    // return a vertex normal
    const Core::Vector3d & GetVertexNormal(const int i) const;

    // return an edge
    const Core::OwnedPtr<Edge> & GetEdge(const int i) const;

    void SetShape(Shape * shape);
    void SetEdges(const std::array<Core::OwnedPtr<Edge>,3> & edges);
    void SetFacegroup(const uint64_t facegroup);
    void SetNormal(const Core::OwnedPtr<Core::Vector3d> & normal); // face normal
    void SetVertexNormals(const std::array<Core::OwnedPtr<Core::Vector3d>,3> & normals); // vertex normals
private:
    Core::OwnedPtr<Edge> m_edges[3];
    Shape * m_shape;
    Core::OwnedPtr<Core::Vector3d> m_normal; 
    Core::OwnedPtr<Core::Vector3d> m_normals[3]; 
    uint64_t m_facegroup;    
    double m_surface = -1.0;
};
