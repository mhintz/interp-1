#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class interp1App : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
};

void interp1App::setup()
{
}

void interp1App::mouseDown( MouseEvent event )
{
}

void interp1App::update()
{
}

void interp1App::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP( interp1App, RendererGl )
