#include <stdio.h>
#include <stdlib.h>

#include "../src/MathGeoLib.h"
#include "../src/Math/myassert.h"
#include "../src/Geometry/PBVolume.h"
#include "TestRunner.h"
#include "ObjectGenerators.h"

MATH_IGNORE_UNUSED_VARS_WARNING

Frustum GenIdFrustum(FrustumType t, FrustumHandedness h, FrustumProjectiveSpace p)
{
	Frustum f;
	f.type = t;
	f.handedness = h;
	f.projectiveSpace = p;
	f.pos = POINT_VEC_SCALAR(0.f);
	f.front = DIR_VEC(0, 0, -1); // In right-handed convention, local view space looks towards -Z.
	f.up = DIR_VEC(0, 1, 0);
	f.nearPlaneDistance = 1.f;
	f.farPlaneDistance = 100.f;
	if (t == PerspectiveFrustum)
	{
		f.horizontalFov = pi/2.f;
		f.verticalFov = pi/2.f;
	}
	else
	{
		f.orthographicWidth = f.orthographicHeight = 100.f;
	}

	return f;
}

#define FOR_EACH_FRUSTUM_CONVENTION(f) \
	for(int ii = 0; ii < 8; ++ii) { \
		switch(ii) { \
		case 0: f = GenIdFrustum(PerspectiveFrustum, FrustumLeftHanded, FrustumSpaceGL); break; \
		case 1: f = GenIdFrustum(PerspectiveFrustum, FrustumRightHanded, FrustumSpaceGL); break; \
		case 2: f = GenIdFrustum(PerspectiveFrustum, FrustumLeftHanded, FrustumSpaceD3D); break; \
		case 3: f = GenIdFrustum(PerspectiveFrustum, FrustumRightHanded, FrustumSpaceD3D); break; \
		case 4: f = GenIdFrustum(OrthographicFrustum, FrustumLeftHanded, FrustumSpaceGL); break; \
		case 5: f = GenIdFrustum(OrthographicFrustum, FrustumRightHanded, FrustumSpaceGL); break; \
		case 6: f = GenIdFrustum(OrthographicFrustum, FrustumLeftHanded, FrustumSpaceD3D); break; \
		case 7: f = GenIdFrustum(OrthographicFrustum, FrustumRightHanded, FrustumSpaceD3D); break; \
		} \
		
UNIQUE_TEST(Frustum_AspectRatio)
{
	Frustum f;
	FOR_EACH_FRUSTUM_CONVENTION(f)
		assert(EqualAbs(f.AspectRatio(), 1.f));
	}
}

UNIQUE_TEST(Frustum_WorldRight)
{
	Frustum f;
	FOR_EACH_FRUSTUM_CONVENTION(f)
		if (f.handedness == FrustumRightHanded)
			assert(f.WorldRight().Equals(DIR_VEC(1, 0, 0)));
		else // In the test func, all cameras look down to -Z, so left-handed cameras need to point their right towards -X then.
			assert(f.WorldRight().Equals(DIR_VEC(-1, 0, 0)));
	}
}

UNIQUE_TEST(Frustum_Chirality)
{
	Frustum f;
	FOR_EACH_FRUSTUM_CONVENTION(f)
		assert(f.WorldMatrix().Determinant() > 0.f);
		assert(f.ViewMatrix().Determinant() > 0.f);
		if (f.handedness == FrustumLeftHanded)
			assert(f.ProjectionMatrix().Determinant4() > 0.f); // left-handed view -> projection space transform does not change handedness.
		else
			assert(f.ProjectionMatrix().Determinant4() < 0.f); // but right-handed transform should.
	}
}

