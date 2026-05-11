#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Batch.h"
#include "cinder/CinderImGui.h"
#include <array>
#include <cinder/CinderMath.h>
#include <cinder/Color.h>
#include <cinder/GeomIo.h>
#include <cinder/Vector.h>
#include <cinder/gl/scoped.h>
#include <cinder/gl/wrapper.h>
#include <cmath>
#include <cstdio>
#include <glm/fwd.hpp>
#include <glm/trigonometric.hpp>
#include <imgui/imgui.h>
#include <vector>
#include "Leg.h"
#include "HexapodBody.h"
#include "TripodGait.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define STARTING_Y 5.0f



class HexapodApp : public App {
public:
    void setup()   override;
    void update()  override;
    void draw()    override;
    void keyDown(KeyEvent event) override;

private:
    void updateCamera();
    void drawScene();
    void drawUI();
    void drawTargetFootPositions();
    void drawAxes(float size, vec3 origin);

    CameraPersp  mCam;
    gl::BatchRef mPlane;

    HexapodBody mHexapodBody;
    TripodGait mTripodGait;

    vec3  mMoveDir = vec3(0);
    float mGravity       = -9.8f;
    float mRestitution   = 0.2f;
    float mPlaneSize     = 100.0f;

    float mCamDistance   = 6.0f;
    float mCamHeight     = 5.0f;
    float mCamAngle      = 45.0f;

    bool  mDrawGui       = true;
};


void HexapodApp::drawAxes(float size, vec3 origin) {
    gl::lineWidth(2.0f);
    // x z ground plane, y height plane
    // X - red
    gl::color(1, 0, 0);
    gl::drawLine(vec3(origin), vec3(origin.x + size, origin.y, origin.z));
    // Y - green
    gl::color(0, 1, 0);
    gl::drawLine(origin, vec3(origin.x, origin.y + size, origin.z));
    // Z - blue
    gl::color(0, 0, 1);
    gl::drawLine(origin, vec3(origin.x, origin.y, origin.z + size));
}



void HexapodApp::updateCamera() {
    float rad = glm::radians(mCamAngle);
    vec3 eye = vec3(
        mCamDistance * cos(rad),
        mCamHeight,
        mCamDistance * sin(rad)
    );
    mCam.lookAt(eye, vec3(0, 0.5f, 0));
}

void HexapodApp::setup() {
    ImGui::Initialize(ImGui::Options().window(getWindow()));

    auto shader = gl::getStockShader(gl::ShaderDef().lambert().color());
    auto blue   = geom::Constant(geom::COLOR, Color(0.3f, 0.6f, 1.0f));
    auto gray   = geom::Constant(geom::COLOR, Color(0.2f, 0.22f, 0.25f));

    mPlane = gl::Batch::create(geom::Plane().size(vec2(mPlaneSize)) >> gray, shader);
    mCam.setPerspective(45.f, getWindowAspectRatio(), 0.001f, 100.f);
    updateCamera();

    mHexapodBody = HexapodBody(vec3(0, STARTING_Y, 0));
}

void HexapodApp::update() { // used to simulate time and physics
    float dt = 1.0f / getFrameRate();

    vec3 vel = mHexapodBody.getVelocity();
    vec3 pos = mHexapodBody.getPosition();
    float bodySize = mHexapodBody.getBodySize();

    vel.y += mGravity * dt;
    pos += vel * dt;

    if(pos.y - bodySize / 2 < 0){
        pos.y = bodySize / 2;
        vel.y *= -mRestitution;
    }


    /*for(Leg& l : mLegs){
        l.setLengths(mLengths);        // update attach points before checking feet
    }*/

    for(Leg& l : mHexapodBody.getLegs()){
        vec3 fp = l.getFootWorldPos(pos);
        if(fp.y < 0){
            pos.y += abs(fp.y);
            vel.y = glm::max(0.0f, vel.y);
        }
    }

    mHexapodBody.setPosition(pos);
    mHexapodBody.setVelocity(vel);

    std::array<vec3, NUM_LEGS> target = mTripodGait.getGaitTarget(dt);
    mHexapodBody.setLegTarget(target);

}

void HexapodApp::drawTargetFootPositions(){
    auto shader = gl::getStockShader(gl::ShaderDef().lambert().color());
    auto red    = geom::Constant(geom::COLOR, Color(1.0f, 0.2f, 0.2f));
    auto dot    = gl::Batch::create(geom::Sphere().radius(0.04f).subdivisions(8) >> red, shader);

    for(Leg& l : mHexapodBody.getLegs()){
        gl::ScopedModelMatrix m;
        vec3 worldPos = mHexapodBody.getPosition() + l.getTargetFootPos();
        gl::translate(worldPos);

        for(int i = 0; i < 10; i++){
            dot->draw();
            gl::translate(vec3(0, float(i * 0.1f), 0));
        }
    }
}


