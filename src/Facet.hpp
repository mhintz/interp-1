#pragma once

#include "cinder/gl/gl.h"
#include "cinder/GeomIo.h"
#include "cinder/Rand.h"

#include "Node.hpp"

using namespace ci;

class Facet;
typedef std::unique_ptr<Facet> FacetBox;
typedef std::shared_ptr<Facet> FacetRef;

class Facet {
public:
	Facet() {};
	Facet(vec3 position, quat orientation, gl::GlslProgRef facetShader);

	static FacetRef create(vec3 position, quat orientation, gl::GlslProgRef facetShader) { return FacetRef(new Facet(position, orientation, facetShader)); }

	void draw();
	void update(vec3 p, quat o, vec3 s);

private:
	Node mNode;
	gl::BatchRef mBatch;
};