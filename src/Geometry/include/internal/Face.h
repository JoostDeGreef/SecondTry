#pragma once

class Shape;

class Face
{
public:
    Face() = default;

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

    void SetShape(Shape * shape) { m_shape = shape; }
    void SetEdges(const std::array<Edge*,3> & edges) { m_edges = edges; }
    void SetFacegroup(const uint64_t facegroup) { m_facegroup = facegroup; }
private:
    Shape * m_shape;
    std::array<Edge *,3> m_edges; // owned by the face, refcount store in shape
    uint64_t m_facegroup;
};
