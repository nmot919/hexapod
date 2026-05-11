#pragma once
#include "cinder/gl/gl.h"
#include "cinder/gl/Batch.h"
#include <cinder/CinderMath.h>
#include <cinder/Color.h>
#include <cinder/GeomIo.h>
#include <cinder/Vector.h>
#include <glm/fwd.hpp>
#include "Leg.h"

#ifndef NUM_LEGS
#define NUM_LEGS 6
#endif

using namespace ci;
using namespace std;

class HexapodBody{

public:

HexapodBody();
HexapodBody(vec3 startingPos);

void draw();
void reset();

vec3 getPosition();
void setPosition(vec3 p);
void setStartingPos(vec3 p);

vec3 getVelocity();
void setVelocity(vec3 v);

float getBodySize();


std::vector<Leg> getLegs();
void setLegLength(vec3 l);
void setStartingLegLength(vec3 l);
vec3 getLegLength();

vec3 getLegTarget(int i);
void setLegTarget(int i, vec3 target);
void setLegTarget(std::array<vec3, NUM_LEGS> targets);

private:

void drawBody();
void drawLegs();
void buildLegs();

float rx = 0.3f, rz = 0.5f; // used to generate leg positions in buildLegs()
float legAngles[6] = {0, 30, 150, 180, 210, 330};
vec3 startingLegLength = vec3(0.25f, 0.25f, 1.0f);
vec3 legLength = startingLegLength; // coxa, femur, tibia


vec3 mStartingPos = vec3(0);
float mBodySize        = 0.5;
vec3  mBodyPos;
vec3  mBodyVelocity   = vec3(0);
gl::BatchRef mBody;
std::vector<Leg> mLegs;

};
