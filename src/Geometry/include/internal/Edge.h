#pragma once

class Face;

class Edge
{
public:
    Edge() = default;

    template<typename NODEMAP,typename EDGEMAP,typename FACEMAP>
    void MapPtrsAfterStoreCopy(NODEMAP & nodeMap,EDGEMAP & edgeMap,FACEMAP & faceMap)
    {
        m_face = faceMap.at(m_face);
        m_next = edgeMap.at(m_next);
        m_twin = edgeMap.at(m_twin);
        m_prev = edgeMap.at(m_prev);
        m_start = nodeMap.at(m_start);
    }

    // calculate or return the cache length of the edge
    double CalcLength();

    const Core::OwnedPtr<Node> & Start() const;
    const Core::ViewedPtr<Edge> & Prev() const;
    const Core::ViewedPtr<Edge> & Next() const;

    void SetFace(const Core::ViewedPtr<Face> & face);
    void SetNext(const Core::ViewedPtr<Edge> & next);
    void SetTwin(const Core::ViewedPtr<Edge> & twin);
    void SetPrev(const Core::ViewedPtr<Edge> & prev);
    void SetStart(const Core::OwnedPtr<Node> & start);
private:
    Core::ViewedPtr<Face> m_face;
    Core::ViewedPtr<Edge> m_next;
    Core::ViewedPtr<Edge> m_twin;
    Core::ViewedPtr<Edge> m_prev;
    Core::OwnedPtr<Node> m_start;
    double m_length = -1.0;
};
