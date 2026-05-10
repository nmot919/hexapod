#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Batch.h"
#include "cinder/CinderImGui.h"
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

using namespace ci;
using namespace ci::app;
using namespace std;

#define STARTING_Y 5.0f
#define NUM_LEGS 6

// allows you to set the leg joint angles manually independent from IK
#define DEBUG_JOINT_ANGLES 0


class HexapodApp : public App {
public:
    void setup()   override;
    void update()  override;
    void draw()    override;
    void keyDown(KeyEvent event) override;

private:
    void resetHexapod();
    void updateCamera();
    void drawScene();
    void drawUI();
    void drawBody();
    void drawLegs();
    void buildLegs();
    void drawTargetFootPositions();
    void drawAxes(float size, vec3 origin);

    CameraPersp  mCam;
    gl::BatchRef mBox;
    gl::BatchRef mPlane;

    std::vector<Leg> mLegs;

    vec3  mBoxPos        = vec3(0, STARTING_Y, 0);
    vec3  mBoxVelocity   = vec3(0, 0, 0);
    float mGravity       = -9.8f;
    float mRestitution   = 0.2f;
    float mBoxSize       = 0.5f;
    float mPlaneSize     = 100.0f;

    float mCamDistance   = 6.0f;
    float mCamHeight     = 5.0f;
    float mCamAngle      = 45.0f;

    // default angles
    float mCoxaAngle = 0.0f, mFemurAngle = 0.0f, mTibiaAngle = 90.0f;

    // default joint lengths
    //vec3 mLengths = vec3(0.1f, 0.1f, 0.5f);
    //vec3 mLengths = vec3(1);
    vec3 mLengths = vec3(0.25f, 0.25f, 1.0f);

    // used for leg placement, when making body class make these constants of that
    float rx = 0.3f, rz = 0.5f;
    float angles[6] = {0, 30, 150, 180, 210, 330};

    bool  mDrawGui       = true;
};

void HexapodApp::resetHexapod(){
    mBoxPos = vec3(0, STARTING_Y, 0);
}


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


