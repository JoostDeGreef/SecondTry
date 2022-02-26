#include "Geometry.h"
#include "Log.h"

using namespace Core;
using namespace Geometry;

Shape Shape::Construct::Cube(const double side)
{
    return Box({side,side,side});
}

Shape Shape::Construct::Box(const Core::Vector3d & sides)
{
    Shape res;

    // create nodes
    std::vector<Core::OwnedPtr<Node>> nodes;
    for(int x=0;x<=1;++x)
    {
        for(int y=0;y<=1;++y)
        {
            for(int z=0;z<=1;++z)
            {
                nodes.emplace_back(res.AddNode(x*sides[0],y*sides[1],z*sides[2]));
            }
        }
    }
    // create normals
    auto normal0 = res.AddNormal(-1, 0, 0);
    auto normal1 = res.AddNormal( 0,-1, 0);
    auto normal2 = res.AddNormal( 0, 0,-1);
    auto normal3 = res.AddNormal( 0, 0, 1);
    auto normal4 = res.AddNormal( 0, 1, 0);
    auto normal5 = res.AddNormal( 1, 0, 0);
    // add edgepairs
    auto [edge01,edge10] = res.AddEdgePair(nodes[0],nodes[1]);
    auto [edge02,edge20] = res.AddEdgePair(nodes[0],nodes[2]);
    auto [edge04,edge40] = res.AddEdgePair(nodes[0],nodes[4]);
    auto [edge05,edge50] = res.AddEdgePair(nodes[0],nodes[5]);
    auto [edge06,edge60] = res.AddEdgePair(nodes[0],nodes[6]);
    auto [edge12,edge21] = res.AddEdgePair(nodes[1],nodes[2]);
    auto [edge13,edge31] = res.AddEdgePair(nodes[1],nodes[3]);
    auto [edge15,edge51] = res.AddEdgePair(nodes[1],nodes[5]);
    auto [edge17,edge71] = res.AddEdgePair(nodes[1],nodes[7]);
    auto [edge23,edge32] = res.AddEdgePair(nodes[2],nodes[3]);
    auto [edge26,edge62] = res.AddEdgePair(nodes[2],nodes[6]);
    auto [edge27,edge72] = res.AddEdgePair(nodes[2],nodes[7]);
    auto [edge37,edge73] = res.AddEdgePair(nodes[3],nodes[7]);
    auto [edge45,edge54] = res.AddEdgePair(nodes[4],nodes[5]);
    auto [edge46,edge64] = res.AddEdgePair(nodes[4],nodes[6]);
    auto [edge47,edge74] = res.AddEdgePair(nodes[4],nodes[7]);
    auto [edge57,edge75] = res.AddEdgePair(nodes[5],nodes[7]);
    auto [edge67,edge76] = res.AddEdgePair(nodes[6],nodes[7]);
    // add faces
    auto face012 = res.AddFace(edge01,edge12,edge20,0,normal0);
    auto face026 = res.AddFace(edge02,edge26,edge60,2,normal2);
    auto face045 = res.AddFace(edge04,edge45,edge50,1,normal1);
    auto face051 = res.AddFace(edge05,edge51,edge10,1,normal1);
    auto face064 = res.AddFace(edge06,edge64,edge40,2,normal2);
    auto face132 = res.AddFace(edge13,edge32,edge21,0,normal0);
    auto face157 = res.AddFace(edge15,edge57,edge71,3,normal3);
    auto face173 = res.AddFace(edge17,edge73,edge31,3,normal3);
    auto face237 = res.AddFace(edge23,edge37,edge72,4,normal4);
    auto face276 = res.AddFace(edge27,edge76,edge62,4,normal4);
    auto face467 = res.AddFace(edge46,edge67,edge74,5,normal5);
    auto face475 = res.AddFace(edge47,edge75,edge54,5,normal5);
    // set the surface to the generated faces
    res.m_surface = {face012,face132,face051,face045,face026,face064,
                     face173,face157,face237,face276,face467,face475};
    return res;
}

