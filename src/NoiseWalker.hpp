#pragma once

#include "cinder/Rand.h"
#include "cinder/Perlin.h"

class NoiseWalker;
typedef std::unique_ptr<NoiseWalker> NoiseWalkerBox;
typedef std::shared_ptr<NoiseWalker> NoiseWalkerRef;

static const float noiseInitialOffset = 100.0f;
static const ci::Perlin noiseSampler = ci::Perlin();

class NoiseWalker {
public:
	NoiseWalker() : mPosition(randVec3() * noiseInitialOffset), mStep(randVec3() * 0.01f) {}
	NoiseWalker(float stepSize) : mPosition(randVec3() * noiseInitialOffset), mStep(randVec3() * stepSize) {}

	static NoiseWalkerRef create(float stepSize) { return NoiseWalkerRef(new NoiseWalker(stepSize)); }

	float step() { mPosition += mStep; return noiseSampler.fBm(mPosition); }
	float sampleAt(float dist) { return noiseSampler.fBm(mPosition + dist * mStep); }

private:
	vec3 mPosition;
	vec3 mStep;
};