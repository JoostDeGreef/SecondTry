#pragma once

class Face
{
public:
    Face()
    {}

private:
    Geometry::Edge * m_edges[3];
    uint64_t m_facegroup;
};
