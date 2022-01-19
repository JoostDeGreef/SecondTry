#pragma once

class Shape;

class Face
{
public:
    Face()
    {}

    void MappedAssign(Face * other,
                      Shape * shape,
                      std::unordered_map<Edge *,Edge *> & edgeMap)
    {
        for(int i=0;i<3;++i)
        {
            m_edges[i] = edgeMap.at(other->m_edges[i]);
        }
        m_shape = shape;
        m_facegroup = other->m_facegroup;
    }

private:
    Shape * m_shape;
    Geometry::Edge * m_edges[3]; // owned by the face, refcount store in shape
    uint64_t m_facegroup;
};
