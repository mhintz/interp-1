#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Arcball.h"
#include "cinder/Rand.h"
#include "cinder/Camera.h"
#include "cinder/CameraUi.h"

#include "FacetSpline.hpp"

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

	void drawFramesDebug();

	CameraPersp mCamera;
	CameraUi mUiCamera;

	vector<FacetSplineRef> mFacetSplines;
};

void interp1App::prepSettings(Settings * settings) {
	settings->setFullScreen();
	settings->setTitle("Interpretation 1");
	settings->setHighDensityDisplayEnabled();
}

void interp1App::setup()
{
	mCamera.lookAt(vec3(0, 0, 200), vec3(0));
	mUiCamera = CameraUi(&mCamera, getWindow());

	int numSplines = 20;
	float splinesInc = 2.0 * M_PI / (float) numSplines;
	for (int cnt = 0; cnt < numSplines; cnt++) {
	    float angle = (float) cnt * splinesInc;
	    vec3 splineOrigin = 5.0f * vec3(cosf(angle), sinf(angle), 0.0f);
	    vec3 splineDirection = normalize(vec3(cosf(angle) / 2.0, sinf(angle) / 2.0, 1.0f));
	    mFacetSplines.push_back(FacetSpline::create(splineOrigin, splineDirection));
	}
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
	for (FacetSplineRef const & facetSpline : mFacetSplines) {
		facetSpline->update();
	}
}

void interp1App::draw()
{
	gl::enableDepth();

	gl::clear( Color( 0, 0, 0.15 ), true );

	gl::setMatrices(mCamera);

	for (FacetSplineRef const & facetSpline : mFacetSplines) {
		facetSpline->draw();
	}
}

CINDER_APP( interp1App, RendererGl, &interp1App::prepSettings )
