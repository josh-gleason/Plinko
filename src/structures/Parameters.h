/**
  * @file Paramters.h
  * Parameter container for Air Hockey
  *
  * @author Marvin Smith
  * @author Joshua Gleason
*/
#ifndef __SRC_STRUCTURES_PARAMETERS_H__
#define __SRC_STRUCTURES_PARAMETERS_H__

// ../opengl
#include "GL.h"

#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>

// ./
#include "Object.h"
#include "Camera.h"
#include "Light.h"
#include "Physics.h"
#include "Scoreboard.h"
#include "HUD.h"
#include "Winner.h"

// ../math/
#include "vec.h"
#include "mat.h"

// bullet physics engine
#include <btBulletDynamicsCommon.h>

using namespace std;

/**
   @class Parameters
   @brief Storage for program options

   This class stores everything for the program
*/
class Parameters{

   public:

      Parameters();
      ~Parameters();
      
      /*************************************/
      /*              General              */
      /*************************************/
      bool mouse_down;
      int timed_pause;
      int view_mode;
     
      /*************************************/
      /*          Window Parameters        */
      /*************************************/
      int glut_window_x;
      int glut_window_y;
      int glut_initial_x;
      int glut_initial_y;
      string glut_window_name;
      
      /*************************************/
      /*            Popup Menus            */
      /*************************************/
      GLuint menu;

      /*************************************/
      /*           Shader Data             */
      /*************************************/
      GLfloat aspect;
      string filename;
      GLuint texture_program;
      GLuint program;
      GLuint model_view;
      GLuint projection;

      /*************************************/
      /*              Viewport             */
      /*************************************/
      GLfloat fova;
      GLfloat ratio;
      GLfloat zNear;
      GLfloat zFar;

      mat4 projectionMatrix;

      /*************************************/
      /*               Camera              */
      /*************************************/
      Camera camera;

      CameraMode cam_mode;
      GLfloat cam_lookStep;
      GLfloat cam_moveStep;
      unsigned int cam_timerStep;

      /*************************************/
      /*            Game Objects           */
      /*************************************/
      Object* board;
      Object* pegs;
      Object* puck;

      std::vector<Object*> temppegs;

      /*************************************/
      /*           Player Info             */
      /*************************************/
      string name;
      GLuint wins;
      GLuint score;

      /*************************************/
      /*               HUD                 */
      /*************************************/
      HUD hud;
      Scoreboard scoreboard;
      Winner* winner;      // TODO I don't know what this is

      /*************************************/
      /*          Physical World           */
      /*************************************/
      Light light;
      Physics_Model physics;
      unsigned int light_timerStep;
      GLfloat light_moveStep;
      GLfloat world_timerStep;
      double puck_x_axis;
      double avg_travel;
      int tick_counter;
      vec3 puck_pos;
};

#endif
