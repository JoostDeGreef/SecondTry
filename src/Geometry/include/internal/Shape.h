#pragma once

class Shape
{
    friend Face;
public:
    Shape();
    Shape(const Shape & other);

    virtual ~Shape();

    Shape & Copy(const Shape & other);
    void Clear();
   
    // return the shape as a sequence of triangle vertices
    std::vector<float> Draw() const;
    std::vector<float> DrawWithNormals();
    
    double CalculateVolume() const;
    double CalculateSurface() const;

    Shape & Translate(const Core::Vector3d & translation);
    Shape & Rotate(const Core::Quat & rotation,const Core::Vector3d & center = Core::Vector3d(0,0,0));
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
    std::tuple<Core::OwnedPtr<Geometry::Edge>,Core::OwnedPtr<Geometry::Edge>> AddEdgePair(
        const Core::OwnedPtr<Node> & n0,
        const Core::OwnedPtr<Node> & n1);
    Core::OwnedPtr<Face> AddFace(
        Core::OwnedPtr<Edge> & e0,
        Core::OwnedPtr<Edge> & e1,
        Core::OwnedPtr<Edge> & e2,
        uint64_t facegroup,
        Core::OwnedPtr<Core::Vector3d> normal = Core::OwnedPtr<Core::Vector3d>());

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

        // extrude the polygon in the z direction
        static Shape Extrude(const TriangulatedPolygon2D & polygon, const double height);
    };
};

