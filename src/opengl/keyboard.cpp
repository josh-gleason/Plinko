/**
 * @file keyboard.cpp
 * @brief File containing keyboard processing
 * @author Marvin Smith
 */
#include "keyboard.h"
#include "Parameters.h"

struct CameraAction
{
   CameraAction() :
      rotateStraight(0),
      rotateVert(0),
      rotateHoriz(0),
      moveStraight(0),
      moveHoriz(0),
      moveVert(0),
      puckLeft(0),
      puckRight(0),
      keysPressed(0) {}

   int rotateStraight;
   int rotateVert;
   int rotateHoriz;
   int moveStraight;
   int moveHoriz;
   int moveVert;
   int puckLeft;
   int puckRight;

   // number of (meaningful) keys currently held down
   int keysPressed;
};

struct LightAction
{
   LightAction() : 
      move_x(0),
      move_z(0){}

   int move_x;
   int move_z;

   int keysPressed;
};

CameraAction action;
LightAction  laction;

void lightTimer(int value){

   if ( laction.move_x != 0 )
      options.light.move_x(laction.move_x*options.light_moveStep);
   if ( laction.move_z != 0 )
      options.light.move_z(laction.move_z*options.light_moveStep);
   
   if ( laction.keysPressed > 0 )
      glutTimerFunc( options.light_timerStep, lightTimer, value);

   glutPostRedisplay();
}

void cameraTimer(int value)
{
   if ( action.rotateStraight != 0 )
      options.camera.rotateStraight(action.rotateStraight*options.cam_lookStep);
   if ( action.rotateVert != 0 )
      options.camera.rotateVert(action.rotateVert*options.cam_lookStep);
   if ( action.rotateHoriz != 0 )
      options.camera.rotateHoriz(action.rotateHoriz*options.cam_lookStep);
   if ( action.moveStraight != 0 )
      options.camera.moveStraight(action.moveStraight*options.cam_moveStep);
   if ( action.moveHoriz != 0 )
      options.camera.moveHoriz(action.moveHoriz*options.cam_moveStep);
   if ( action.moveVert != 0 )
      options.camera.moveVert(action.moveVert*options.cam_moveStep);
   if ( action.puckLeft != 0 ){
      options.physics.adjPuckTranslation(vec3(-0.01,0,0));
      options.puck->adjust_translation(vec3(-0.01,0,0));
   }
   if ( action.puckRight != 0 ){
      options.physics.adjPuckTranslation(vec3(0.01,0,0));
      options.puck->adjust_translation(vec3(0.01,0,0));
   }

   vec3 pos = options.physics.getPuckTranslation();
   
   double ran = 1.6;
   if( pos.x > ran ){
      options.physics.setPuckTranslation( vec3(ran, pos.y, pos.z));
      options.puck->set_translation( vec3( ran, pos.y, pos.z ) );
   }
   if( pos.x < -ran ){
      options.physics.setPuckTranslation( vec3(-ran, pos.y, pos.z));
      options.puck->set_translation( vec3( -ran, pos.y, pos.z ) );
   }

   if ( action.keysPressed > 0 )
      glutTimerFunc(options.cam_timerStep, cameraTimer, value);

   glutPostRedisplay();
}

