#ifndef __SRC_STRUCTURES_PHYSICS_H__
#define __SRC_STRUCTURES_PHYSICS_H__

// bullet physics engine
#include <btBulletDynamicsCommon.h>

// Was trying to use this for triangle paddle
//#include <bullet/BulletCollision/Gimpact/btGImpactShape.h>

#include "vec.h"
#include "Object.h"

class Physics_Model{
   
   public:
      
      Physics_Model();
      ~Physics_Model(); 
      void init( vec3 const& boardSize, vec2 const& puckSize, vec2 const& paddle1Size, vec2 const& paddle2Size, 
                 vec3 const& boardCent, vec3 const& puckCent, vec3 const& paddle1Cent, vec3 const& paddle2Cent,
                 vec4 const* board_points,  const size_t num_boardPoints,      
                 vec4 const* puck_points,   const size_t num_puckPoints,      
                 vec4 const* paddle_points, const size_t num_paddlePoints);      

      Object* tri1_walls;
      Object* tri2_walls;

      btCollisionShape* boardShape;
      btCollisionShape* puckShape;
      btCollisionShape* paddle1Shape;
      btCollisionShape* paddle2Shape;

      btCollisionShape* paddle1Circle;
      btCollisionShape* paddle2Circle;
      btCollisionShape* paddle1Square;
      btCollisionShape* paddle2Square;
      btCollisionShape* paddle1Triangle;
      btCollisionShape* paddle2Triangle;

      btRigidBody* puckRigidBody;
      btRigidBody* boardRigidBody;
      btRigidBody* paddle1RigidBody;
      btRigidBody* paddle2RigidBody;
      btDiscreteDynamicsWorld* dynamicsWorld;
     
      btGeneric6DofConstraint *puckXZplaneConstraint;
      btGeneric6DofConstraint *pdl1XZplaneConstraint;
      btGeneric6DofConstraint *pdl2XZplaneConstraint;

      btTransform puck_trans;
      btTransform paddle1_trans;
      btTransform paddle2_trans;
      
      vec3 puck_pre_motion;
      vec3 paddle1_pre_motion;
      vec3 paddle2_pre_motion;
      vec3 puck_post_motion;
      vec3 paddle1_post_motion;
      vec3 paddle2_post_motion;
      
      ///collision configuration contains default setup for memory, collision setup.
      btDefaultCollisionConfiguration* collisionConfiguration;

      ///use the default collision dispatcher. 
      btCollisionDispatcher* dispatcher;

      ///btDbvtBroadphase is a good general purpose broadphase. 
      btBroadphaseInterface* overlappingPairCache;

      ///the default constraint solver. 
      btSequentialImpulseConstraintSolver* solver;

      btDefaultMotionState* boardMotionState;
      btDefaultMotionState* puckMotionState;
      btDefaultMotionState* paddle1MotionState;
      btDefaultMotionState* paddle2MotionState;

      btScalar board_friction;
      btScalar board_restitution;

      btScalar pdl_mass;
      btScalar pdl_friction;
      btScalar pdl_restitution;
      
      btScalar puck_mass;
      btScalar puck_friction;
      btScalar puck_restitution;

      int shift_z_pos1;
      int shift_z_neg1;
      int shift_z_pos2;
      int shift_z_neg2;
      int shift_x_pos1;
      int shift_x_neg1;
      int shift_x_pos2;
      int shift_x_neg2;

      int action;
};

void paddleTimer(int value);

#endif