UNIQUE_TEST(Frustum_Planes)
{
	Frustum f;
	FOR_EACH_FRUSTUM_CONVENTION(f)
		assert(f.NearPlane().normal.Equals(DIR_VEC(0, 0, 1)));
		assert(EqualAbs(f.NearPlane().d, -1.f));

		assert(f.FarPlane().normal.Equals(DIR_VEC(0, 0, -1)));
		assert(EqualAbs(f.FarPlane().d, 100.f));

		for(int i = 0; i < 9; ++i)
		{
			vec pt;
			if (i == 8)
				pt = f.CenterPoint();
			else
				pt = f.CornerPoint(i);
			assert(f.NearPlane().SignedDistance(pt) < 1e-3f);
			assert(f.FarPlane().SignedDistance(pt) < 1e-3f);
			assert(f.LeftPlane().SignedDistance(pt) < 1e-3f);
			assert(f.RightPlane().SignedDistance(pt) < 1e-3f);
			assert(f.TopPlane().SignedDistance(pt) < 1e-3f);
			assert(f.BottomPlane().SignedDistance(pt) < 1e-3f);
			assert(f.Contains(pt));
		}
	}
}

UNIQUE_TEST(Frustum_Corners)
{
	Frustum f;
	FOR_EACH_FRUSTUM_CONVENTION(f)

		// Corner points are returned in XYZ order: 0: ---, 1: --+, 2: -+-, 3: -++, 4: +--, 5: +-+, 6: ++-, 7: +++
		if (f.type == PerspectiveFrustum && f.handedness == FrustumLeftHanded)
		{
			assert(f.CornerPoint(0).Equals(POINT_VEC(1.f, -1.f, -1.f)));
			assert(f.CornerPoint(1).Equals(POINT_VEC(100.f, -100.f, -100.f)));
			assert(f.CornerPoint(2).Equals(POINT_VEC(1.f, 1, -1.f)));
			assert(f.CornerPoint(3).Equals(POINT_VEC(100.f, 100.f, -100.f)));
			assert(f.CornerPoint(4).Equals(POINT_VEC(-1.f, -1.f, -1.f)));
			assert(f.CornerPoint(5).Equals(POINT_VEC(-100.f, -100.f, -100.f)));
			assert(f.CornerPoint(6).Equals(POINT_VEC(-1.f, 1.f, -1.f)));
			assert(f.CornerPoint(7).Equals(POINT_VEC(-100.f, 100.f, -100.f)));
		}
		else if (f.type == PerspectiveFrustum && f.handedness == FrustumRightHanded)
		{
			assert(f.CornerPoint(0).Equals(POINT_VEC(-1.f, -1.f, -1.f)));
			assert(f.CornerPoint(1).Equals(POINT_VEC(-100.f, -100.f, -100.f)));
			assert(f.CornerPoint(2).Equals(POINT_VEC(-1.f, 1, -1.f)));
			assert(f.CornerPoint(3).Equals(POINT_VEC(-100.f, 100.f, -100.f)));
			assert(f.CornerPoint(4).Equals(POINT_VEC(1.f, -1.f, -1.f)));
			assert(f.CornerPoint(5).Equals(POINT_VEC(100.f, -100.f, -100.f)));
			assert(f.CornerPoint(6).Equals(POINT_VEC(1.f, 1.f, -1.f)));
			assert(f.CornerPoint(7).Equals(POINT_VEC(100.f, 100.f, -100.f)));
		}
		else if (f.type == OrthographicFrustum && f.handedness == FrustumLeftHanded)
		{
			assert(f.CornerPoint(0).Equals(POINT_VEC(50.f, -50.f, -1.f)));
			assert(f.CornerPoint(1).Equals(POINT_VEC(50.f, -50.f, -100.f)));
			assert(f.CornerPoint(2).Equals(POINT_VEC(50.f, 50.f, -1.f)));
			assert(f.CornerPoint(3).Equals(POINT_VEC(50.f, 50.f, -100.f)));
			assert(f.CornerPoint(4).Equals(POINT_VEC(-50.f, -50.f, -1.f)));
			assert(f.CornerPoint(5).Equals(POINT_VEC(-50.f, -50.f, -100.f)));
			assert(f.CornerPoint(6).Equals(POINT_VEC(-50.f, 50.f, -1.f)));
			assert(f.CornerPoint(7).Equals(POINT_VEC(-50.f, 50.f, -100.f)));
		}
		else if (f.type == OrthographicFrustum && f.handedness == FrustumRightHanded)
		{
			assert(f.CornerPoint(0).Equals(POINT_VEC(-50.f, -50.f, -1.f)));
			assert(f.CornerPoint(1).Equals(POINT_VEC(-50.f, -50.f, -100.f)));
			assert(f.CornerPoint(2).Equals(POINT_VEC(-50.f, 50.f, -1.f)));
			assert(f.CornerPoint(3).Equals(POINT_VEC(-50.f, 50.f, -100.f)));
			assert(f.CornerPoint(4).Equals(POINT_VEC(50.f, -50.f, -1.f)));
			assert(f.CornerPoint(5).Equals(POINT_VEC(50.f, -50.f, -100.f)));
			assert(f.CornerPoint(6).Equals(POINT_VEC(50.f, 50.f, -1.f)));
			assert(f.CornerPoint(7).Equals(POINT_VEC(50.f, 50.f, -100.f)));
		}
	}
}