Shape Shape::Construct::Cylinder(const double length, double outerRadius, double innerRadius)
{
    if(innerRadius<0.0)
    {
        innerRadius *= -outerRadius;
    }
    if(innerRadius>=outerRadius)
    {
        std::swap(innerRadius,outerRadius);
        LogWarning("Inner radius should be smaller than outer radius.");
    }
    double halfCorner = acos(innerRadius/outerRadius);
    size_t sections = ceil(Core::Constants::Pi / halfCorner);
    double corner = (Core::Constants::Pi*2.0)/sections;
    // create nodes
    Polygon2D polygon;
    for(size_t i=0;i<sections;++i)
    {
        double alpha = corner*i;
        auto v = Core::Vector2d(outerRadius*cos(alpha), outerRadius*sin(alpha));
        auto n = v.Normalized();
        polygon.AddNode(v,n);
    }
    TriangulatedPolygon2D triangulatedPolygon(polygon);
    return Extrude(triangulatedPolygon, length);
}

Shape Shape::Construct::Extrude(const TriangulatedPolygon2D & polygon, const double height)
{
    Shape res;
    std::map<size_t,Core::OwnedPtr<Node>> topVertices;
    std::map<size_t,Core::OwnedPtr<Node>> bottomVertices;
    std::map<size_t,Core::OwnedPtr<Geometry::Edge>> topEdges;
    std::map<size_t,Core::OwnedPtr<Geometry::Edge>> bottomEdges;
    std::map<size_t,Core::OwnedPtr<Geometry::Edge>> sideEdges;
    const auto & edges2d = polygon.GetEdges();
    const auto & vertices2d = polygon.GetVertices();
    for(auto & vertex2d:vertices2d)
    {
        bottomVertices.emplace(vertex2d.m_index, res.AddNode(vertex2d.m_node.m_vertex[0],vertex2d.m_node.m_vertex[1],0));
        topVertices.emplace(vertex2d.m_index, res.AddNode(vertex2d.m_node.m_vertex[0],vertex2d.m_node.m_vertex[1],height));
    }
    for(auto & edge2d:edges2d)
    {
        auto edgeBottom = res.AddEdge(); 
        auto edgeTop = res.AddEdge(); 
        edgeTop->SetStart(topVertices[edge2d.m_node_a]);
        edgeBottom->SetStart(bottomVertices[edge2d.m_node_b]);
        topEdges.emplace(edge2d.m_index,edgeTop);
        bottomEdges.emplace(edge2d.m_index,edgeBottom);
    }
    assert(topEdges.size() == edges2d.size());
    assert(bottomEdges.size() == edges2d.size());
    for(auto & vertex2d:vertices2d)
    {
        size_t step = vertices2d.size();
        size_t i = vertex2d.m_index;
        size_t j = (vertex2d.m_index+1)%vertices2d.size();
        // straight up/down
        auto [down_s,up_s] = res.AddEdgePair(topVertices[i],bottomVertices[i]);
        up_s->SetTwin(down_s);
        down_s->SetTwin(up_s);
        sideEdges.emplace(i + 0*step,down_s);
        sideEdges.emplace(i + 1*step,up_s);
        // diagonal up/down
        auto [down_d,up_d] = res.AddEdgePair(topVertices[i],bottomVertices[j]);
        up_d->SetTwin(down_d);
        down_d->SetTwin(up_d);
        sideEdges.emplace(i + 2*step,down_d);
        sideEdges.emplace(i + 3*step,up_d);
        // horizontal left/right
        auto left = res.AddEdge(); 
        auto right = res.AddEdge(); 
        left->SetStart(topVertices[j]);
        right->SetStart(bottomVertices[i]);
        auto & top = topEdges[i];
        auto & bottom = bottomEdges[j];
        top->SetTwin(left);
        left->SetTwin(top);
        bottom->SetTwin(right);
        right->SetTwin(bottom);
        sideEdges.emplace(i + 4*step,left);
        sideEdges.emplace(i + 5*step,right);
    }
    assert(topEdges.size() == edges2d.size());
    assert(bottomEdges.size() == edges2d.size());
    auto ConnectEdges = [](Core::OwnedPtr<Geometry::Edge> & ein, Core::OwnedPtr<Geometry::Edge> & eout)
    {
        ein->SetNext(eout);
        eout->SetPrev(ein);
    };
    for(auto & edge2d:edges2d)
    {
        auto & topEdge = *topEdges[edge2d.m_index];
        topEdge.SetNext(topEdges[edge2d.m_next]);
        topEdge.SetPrev(topEdges[edge2d.m_prev]);
        auto & bottomEdge = *bottomEdges[edge2d.m_index];
        bottomEdge.SetNext(bottomEdges[edge2d.m_prev]);
        bottomEdge.SetPrev(bottomEdges[edge2d.m_next]);
        if(edge2d.m_twin!=(size_t)-1)
        {
            topEdge.SetTwin(topEdges[edge2d.m_twin]);
            bottomEdge.SetTwin(topEdges[edge2d.m_twin]);
        }
    }
    assert(topEdges.size() == edges2d.size());
    assert(bottomEdges.size() == edges2d.size());
    auto CreateFacegroupFromEdges = [&](Core::OwnedPtr<Geometry::Edge> & edge0, Core::OwnedPtr<Geometry::Edge> & edge1, Core::OwnedPtr<Geometry::Edge> & edge2, size_t group, Core::OwnedPtr<Core::Vector3d> normal)
    {
        assert(edge0 == edge2->Next());
        assert(edge1 == edge0->Next());
        assert(edge2 == edge1->Next());
        assert(edge0 == edge1->Prev());
        assert(edge1 == edge2->Prev());
        assert(edge2 == edge0->Prev());
        auto face = res.AddFace(edge0,edge1,edge2,group,normal);
        res.m_surface.emplace_back(face);
        return res.m_surface.back();
    };
    auto CreateFacegroupFromSet = [&](std::set<Core::OwnedPtr<Geometry::Edge>> & edges, size_t group, Core::OwnedPtr<Core::Vector3d> & normal)
    {
        while(!edges.empty())
        {
            auto iter0 = edges.begin();
            auto edge0 = *iter0;
            auto edge1 = edge0->Next().BecomeOwner();
            auto edge2 = edge1->Next().BecomeOwner();
            edges.erase(iter0);
            edges.erase(edge1);
            edges.erase(edge2);
            CreateFacegroupFromEdges(edge0,edge1,edge2,group,normal)->SetVertexNormals({normal,normal,normal});
        }
    };
    auto CreateFacegroupFromMap = [&](std::map<size_t,Core::OwnedPtr<Geometry::Edge>> & edgesMap, size_t group, Core::OwnedPtr<Core::Vector3d> normal)
    {
        std::set<Core::OwnedPtr<Geometry::Edge>> edges;
        std::transform(edgesMap.begin(), edgesMap.end(),
               std::inserter(edges, edges.begin()),
               [](const std::pair<size_t, Core::OwnedPtr<Geometry::Edge>>& key_value)
               { return key_value.second; });
        return CreateFacegroupFromSet(edges,group,normal);
    };
    size_t groupid = 1;
    CreateFacegroupFromMap(bottomEdges,groupid++,res.AddNormal(0,0,-1));
    CreateFacegroupFromMap(topEdges,groupid++,res.AddNormal(0,0,1));
    
    // todo: create map with vertex normals
    std::map<size_t,Core::OwnedPtr<Core::Vector3d>> vertexNormalsIn;
    std::map<size_t,Core::OwnedPtr<Core::Vector3d>> vertexNormalsOut;
    for(size_t i=0;i<vertices2d.size();++i)
    {
        auto & vn = vertices2d[i].m_node;
        switch(vn.m_normal)
        {
        case Geometry::Polygon2D::Node::Normal::Both:
            vertexNormalsIn.emplace(i,res.AddNormal(vn.m_normal1[0],vn.m_normal1[1],0));
            vertexNormalsOut.emplace(i,res.AddNormal(vn.m_normal2[0],vn.m_normal2[1],0));
            break;
        case Geometry::Polygon2D::Node::Normal::Single:
            vertexNormalsIn.emplace(i,res.AddNormal(vn.m_normal1[0],vn.m_normal1[1],0));
            vertexNormalsOut.emplace(i,vertexNormalsIn[i]);
            break;
        case Geometry::Polygon2D::Node::Normal::In:
            vertexNormalsIn.emplace(i,res.AddNormal(vn.m_normal1[0],vn.m_normal1[1],0));
            vertexNormalsOut.emplace(i,res.AddNormal());
            break;
        case Geometry::Polygon2D::Node::Normal::Out:
            vertexNormalsIn.emplace(i,res.AddNormal());
            vertexNormalsOut.emplace(i,res.AddNormal(vn.m_normal2[0],vn.m_normal2[1],0));
            break;
        case Geometry::Polygon2D::Node::Normal::None:
            vertexNormalsIn.emplace(i,res.AddNormal());
            vertexNormalsOut.emplace(i,res.AddNormal());
            break;
        }
    }

    for(size_t i=0;i<vertices2d.size();++i)
    {
        size_t step = vertices2d.size();
        size_t ii = (i+1)%step;
        auto & e0 = sideEdges[ i + 0*step];
        auto & e1 = sideEdges[ii + 1*step];
        auto & e2 = sideEdges[ i + 2*step];
        auto & e3 = sideEdges[ i + 3*step];
        auto & e4 = sideEdges[ i + 4*step];
        auto & e5 = sideEdges[ i + 5*step];
        ConnectEdges(e0,e5);
        ConnectEdges(e5,e3);
        ConnectEdges(e3,e0);
        ConnectEdges(e1,e4);
        ConnectEdges(e4,e2);
        ConnectEdges(e2,e1);
        auto n = (vertices2d[ii].m_node.m_vertex-vertices2d[i].m_node.m_vertex).Normalized();
        auto normal = res.AddNormal(n[1],-n[0],0);
        auto & n0 = vertexNormalsOut[i] || normal;
        auto & n1 = vertexNormalsIn[ii] || normal;
        auto & n2 = vertexNormalsOut[i] || normal;
        auto & n3 = vertexNormalsIn[ii] || normal;
        auto & n4 = vertexNormalsIn[ii] || normal;
        auto & n5 = vertexNormalsOut[i] || normal;
        CreateFacegroupFromEdges(e0,e5,e3,groupid,normal)->SetVertexNormals({n0,n5,n3});
        CreateFacegroupFromEdges(e1,e4,e2,groupid,normal)->SetVertexNormals({n1,n4,n2});
        groupid++;
    }
    return res;
}

