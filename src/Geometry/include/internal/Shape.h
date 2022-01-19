#pragma once

class Shape
{
public:
    Shape();
    Shape(Shape && other);
    Shape(const Shape & other);

    ~Shape();

    Shape & Copy(const Shape & other);
    Shape & Swap(Shape && other);
    void Clear();

private:
    PODPtrStore<Node> m_nodes;
    PODPtrStore<Edge> m_edges;
    PODPtrStore<Face> m_faces;
};
