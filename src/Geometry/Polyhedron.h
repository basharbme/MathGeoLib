/* Copyright 2011 Jukka Jyl�nki

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

/** @file Polyhedron.h
    @author Jukka Jyl�nki
	@brief The Polyhedron geometry object. */
#pragma once

#include "Math/MathFwd.h"
#include "Math/float3.h"

//#ifdef MATH_ENABLE_STL_SUPPORT
#include <vector>
//#endif

MATH_BEGIN_NAMESPACE

/// Represents a three-dimensional closed geometric solid defined by flat polygonal faces.
class Polyhedron
{
public:
    struct Face
    {
        /// Specifies the indices of the corner vertices of this polyhedron.
        /// Indices point to the polyhedron vertex array.
        /// The face vertices should all lie on the same plane.
        /// The positive direction of the plane (the direction the face outwards normal points to) 
        /// is the one where the vertices are wound in counter-clockwise order.
        std::vector<int> v;
    };

    /// Specifies the vertices of this polyhedron.
    std::vector<float3> v;

    /// Specifies the individual faces of this polyhedron.
    std::vector<Face> f;

    /// The default ctor creates a null polyhedron (no vertices or faces).
    Polyhedron() {}

    /// Returns the number of vertices in this polyhedron.
    /// The running time of this function is O(1).
    int NumVertices() const { return v.size(); }

    /// Returns the number of faces in this polyhedron.
    /// The running time of this function is O(1).
    int NumFaces() const { return f.size(); }

    /// Returns the number of (unique)edges in this polyhedron.
    /// This function will enumerate through all faces of this polyhedron to compute the number of unique edges.
    /// The running time is linear to the number of faces and vertices in this Polyhedron.
    int NumEdges() const;

    /// Returns the i'th vertex of this polyhedron.
    float3 Vertex(int vertexIndex) const { return v[vertexIndex]; }

    /// Returns the i'th edge of this polyhedron.
    /// Performance warning: Use this function only if you are interested in a single edge of this Polyhedron.
    /// This function calls the Edges() member function to receive a list of all the edges, so it is very slow
    /// if called in a loop, O(n^2).
    LineSegment Edge(int edgeIndex) const;

    /// Returns all the (unique) edges of this polyhedron.
    std::vector<LineSegment> Edges() const;

    /// Returns all the (unique) edges of this polyhedron, as indices to the polyhedron vertex array.
    std::vector<std::pair<int, int> > EdgeIndices() const;

    /// Returns a polygon representing the given face.
    /// The winding order of the polygon will be the same as in the input.
    Polygon FacePolygon(int faceIndex) const;

    /// Returns the plane of the given polyhedron face.
    /// The normal of the plane points outwards from this polyhedron.
    Plane FacePlane(int faceIndex) const;

    /// Returns the index of the vertex of this polyhedron that reaches farthest in the given direction.
    /// @param dir The direction vector to query for. This vector can be unnormalized.
    /// @return The supporting point of this polyhedron that reaches farthest in the given direction.
    int ExtremeVertex(const float3 &dir) const;
    // float3 SupportingPoint(const float3 &dir) const;
    // bool IsSupportingPlane(const Plane &plane) const;

    /// Returns the arithmetic mean of all the corner vertices.
    float3 Centroid() const;

    /// Computes the total surface area of the faces of this polyhedron.
    float SurfaceArea() const;

    /// Computes the internal volume of this polyhedron.
    float Volume() const;

    AABB MinimalEnclosingAABB() const;

    /// Returns true if this polyhedron is closed and does not have any gaps.
    /// \note This function performs a quick check, which might not be complete.
    bool IsClosed() const;

    /// Returns true if this polyhedron forms a single connected solid volume.
//    bool IsConnected() const;

    /// Returns true if this polyhedron is convex.
    /// The running time is O(F*V) ~ O(N^2).
    bool IsConvex() const;

    /// Returns true if the Euler formula (V + F - E == 2) holds for this Polyhedron.
    bool EulerFormulaHolds() const;

    /// Returns true if this polyhedron contains the given point.
    bool Contains(const float3 &point) const;
    bool Contains(const LineSegment &lineSegment) const;
    bool Contains(const Triangle &triangle) const;
    bool Contains(const Polygon &polygon) const;
    bool Contains(const AABB &aabb) const;
    bool Contains(const OBB &obb) const;
    bool Contains(const Frustum &frustum) const;
    bool Contains(const Polyhedron &polyhedron) const;

    /// Tests if this convex polyhedron contains the given point.
    /// This function is exactly like Contains(float3), except this version of the containment test 
    /// this polyhedron is convex, and uses a faster method of testing containment.
    bool ContainsConvex(const float3 &point) const;
    bool ContainsConvex(const LineSegment &lineSegment) const;
    bool ContainsConvex(const Triangle &triangle) const;

    /// Returns the closest point on this convex polyhedron to the given point.
    /// Assumes that this polyhedron is convex.
    float3 ClosestPointConvex(const float3 &point) const;

    float3 ClosestPoint(const float3 &point) const;
    float3 ClosestPoint(const LineSegment &lineSegment) const;
    float3 ClosestPoint(const LineSegment &lineSegment, float3 *lineSegmentPt) const;

    float Distance(const float3 &point) const;

    bool Intersects(const LineSegment &lineSegment) const;
    bool Intersects(const Line &line) const;
    bool Intersects(const Ray &ray) const;
    bool Intersects(const Plane &plane) const;
    bool Intersects(const Polyhedron &polyhedron) const;
    bool Intersects(const AABB &aabb) const;
    bool Intersects(const OBB &obb) const;
    bool Intersects(const Triangle &triangle) const;
    bool Intersects(const Polygon &polygon) const;
    bool Intersects(const Frustum &frustum) const;
    bool Intersects(const Sphere &sphere) const;
    bool IntersectsConvex(const Line &line) const;
    bool IntersectsConvex(const Ray &ray) const;
    bool IntersectsConvex(const LineSegment &lineSegment) const;
};

MATH_END_NAMESPACE