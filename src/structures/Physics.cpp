#include "Physics.h"

// diagram of pegs and board
/*

   y -2.0    -1.5    -1.0    -0.5     0.0     0.5     1.0     1.5     2.0
x
0.0    |                             (0,0) <--- origin                 |
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

InitParams::InitParams()
 : boardFriction(0.1f),
   boardRestitution(0.3f),
   pegFriction(0.3f),
   pegRestitution(0.5f),
   puckMass(4.0f),
   puckFriction(0.1f),
   puckRestitution(0.8f)
{}
   
Physics_Model::Physics_Model()
 : m_collisionConfiguration(NULL),
   m_dispatcher(NULL),
   m_broadphase(NULL),
   m_solver(NULL),
   m_dynamicsWorld(NULL),
   m_boardShape(NULL),
   m_puckShape(NULL),
   m_pegShape(NULL),
   m_boardRigidBody(NULL),
   m_puckRigidBody(NULL),
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
 
// local function (assumes shape has just been allocated)
void loadTriMesh(btTriangleMesh* mesh_bullet, const vec4* mesh_vec4, const int mesh_points)
{
   vec3 a0, a1, a2;
   for(int i=0; i<mesh_points; i+=3){

      a0 = vec3( mesh_vec4[i+0].x, mesh_vec4[i+0].y, mesh_vec4[i+0].z );
      a1 = vec3( mesh_vec4[i+1].x, mesh_vec4[i+1].y, mesh_vec4[i+1].z );
      a2 = vec3( mesh_vec4[i+2].x, mesh_vec4[i+2].y, mesh_vec4[i+2].z );

      // For whatever your source of triangles is
      //   give the three points of each triangle:
      btVector3 v0(a0.x,a0.y,a0.z);
      btVector3 v1(a1.x,a1.y,a1.z);
      btVector3 v2(a2.x,a2.y,a2.z);

      // Then add the triangle to the mesh:
      mesh_bullet->addTriangle(v0,v1,v2);
   }
}

void Physics_Model::init( const vec4* boardVertices, const int boardVertexCount, const InitParams& parameters )
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
      // load board geometry into triangle mesh object
      btTriangleMesh *mTriMesh = new btTriangleMesh();
      loadTriMesh(mTriMesh, boardVertices, boardVertexCount);

      // create collision info from triangle mesh
      m_boardShape = new btBvhTriangleMeshShape(mTriMesh,true);
      btVector3 localInertia(0, 0, 0);
      m_collisionShapes.push_back(m_boardShape);   // for deleting later

      // transform board so that top top is centered and front lays on XZ plane
      m_boardTransform.setIdentity();
      m_boardTransform.setOrigin(btVector3(0.0, 0.0, 0.0));

      // construct motion state and rigid body
      btDefaultMotionState* myMotionState = new btDefaultMotionState(m_boardTransform);
      btRigidBody::btRigidBodyConstructionInfo rbInfo(0,myMotionState,m_boardShape,localInertia);
      rbInfo.m_friction = m_parameters.boardFriction;
      rbInfo.m_restitution = m_parameters.boardRestitution;
      m_boardRigidBody = new btRigidBody(rbInfo);

      // add board to dynamics world
      m_dynamicsWorld->addRigidBody(m_boardRigidBody);
   }

   /// pegs ///
   {
      // building pegs out of cylinder shapes for more accurate physics simulation
      // pegs are 0.1 diameter with 1.0 height around Z axis
      m_pegShape = new btCylinderShapeZ(btVector3(0.05,0.05,0.5));
      
      btVector3 localInertia(0, 0, 0);
      m_collisionShapes.push_back(m_pegShape);     // for deleting later

      // set peg locations (see diagram at top)
      for ( float row = -1.0; row > -6.1; row -= 1.0 )
      {
         for ( float col = -1.5; col < 1.1; col += 0.5 )
         {
            /// add peg to world ///
            {
               m_pegTransforms.push_back(btTransform());
               m_pegTransforms.back().setIdentity();
               m_pegTransforms.back().setOrigin(btVector3(col, row, 0.4));

               // construct motion state and rigid body then add body to dynamics world
               btDefaultMotionState* myMotionState = new btDefaultMotionState(m_pegTransforms.back());
               btRigidBody::btRigidBodyConstructionInfo rbInfo(0, myMotionState, m_pegShape, localInertia);
               rbInfo.m_friction = m_parameters.pegFriction;
               rbInfo.m_restitution = m_parameters.pegRestitution;
               m_pegRigidBodies.push_back(new btRigidBody(rbInfo));
               m_dynamicsWorld->addRigidBody(m_pegRigidBodies.back());
            }

            /// add peg to world on next row ///
            {
               m_pegTransforms.push_back(btTransform());
               m_pegTransforms.back().setIdentity();
               m_pegTransforms.back().setOrigin(btVector3(col+0.25, row-0.5, 0.4));

               // construct motion state and rigid body then add body to dynamics world
               btDefaultMotionState* myMotionState = new btDefaultMotionState(m_pegTransforms.back());
               btRigidBody::btRigidBodyConstructionInfo rbInfo(0, myMotionState, m_pegShape, localInertia);
               rbInfo.m_friction = m_parameters.pegFriction;
               rbInfo.m_restitution = m_parameters.pegRestitution;
               m_pegRigidBodies.push_back(new btRigidBody(rbInfo));
               m_dynamicsWorld->addRigidBody(m_pegRigidBodies.back());
            }
         }
         
         /// add last peg on far right without a pair ///
         {
            m_pegTransforms.push_back(btTransform());
            m_pegTransforms.back().setIdentity();
            m_pegTransforms.back().setOrigin(btVector3(1.5, row, 0.4));

            // construct motion state and rigid body then add body to dynamics world
            btDefaultMotionState* myMotionState = new btDefaultMotionState(m_pegTransforms.back());
            btRigidBody::btRigidBodyConstructionInfo rbInfo(0, myMotionState, m_pegShape, localInertia);
            rbInfo.m_friction = m_parameters.pegFriction;
            rbInfo.m_restitution = m_parameters.pegRestitution;
            m_pegRigidBodies.push_back(new btRigidBody(rbInfo));
            m_dynamicsWorld->addRigidBody(m_pegRigidBodies.back());
         }
      }
   }

   /// puck ///
   {
      // build puck
      m_puckShape = new btCylinderShapeZ(btVector3(0.18, 0.18, 0.1));
      btVector3 localInertia(0.0, 0.0, 0.0);
      m_puckShape->calculateLocalInertia(m_parameters.puckMass, localInertia); 
      m_collisionShapes.push_back(m_puckShape);     // for deleting later

      m_puckTransform.setIdentity();
      m_puckTransform.setOrigin(btVector3(0.1, 0.0, 0.3));
      //m_puckTransform.setRotation(btQuaternion(0.0, M_PI_2, 0.0));

      // construct motion state and rigid body then add body to dynamics world
      btDefaultMotionState* myMotionState = new btDefaultMotionState(m_puckTransform);
      btRigidBody::btRigidBodyConstructionInfo rbInfo(m_parameters.puckMass, myMotionState, m_puckShape, localInertia);
      rbInfo.m_friction = m_parameters.puckFriction;
      rbInfo.m_restitution = m_parameters.puckRestitution;
      m_puckRigidBody = new btRigidBody(rbInfo);
      m_dynamicsWorld->addRigidBody(m_puckRigidBody);
      
      // puck uses added constraints
      m_puckRigidBody->setActivationState(DISABLE_DEACTIVATION);
      m_puckRigidBody->setLinearFactor(btVector3(1,1,0));

      btTransform frameInB = btTransform::getIdentity();
      frameInB.setOrigin(btVector3(0.0, 0.0, 0.01));
      m_puckXYplaneConstraint = new btGeneric6DofConstraint(*m_puckRigidBody, frameInB, true);
      
      // lowerlimit = upperlimit --> axis locked
      // lowerlimit < upperlimit --> motion limited between values
      // lowerlimit > upperlimit --> axis is free

      // lock the Z axis movement
      m_puckXYplaneConstraint->setLinearLowerLimit(btVector3(1,1,0));
      m_puckXYplaneConstraint->setLinearUpperLimit(btVector3(0,0,0));

      // lock the X, Y, rotations
      m_puckXYplaneConstraint->setAngularLowerLimit(btVector3(0,0,1));
      m_puckXYplaneConstraint->setAngularUpperLimit(btVector3(0,0,0));

      m_dynamicsWorld->addConstraint(m_puckXYplaneConstraint);
   }
}

void Physics_Model::stepSimulation(btScalar time)
{
   m_dynamicsWorld->stepSimulation(time,10);
   
   // update puck location
   m_puckRigidBody->getMotionState()->getWorldTransform(m_puckTransform);
}

vec3 Physics_Model::getPuckTranslation() const
{
   return vec3(m_puckTransform.getOrigin().getX(),
               m_puckTransform.getOrigin().getY(),
               m_puckTransform.getOrigin().getZ());
}

mat4 Physics_Model::getPuckTranslationMat() const
{
   return mat4(1.0, 0.0, 0.0, m_puckTransform.getOrigin().getX(),
               0.0, 1.0, 0.0, m_puckTransform.getOrigin().getY(),
               0.0, 0.0, 1.0, m_puckTransform.getOrigin().getZ(),
               0.0, 0.0, 0.0, 1.0);
}

mat4 Physics_Model::getPuckRotation() const
{
   btMatrix3x3 rotation = m_puckTransform.getBasis();
   
   return mat4(rotation[0][0],rotation[0][1],rotation[0][2],0.0,
               rotation[1][0],rotation[1][1],rotation[1][2],0.0,
               rotation[2][0],rotation[2][1],rotation[2][2],0.0,
               0.0,           0.0,           0.0,           1.0);
}

mat4 Physics_Model::getPuckTransform() const
{
   return getPuckTranslationMat()*getPuckRotation();
}

std::vector<mat4> Physics_Model::getPegTransforms() const
{
   std::vector<mat4> transforms(m_pegTransforms.size());

   for ( size_t i = 0; i < m_pegTransforms.size(); ++i )
   {
      btMatrix3x3 rotation = m_pegTransforms[i].getBasis();
      
      transforms[i] =
          mat4(1.0, 0.0, 0.0, m_pegTransforms[i].getOrigin().getX(),
               0.0, 1.0, 0.0, m_pegTransforms[i].getOrigin().getY(),
               0.0, 0.0, 1.0, m_pegTransforms[i].getOrigin().getZ(),
               0.0, 0.0, 0.0, 1.0);
   
      
      transforms[i] *= mat4(rotation[0][0],rotation[0][1],rotation[0][2],0.0,
                            rotation[1][0],rotation[1][1],rotation[1][2],0.0,
                            rotation[2][0],rotation[2][1],rotation[2][2],0.0,
                            0.0,           0.0,           0.0,           1.0);
   }

   return transforms;
}

void Physics_Model::setPuckTranslation(const vec3& trans)
{
   m_puckRigidBody->setCenterOfMassTransform(
      btTransform(
         m_puckTransform.getRotation(),
         btVector3(trans.x,trans.y,trans.z)));
   
   // update puckTransform
   m_puckTransform = m_puckRigidBody->getCenterOfMassTransform();
}

void Physics_Model::adjPuckTranslation(const vec3& trans)
{
   m_puckRigidBody->setCenterOfMassTransform(
      btTransform(
         m_puckTransform.getRotation(),
         m_puckTransform.getOrigin()+btVector3(trans.x,trans.y,trans.z)));
   
   // update puckTransform
   m_puckTransform = m_puckRigidBody->getCenterOfMassTransform();
}

void Physics_Model::setPuckRotation(const vec3& rot)
{
   m_puckRigidBody->setCenterOfMassTransform(
      btTransform(
         btQuaternion(rot.x,rot.y,rot.z),
         m_puckTransform.getOrigin()));
  
   // update puckTransform
   m_puckTransform = m_puckRigidBody->getCenterOfMassTransform();
}

