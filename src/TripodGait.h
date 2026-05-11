#pragma once
#include "cinder/gl/gl.h"
#include "cinder/gl/Batch.h"
#include <cinder/CinderMath.h>
#include <cinder/Color.h>
#include <cinder/GeomIo.h>
#include <cinder/Vector.h>
#include <glm/fwd.hpp>


#ifndef NUM_LEGS
#define NUM_LEGS 6
#endif


using namespace ci;
using namespace std;

class TripodGait{

public:

std::array<vec3, NUM_LEGS> getGaitTarget(float dt);
void setMoveDir(vec3 moveDir);
vec3 getMoveDir();

private:
int groupA[3] = {0, 2, 4};
int groupB[3] = {1, 3, 5};

float mGaitPhase    = 0.0f;
float mStepDuration = 0.5f;
float mStepHeight   = 0.3f;
vec3  mMoveDir = vec3(0);

std::array<vec3, NUM_LEGS>  mRestTargets = {vec3(0, -0.69, 1.33), vec3(0.42, -0.69, 1.22), vec3(0.42, -0.69, -1.22), vec3(0, -0.69, -1.33), vec3(-0.42, -0.69, -1.22), vec3(-0.42, -0.69, 1.22)};
std::array<vec3, NUM_LEGS>  mCurrentTargets;


void updateGroup(bool group, float phase);

};
