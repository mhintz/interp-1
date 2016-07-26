#include "Facet.hpp"

using namespace ci;

Facet::Facet(vec3 position, quat orientation, gl::GlslProgRef facetShader) : mNode(position, orientation, vec3(1)) {
	static geom::Plane facetGeom = geom::Plane();
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