// Tests that whatever can be unprojected projects back to the same location on the Frustum 2D plane.
UNIQUE_TEST(Frustum_Project_Unproject_Symmetry)
{
	Frustum f;
	FOR_EACH_FRUSTUM_CONVENTION(f)

		for(int i = 0; i < 10; ++i)
		{
			// Orient and locate the Frustum randomly
			float3x3 rot = float3x3::RandomRotation(rng);
			f.Transform(rot);
			f.pos += vec::RandomDir(rng, rng.Float(1.f, 100.f));

			for(int j = 0; j < 100; ++j)
			{
				float2 pt = float2::RandomBox(rng, -1.f, 1.f);
				vec pos = f.NearPlanePos(pt);
				vec pt2 = f.Project(pos);
				assert2(pt.Equals(pt2.xy()), pt, pt2.xy());

				pos = f.FarPlanePos(pt);
				pt2 = f.Project(pos);
				assert2(pt.Equals(pt2.xy()), pt, pt2.xy());

				pos = f.PointInside(pt.x, pt.y, rng.Float());
				pt2 = f.Project(pos);
				assert2(pt.Equals(pt2.xy()), pt, pt2.xy());
			}
		}
	}
}

UNIQUE_TEST(Frustum_Plane_Normals_Are_Correct)
{
	Frustum f;
	FOR_EACH_FRUSTUM_CONVENTION(f)
		PBVolume<6> pb = ToPBVolume(f);
		vec corners[8];
		Plane planes[6];
		f.GetCornerPoints(corners);
		f.GetPlanes(planes);
		for(int i = 0; i < 8; ++i)
			assert(pb.Contains(corners[i]));
		for(int i = 0; i < 6; ++i)
			for(int j = 0; j < 8; ++j)
				assert(planes[i].SignedDistance(corners[j]) <= 0.f);
	}
}

UNIQUE_TEST(Frustum_IsConvex)
{
	Frustum f;
	FOR_EACH_FRUSTUM_CONVENTION(f)
		Polyhedron p = f.ToPolyhedron();

		for(int i = 0; i < 6; ++i)
		{
			Plane p1 = f.GetPlane(i);
			Plane p2 = p.FacePlane(i);
			assert3(p1.Equals(p2), i, p1, p2);
		}
		assert(p.EulerFormulaHolds());
		assert(p.IsClosed());
		assert(p.IsConvex());
		assert(!p.IsNull());
	}
}

UNIQUE_TEST(Plane_ProjectToNegativeHalf)
{
	Plane p(DIR_VEC(0,1,0), 50.f);

	vec neg = POINT_VEC(0,-100.f, 0);
	vec pos = POINT_VEC(0, 100.f, 0);
	assert(neg.Equals(p.ProjectToNegativeHalf(neg)));
	assert(!neg.Equals(p.ProjectToPositiveHalf(neg)));

	assert(pos.Equals(p.ProjectToPositiveHalf(pos)));
	assert(!pos.Equals(p.ProjectToNegativeHalf(pos)));
}

