#include "FacetSpline.hpp"

vector<mat4> calculateFrames(BSpline3f const & spline, int numSamples) {
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
      float tVal = easeOutQuad(tInc * i);
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

FacetSpline::FacetSpline(vec3 origin, vec3 direction) {
	direction = normalize(direction);
	quat directionOrientation = glm::rotation(vec3(0.0, 1.0, 0.0), direction);

	vector<vec3> splinePts = { origin };
	float tInc = 1.0f / (float) (mNumCtrlPts - 1);
	for (int num = 0; num < mNumCtrlPts; num++) {
		origin += mSplineIncrement * direction;
		float ctrlAngle = num * mCtrlAngleInc;
		float tVal = num * tInc;
		vec3 offset = directionOrientation * vec3(sinf(ctrlAngle), 0.0f, cosf(ctrlAngle));
		splinePts.push_back(origin + offset * easeOutCubic(tVal) * mSplineRadius);
	}

	mSpline = BSpline3f(splinePts, mSplineDegree, false, true);

	mFrames = calculateFrames(mSpline, mNumFacets);

	for (mat4 const & theFrame : mFrames) {
		vec3 facetPos, facetXAx, facetZAx, facetYAx;
		FacetSpline::decomposeFrame(theFrame, & facetPos, & facetXAx, & facetYAx, & facetZAx);
	    mFacets.push_back(Facet::create(facetPos, quat_cast(mat3(facetXAx, facetYAx, facetZAx))));
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