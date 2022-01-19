#pragma once

class Face;

class Edge
{
public:
    Edge()
    {}

private:
    Face * m_face;
    Edge * m_next;
    Edge * m_twin;
    Edge * m_prev;
    Node * m_start; // owned by the edge, refcount store in shape
};
