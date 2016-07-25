#pragma once

#include <vector>

#include "cinder/BSpline.h"
#include "cinder/app/AppBase.h"
#include "cinder/Easing.h"

#include "glm/gtx/orthonormalize.hpp"

#include "Node.hpp"
#include "Facet.hpp"

using namespace ci;
using std::vector;

class FacetSpline;
typedef std::unique_ptr<FacetSpline> FacetSplineBox;
typedef std::shared_ptr<FacetSpline> FacetSplineRef;

class FacetSpline {
public:
	FacetSpline(vec3 origin, vec3 direction);

	static FacetSplineRef create(vec3 origin, vec3 direction) { return FacetSplineRef(new FacetSpline(origin, direction)); }
	static void decomposeFrame(mat4 const & theFrame, vec3 * pos, vec3 * xAxis, vec3 * yAxis, vec3 * zAxis);

	void update();
	void draw();

private:
	vector<FacetRef> mFacets;

	int mNumCtrlPts = 10;
	float mCtrlAngleInc = 1.34 * M_2_PI;
	float mSplineIncrement = 2.0f;
	float mSplineRadius = 10.0f;
	int mSplineDegree = 3;
	int mNumFacets = 30;

	BSpline3f mSpline;
	vector<mat4> mFrames;
	bool mDrawSpline = true;
	bool mDrawFrames = true;
};

vector<mat4> calculateFrames(BSpline3f const & spline, int numSamples);