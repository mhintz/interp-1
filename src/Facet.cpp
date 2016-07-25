#include "Facet.hpp"

using namespace ci;
using namespace ci::app;

Facet::Facet(vec3 position, quat orientation) : mNode(position, orientation, vec3(1)) {
	static geom::Plane facetGeom = geom::Plane();
	static gl::GlslProgRef facetShader = gl::GlslProg::create(loadResource(VS_FACET_SHADER), loadResource(FS_FACET_SHADER));
	auto facetColor = geom::Constant(geom::COLOR, Color(CM_RGB, randFloat(0.4), randFloat(0.4, 0.8), randFloat(0.4, 0.8)));
	mBatch = gl::Batch::create(facetGeom >> facetColor, facetShader);
}

void Facet::update(vec3 p, quat o, vec3 s) {
	mNode.setAll(p, o, s);
}

void Facet::draw() {
	gl::ScopedModelMatrix modelMat;

	gl::translate(mNode.position());
	gl::rotate(mNode.orientation());
	gl::scale(mNode.scale());

	mBatch->draw();
}