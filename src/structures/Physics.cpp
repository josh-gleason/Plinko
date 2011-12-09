#include "Physics.h"

Physics_Model::Physics_Model(){
   
   shift_x_pos1 = 0;
   shift_x_neg1 = 0;
   shift_x_pos2 = 0;
   shift_x_neg2 = 0;
   shift_z_pos1 = 0;
   shift_z_neg1 = 0;
   shift_z_pos2 = 0;
   shift_z_neg2 = 0;

   action = 0;

   puck_mass = 7.0;
   puck_friction = 0.01;
   puck_restitution = 0.9;

   pdl_mass = 15.0;
   pdl_friction = 0.01;
   pdl_restitution = 0.9;

   board_friction = 0.5;
   board_restitution = 0.9;
   
   tri1_walls = NULL;
   tri2_walls = NULL;

   boardShape = NULL;
   puckShape = NULL;
   paddle1Shape = NULL;
   paddle2Shape = NULL;
   paddle1Circle = NULL;
   paddle2Circle = NULL;
   paddle1Square = NULL;
   paddle2Square = NULL;
   paddle1Triangle = NULL;
   paddle2Triangle = NULL;
   puckRigidBody = NULL;
   boardRigidBody = NULL;
   paddle1RigidBody = NULL;
   paddle2RigidBody = NULL;
   puckXZplaneConstraint = NULL;
   pdl1XZplaneConstraint = NULL;
   pdl2XZplaneConstraint = NULL;
   collisionConfiguration = NULL;
   dispatcher = NULL;
   overlappingPairCache = NULL;
   solver = NULL;
   boardMotionState = NULL;
   puckMotionState = NULL;
   paddle1MotionState = NULL;
   paddle2MotionState = NULL;
   dynamicsWorld = NULL;
}

Physics_Model::~Physics_Model()
{
   if ( tri1_walls ) delete tri1_walls;
   if ( tri2_walls ) delete tri2_walls;

   if ( boardShape ) delete boardShape;
   if ( puckShape )  delete puckShape;
   //  delete [] paddle1Shape;   // these are pointers to other memory
   //  delete [] paddle2Shape;
   if ( paddle1Circle )  delete paddle1Circle;
   if ( paddle2Circle )  delete paddle2Circle;
   if ( paddle1Square)   delete paddle1Square;
   if ( paddle2Square)   delete paddle2Square;
   if ( paddle1Triangle) delete paddle1Triangle;
   if ( paddle2Triangle) delete paddle2Triangle;

   if ( puckRigidBody ) delete puckRigidBody;
   if ( boardRigidBody ) delete boardRigidBody;
   if ( paddle1RigidBody ) delete paddle1RigidBody;
   if ( paddle2RigidBody ) delete paddle2RigidBody;

   if ( puckXZplaneConstraint ) delete puckXZplaneConstraint;
   if ( pdl1XZplaneConstraint ) delete pdl1XZplaneConstraint;
   if ( pdl2XZplaneConstraint ) delete pdl2XZplaneConstraint;

   if ( collisionConfiguration ) delete collisionConfiguration;
   if ( dispatcher ) delete dispatcher;
   if ( overlappingPairCache ) delete overlappingPairCache;
   if ( solver ) delete solver;
   
   if ( boardMotionState ) delete boardMotionState;
   if ( puckMotionState ) delete puckMotionState;
   if ( paddle1MotionState ) delete paddle1MotionState;
   if ( paddle2MotionState ) delete paddle2MotionState;
   
   // deleting dynamics world breaks things
}

