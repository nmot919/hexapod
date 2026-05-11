#include "TripodGait.h"




std::array<vec3, NUM_LEGS> TripodGait::getGaitTarget(float dt){
    if(glm::length(mMoveDir) < 0.001f)
        return mRestTargets;


    mGaitPhase = fmod(mGaitPhase + dt / mStepDuration, 1.0f);


    updateGroup(true, mGaitPhase);
    updateGroup(false, fmod(mGaitPhase + 0.5f, 1.0f));

    return mCurrentTargets;
}

// this is ai generated lol, didnt wanna bother actually making it
void TripodGait::updateGroup(bool group, float phase){
    for(int i = 0; i < NUM_LEGS/2; i++){
        int idx;
        if(group){
            idx = groupA[i];
        } else{
            idx = groupB[i];
        }

        if(phase < 0.5f){
            float t = phase / 0.5f;

            vec3 stepTarget = mRestTargets[idx] + this->mMoveDir * 0.4f;

            // interpolate foot from behind to ahead
            vec3 start = mRestTargets[idx] - this->mMoveDir * 0.4f;
            vec3 pos   = glm::mix(start, stepTarget, t);

            // arc upward using a sine curve
            pos.y += mStepHeight * sin(t * M_PI);

            mCurrentTargets[idx] = pos;
        } else {
            // stance phase — foot pushes back against ground
            float t = (phase - 0.5f) / 0.5f;  // 0->1 within stance
            vec3 stepTarget = mRestTargets[idx] + this->mMoveDir * 0.4f;
            vec3 start      = mRestTargets[idx] - this->mMoveDir * 0.4f;
            mCurrentTargets[idx] = glm::mix(stepTarget, start, t);
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
