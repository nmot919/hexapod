#include "HexapodBody.h"

HexapodBody::HexapodBody(){

    auto shader = gl::getStockShader(gl::ShaderDef().lambert().color());
    auto blue   = geom::Constant(geom::COLOR, Color(0.3f, 0.6f, 1.0f));
    mBody   = gl::Batch::create(geom::Cube().size(vec3(mBodySize)) >> blue, shader);
    buildLegs();
}


vec3 HexapodBody::getLegLength(){
    return this->legLength;
}

void HexapodBody::setLegLength(vec3 l){
    this->legLength = l;
}

std::vector<Leg> HexapodBody::getLegs(){
    return mLegs;
}


void HexapodBody::buildLegs(){
    for(int i = 0; i < NUM_LEGS; i++){
        float a = toRadians(legAngles[i]);

        vec3 attach = vec3(rx * sin(a), 0, rz * cos(a));

        // femur 120 degrees, tibia 80 degrees
        vec3 targets[] = {vec3(0, -0.69, 1.33), vec3(0.42, -0.69, 1.22), vec3(0.42, -0.69, -1.22), vec3(0, -0.69, -1.33), vec3(-0.42, -0.69, -1.22), vec3(-0.42, -0.69, 1.22)};

        // outDir should point away from body-center along the actual attach direction
        vec3 out = normalize(vec3(attach.x, 0, attach.z));

        Leg leg(out);
        leg.setLocalOffset(attach);
        leg.setTargetFootPos(targets[i]);
        mLegs.push_back(leg);
    }
}

void HexapodBody::drawLegs(){
  for(int i = 0; i < NUM_LEGS; i++){
    mLegs[i].draw(mBodyPos);
  }
}

void HexapodBody::drawBody(){
    gl::ScopedModelMatrix m;
    gl::translate(mBodyPos);
    gl::scale(vec3(0.75f, 0.10f, 1.0f));
    mBody->draw();
}


void HexapodBody::draw(){
    drawBody();
    drawLegs();
}

void HexapodBody::reset(){
    this->mBodyPos = mStartingPos;
    this->mBodyVelocity = vec3(0);
    buildLegs();
}

vec3 HexapodBody::getPosition(){
    return this->mBodyPos;
}

void HexapodBody::setPosition(vec3 p){
    this->mBodyPos = p;
}

vec3 HexapodBody::getVelocity(){
    return this->mBodyVelocity;
}

void HexapodBody::setVelocity(vec3 v){
    this->mBodyVelocity = v;
}
