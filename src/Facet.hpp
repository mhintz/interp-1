#pragma once

#include "cinder/gl/gl.h"
#include "cinder/GeomIo.h"
#include "cinder/Rand.h"

#include "Resources.h"

#include "Node.hpp"

class Facet;
typedef std::unique_ptr<Facet> FacetBox;
typedef std::shared_ptr<Facet> FacetRef;

class Facet {
public:
	Facet() {};
	Facet(vec3 position, quat orientation);

	static FacetRef create(vec3 position, quat orientation) { return FacetRef(new Facet(position, orientation)); }

	void draw();
	void update(vec3 p, quat o, vec3 s);

private:
	Node mNode;
	gl::BatchRef mBatch;
};