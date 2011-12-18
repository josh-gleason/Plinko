#include "mouse.h"

/**
  * window2world
  * 
  * A very simple function which takes the window parameters from 
  * glut and returns the world coordinate positions on the y=0
  * plane
  *
  * @param[in] x Glut x coordinate
  * @param[in] y Glut y coordinate
  * @return world coordinates
*/
static vec3 window2world( const int x, const int y ){

   /************************************************************/
   /*   Convert the mouse coordinates into World Coordinates   */
   /************************************************************/
   //retrieve the viewport data
   GLint viewport[4];
   glGetIntegerv(GL_VIEWPORT, viewport );

   const mat4 cam = options.camera.getTransform();

   //retrieve the modelview (and put it in column major)
   GLdouble modelview[16];
   for ( int i = 0; i < 4; ++i )
      for ( int j = 0; j < 4; ++j )
         modelview[i*4+j] = cam[j][i];

   //retieve the projection matrix (column major too)
   GLdouble projection[16];
   for ( int i = 0; i < 4; ++i )
      for ( int j = 0; j < 4; ++j )
         projection[i*4+j] = options.projectionMatrix[j][i];

   // invert y for better accuracy
   GLdouble wx = x;
   GLdouble wy = viewport[3] - y;

   // to get intersection with near zNear plane setting wz=0
   GLdouble wz = 0;

   //compute the world coordinates first point
   GLdouble nearX, nearY, nearZ;
   gluUnProject( wx, wy, wz, modelview, projection, viewport, &nearX, &nearY, &nearZ);
  
   // to get intersection with zFar plane setting wz=1
   wz = 1;

   //compute the world coordinates second point
   GLdouble farX, farY, farZ;
   gluUnProject( wx, wy, wz, modelview, projection, viewport, &farX, &farY, &farZ);

   // using line-plane intersection formula find intersection with y=0 (x-z plane)

   // d is the distance of the intersecting point from l0 in the direction l
  
   // I simplified it specifically for the y=0 plane
   // for general form uncomment the 3 lines below
   //    - Josh

   vec3 l0(nearX,nearY,nearZ); // a point on the line
   vec3 l = vec3(farX,farY,farZ) - l0; // vector in dir of line
   
   // vec3 n(0.0,1.0,0.0); // the norm of the plane
   // vec3 p0(0.0,0.0,0.0);   // a point on the plane y=0
   // GLfloat d = dot(p0-l0,n)/dot(l,n);

   GLfloat d = -nearY/(farY-nearY);

   return l0+l*d;
}

/**
 * function to handle mouse presses
 * @param button  the button pressed
 * @param state  the type of press (up or down)
 * @param x  the x coordinate of the press
 * @param y  the y coordinate of the press
 */
void mouseEvent(int button, int state, int x, int y) 
{
   // TODO
#if 0
   //Get the current mouse position in world coordinates
   vec3 world_points = window2world( x, y);
   
   options.set_paddle2_dest(vec2(world_points.x,world_points.z));
#endif 
}

/**
 * function to handle mouse movement
 * @param x  the x coordinate of the mouse
 * @param y  the y coordinate of the mouse
 */
void mouseMove(int x, int y)
{
   // TODO
   const double bound = 1.5;
   
   if( options.view_mode == 0 ){

      //Get the current mouse position in world coordinates
      vec4 world_points = window2world( x, y);
      
      //compare axis movement with previous x axis position
      double motion = world_points.x - options.puck_x_axis;
      
      if( options.puck->get_translation().x + motion > bound )
         motion = bound - options.puck->get_translation().x;
      
      if( options.puck->get_translation().x + motion < -bound )
         motion = -bound + options.puck->get_translation().x;
      
      options.puck->adjust_translation(vec3(motion,0,0));
      options.physics.adjPuckTranslation(vec3(motion,0,0));
      options.puck_x_axis = options.physics.getPuckTranslation().x;

   }
}

