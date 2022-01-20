#pragma once

class Face;

class Edge
{
public:
    Edge() = default;

    void MappedAssign(Edge * other,
                      std::unordered_map<Node *,Node *> nodeMap,
                      std::unordered_map<Edge *,Edge *> edgeMap,
                      std::unordered_map<Face *,Face *> faceMap)
    {
        m_face = faceMap.at(other->m_face);
        m_next = edgeMap.at(other->m_next);
        m_twin = edgeMap.at(other->m_twin);
        m_prev = edgeMap.at(other->m_prev);
        m_start = nodeMap.at(other->m_start);
    }

    void SetFace(Face * face) { m_face = face; }
    void SetNext(Edge * next) { m_next = next; }
    void SetTwin(Edge * twin) { m_twin = twin; }
    void SetPrev(Edge * prev) { m_prev = prev; }
    void SetStart(Node * start) { m_start = start; }
private:
    Face * m_face;
    Edge * m_next;
    Edge * m_twin;
    Edge * m_prev;
    Node * m_start; // owned by the edge, refcount store in shape
};