void HexapodApp::buildLegs(){
    for(int i = 0; i < NUM_LEGS; i++){
        float a = toRadians(angles[i]);

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

void HexapodApp::drawBody(){
    gl::ScopedModelMatrix m;
    gl::translate(mBoxPos);
    gl::scale(vec3(0.75f, 0.10f, 1.0f));
    mBox->draw();
}

void HexapodApp::drawLegs(){
  for(int i = 0; i < 6; i++){
    mLegs[i].draw(mBoxPos);
  }
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
    mBox   = gl::Batch::create(geom::Cube().size(vec3(mBoxSize)) >> blue, shader);

    mCam.setPerspective(45.f, getWindowAspectRatio(), 0.001f, 100.f);
    updateCamera();

    buildLegs();
}

void HexapodApp::update() {
    float dt = 1.0f / getFrameRate();

    mBoxVelocity.y += mGravity * dt;
    mBoxPos        += mBoxVelocity * dt;

    // existing floor bounce for the body box
    if (mBoxPos.y - mBoxSize / 2 < 0) {
        mBoxPos.y      = mBoxSize / 2;
        mBoxVelocity.y *= -mRestitution;
    }

    for(Leg& l : mLegs){

        #if DEBUG_JOINT_ANGLES
        l.setAngles(vec3(mCoxaAngle, mFemurAngle, mTibiaAngle));
        #endif

        l.setLengths(mLengths);        // update attach points before checking feet
    }

    for(Leg& l : mLegs){
        vec3 fp = l.getFootWorldPos(mBoxPos);
        if(fp.y < 0){
            mBoxPos.y += abs(fp.y);
            mBoxVelocity.y = glm::max(0.0f, mBoxVelocity.y);
        }
    }

}

void HexapodApp::drawTargetFootPositions(){
    auto shader = gl::getStockShader(gl::ShaderDef().lambert().color());
    auto red    = geom::Constant(geom::COLOR, Color(1.0f, 0.2f, 0.2f));
    auto dot    = gl::Batch::create(geom::Sphere().radius(0.04f).subdivisions(8) >> red, shader);

    for(Leg& l : mLegs){
        gl::ScopedModelMatrix m;
        vec3 worldPos = mBoxPos + l.getTargetFootPos();  // ← remove getLocalOffset()
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
    drawAxes(2.0, vec3(-2, 0, -2));
    drawBody();
    drawLegs();
    drawTargetFootPositions();
}



void HexapodApp::drawUI() {
    if (!mDrawGui) return;

    ImGui::Begin("Controls");

    if (ImGui::CollapsingHeader("Hexapod", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::Button("Reset"))  resetHexapod();
        ImGui::Separator();
        if(ImGui::CollapsingHeader("Debug")){



          bool bodyChanged = false;
          bodyChanged |= ImGui::SliderFloat("rx", &rx, 0.1f, 0.9f);
          bodyChanged |= ImGui::SliderFloat("rz", &rz, 0.1f, 0.9f);
          bodyChanged |= ImGui::SliderFloat("Angle[0]", &angles[0], 0.0f, 360.0f);
          bodyChanged |= ImGui::SliderFloat("Angle[1]", &angles[1], 0.0f, 360.0f);
          bodyChanged |= ImGui::SliderFloat("Angle[2]", &angles[2], 0.0f, 360.0f);
          bodyChanged |= ImGui::SliderFloat("Angle[3]", &angles[3], 0.0f, 360.0f);
          bodyChanged |= ImGui::SliderFloat("Angle[4]", &angles[4], 0.0f, 360.0f);
          bodyChanged |= ImGui::SliderFloat("Angle[5]", &angles[5], 0.0f, 360.0f);

          if (bodyChanged) {
              mLegs.clear();
              buildLegs();
          }
        }
        ImGui::Separator();

        #if DEBUG_JOINT_ANGLES
        if(ImGui::CollapsingHeader("Joint Angles", ImGuiTreeNodeFlags_DefaultOpen)){
          ImGui::SliderFloat("Coxa Angle", &mCoxaAngle, 0, 180.0f);
          ImGui::SliderFloat("Femur Angle", &mFemurAngle, 0, 180.0f);
          ImGui::SliderFloat("Tibia Angle", &mTibiaAngle, 0, 180.0f);
        }
        #endif

        ImGui::Separator();
        if(ImGui::CollapsingHeader("Joint Lengths")){
          ImGui::SliderFloat("Coxa Length", &mLengths.x, 0.1f, 1.0f);
          ImGui::SliderFloat("Femur Length", &mLengths.y, 0.1f, 1.0f);
          ImGui::SliderFloat("Tibia Length", &mLengths.z, 0.1f, 1.0f);
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
    ImGui::Text("Pos: %.2f  %.2f  %.2f",
            mBoxPos.x, mBoxPos.y, mBoxPos.z);
    ImGui::Text("Vel: %.2f  %.2f  %.2f",
            mBoxVelocity.x, mBoxVelocity.y, mBoxVelocity.z);
    ImGui::Separator();
    if(ImGui::CollapsingHeader("Foot Positions")){
      ImGui::Text("World Coordinates:");
      for(int i = 0; i < mLegs.size(); i++){
          vec3 p = mLegs[i].getFootWorldPos(mBoxPos);
          ImGui::Text("Foot[%d] Position: %.2f %.2f %.2f", i, p.x, p.y, p.z);
      }
      ImGui::Separator();
      ImGui::Text("Local Coordinates:");
      for(int i = 0; i < mLegs.size(); i++){
          vec3 attachWorld = mBoxPos + mLegs[i].getLocalOffset();
          vec3 world = mLegs[i].getFootWorldPos(mBoxPos);
          vec3 local = world - attachWorld;
          ImGui::Text("Foot[%d] Position: %.2f %.2f %.2f", i, local.x, local.y, local.z);
      }
    }
    ImGui::Separator();
    if(ImGui::CollapsingHeader("Target Foot Positions")){
    for(int i = 0; i < mLegs.size(); i++){
        vec3 target = mLegs[i].getTargetFootPos();
        char label[32];
        snprintf(label, sizeof(label), "Foot[%d]:", i);
        if(ImGui::DragFloat3(label, &target, 0.05f, 0, 10.0f)){
            mLegs[i].setTargetFootPos(target);
        }
    }
}

if(ImGui::CollapsingHeader("IK Debug")){
    for(int i = 0; i < mLegs.size(); i++){
        ImGui::Text("--- Leg %d ---", i);
        vec3 a = mLegs[i].getAngles();
        ImGui::Text("Angles (deg): %.2f %.2f %.2f", a.x, a.y, a.z);
        vec3 t = mLegs[i].getTargetFootPos();
        ImGui::Text("Target (body-center): %.2f %.2f %.2f", t.x, t.y, t.z);
        vec3 world = mLegs[i].getFootWorldPos(mBoxPos);
        vec3 bodyCenter = world - mBoxPos;  // body-center-relative, matches how targets are stored
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
        resetHexapod();
}

CINDER_APP(HexapodApp, RendererGl, [](App::Settings* settings) {
    settings->setWindowSize(1200, 800);
    settings->setTitle("Hexapod Sim");
})
