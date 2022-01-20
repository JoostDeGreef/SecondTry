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
    Core::PODPtrStore<Node> m_nodes;
    Core::PODPtrStore<Edge> m_edges;
    Core::PODPtrStore<Face> m_faces;

    std::vector<Face*> m_surface; // faces are owned by the shape

    template<typename ... ARGS>
    Node * AddNode(ARGS ... args)
    {
        return m_nodes.Emplace(args...);
    }
    template<typename ... ARGS>
    Edge * AddEdge(ARGS ... args)
    {
        return m_edges.Emplace(args...);
    }
    auto AddEdgePair(Node * n0,Node * n1)
    {
        Edge * edge0 = AddEdge();
        Edge * edge1 = AddEdge();
        edge0->SetTwin(edge1);
        edge1->SetTwin(edge0);
        edge0->SetStart(m_nodes.Attach(n0));
        edge1->SetStart(m_nodes.Attach(n1));
        return std::make_tuple(edge0,edge1);
    }
    Face * AddFace(Edge * e0,Edge * e1,Edge * e2,uint64_t facegroup)
    {
        Face * face = m_faces.Emplace();
        face->SetShape(this);
        face->SetEdges({e0,e1,e2});
        face->SetFacegroup(facegroup);
        e0->SetFace(face);
        e1->SetFace(face);
        e2->SetFace(face);
        e0->SetNext(e1);
        e1->SetNext(e2);
        e2->SetNext(e0);
        e0->SetPrev(e2);
        e1->SetPrev(e0);
        e2->SetPrev(e1);
        return face;
    }

public:
    class Construct
    {
    private:
        Construct() = delete;

    public:
        // construct a cube at 0,0,0 with 'side' length sides
        static Shape Cube(const double side);

        // construct a box at 0,0,0 with 'sides' length sides
        static Shape Box(const Core::Vector3d & sides);
        
    };
};

