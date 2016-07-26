#include "FacetSpline.hpp"

using std::vector;

vector<mat4> calculateFrames(BSpline3f const & spline, int numSamples, std::function<float (float)> easeFunc) {
  if (numSamples < 3) {
    app::AppBase::get()->console() << "Asked to calculate frames with less than 3 steps. Invalid";
    return {};
  }

  // Prepare the samples
  vector<vec3> pointsList;
  vector<vec3> tangentsList;

  // Note: t iterates from 0 to 1, over numSamples
  float tInc = 1.0f / (float) (numSamples - 1);
  for (int i = 0; i < numSamples; i++) {
      float tVal = easeFunc(tInc * i);
      vec3 point, tangent;
      spline.get(tVal, & point, & tangent);
      pointsList.push_back(point);
      tangentsList.push_back(normalize(tangent));
  }

  // Create the frames
  vector<mat4> framesList;
  framesList.resize(numSamples);

  framesList.at(0) = firstFrame(pointsList.at(0), pointsList.at(1), pointsList.at(2));
  for (int fN = 1; fN < numSamples - 1; fN++) {
      framesList.at(fN) = nextFrame(framesList.at(fN - 1), pointsList.at(fN - 1), pointsList.at(fN), tangentsList.at(fN - 1), tangentsList.at(fN));
  }
  framesList.at(numSamples - 1) = lastFrame(framesList.at(numSamples - 2), pointsList.at(numSamples - 2), pointsList.at(numSamples - 1));

  return framesList;
}

void FacetSpline::decomposeFrame(mat4 const & theFrame, vec3 * pos, vec3 * xAxis, vec3 * yAxis, vec3 * zAxis) {
	*pos = vec3(theFrame[3]);

	// This isn't the only way to extract the axes from the frame, but it's the one that works
	*xAxis = normalize(vec3(theFrame[0]));
	*zAxis = normalize(vec3(theFrame[2]));
	*yAxis = normalize(cross(* zAxis, * xAxis));

	// For example, this doesn't work
	// *xAxis = vec3(theFrame * vec4( 1, 0, 0, 0 ));
	// *yAxis = vec3(theFrame * vec4( 0, 1, 0, 0 ));
	// *zAxis = vec3(theFrame * vec4( 0, 0, 1, 0 ));
}

FacetSpline::FacetSpline(vec3 origin, vec3 direction) : mOrigin(origin), mDirection(normalize(direction)) {
	for (int i = 0; i < mNumFacets; i++) {
	    mFacets.push_back(Facet::create(vec3(), quat(), getFacetShader()));
	}

	mGlobalScaleOffset = randFloat(10.0);

	this->update();
}

void FacetSpline::update() {
	mXWalker.step();
	mYWalker.step();

	vector<vec3> splinePts = { mOrigin }; // Always start at the spline's origin

	quat directionOrientation = glm::rotation(vec3(0.0, 0.0, 1.0), mDirection);
	float noiseInc = 0.2f;
	for (int num = 0; num < mNumCtrlPts; num++) {
		vec3 localOrigin = mOrigin + (num * mSplineIncrement) * mDirection;
		float xOffset = mXWalker.sampleAt(num * noiseInc);
		float yOffset = mYWalker.sampleAt(num * noiseInc);
		vec3 offset = directionOrientation * vec3(xOffset, yOffset, 0.0);
		splinePts.push_back(localOrigin + mSplineRadius * offset);
	}

	mSpline = BSpline3f(splinePts, mSplineDegree, false, true);
	mFrames = calculateFrames(mSpline, mNumFacets, easeNone);

	mGlobalScaleOffset += mScaleIncrement;

	float scaleInc = 2.0 * M_PI / mNumFacets;
	for (int idx = 0; idx < mNumFacets; idx++) {
		vec3 facetPos, facetXAx, facetZAx, facetYAx;
		FacetSpline::decomposeFrame(mFrames.at(idx), & facetPos, & facetXAx, & facetYAx, & facetZAx);
		// To animate the scale, use this
		// float scale = cosf(mGlobalScaleOffset + (mNumFacets - idx) * scaleInc) + 2.0f; // Ranges from 1.0 to 3.0
		float scale = 1.0f;
		mFacets.at(idx)->update(facetPos, quat_cast(mat3(facetXAx, facetYAx, facetZAx)), vec3(scale));
	}
}

void FacetSpline::draw() {
	for (FacetRef const & facet : mFacets) {
		facet->draw();
	}

	if (mDrawSpline) {
		gl::ScopedColor(Color(1, 1, 1));
		gl::draw(geom::BSpline(mSpline, 90));
	}

	if (mDrawFrames) {
		const float lineLength = 1.0;
		const float lineWidth = 1.0;

		gl::ScopedLineWidth scLW(lineWidth);

		gl::begin(GL_LINES);
		for (mat4 const & theFrame : mFrames) {
			vec3 facetPos, facetXAx, facetZAx, facetYAx;
			FacetSpline::decomposeFrame(theFrame, & facetPos, & facetXAx, & facetYAx, & facetZAx);

			gl::ScopedColor xColor( Color( 1, 0.5f, 0 ) ); // Rust
	 		gl::vertex(facetPos);
	 		gl::vertex(facetPos + facetXAx * lineLength);

			gl::ScopedColor yColor( Color( 1, 0, 1 ) ); // Magenta
	 		gl::vertex(facetPos);
	 		gl::vertex(facetPos + facetYAx * lineLength);

			gl::ScopedColor zColor( Color( 0, 1, 1 ) ); // Cyan
	 		gl::vertex(facetPos);
	 		gl::vertex(facetPos + facetZAx * lineLength);
		}
		gl::end();
	}
}