void keyboardPress( unsigned char key, int x, int y ){

   /** Process for the key */
   bool cameraKeyPressed = false;
   bool lightKeyPressed  = false;
   btVector3 force = btVector3(0,5,0);
   btVector3 distance;
   
   if( options.view_mode == 2 ){
      options.view_mode = 0;
   }

   switch( key ){
      
      /**********************************/
      /*       Change the view mode     */
      /**********************************/
      case '8':
         if( options.view_mode == 0 )
            options.view_mode = 1;
         break;
         /********************************/
         /*         Light Actions        */
         /********************************/
      case 'n': //move light -z
         laction.move_z--;
         laction.keysPressed++;
         lightKeyPressed = true;
         break;
      case 'm': //move light +z
         laction.move_z++;
         laction.keysPressed++;
         lightKeyPressed = true;
         break;
      case ',': //move light -x
         laction.move_x--;
         laction.keysPressed++;
         lightKeyPressed = true;
         break;
      case '.': //move light +x
         laction.move_x++;
         laction.keysPressed++;
         lightKeyPressed = true;
         break;
      
      /*************************/
      /*      Puck Motion      */
      /*************************/
      case '[': //move
         action.puckLeft++;
         action.keysPressed++;
         cameraKeyPressed = true;
         break;
      case ']': //move 
         action.puckRight++;
         action.keysPressed++;
         cameraKeyPressed = true;
         break;

         /********************************/
         /*        Camera Actions        */
         /********************************/
      case 'w':
         action.moveStraight++;
         action.keysPressed++;
         cameraKeyPressed = true;
         break;
      case 's':
         action.moveStraight--;
         action.keysPressed++;
         cameraKeyPressed = true;
         break;
      case 'a':
         action.moveHoriz--;
         action.keysPressed++;
         cameraKeyPressed = true;
         break;
      case 'd':
         action.moveHoriz++;
         action.keysPressed++;
         cameraKeyPressed = true;
         break;
      case 'q':
         action.moveVert++;
         action.keysPressed++;
         cameraKeyPressed = true;
         break;
      case 'e':
         action.moveVert--;
         action.keysPressed++;
         cameraKeyPressed = true;
         break;
      case 'i':
         action.rotateVert++;
         action.keysPressed++;
         cameraKeyPressed = true;
         break;
      case 'k':
         action.rotateVert--;
         action.keysPressed++;
         cameraKeyPressed = true;
         break;
      case 'j':
         action.rotateHoriz++;
         action.keysPressed++;
         cameraKeyPressed = true;
         break;
      case 'l':
         action.rotateHoriz--;
         action.keysPressed++;
         cameraKeyPressed = true;
         break;
      case 'u':
         action.rotateStraight--;
         action.keysPressed++;
         cameraKeyPressed = true;
         break;
      case 'o':
         action.rotateStraight++;
         action.keysPressed++;
         cameraKeyPressed = true;
         break;
         /**********************/
         /*    Quit Program    */
         /**********************/
      case 033: //escape key
         // TODO: this function is broken b/c there is only 1 player
         //options.scoreboard.win_count( options.p1_wins, options.p2_wins);
         exit( EXIT_SUCCESS );     
   }

   if ( cameraKeyPressed ) {
      // enable the timer
      if ( action.keysPressed == 1 )
         glutTimerFunc(options.cam_timerStep, cameraTimer, 0);
   }
   if ( lightKeyPressed ) {
      // enable the timer
      if ( laction.keysPressed == 1 )
         glutTimerFunc(options.light_timerStep, lightTimer, 0);
   }

}

void keyboardUp( unsigned char key, int x, int y )
{
   /** Process for the key */
   switch( key ){

      case '[':
         action.puckLeft--;
         action.keysPressed = 0;
         break;
      case ']':
         action.puckRight--;
         action.keysPressed= 0;
         break;

      /********************************/
      /*         Light Actions        */
      /********************************/
      case 'n': //move light -z
         laction.move_z++;
         laction.keysPressed--;
         break;
      case 'm': //move light +z
         laction.move_z--;
         laction.keysPressed--;
         break;
      case ',': //move light -x
         laction.move_x++;
         laction.keysPressed--;
         break;
      case '.': //move light +x
         laction.move_x--;
         laction.keysPressed--;
         break;
         /********************************/
         /*        Camera Actions        */
         /********************************/
      case 'w':
         action.moveStraight--;
         action.keysPressed--;
         break;
      case 's':
         action.moveStraight++;
         action.keysPressed--;
         break;
      case 'a':
         action.moveHoriz++;
         action.keysPressed--;
         break;
      case 'd':
         action.moveHoriz--;
         action.keysPressed--;
         break;
      case 'q':
         action.moveVert--;
         action.keysPressed--;
         break;
      case 'e':
         action.moveVert++;
         action.keysPressed--;
         break;
      case 'i':
         action.rotateVert--;
         action.keysPressed--;
         break;
      case 'k':
         action.rotateVert++;
         action.keysPressed--;
         break;
      case 'j':
         action.rotateHoriz--;
         action.keysPressed--;
         break;
      case 'l':
         action.rotateHoriz++;
         action.keysPressed--;
         break;
      case 'u':
         action.rotateStraight++;
         action.keysPressed--;
         break;
      case 'o':
         action.rotateStraight--;
         action.keysPressed--;
         break;

         /**********************/
         /*    Quit Program    */
         /**********************/
      case 033: //escape key
         exit( EXIT_SUCCESS );
   }

}

void special_keys( int key, int x, int y ){
   // Was used for Airhockey paddle
}

void special_Upkeys( int key, int x, int y ){
   // Was used for Airhockey paddle
}
