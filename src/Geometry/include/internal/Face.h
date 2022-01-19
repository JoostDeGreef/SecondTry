#pragma once

class Shape;

class Face
{
public:
    Face()
    {}

private:
    Shape * m_shape;
    Geometry::Edge * m_edges[3]; // owned by the face, refcount store in shape
    uint64_t m_facegroup;
};