UNIQUE_TEST(Frustum_Contains)
{
	Frustum f;
	FOR_EACH_FRUSTUM_CONVENTION(f)
		for(int i = 0; i < 8; ++i)
		{
			vec corner = f.CornerPoint(i);
			vec closestPoint = f.ClosestPoint(corner);
			float distance = f.Distance(corner);
			if (!f.Contains(corner) || distance > 1e-4f)
				LOGE("Closest point to %s: %s", corner.ToString().c_str(), closestPoint.ToString().c_str());
			assert4(f.Contains(corner), i, corner, f, distance);
			assert1(distance < 10.f, distance);
		}

		assert3(f.Contains(f.CenterPoint()), f, f.CenterPoint(), f.Distance(f.CenterPoint()));
	}
}

RANDOMIZED_TEST(Frustum_Contains_Corners)
{
	vec pt = vec::RandomBox(rng, POINT_VEC_SCALAR(-SCALE), POINT_VEC_SCALAR(SCALE));
	Frustum b = RandomFrustumContainingPoint(pt);

	for(int i = 0; i < 9; ++i)
	{
		vec point = (i == 8) ? b.CenterPoint() : b.CornerPoint(i);

		assert(b.NearPlane().SignedDistance(point) < 1e-3f);
		assert(b.FarPlane().SignedDistance(point) < 1e-3f);
		assert(b.LeftPlane().SignedDistance(point) < 1e-3f);
		assert(b.RightPlane().SignedDistance(point) < 1e-3f);
		assert(b.TopPlane().SignedDistance(point) < 1e-3f);
		assert(b.BottomPlane().SignedDistance(point) < 1e-3f);
		assert1(b.Contains(point), b.Distance(point));
	}
}

UNIQUE_TEST(Frustum_Matrices)
{
	Frustum f;
	FOR_EACH_FRUSTUM_CONVENTION(f)
		if (f.handedness == FrustumRightHanded)
		{
			float3x4 wm = f.WorldMatrix();
			assert(wm.IsIdentity());

			float3x4 vm = f.ViewMatrix();
			assert(vm.IsIdentity());
		}
		else
		{
			float3x4 wm = f.WorldMatrix() * float3x4::RotateY(pi);
			assert(wm.IsIdentity());

			float3x4 vm = f.ViewMatrix() * float3x4::RotateY(pi);
			assert(vm.IsIdentity());
		}
	}
}

UNIQUE_TEST(Frustum_Projection)
{
	Frustum f;
	FOR_EACH_FRUSTUM_CONVENTION(f)
		const float nearD = (f.projectiveSpace == FrustumSpaceD3D) ? 0.f : -1.f;

		// Corner points are returned in XYZ order: 0: ---, 1: --+, 2: -+-, 3: -++, 4: +--, 5: +-+, 6: ++-, 7: +++
		assert(f.Project(f.CornerPoint(0)).Equals(POINT_VEC(-1, -1, nearD)));
		assert(f.Project(f.CornerPoint(1)).Equals(POINT_VEC(-1, -1, 1)));
		assert(f.Project(f.CornerPoint(2)).Equals(POINT_VEC(-1, 1, nearD)));
		assert(f.Project(f.CornerPoint(3)).Equals(POINT_VEC(-1, 1, 1)));
		assert(f.Project(f.CornerPoint(4)).Equals(POINT_VEC(1, -1, nearD)));
		assert(f.Project(f.CornerPoint(5)).Equals(POINT_VEC(1, -1, 1)));
		assert(f.Project(f.CornerPoint(6)).Equals(POINT_VEC(1, 1, nearD)));
		assert(f.Project(f.CornerPoint(7)).Equals(POINT_VEC(1, 1, 1)));
	}
}

UNIQUE_TEST(Frustum_UnProject)
{
	Frustum f;
	FOR_EACH_FRUSTUM_CONVENTION(f)
		if (f.type == PerspectiveFrustum)
		{
			Ray r = f.UnProject(0, 0);
			assert(r.pos.Equals(f.pos));
			assert(r.pos.Equals(POINT_VEC(0, 0, 0)));
			assert(r.dir.Equals(DIR_VEC(0,0,-1)));

			r = f.UnProject(-1, -1);
			assert(r.pos.Equals(f.pos));
			assert(r.pos.Equals(POINT_VEC(0, 0, 0)));
			assert(r.dir.Equals((f.CornerPoint(1)-f.CornerPoint(0)).Normalized()));

			r = f.UnProject(1, 1);
			assert(r.pos.Equals(f.pos));
			assert(r.pos.Equals(POINT_VEC(0, 0, 0)));
			assert(r.dir.Equals((f.CornerPoint(7)-f.CornerPoint(6)).Normalized()));
		}
	}
}

