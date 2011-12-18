#include "Physics.h"

// diagram of pegs and board
/*

   y -2.0    -1.5    -1.0    -0.5     0.0     0.5     1.0     1.5     2.0
 x
0.0    |                                                               |
       |           P       L       I       N       K       O           |
0.5    |                                                               |
       |                                                               |
1.0    |_      o       o       o       o       o       o       o      _|
         \_                                                         _/
1.5       _}       o       o       o       o       o       o       {_
        _/                                                           \_
2.0    {_      o       o       o       o       o       o       o      _}
         \_                                                         _/
2.5       _}       o       o       o       o       o       o       {_
        _/                                                           \_

       .................................................................
       .........................pattern.repeats.........................
       .................................................................

         \_                                                         _/
6.5        |       o       o       o       o       o       o       |
           |                                                       |
7.0        |       |       |       |       |       |       |       |
           |       |       |       |       |       |       |       |
7.5        |       |       |       |       |       |       |       |
           | $500  | $1000 |  $0   |$10,000|  $0   | $1000 | $500  |
8.0        +-------+-------+-------+-------+-------+-------+-------+
     -2.0    -1.5    -1.0    -0.5     0.0     0.5     1.0     1.5     2.0

*/

// TODO: Check the geometric properties against the models,
//       they should match otherwise collisions won't match
//       what is being displayed. The geometric properties
//       are used when building the transformations that need
//       to be passed to the objects for displaying.
InitParams::InitParams()
 : puckPos(0.f,0.f),
   splineWidthHeight(1.f,1.f),
   boardWidthHeight(4.f,8.f),
   puckDiameter(1.f),
   pegDiameter(1.f),
   boardFriction(0.01f),
   boardRestitution(0.01f),
   pegFriction(0.01f),
   pegRestitution(0.9f),
   splineFriction(0.01f),
   splineRestitution(0.9f),
   puckMass(7.0f),
   puckFriction(0.01f),
   puckRestitution(0.9f)
{}
   
Physics_Model::Physics_Model()
 : m_collisionConfiguration(NULL),
   m_dispatcher(NULL),
   m_broadphase(NULL),
   m_solver(NULL),
   m_dynamicsWorld(NULL),
   m_boardShape(NULL),
   m_pegShape(NULL),
   m_splineShape(NULL),
   m_puckShape(NULL),
   m_puckXYplaneConstraint(NULL)
{}

Physics_Model::~Physics_Model()
{
   // remove the rigidbodies from the dynamics world and
   // delete them and their motion states
   if ( m_dynamicsWorld ) {
      for ( int i = m_dynamicsWorld->getNumCollisionObjects()-1; i >= 0; --i ) {
         btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
         btRigidBody* body = btRigidBody::upcast(obj);
         if ( body && body->getMotionState() )
            delete body->getMotionState();
         m_dynamicsWorld->removeCollisionObject(obj);
         delete obj;
      }
   }

   // delete collision shapes (assumes all shapes were pushed to m_collisionShapes
   for ( int j = 0; j < m_collisionShapes.size(); ++j )
   {
      btCollisionShape* shape = m_collisionShapes[j];
      m_collisionShapes[j] = 0;
      delete shape;
   }

   if ( m_dynamicsWorld ) delete m_dynamicsWorld;
   if ( m_solver ) delete m_solver;
   if ( m_broadphase ) delete m_broadphase;
   if ( m_dispatcher ) delete m_dispatcher;
   if ( m_collisionConfiguration ) delete m_collisionConfiguration;

   m_collisionShapes.clear();

   if ( m_puckXYplaneConstraint ) delete m_puckXYplaneConstraint;
}

