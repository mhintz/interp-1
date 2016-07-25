#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Arcball.h"
#include "cinder/Rand.h"
#include "cinder/Camera.h"
#include "cinder/CameraUi.h"
#include "cinder/Easing.h"

#include "Facet.hpp"

using namespace ci;
using namespace ci::app;
using namespace ci::gl;
using namespace std;

class interp1App : public App {
  public:
  	static void prepSettings( Settings * settings );
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void mouseDrag(MouseEvent event) override;
	void keyDown(KeyEvent event) override;
	void update() override;
	void draw() override;
	void resize() override;

	vector<mat4> calculateFrames(BSpline3f const & spline, int count, vector<vec3> * posOut, vector<vec3> * uAxisOut, vector<vec3> * vAxisOut);
	void drawFramesDebug();

	CameraPersp mCamera;
	CameraUi mUiCamera;

	vector<FacetRef> mFacets;
	vector<BSpline3f> mSplines;
	BatchRef mSplineVis;

	vector<mat4> mFramesDebug;
	vector<vec3> mPositionsDebug;

	float mSplineIncrement = 2.0f;
	float mSplineRadius = 10.0f;
	int mSplineDegree = 3;
	int mNumFacets = 30;
};

vector<mat4> interp1App::calculateFrames(BSpline3f const & spline, int numSamples, vector<vec3> * posOut, vector<vec3> * uAxisOut, vector<vec3> * vAxisOut) {
	if (numSamples < 3) {
		console() << "Asked to calculate frames with less than 3 steps. Invalid";
		return {}; // Empty vector :(
	}

	// Prepare the samples
	vector<vec3> pointsList;
	vector<vec3> tangentsList;

	// Note: t iterates from 0 to 1, over numSamples
	float tInc = 1.0f / (float) (numSamples - 1);
	for (int i = 0; i < numSamples; i++) {
	    float tVal = tInc * i;
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

	// Load the frames into the out vectors
	posOut->clear();
	uAxisOut->clear();
	vAxisOut->clear();

	for (int i = 0; i < numSamples; i++) {
	    mat4 const & frame = framesList.at(i);
	    vec3 pos(frame[3]); // The frame's translation component
	    // vec3 uAxis(frame[0]); // The frame's x-axis
	    vec3 vAxis(frame[1]); // The frame's y-axis
	    vec3 uAxis(frame[2]); // The frame's z-axis
	    // vec3 uAxis(frame * vec4(0, 0, 1, 0));
	    // vec3 vAxis(frame * vec4(0, 1, 0, 0));

	    posOut->push_back(pos);
	    uAxisOut->push_back(uAxis);
	    vAxisOut->push_back(vAxis);
	}

	return framesList;
}

void interp1App::prepSettings(Settings * settings) {
	settings->setFullScreen();
	settings->setTitle("Interpretation 1");
	settings->setHighDensityDisplayEnabled();
}

void interp1App::setup()
{
	gl::enableDepth();

	mCamera.lookAt(vec3(0, 0, 16), vec3(0));
	mUiCamera = CameraUi(&mCamera, getWindow());

	GlslProgRef stockColorShader = gl::getStockShader(gl::ShaderDef().color());

	vec3 origin(0);
	vec3 splineAxis(0.0, 1.0, 0.0);
	vector<vec3> splinePts = { origin };
	int numCtrlPts = 10;
	float tInc = 1.0f / (float) (numCtrlPts - 1);
	float angleInc = 1.34 * M_2_PI;
	for (int num = 0; num < numCtrlPts; num++) {
		origin += mSplineIncrement * splineAxis;
		float ctrlAngle = num * angleInc;
		float tVal = num * tInc;
		splinePts.push_back(origin + vec3(sinf(ctrlAngle), 0.0f, cosf(ctrlAngle)) * easeOutCubic(tVal) * mSplineRadius);
	}

	BSpline3f faceSpline(splinePts, mSplineDegree, false, true);
	mSplineVis = Batch::create(geom::BSpline(faceSpline, 90), stockColorShader);

	vector<geom::Plane> planeGeometries;

	vector<vec3> positions;
	vector<vec3> tangents;
	vector<vec3> normals;

	mFramesDebug = calculateFrames(faceSpline, mNumFacets, &positions, &tangents, &normals);
	mPositionsDebug = positions;

	for (int cnt = 0; cnt < mNumFacets; cnt++) {
		vec3 facetPos(positions.at(cnt));
		vec3 facetXAx(tangents.at(cnt));
		vec3 facetYAx(normals.at(cnt));
		vec3 facetZAx(normalize(cross(facetXAx, facetYAx)));
	    mFacets.push_back(Facet::create(facetPos, quat_cast(mat3(facetXAx, facetYAx, facetZAx))));
	}

	// for (int cnt = 0; cnt < mNumFacets; cnt++) {
	//     auto facetGeometry = geom::Plane().origin(positions.at(cnt)).axes(tangents.at(cnt), normals.at(cnt));
	//     planeGeometries.push_back(facetGeometry);
	// }

	// for (int cnt = 0; cnt < mNumFacets; cnt++) {
	// 	float splineT = 1.0 / mNumFacets * cnt;
	// 	vec3 planeOrigin, planeTangent;
	// 	faceSpline.get(splineT, & planeOrigin, & planeTangent);
	// 	planeTangent = normalize(planeTangent);
	// 	auto facetGeometry = geom::Plane().origin(planeOrigin).axes(planeTangent, orthogonal(planeTangent));
	// 	planeGeometries.push_back(facetGeometry);
	// }

	// for (auto & facetGeom : planeGeometries) {
	// 	auto facetColor = geom::Constant(geom::COLOR, Color(CM_RGB, randFloat(0.4), randFloat(0.4, 0.8), randFloat(0.4, 0.8)));
	// 	mFacets.push_back(Batch::create(facetGeom >> facetColor, facetShader));
	// }
}

void interp1App::resize() {

}

void interp1App::mouseDown( MouseEvent event )
{

}

void interp1App::mouseDrag(MouseEvent event) {

}

void interp1App::keyDown(KeyEvent event) {
	if (event.getCode() == KeyEvent::KEY_ESCAPE) {
		quit();
	}
}

void interp1App::update()
{
}

void interp1App::draw()
{
	gl::clear( Color( 0, 0, 0.15 ), true );

	gl::setMatrices(mCamera);

	for (FacetRef const & theMesh : mFacets) {
		theMesh->draw();
	}

	mSplineVis->draw();

	drawFramesDebug();
}

void interp1App::drawFramesDebug() {
	float lineLength = 1.0;
	float lineWidth = 1.0;

	gl::lineWidth(lineWidth);
	gl::begin(GL_LINES);
	for (int i = 0; i < mFramesDebug.size(); i++) {
		vec3 xAxis = vec3( mFramesDebug.at(i) * vec4( 1, 0, 0, 0 ) );
		vec3 yAxis = vec3( mFramesDebug.at(i) * vec4( 0, 1, 0, 0 ) );
		vec3 zAxis = vec3( mFramesDebug.at(i) * vec4( 0, 0, 1, 0 ) );

		gl::ScopedColor xColor( Color( 1, 0.5f, 0 ) );
 		gl::vertex(mPositionsDebug.at(i));
 		gl::vertex(mPositionsDebug.at(i) + xAxis * lineLength);

		gl::ScopedColor yColor( Color( 1, 0, 1 ) );
 		gl::vertex(mPositionsDebug.at(i));
 		gl::vertex(mPositionsDebug.at(i) + yAxis * lineLength);

		gl::ScopedColor zColor( Color( 0, 1, 1 ) );
 		gl::vertex(mPositionsDebug.at(i));
 		gl::vertex(mPositionsDebug.at(i) + zAxis * lineLength);
	}
	gl::end();
}

CINDER_APP( interp1App, RendererGl, &interp1App::prepSettings )