UNIQUE_TEST(Frustum_UnProjectFromNearPlane)
{
	Frustum f;
	FOR_EACH_FRUSTUM_CONVENTION(f)
		Ray r = f.UnProjectFromNearPlane(0, 0);
		assert(r.pos.Equals(POINT_VEC(0,0,-1)));
		assert(r.dir.Equals(DIR_VEC(0,0,-1)));

		r = f.UnProjectFromNearPlane(-1, -1);
		assert(r.pos.Equals(f.CornerPoint(0)));
		assert(r.dir.Equals((f.CornerPoint(1)-f.CornerPoint(0)).Normalized()));

		r = f.UnProjectFromNearPlane(1, 1);
		assert(r.pos.Equals(f.CornerPoint(6)));
		assert(r.dir.Equals((f.CornerPoint(7)-f.CornerPoint(6)).Normalized()));
	}
}

UNIQUE_TEST(Frustum_UnProjectLineSegment)
{
	Frustum f;
	FOR_EACH_FRUSTUM_CONVENTION(f)
		LineSegment ls = f.UnProjectLineSegment(0, 0);
		assert(ls.a.Equals(POINT_VEC(0,0,-1)));
		assert(ls.b.Equals(POINT_VEC(0,0,-100)));

		ls = f.UnProjectLineSegment(-1, -1);
		assert(ls.a.Equals(f.CornerPoint(0)));
		assert(ls.b.Equals(f.CornerPoint(1)));

		ls = f.UnProjectLineSegment(1, 1);
		assert(ls.a.Equals(f.CornerPoint(6)));
		assert(ls.b.Equals(f.CornerPoint(7)));
	}
}

UNIQUE_TEST(Frustum_NearPlanePos)
{
	Frustum f;
	FOR_EACH_FRUSTUM_CONVENTION(f)
		if (f.type == PerspectiveFrustum)
		{
			if (f.handedness == FrustumLeftHanded)
			{
				assert(f.NearPlanePos(1,-1).Equals(POINT_VEC(-1,-1,-1)));
				assert(f.NearPlanePos(-1,1).Equals(POINT_VEC(1,1,-1)));
			}
			else
			{
				assert(f.NearPlanePos(-1,-1).Equals(POINT_VEC(-1,-1,-1)));
				assert(f.NearPlanePos(1,1).Equals(POINT_VEC(1,1,-1)));
			}
			assert(f.NearPlanePos(0,0).Equals(POINT_VEC(0,0,-1)));
		}
	}
}

UNIQUE_TEST(Frustum_FarPlanePos)
{
	Frustum f;
	FOR_EACH_FRUSTUM_CONVENTION(f)
		if (f.type == PerspectiveFrustum)
		{
			if (f.handedness == FrustumLeftHanded)
			{
				assert(f.FarPlanePos(1,-1).Equals(POINT_VEC(-100,-100,-100)));
				assert(f.FarPlanePos(-1,1).Equals(POINT_VEC(100,100,-100)));
			}
			else
			{
				assert(f.FarPlanePos(-1,-1).Equals(POINT_VEC(-100,-100,-100)));
				assert(f.FarPlanePos(1,1).Equals(POINT_VEC(100,100,-100)));
			}
			assert(f.FarPlanePos(0,0).Equals(POINT_VEC(0,0,-100)));
		}
	}
}

UNIQUE_TEST(Frustum_Finite)
{
	Frustum f;
	FOR_EACH_FRUSTUM_CONVENTION(f)
		assert(f.IsFinite());
	}
}

UNIQUE_TEST(Frustum_MinimalEnclosingAABB)
{
	Frustum f;
	FOR_EACH_FRUSTUM_CONVENTION(f)
		AABB a = f.MinimalEnclosingAABB();
		assert(a.Contains(f));
	}
}

