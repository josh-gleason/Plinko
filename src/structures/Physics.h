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

      // static (mass = 0)
      btScalar boardFriction;
      btScalar boardRestitution;

      // static (mass = 0)
      btScalar pegFriction;
      btScalar pegRestitution;
      
      btScalar puckMass;
      btScalar puckFriction;
      btScalar puckRestitution;
};

class Physics_Model{
   
   public: 
      Physics_Model();
      ~Physics_Model(); 

      void stepSimulation(btScalar time);
     
      void init( const vec4* boardVertices, const int boardVertexCount,
                 const InitParams& params = InitParams());

      // only care about rigid transformations (no scale)
      mat4 getPuckTranslationMat() const;
      vec3 getPuckTranslation() const;
      mat4 getPuckRotation() const;    // returns as quarternian x,y,z,w

      // returns the matrix transformation for translation and rotation
      mat4 getPuckTransform() const;
      
      // only able to change dynamic objects (so only the puck)
      void setPuckTranslation(const vec3& trans);
      void setPuckRotation(const vec3& rot);
      void adjPuckTranslation(const vec3& trans);

      std::vector<mat4> getPegTransforms() const;
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

      btCollisionShape                    *m_boardShape;   // static
      btCollisionShape                    *m_puckShape;    // dynamic
      btCollisionShape                    *m_pegShape;     // static

      btRigidBody                         *m_boardRigidBody;
      btRigidBody                         *m_puckRigidBody;
      std::vector<btRigidBody*>           m_pegRigidBodies;

      btTransform                         m_boardTransform;
      btTransform                         m_puckTransform;
      std::vector<btTransform>            m_pegTransforms;

      btGeneric6DofConstraint             *m_puckXYplaneConstraint;

      /************************************************************/
      /*                         Parameters                       */
      /************************************************************/
      
      InitParams m_parameters;
};

#endif
