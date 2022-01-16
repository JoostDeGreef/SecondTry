#pragma once

class Face;

class Edge
{
public:
    Edge()
    {}

private:
    Face * m_face;
    Edge * m_twin;
    Node * m_start;
};
