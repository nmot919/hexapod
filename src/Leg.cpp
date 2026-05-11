#include "Leg.h"
#include <cinder/CinderMath.h>
#include <glm/common.hpp>

    Leg::Leg(vec3 outDir) : outDir(outDir){
      // starting angles should be calculated from outDir to stand straight up
      angles = vec3(toRadians(90.0f), toRadians(120.0f), toRadians(180.0f));

      rebuildBatches();
    }

    // targetFootPos is in local coordinates, maybe make private in the future.
    void Leg::setTargetFootPos(vec3 targetFootPos){
      this->targetFootPos = targetFootPos;
    }

    vec3 Leg::getTargetFootPos(){return targetFootPos;}

    void Leg::setLocalOffset(vec3 localOffset){
      this->localOffset = localOffset;
    }

    vec3 Leg::getLocalOffset(){return localOffset;}

    // takes inputs as degrees since those are easier to visualize but uses radians for internal math because it's easier to do
    void Leg::setAngles(vec3 angleDeg){angles = vec3(toRadians(angleDeg.x), toRadians(angleDeg.y), toRadians(angleDeg.z));}

    vec3 Leg::getAngles(){return vec3(toDegrees(angles.x), toDegrees(angles.y), toDegrees(angles.z));}

    void Leg::setLengths(vec3 lengths){
      if(this->lengths != lengths){
        this->lengths = lengths;
        rebuildBatches();
      }
    }

    vec3 Leg::getLengths(){return lengths;}

    vec3 Leg::getFootWorldPos(vec3 bodyPos){
      glm::mat4 m = glm::mat4(1.0f);

      // move to mount point
      m = glm::translate(m, localOffset + bodyPos);
      m = glm::rotate(m, float(atan2(outDir.x, outDir.z) + angles.x - (M_PI/2.0f)), vec3(0, 1, 0));

      // move to tip of coxa
      m = glm::translate(m, vec3(lengths.x, 0, 0));
      m = glm::rotate(m, float(angles.y), vec3(0, 0, 1));

      // move to tip of femur
      m = glm::translate(m, vec3(0, -lengths.y, 0));
      m = glm::rotate(m, float(angles.z + (M_PI)), vec3(0, 0, 1));

      // move to tip of tibia (in other words to the foot)
      m = glm::translate(m, vec3(0, -lengths.z, 0));

      // return the x/y/z vector components from the translation
      return vec3(m[3]);
    }

    void Leg::draw(vec3 bodyPos){

      // don't forget that changing this may mean that getFootWorldPos has to be changed as

      IK_Solver();

      gl::ScopedModelMatrix root;
      gl::translate(bodyPos + localOffset);
      //gl::rotate(-atan2(outDir.z, outDir.x) + angles.x - (M_PI / 2.0), vec3(0, 1, 0));
      gl::rotate(atan2(outDir.x, outDir.z) - (M_PI / 2.0) + angles.x, vec3(0, 1, 0));

      {
        // move to coxa draw point and draw coxa
        gl::ScopedModelMatrix m;
        gl::translate(vec3(lengths.x * 0.5f, 0, 0));
        mCoxa->draw();
      }

      // move to tip of coxa
      gl::translate(vec3(lengths.x, 0, 0));
      gl::rotate(angles.y, vec3(0, 0, 1));

      {
        // move to femur draw point and draw femur
        // length being negative shouldnt affect the rest of the system, just makes the drawing correct
          gl::ScopedModelMatrix m;
          gl::translate(vec3(0, -lengths.y * 0.5f, 0));
          mFemur->draw();
      }

      // move to tip of femur
      gl::translate(vec3(0, -lengths.y, 0));
      //gl::rotate(-angles.z + (M_PI / 2.0), vec3(0, 0, 1));
      gl::rotate(angles.z + (M_PI), vec3(0, 0, 1));


      {
        // move to tibia draw point and draw tibia
          gl::ScopedModelMatrix m;
          gl::translate(vec3(0, -lengths.z * 0.5, 0));
          mTibia->draw();
      }

      // move to tip of tibia to draw foot
      gl::translate(vec3(0, -lengths.z, 0));
      mFoot->draw();
    }


    // takes target foot position which is in local coordinates and computes the required angle coxa, femur, and tibia angles required
    // to reach that foot position
    void Leg::IK_Solver(){

  vec3 t = targetFootPos - localOffset;
  float targetAngle = atan2(t.x, t.z);
  float baseAngle = atan2(outDir.x, outDir.z);
  angles.x = targetAngle - baseAngle;

  float horizontal = sqrt(t.x*t.x + t.z*t.z) - lengths.x;
  float L = sqrt(t.y*t.y + horizontal*horizontal);
  float maxReach = lengths.y + lengths.z;
  float minReach = abs(lengths.y - lengths.z);
  if(L > maxReach) L = maxReach - 0.005f;
  if(L < minReach) L = minReach + 0.005f;

  float alpha1 = acos(-t.y/L);
  float alpha2 = acos((lengths.z*lengths.z - lengths.y*lengths.y - L*L)/(-2*lengths.y*L));
  float alpha  = alpha1 + alpha2;

  float beta = acos((L*L - lengths.z*lengths.z - lengths.y*lengths.y)/(-2*lengths.z*lengths.y));

  angles.y = alpha;
  angles.z = beta;
}


    void Leg::rebuildBatches(){
      auto shader = gl::getStockShader(gl::ShaderDef().lambert().color());
      auto blue   = geom::Constant(geom::COLOR, Color(0.3f, 0.6f, 1.0f));
      auto yellow = geom::Constant(geom::COLOR, Color(1, 1, 0));

      mCoxa  = gl::Batch::create(geom::Cube().size(vec3(lengths.x, 0.03, 0.03)) >> blue, shader);
      mFemur = gl::Batch::create(geom::Cube().size(vec3(0.03, lengths.y, 0.03)) >> blue, shader);
      mTibia = gl::Batch::create(geom::Cube().size(vec3(0.03, lengths.z, 0.03)) >> blue, shader);
      mFoot  = gl::Batch::create(geom::Sphere().radius(0.03).subdivisions(8) >> yellow, shader);
    }