UNIQUE_TEST(Frustum_MinimalEnclosingOBB)
{
	Frustum f;
	FOR_EACH_FRUSTUM_CONVENTION(f)
		OBB o = f.MinimalEnclosingOBB();
		if (!o.Contains(f))
		{
			LOGE("OBB: %s", o.ToString().c_str());
			LOGE("Frustum: %s", f.ToString().c_str());
		}
		assert(o.Contains(f));
	}
}

UNIQUE_TEST(Frustum_AspectRatio_NearPlanePos)
{
	Frustum f;
	f.type = PerspectiveFrustum;
	f.handedness = FrustumRightHanded;
	f.projectiveSpace = FrustumSpaceGL;
	f.pos = POINT_VEC_SCALAR(0.f);
	f.front = DIR_VEC(0, 0, -1);
	f.up = DIR_VEC(0, 1, 0);
	f.nearPlaneDistance = 0.5f;
	f.farPlaneDistance = 10.f;
	f.horizontalFov = DegToRad(140.f);
	f.verticalFov = DegToRad(30.f);

	assert(EqualAbs(f.NearPlaneWidth(), 0.5f*Tan(DegToRad(140.f)/2.f)*2.f));
	assert(EqualAbs(f.NearPlaneHeight(), 0.5f*Tan(DegToRad(30.f)/2.f)*2.f));

	float aspect = f.NearPlaneWidth() / f.NearPlaneHeight();
	assert(EqualAbs(aspect, f.AspectRatio()));
}

RANDOMIZED_TEST(Frustum_ToPbVolume_And_Back)
{
	vec pt = vec::RandomBox(rng, POINT_VEC_SCALAR(-SCALE), POINT_VEC_SCALAR(SCALE));
	Frustum f = RandomFrustumContainingPoint(pt);
	PBVolume<6> pbvol = ToPBVolume(f);

	Polyhedron ph2 = f.ToPolyhedron();
	assert(ph2.EulerFormulaHolds());
	///\todo Due to numeric instability, this condition does not always hold, but would be nice to have it.
//	assert(ph2.IsConvex());
	assert(ph2.IsClosed());
	Polyhedron ph = pbvol.ToPolyhedron();
	assert(ph.EulerFormulaHolds());
	assert(ph.SetEquals(ph2));
	assert(ph.IsClosed());
//	assert(ph.IsConvex());
	assert(!ph.IsNull());

	assert(ph.SetEquals(ph2));

	// .SetEquals() canonicalizes the Polyhedrons, so run the assert()s again to ensure it didn't change anything for real.
	assert(ph2.EulerFormulaHolds());
	assert(ph.EulerFormulaHolds());
//	assert(ph2.IsConvex());
	assert(ph2.IsClosed());
	assert(ph.IsClosed());
//	assert(ph.IsConvex());
	assert(!ph.IsNull());
}

// This tests the ability to compute the set intersection of two convex objects (Frustums) represented as PBVolumes.
RANDOMIZED_TEST(Intersect_Two_Frustums)
{
	vec pt = vec::RandomBox(rng, POINT_VEC_SCALAR(-SCALE), POINT_VEC_SCALAR(SCALE));

	// First, create two Frustums.
	Frustum a = RandomFrustumContainingPoint(pt);
	PBVolume<6> A = ToPBVolume(a);

	Frustum b = RandomFrustumContainingPoint(pt);
	PBVolume<6> B = ToPBVolume(b);

	// When convex objects are represented as plane-bounded volumes, their set intersection is easy: just add
	// up the face planes of each to the same list.
	PBVolume<12> intersection = A.SetIntersection(B);

	// Finally, convert the plane-bounded volume representation back to a Polyhedron representation.
	Polyhedron ph = intersection.ToPolyhedron();

	// ph is the set intersection of a and b, so must be contained in both a and b.
	assert(a.Contains(ph));
	assert(b.Contains(ph));

	// ph must be valid in itself.
	assert(!ph.IsNull());
	assert(ph.IsClosed());
	assert(ph.Contains(pt));
}