void Physics_Model::init( vec3 const& boardSize, vec2 const& puckSize, vec2 const& paddle1Size, vec2 const& paddle2Size, 
                          vec3 const& boardCent, vec3 const& puckCent, vec3 const& paddle1Cent, vec3 const& paddle2Cent, 
                          vec4 const* board_points,  const size_t num_boardPoints, 
                          vec4 const* puck_points,   const size_t num_puckPoints, 
                          vec4 const* paddle_points, const size_t num_paddlePoints ){

   /******************************************/
   /*       General Physics Environment      */
   /******************************************/
   //create options (leave as default)
   collisionConfiguration = new btDefaultCollisionConfiguration();

   //create collision checker
   dispatcher = new	btCollisionDispatcher(collisionConfiguration);

   //create Broadphase collision checker
   overlappingPairCache = new btDbvtBroadphase();

   ///the default constraint solver. (leave as default)
   solver = new btSequentialImpulseConstraintSolver;

   ///create dynamic environment 
   dynamicsWorld = new btDiscreteDynamicsWorld( dispatcher, overlappingPairCache, solver, collisionConfiguration);

   ///set value of gravity
   dynamicsWorld->setGravity(btVector3(0,-10,0));

   /******************************************/
   /*                   Board                */
   /******************************************/
   btTriangleMesh *mTriMesh = new btTriangleMesh();

   vec3 a0, a1, a2;
   for(size_t i=0; i<num_boardPoints; i+=3){

      a0 = vec3( board_points[i+0].x, board_points[i+0].y, board_points[i+0].z );
      a1 = vec3( board_points[i+1].x, board_points[i+1].y, board_points[i+1].z );
      a2 = vec3( board_points[i+2].x, board_points[i+2].y, board_points[i+2].z );

      // For whatever your source of triangles is
      //   give the three points of each triangle:
      btVector3 v0(a0.x,a0.y,a0.z);
      btVector3 v1(a1.x,a1.y,a1.z);
      btVector3 v2(a2.x,a2.y,a2.z);

      // Then add the triangle to the mesh:
      mTriMesh->addTriangle(v0,v1,v2);
   }

   btCollisionShape *boardShape = new btBvhTriangleMeshShape(mTriMesh,true);

   // Now use mTriMeshShape as your collision shape.
   // Everything else is like a normal rigid body

   //build motion state (BOARD)
   boardMotionState = new btDefaultMotionState( btTransform(btQuaternion(0,0,0,1),btVector3( 0, -0.1,0)));

   btRigidBody::btRigidBodyConstructionInfo boardRigidBodyCI( 0, boardMotionState, boardShape, btVector3(0,0,0));
   boardRigidBodyCI.m_friction = board_friction;     //this is the friction of its surfaces
   boardRigidBodyCI.m_restitution = board_restitution;     //this is the "bouncyness"

   boardRigidBody = new btRigidBody( boardRigidBodyCI);

   dynamicsWorld->addRigidBody( boardRigidBody );

   /******************************************/
   /*                    Puck                */
   /******************************************/
   ///build puck collision model
   puckShape = new btCylinderShape(btVector3(btScalar(puckSize.x),btScalar(0.1),btScalar(puckSize.x)));

   //build motion state (PUCK)
   puckMotionState = new btDefaultMotionState( btTransform(btQuaternion(0,0,0,1), btVector3( 0,0.0,0)));

   btVector3 puckInertia(0,0,0);
   puckShape->calculateLocalInertia( puck_mass, puckInertia);

   btRigidBody::btRigidBodyConstructionInfo puckRigidBodyCI( puck_mass, puckMotionState, puckShape, puckInertia);
   puckRigidBodyCI.m_friction = puck_friction;        //this is the friction of its surfaces
   puckRigidBodyCI.m_restitution = puck_restitution;  //this is the "bounciness"

   puckRigidBody = new btRigidBody( puckRigidBodyCI );
   dynamicsWorld->addRigidBody( puckRigidBody );

   puckRigidBody->setActivationState(DISABLE_DEACTIVATION);
   puckRigidBody->setLinearFactor(btVector3(1,0,1));

   {
      btTransform frameInB = btTransform::getIdentity();
      frameInB.setOrigin(btVector3(0.0,0.01,0.0));

      puckXZplaneConstraint = new btGeneric6DofConstraint( *puckRigidBody,frameInB, true );

      // lowerlimit = upperlimit --> axis locked
      // lowerlimit < upperlimit --> motion limited between values
      // lowerlimit > upperlimit --> axis is free

      // lock the Y axis movement
      puckXZplaneConstraint->setLinearLowerLimit( btVector3(1,0,1));
      puckXZplaneConstraint->setLinearUpperLimit( btVector3(0,0,0));

      // lock the X, Z, rotations
      puckXZplaneConstraint->setAngularLowerLimit(btVector3(0,1,0));
      puckXZplaneConstraint->setAngularUpperLimit(btVector3(0,0,0));

      dynamicsWorld->addConstraint(puckXZplaneConstraint);
   }

   /******************************************/
   /*                  Paddle1               */
   /******************************************/
   ///build paddle1 collision model
   paddle1Circle = new btCylinderShape(btVector3(btScalar(paddle1Size.x),btScalar(0.1),btScalar(paddle1Size.x)));

   paddle1Square = new btBoxShape(btVector3(btScalar(paddle1Size.x), btScalar(0.1), btScalar(paddle1Size.x)));

/////////////////////////////
   /*
   size_t num_pts_tri1 = tri1_walls->num_verts();
   vec4* vertices1 = tri1_walls->get_vertices();

   btTriangleMesh *mTriMesh_pdl1 = new btTriangleMesh();

   vec3 pt0, pt1, pt2;
   for(size_t i=0; i<num_pts_tri1; i+=3){

      pt0 = vec3( vertices1[i+0].x, vertices1[i+0].y, vertices1[i+0].z );
      pt1 = vec3( vertices1[i+1].x, vertices1[i+1].y, vertices1[i+1].z );
      pt2 = vec3( vertices1[i+2].x, vertices1[i+2].y, vertices1[i+2].z );

      // For whatever your source of triangles is
      //   give the three points of each triangle:
      btVector3 v0(pt0.x,pt0.y,pt0.z);
      btVector3 v1(pt1.x,pt1.y,pt1.z);
      btVector3 v2(pt2.x,pt2.y,pt2.z);

      // Then add the triangle to the mesh:
      mTriMesh_pdl1->addTriangle(v0,v1,v2);
   }

   paddle1Triangle = new btGImpactMeshShape(mTriMesh_pdl1);
   */
////////////////////////////////

   paddle1Shape = paddle1Circle;

   //build motion state (PADDLE1)
   paddle1MotionState = new btDefaultMotionState( btTransform(btQuaternion(0,0,0,1), btVector3( paddle1Cent.x,0.0, paddle1Cent.z)));

   btVector3 paddle1Inertia(0,0,0);
   paddle1Shape->calculateLocalInertia( pdl_mass, paddle1Inertia);

   btRigidBody::btRigidBodyConstructionInfo paddle1RigidBodyCI( pdl_mass, paddle1MotionState, paddle1Shape, paddle1Inertia);
   paddle1RigidBody = new btRigidBody( paddle1RigidBodyCI );
   paddle1RigidBodyCI.m_friction = pdl_friction;        //this is the friction of its surfaces
   paddle1RigidBodyCI.m_restitution = pdl_restitution;  //this is the "bounciness"
   
   dynamicsWorld->addRigidBody( paddle1RigidBody );

   paddle1RigidBody->setActivationState(DISABLE_DEACTIVATION);
   paddle1RigidBody->setLinearFactor(btVector3(1,0,1));
   
   {
      btTransform frameInB = btTransform::getIdentity();
      frameInB.setOrigin(btVector3(0.0,0.0,0.0));

      pdl1XZplaneConstraint = new btGeneric6DofConstraint( *paddle1RigidBody,frameInB, true );

      // lowerlimit = upperlimit --> axis locked
      // lowerlimit < upperlimit --> motion limited between values
      // lowerlimit > upperlimit --> axis is free

      // lock the Y axis movement
      pdl1XZplaneConstraint->setLinearLowerLimit( btVector3(1,0,1));
      pdl1XZplaneConstraint->setLinearUpperLimit( btVector3(0,0,0));

      // lock the X, Z, rotations
      pdl1XZplaneConstraint->setAngularLowerLimit(btVector3(0,1,0));
      pdl1XZplaneConstraint->setAngularUpperLimit(btVector3(0,0,0));

      dynamicsWorld->addConstraint(pdl1XZplaneConstraint);
   }


   /******************************************/
   /*                  Paddle2               */
   /******************************************/
   ///build paddle2 collision model
   paddle2Circle = new btCylinderShape(btVector3(btScalar(paddle2Size.x),btScalar(0.1),btScalar(paddle2Size.x)));

   paddle2Square = new btBoxShape(btVector3(btScalar(paddle2Size.x), btScalar(0.1), btScalar(paddle2Size.x)));

//////////////////////////////////////////////////////////////////
   /* 
   size_t num_pts_tri2 = tri2_walls->num_verts();
   vec4* vertices2 = tri2_walls->get_vertices();

   btTriangleMesh *mTriMesh_pdl2 = new btTriangleMesh();

   vec3 pt0_2, pt1_2, pt2_2;
   for(size_t i=0; i<num_pts_tri2; i+=3){

      pt0_2 = vec3( vertices2[i+0].x, vertices2[i+0].y, vertices2[i+0].z );
      pt1_2 = vec3( vertices2[i+1].x, vertices2[i+1].y, vertices2[i+1].z );
      pt2_2 = vec3( vertices2[i+2].x, vertices2[i+2].y, vertices2[i+2].z );

      // For whatever your source of triangles is
      //   give the three points of each triangle:
      btVector3 v0(pt0_2.x,pt0_2.y,pt0_2.z);
      btVector3 v1(pt1_2.x,pt1_2.y,pt1_2.z);
      btVector3 v2(pt2_2.x,pt2_2.y,pt2_2.z);

      // Then add the triangle to the mesh:
      mTriMesh_pdl2->addTriangle(v0,v1,v2);
   }

   paddle2Triangle = new btGImpactMeshShape(mTriMesh_pdl2);
   */
//////////////////////////////////////////////////////////////////

   paddle2Shape = paddle2Circle;

   //build motion state (PADDLE2)
   paddle2MotionState = new btDefaultMotionState( btTransform(btQuaternion(0,0,0,1), btVector3( paddle2Cent.x,0.0, paddle2Cent.z)));

   btVector3 paddle2Inertia(0,0,0);
   paddle2Shape->calculateLocalInertia( pdl_mass, paddle2Inertia);

   btRigidBody::btRigidBodyConstructionInfo paddle2RigidBodyCI( pdl_mass, paddle2MotionState, paddle2Shape, paddle2Inertia);
   paddle2RigidBody = new btRigidBody( paddle2RigidBodyCI );
   paddle2RigidBodyCI.m_friction = pdl_friction;       //this is the friction of its surfaces
   paddle2RigidBodyCI.m_restitution = pdl_restitution; //this is the "bounciness"
   
   dynamicsWorld->addRigidBody( paddle2RigidBody );

   paddle2RigidBody->setActivationState(DISABLE_DEACTIVATION);
   paddle2RigidBody->setLinearFactor(btVector3(1,0,1));
   
   {
      btTransform frameInB = btTransform::getIdentity();
      frameInB.setOrigin(btVector3(0.0,0.0,0.0));

      pdl2XZplaneConstraint = new btGeneric6DofConstraint( *paddle2RigidBody,frameInB, true );

      // lowerlimit = upperlimit --> axis locked
      // lowerlimit < upperlimit --> motion limited between values
      // lowerlimit > upperlimit --> axis is free

      // lock the Y axis movement
      pdl2XZplaneConstraint->setLinearLowerLimit( btVector3(1,0,1));
      pdl2XZplaneConstraint->setLinearUpperLimit( btVector3(0,0,0));

      // lock the X, Z, rotations
      pdl2XZplaneConstraint->setAngularLowerLimit(btVector3(0,1,0));
      pdl2XZplaneConstraint->setAngularUpperLimit(btVector3(0,0,0));

      dynamicsWorld->addConstraint(pdl2XZplaneConstraint);
   }


}