Shape Shape::Construct::Sphere(double outerRadius, double innerRadius)
{
    Shape res;
    // vertices
    std::vector<Core::OwnedPtr<Node>> vertices;
    double ϕ = 2/(1+sqrt(5));
    // (±1, ±1, ±1)
    // (0, ±ϕ, ±1/ϕ)
    // (±1/ϕ, 0, ±ϕ)
    // (±ϕ, ±1/ϕ, 0)
    double a = 0;
    double b = 1;
    double c = ϕ;   // 0.62
    double d = 1/ϕ; // 1.62
    vertices.emplace_back(res.AddNode(-c,-d, a)); //  0  blue
    vertices.emplace_back(res.AddNode( c, d, a)); //  1  blue
    vertices.emplace_back(res.AddNode( b, b, b)); //  2  yellow
    vertices.emplace_back(res.AddNode( a, c, d)); //  3  green
    vertices.emplace_back(res.AddNode(-b, b, b)); //  4  yellow
    vertices.emplace_back(res.AddNode(-c, d, a)); //  5  blue
    vertices.emplace_back(res.AddNode( d,-a, c)); //  6  red
    vertices.emplace_back(res.AddNode( b,-b, b)); //  7  yellow
    vertices.emplace_back(res.AddNode( a,-c, d)); //  8  green
    vertices.emplace_back(res.AddNode( b, b,-b)); //  9  yellow
    vertices.emplace_back(res.AddNode( d,-a,-c)); // 10  red
    vertices.emplace_back(res.AddNode( b,-b,-b)); // 11  yellow
    vertices.emplace_back(res.AddNode( c,-d, a)); // 12  blue
    vertices.emplace_back(res.AddNode(-b,-b, b)); // 13  yellow
    vertices.emplace_back(res.AddNode(-d,-a, c)); // 14  red
    vertices.emplace_back(res.AddNode(-d,-a,-c)); // 15  red
    vertices.emplace_back(res.AddNode(-b, b,-b)); // 16  yellow
    vertices.emplace_back(res.AddNode( a, c,-d)); // 17  green
    vertices.emplace_back(res.AddNode( a,-c,-d)); // 18  green
    vertices.emplace_back(res.AddNode(-b,-b,-b)); // 19  yellow
    // define pentagons
    std::vector<std::vector<size_t>> pentagons = 
    {
        { 8,13, 0,12, 7}, //  0
        { 1, 5, 4, 3, 2}, //  1
        { 2, 3, 8, 7, 6}, //  2
        { 1, 2, 6,10, 9}, //  3
        { 3, 4,14,13, 8}, //  4
        { 6, 7,12,11,10}, //  5
        { 9,10,11,18,17}, //  6
        { 1, 9,17,16, 5}, //  7
        { 4, 5,16,15,14}, //  8
        { 0,13,14,15,19}, //  9
        { 0,19,18,11,12}, // 10
        {15,16,17,18,19}  // 11
    };
    // add center point to each pentagon
    for(auto & pentagon:pentagons)
    {
        auto & v0 = *vertices[pentagon[0]];
        auto & v1 = *vertices[pentagon[1]];
        auto & v2 = *vertices[pentagon[2]];
        auto & v3 = *vertices[pentagon[3]];
        auto & v4 = *vertices[pentagon[4]];
        auto v = v0 + v1 + v2 + v3 + v4;
        pentagon.emplace_back(vertices.size());
        vertices.emplace_back(res.AddNode(v));
    }
    // normalize nodes and use them as vertex normals too, then scale the vertices to match outerRadius
    std::vector<Core::OwnedPtr<Node>> vertexNormals;
    for(auto & vertex:vertices)
    {
        auto & v = *vertex;
        v.Normalize();
        vertexNormals.emplace_back(res.AddNormal(v));
        v *= outerRadius;
    }
    // add edges and faces
    std::map<std::pair<size_t,size_t>,Core::OwnedPtr<Geometry::Edge>> edges;
    auto GetEdge = [&](size_t v0,size_t v1)
    {
        auto index01 = std::make_pair(v0,v1);
        auto iter = edges.find(index01);
        if(iter == edges.end())
        {
            auto [edge01,edge10] = res.AddEdgePair(vertices[v0],vertices[v1]);
            auto index10 = std::make_pair(v1,v0);
            edges.emplace(index10,edge10);
            iter = edges.emplace(index01,edge01).first;
        }
        return iter->second;
    };
    auto CreateFace = [&](size_t v0,size_t v1,size_t v2)
    {
        auto e0 = GetEdge(v0,v1);
        auto e1 = GetEdge(v1,v2);
        auto e2 = GetEdge(v2,v0);
        auto face = res.AddFace(e0,e1,e2,0);
        face->SetVertexNormals({vertexNormals[v0],vertexNormals[v1],vertexNormals[v2]});
        res.m_surface.emplace_back(face);
    };
    // TODO: Change to CreateTriangle
    for(auto & pentagon:pentagons)
    {
        CreateFace(pentagon[0],pentagon[1],pentagon[5]);
        CreateFace(pentagon[1],pentagon[2],pentagon[5]);
        CreateFace(pentagon[2],pentagon[3],pentagon[5]);
        CreateFace(pentagon[3],pentagon[4],pentagon[5]);
        CreateFace(pentagon[4],pentagon[0],pentagon[5]);
    }
    assert(edges.size() == 12*5+12*10);
    // split the faces in 4 until the required inner radius is reached
    // while(DistancePlaneToPoint(res.m_surface[0]->GetNormal(),res.m_surface[0]->GetEdge(0)->Start(),{0,0,0})<innerRadius)
    // {
    //     edges.clear();

    // }
    // TODO: turn triangles into faces
    return res;
}
