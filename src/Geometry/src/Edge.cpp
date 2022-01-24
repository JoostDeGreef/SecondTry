#include "Geometry.h"

using namespace Core;
using namespace Geometry;

double Edge::CalcLength()
{
    if(m_length < 0)
    {
        m_length = (*m_next->m_start - *m_start).Length();
    }
    return m_length;
}

void Edge::SetFace(const Core::ViewedPtr<Face> & face) 
{ 
    m_face = face; 
}

void Edge::SetNext(const Core::ViewedPtr<Edge> & next) 
{ 
    m_next = next; 
    m_length = -1.0;
}

void Edge::SetTwin(const Core::ViewedPtr<Edge> & twin) 
{ 
    m_twin = twin; 
}

void Edge::SetPrev(const Core::ViewedPtr<Edge> & prev) 
{ 
    m_prev = prev; 
}

const Core::OwnedPtr<Node> & Edge::Start() const
{
    return m_start;
}

void Edge::SetStart(const Core::OwnedPtr<Node> & start) 
{ 
    m_start = start; 
    m_length = -1.0;
}
