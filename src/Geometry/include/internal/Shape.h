#pragma once

class Shape
{
    friend Face;
public:
    Shape();
    Shape(const Shape & other);

    ~Shape();

    Shape & Copy(const Shape & other);
    void Clear();
   
    // return the shape as a sequence of triangle vertices
    std::vector<float> Draw() const;
    
    double CalculateVolume() const;
    double CalculateSurface() const;
private:
    // The shape owns the memory stores for all the objects which define it
    Core::SmartPtrStore<Core::Vector3d> m_normals;
    Core::SmartPtrStore<Node> m_nodes;
    Core::SmartPtrStore<Edge> m_edges;
    Core::SmartPtrStore<Face> m_faces;

    // The shape consists out of faces
    std::vector<Core::OwnedPtr<Face>> m_surface; 

protected:
    template<typename ... ARGS>
    Core::OwnedPtr<Core::Vector3d> AddNormal(ARGS ... args)
    {
        return m_normals.Create(args...);
    }
    template<typename ... ARGS>
    Core::OwnedPtr<Node> AddNode(ARGS ... args)
    {
        return m_nodes.Create(args...);
    }
    template<typename ... ARGS>
    Core::OwnedPtr<Edge> AddEdge(ARGS ... args)
    {
        return m_edges.Create(args...);
    }
    auto AddEdgePair(
        const Core::OwnedPtr<Node> & n0,
        const Core::OwnedPtr<Node> & n1)
    {
        auto edge0 = AddEdge();
        auto edge1 = AddEdge();
        edge0->SetTwin(edge1);
        edge1->SetTwin(edge0);
        edge0->SetStart(n0);
        edge1->SetStart(n1);
        return std::make_tuple(edge0,edge1);
    }
    auto AddFace(
        Core::OwnedPtr<Edge> & e0,
        Core::OwnedPtr<Edge> & e1,
        Core::OwnedPtr<Edge> & e2,
        uint64_t facegroup,
        Core::OwnedPtr<Core::Vector3d> normal = Core::OwnedPtr<Core::Vector3d>())
    {
        auto face = m_faces.Create();
        face->SetShape(this);
        face->SetEdges({e0,e1,e2});
        face->SetFacegroup(facegroup);
        face->SetNormal(normal);
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
        // construct a cube at 0,0,0 with 'side' length sides in directions x,y,z
        static Shape Cube(const double side);

        // construct a box at 0,0,0 with 'sides' length sides in directions x,y,z
        static Shape Box(const Core::Vector3d & sides);
        
        // construct a cilinder at 0,0,0 with 'length' in the z directions and an aproximated radius between 'outerRadius' and 'innerRadius'
        // note: a negative inner radius means it's a fraction of the outer radius
        static Shape Cylinder(const double length, double outerRadius, double innerRadius = -0.95);
    };
};

