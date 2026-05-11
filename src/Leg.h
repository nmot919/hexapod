#pragma once
#include "cinder/gl/gl.h"
#include "cinder/gl/Batch.h"
#include <cinder/CinderMath.h>
#include <cinder/Color.h>
#include <cinder/GeomIo.h>
#include <cinder/Vector.h>

using namespace ci;
using namespace std;




class Leg {
  public:
    Leg(vec3 outDir);
    void setTargetFootPos(vec3 targetFootPos);
    vec3 getTargetFootPos();
    void setLocalOffset(vec3 localOffset);
    vec3 getLocalOffset();
    void setAngles(vec3 angleDeg);
    vec3 getAngles();
    void setLengths(vec3 lengths);
    vec3 getLengths();
    vec3 getFootWorldPos(vec3 bodyPos);
    void draw(vec3 bodyPos);



  private:
    // coxa, femur, tibia
    glm::vec3 angles;
    glm::vec3 lengths;
    glm::vec3 outDir;
    glm::vec3 targetFootPos; // target foot position in local space
    glm::vec3 localOffset; // offset from center of body to leg in local position

    gl::BatchRef mCoxa;
    gl::BatchRef mFemur;
    gl::BatchRef mTibia;
    gl::BatchRef mFoot;

    void rebuildBatches(); // for rebuilding legs when they change lengths
    void IK_Solver();
};
