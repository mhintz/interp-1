#pragma once

#include <vector>
#include <functional>

#include "cinder/BSpline.h"
#include "cinder/app/AppBase.h"
#include "cinder/Easing.h"

#include "Node.hpp"
#include "Facet.hpp"
#include "NoiseWalker.hpp"
#include "ShaderCache.hpp"

using namespace ci;

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
	std::vector<FacetRef> mFacets;
	vec3 mOrigin;
	vec3 mDirection;
	NoiseWalker mXWalker = NoiseWalker(0.01f);
	NoiseWalker mYWalker = NoiseWalker(0.01f);
	float mGlobalScaleOffset;

	int mNumCtrlPts = 10;
	float mCtrlAngleInc = 1.34 * M_2_PI;
	float mSplineIncrement = 10.0f;
	float mSplineRadius = 20.0f;
	int mSplineDegree = 3;
	int mNumFacets = 30;
	float mScaleIncrement = 0.04;

	// Debug stuff for checking spline and frame appearances
	BSpline3f mSpline;
	std::vector<mat4> mFrames;
	bool mDrawSpline = false;
	bool mDrawFrames = false;
};