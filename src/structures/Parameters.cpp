#include "Parameters.h"

Parameters::Parameters( ){
      
   /*************************************/
   /*              General              */
   /*************************************/
   mouse_down = 0;
   timed_pause = 0;
   view_mode = 0;
   
   /*************************************/
   /*        Window Parameters          */
   /*************************************/
   glut_window_x = 700;
   glut_window_y = 700;
   glut_initial_x = 40;
   glut_initial_y = 5;
   glut_window_name = "Plinko";

   /*************************************/
   /*              Viewport             */
   /*************************************/
   fova  = 20.0;
   ratio = 1.0;
   zNear = 0.25;
   zFar  = 5.0;

   projectionMatrix = Perspective(fova, ratio, zNear, zFar);

   /*************************************/
   /*               Camera              */
   /*************************************/
   // cam_mode = CAMERA_Y_LOCK_BOTH;
   cam_mode = CAMERA_Y_LOCK_VERT;
   // cam_mode = CAMERA_FREE;
   cam_lookStep = 1.0;
   cam_moveStep = 0.06;
   cam_timerStep = 10;

   /*************************************/
   /*            Game Objects           */
   /*************************************/
   board = NULL;
   pegs = NULL;
   puck = NULL;
      
   /*************************************/
   /*           Player Info             */
   /*************************************/
   name = "Marvin";
   wins = 0;
   score = 0;
      
   /*************************************/
   /*               HUD                 */
   /*************************************/
   winner = NULL;      // TODO I don't know what this is
      
   /*************************************/
   /*          Physical World           */
   /*************************************/
   light_timerStep = 10;
   light_moveStep = 0.06;
   world_timerStep = 17;
   puck_x_axis = 0;

}

Parameters::~Parameters(){

   if( board ) delete board;
   if( pegs )  delete pegs;
   if( puck )  delete puck;
}