void HexapodApp::drawScene() {
    gl::enableDepthRead();
    gl::enableDepthWrite();
    gl::setMatrices(mCam);

    mPlane->draw();
    //drawAxes(2.0, vec3(-2, 0, -2));
    mHexapodBody.draw();
    //drawTargetFootPositions();
}



void HexapodApp::drawUI() {
    if (!mDrawGui) return;

    ImGui::Begin("Controls");

    if (ImGui::CollapsingHeader("Hexapod", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::Button("Reset")) mHexapodBody.reset();

        if(ImGui::DragFloat3("MoveDir", &mMoveDir, 0.01f, -5.0f, 5.0f)){
            mTripodGait.setMoveDir(mMoveDir);
        }
        vec3 pos = mHexapodBody.getPosition();
        vec3 vel = mHexapodBody.getVelocity();

        ImGui::Text("Body Position: %.2f  %.2f  %.2f",
            pos.x, pos.y, pos.z);
        ImGui::Text("Body Velocity: %.2f  %.2f  %.2f",
            vel.x, vel.y, vel.z);

        ImGui::Separator();
        if(ImGui::CollapsingHeader("Joint Lengths")){
            vec3 mLengths = mHexapodBody.getLegLength();
          bool changed = false;
        changed |= ImGui::SliderFloat("Coxa Length",  &mLengths.x, 0.1f, 1.0f);
        changed |= ImGui::SliderFloat("Femur Length", &mLengths.y, 0.1f, 1.0f);
        changed |= ImGui::SliderFloat("Tibia Length", &mLengths.z, 0.1f, 1.0f);

        if (changed)
            mHexapodBody.setLegLength(mLengths);
        }

        ImGui::Separator();

    }

    if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
        bool changed = false;
        changed |= ImGui::SliderFloat("Distance", &mCamDistance, 1.0f,   20.0f);
        changed |= ImGui::SliderFloat("Height",   &mCamHeight,   0.1f,   10.0f);
        changed |= ImGui::SliderFloat("Angle",    &mCamAngle,    0.0f,  360.0f);
        if (changed) updateCamera();
    }

    ImGui::Separator();
    if(ImGui::CollapsingHeader("Target Foot Positions")){
    vector<Leg> legs = mHexapodBody.getLegs();
    for(int i = 0; i < legs.size(); i++){
        vec3 target = legs[i].getTargetFootPos();
        char label[32];
        snprintf(label, sizeof(label), "Foot[%d]:", i);
        if(ImGui::DragFloat3(label, &target, 0.01f, -5.0f, 5.0f)){
            mHexapodBody.setLegTarget(i, target);
        }
    }
}

if(ImGui::CollapsingHeader("IK Debug")){

    vector<Leg> legs = mHexapodBody.getLegs();
    vec3 bodyPos = mHexapodBody.getPosition();

    for(int i = 0; i < legs.size(); i++){
        ImGui::Text("--- Leg %d ---", i);
        vec3 a = legs[i].getAngles();
        ImGui::Text("Angles (deg): %.2f %.2f %.2f", a.x, a.y, a.z);
        vec3 t = mHexapodBody.getLegTarget(i);
        ImGui::Text("Target (body-center): %.2f %.2f %.2f", t.x, t.y, t.z);
        vec3 world = legs[i].getFootWorldPos(bodyPos);
        vec3 bodyCenter = world - bodyPos;  // body-center-relative, matches how targets are stored
        ImGui::Text("FK result (body-center): %.2f %.2f %.2f", bodyCenter.x, bodyCenter.y, bodyCenter.z);
        ImGui::Text("Error: %.4f", glm::length(bodyCenter - t));
        ImGui::Separator();
    }
}


    ImGui::Text("Ctrl+G - toggle GUI");

    ImGui::End();
}

void HexapodApp::draw() {
    gl::clear(Color(0.08f, 0.09f, 0.11f));
    drawScene();
    drawUI();
}

void HexapodApp::keyDown(KeyEvent event) {
    if (event.getChar() == 'g' && event.isAccelDown())
        mDrawGui = !mDrawGui;
    if (event.getCode() == KeyEvent::KEY_r)
        mHexapodBody.reset();
}

CINDER_APP(HexapodApp, RendererGl, [](App::Settings* settings) {
    settings->setWindowSize(1200, 800);
    settings->setTitle("Hexapod Sim");
})