void Physics_Model::init( const InitParams& parameters )
{
   m_parameters = parameters;
   
   /******************************************/
   /*       General Physics Environment      */
   /******************************************/
   //create options (leave as default)
   m_collisionConfiguration = new btDefaultCollisionConfiguration();

   //create collision checker
   m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);

   //create Broadphase collision checker
   m_broadphase = new btDbvtBroadphase();

   //the default constraint solver. (leave as default)
   m_solver = new btSequentialImpulseConstraintSolver;

   //create dynamic environment 
   m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfiguration);

   //set value of gravity
   m_dynamicsWorld->setGravity(btVector3(0,-10,0));

   /********************************************/
   /*                  Bodies                  */
   /********************************************/

   /// board ///
   {
      // board is 4x8 (function takes 1/2 extents)
      m_boardShape = new btBoxShape(btVector3(2.0, 4.0, 0.05));
      btVector3 localInertia(0, 0, 0);
      m_collisionShapes.push_back(m_boardShape);   // for deleting later

      // transform board so that top top is centered and front lays on XZ plane
      m_boardTransform.setIdentity();
      m_boardTransform.setOrigin(btVector3(0.0, -4.0, -0.05));

      btDefaultMotionState* myMotionState = new btDefaultMotionState(m_boardTransform);
      btRigidBody::btRigidBodyConstructionInfo rbInfo(0,myMotionState,m_boardShape,localInertia);
      btRigidBody* body = new btRigidBody(rbInfo);

      // add board to dynamics world
      m_dynamicsWorld->addRigidBody(body);
   }

   /// pegs ///
   {
      // pegs are 0.1 diameter with 1.0 height around Z axis
      m_pegShape = new btCylinderShapeZ(btVector3(0.05,0.5,0.05));   // XXX not sure which is correct
      //m_pegShape = new btCylinderShapeZ(btVector3(0.05,0.05,0.5)); // XXX
      btVector3 localInertia(0, 0, 0);
      m_collisionShapes.push_back(m_pegShape);     // for deleting later

      // set peg locations (see diagram at top)
      for ( float row = 1.0; row < 6.1; row += 1.0 )
      {
         for ( float col = -1.5; col < 1.1; col += 0.5 )
         {
            /// add peg to world ///
            {
               m_pegTransforms.push_back(btTransform());
               m_pegTransforms.back().setIdentity();
               m_pegTransforms.back().setOrigin(btVector3(col, row, 0.4));

               btDefaultMotionState* myMotionState = new btDefaultMotionState(m_pegTransforms.back());
               btRigidBody::btRigidBodyConstructionInfo rbInfo(0,myMotionState,m_pegShape,localInertia);
               btRigidBody* body = new btRigidBody(rbInfo);
               m_dynamicsWorld->addRigidBody(body);
            }

            /// add peg to world on next row ///
            {
               m_pegTransforms.push_back(btTransform());
               m_pegTransforms.back().setIdentity();
               m_pegTransforms.back().setOrigin(btVector3(col+0.25, row+0.5, 0.4));

               btDefaultMotionState* myMotionState = new btDefaultMotionState(m_pegTransforms.back());
               btRigidBody::btRigidBodyConstructionInfo rbInfo(0,myMotionState,m_pegShape,localInertia);
               btRigidBody* body = new btRigidBody(rbInfo);
               m_dynamicsWorld->addRigidBody(body);
            }
         }
         
         /// add last peg on far right without a pair ///
         {
            m_pegTransforms.push_back(btTransform());
            m_pegTransforms.back().setIdentity();
            m_pegTransforms.back().setOrigin(btVector3(1.5, row, 0.4));

            btDefaultMotionState* myMotionState = new btDefaultMotionState(m_pegTransforms.back());
            btRigidBody::btRigidBodyConstructionInfo rbInfo(0,myMotionState,m_pegShape,localInertia);
            btRigidBody* body = new btRigidBody(rbInfo);
            m_dynamicsWorld->addRigidBody(body);
         }
      }
   }

   /// splines ///
   {

   }

   /// puck ///
   {

   }
}

#if 0
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
   dispatcher = new btCollisionDispatcher(collisionConfiguration);

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
#endif
