#ifndef __SRC_STRUCTURES_PHYSICS_H__
#define __SRC_STRUCTURES_PHYSICS_H__

// bullet physics engine
#include <btBulletDynamicsCommon.h>

// Was trying to use this for triangle paddle
//#include <bullet/BulletCollision/Gimpact/btGImpactShape.h>

#include <vector>

#include "Object.h"

// ../math
#include "vec.h"
#include "mat.h"

struct InitParams{
      InitParams();

      // XXX shallow copy is okay

      // assumes location on the Z-coord is zero
      // initial puck position
      vec2 puckPos;

      // geometric properties of object MODELS
      vec2  splineWidthHeight;  // width=x height=y
      vec2  boardWidthHeight;   // width=x height=y
      float puckDiameter;
      float pegDiameter;

      // static (mass = 0)
      btScalar boardFriction;
      btScalar boardRestitution;

      // static (mass = 0)
      btScalar pegFriction;
      btScalar pegRestitution;

      // static (mass = 0)
      btScalar splineFriction;
      btScalar splineRestitution;
      
      btScalar puckMass;
      btScalar puckFriction;
      btScalar puckRestitution;
};

class Physics_Model{
   
   public: 
      Physics_Model();
      ~Physics_Model(); 

      void stepSimulation(btScalar time);
     
      void init(const InitParams& params = InitParams());

      vec3 getPuckTranslation() const;
      vec3 getBoardTranslation() const;
      std::vector<vec3> getPegTranslations() const;
      std::vector<vec3> getSplineTranslations() const;

      vec3 getPuckRotation() const;
      vec3 getBoardRotation() const;
      std::vector<vec3> getPegRotations() const;
      std::vector<vec3> getSplineRotations() const;

      vec3 getPuckScale() const;
      vec3 getBoardScale() const;
      std::vector<vec3> getPegScales() const;
      std::vector<vec3> getSplineScales() const;

      mat4 getPuckTransform() const;
      mat4 getBoardTransform() const;
      std::vector<mat4> getPegTranforms() const;
      std::vector<mat4> getSplineTransforms() const;
      
      // only able to change dynamic objects (so only the puck)
      void setPuckTranslation(const vec3& trans);
      void setPuckRotation(const vec3& rot);
   protected:

      /*************************************************************/
      /*                       Physics Base                        */
      /*************************************************************/
      btDefaultCollisionConfiguration     *m_collisionConfiguration;
      btCollisionDispatcher               *m_dispatcher;
      btBroadphaseInterface               *m_broadphase;
      btSequentialImpulseConstraintSolver *m_solver;
      btDiscreteDynamicsWorld             *m_dynamicsWorld;

      /*************************************************************/
      /*                          Bodies                           */
      /*************************************************************/
      // holds a list of all items in the world
      btAlignedObjectArray<btCollisionShape*> m_collisionShapes;

      // Note: Splines make up the boarders and seperate prize cubbys.

      btCollisionShape                    *m_board;   // static
      btCollisionShape                    *m_peg;     // static
      btCollisionShape                    *m_spline;  // static
      btCollisionShape                    *m_puck;    // dynamic

      btTransform                         m_boardTransform;
      std::vector<btTransform>            m_pegTransforms;
      std::vector<btTransform>            m_splines;
      btTransform                         m_puckTransform;

      btGeneric6DofConstraint             *m_puckXYplaneConstraint;

      /************************************************************/
      /*                         Parameters                       */
      /************************************************************/
      
      InitParams m_parameters;
};

#endif
