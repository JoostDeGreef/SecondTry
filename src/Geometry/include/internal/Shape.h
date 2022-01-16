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
    std::unordered_map<Node*,int> m_nodes;
    std::unordered_map<Edge*,int> m_edges;
    std::unordered_map<Face*,int> m_faces;

    std::vector<Face*> m_surface;
};
