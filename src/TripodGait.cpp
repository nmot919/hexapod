#include "TripodGait.h"
#include <array>
#include <glm/gtx/rotate_vector.hpp>





std::array<vec3, NUM_LEGS> TripodGait::getGaitTarget(float dt){
    if(glm::length(mMoveDir) < 0.01f && abs(mTurnRate) < 0.01f)
        return mRestTargets;


    mGaitPhase = fmod(mGaitPhase + dt / mStepDuration, 1.0f);

    updateGroup(true, mGaitPhase);
    updateGroup(false, fmod(mGaitPhase + 0.5f, 1.0f));

    return mCurrentTargets;
}

// this is ai generated lol, didnt wanna bother actually making it
void TripodGait::updateGroup(bool group, float phase){

    for(int i = 0; i < NUM_LEGS/2; i++){
        int idx = group ? groupA[i] : groupB[i];

        vec3 rest = mRestTargets[idx];

        vec3 tangent = glm::normalize(vec3(rest.z, 0, -rest.x)) * mTurnRate;
        vec3 stride = mMoveDir * 0.4f + tangent * 0.4f;

        if(phase < 0.5f){
            float t = phase / 0.5f;

            vec3 pos   = glm::mix(rest - stride, rest + stride, t);

            pos.y += mStepHeight * sin(t * M_PI);

            mCurrentTargets[idx] = pos;
        } else {
            // stance phase — foot pushes back against ground
            float t = (phase - 0.5f) / 0.5f;  // 0->1 within stance
            mCurrentTargets[idx] = glm::mix(rest + stride, rest - stride, t);
            mCurrentTargets[idx].y = mRestTargets[idx].y; // keep grounded
        }
    }
}

void TripodGait::setMoveDir(vec3 moveDir){
    this->mMoveDir = moveDir;
}

vec3 TripodGait::getMoveDir(){
    return this->mMoveDir;
}

void TripodGait::setTurnRate(float tr){
    this->mTurnRate = tr;
}

float TripodGait::getTurnRate(){
    return this->mTurnRate;
}

void TripodGait::setRestTargets(std::array<vec3, NUM_LEGS> t){
    this->mRestTargets = t;
}

std::array<vec3, NUM_LEGS> TripodGait::getRestTargets(){
    return this->mRestTargets;
}
