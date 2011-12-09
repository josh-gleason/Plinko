#include <algorithm>
#include <iomanip>

// math/
#include "mat.h"
#include "vec.h"

// opengl/
#include "shader_processing.h"
#include "display.h"
#include "keyboard.h"
#include "mouse.h"
#include "timer.h"
#include "reshape.h"
#include "menu.h"

// core/
#include "parser.h"

// structures/
#include "Parameters.h"

// bullet physics engine
#include <btBulletDynamicsCommon.h>

Parameters options;

//----------------------------------------------------------------------------
// OpenGL initialization
void init( int& argc, char** argv )
{
   /*********************************************/
   /*                 GLUT Setup                */
   /*********************************************/

   // glut initialization
   glutInit( &argc, argv );
   glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
   glutInitWindowSize( options.glut_window_x, options.glut_window_y );
   glutInitWindowPosition(options.glut_initial_x,options.glut_initial_y);
   glutCreateWindow( options.glut_window_name.c_str() );
   
   // glut settings
   glutSetKeyRepeat( GLUT_KEY_REPEAT_OFF );

   // initialize the OpenGL extention wrangler
#ifndef __APPLE__
   glewInit();
#endif

   // initialize the pop-up menu structure
   init_menu();

   // function registration
   glutDisplayFunc( display_function );
   glutKeyboardFunc( keyboardPress );
   glutKeyboardUpFunc( keyboardUp );
   glutSpecialFunc( special_keys);
   glutSpecialUpFunc( special_Upkeys);
   glutMouseFunc( mouseEvent );
   glutMotionFunc( mouseMove );
   glutReshapeFunc( reshape );
   glutTimerFunc( 17, timerHandle, 0);

   /*********************************************/
   /*               OpenGL Setup                */
   /*********************************************/

   // enable anti-aliasing (and transparency)
   glEnable(GL_LINE_SMOOTH);
   glEnable(GL_POLYGON_SMOOTH);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glEnable( GL_DEPTH_TEST );
   glClearColor( 0.5, 0.5, 0.5, 1.0 );
   
   // Load shaders and use the resulting shader program
   options.program = InitShader( "data/shaders/vshader.glsl", "data/shaders/fshader.glsl" );
   glUseProgram( options.program );

   /*********************************************/
   /*                View Setup                 */
   /*********************************************/

   // set perspective
   options.zNear = 0.25;
   options.zFar  = 50.0;
   options.fova  = 30.0;
   options.ratio = 1.0 * options.glut_window_y / options.glut_window_x; 

   // TODO this probably shouldn't be done manually (make a function somewhere)
   options.projectionMatrix = Perspective(options.fova, options.ratio,
         options.zNear, options.zFar);

   // set up initial camera position
   options.camera = Camera(vec4(8.0,5.0,0.0,1.0), options.cam_mode);
   options.camera.rotateHoriz(90);
   options.camera.rotateVert(-35);

   // set up initial light position
   options.light_timerStep = 10;
   options.light_moveStep = 0.06;

   /*********************************************/
   /*               Model Loader                */
   /*********************************************/

   // load models from file

   // init buffers for models

   // set initial object positions

   /*********************************************/
   /*               Texture Mapping             */
   /*********************************************/

   // build scoreboard
   options.scoreboard.init( options.program );
   
   // TODO There should be only 1 player
   options.hud.init( options.program, options.name, options.name );
	
   /*********************************************/
   /*               Physics Engine              */
   /*********************************************/
   
   // add objects to physical world
   
   // initialize physics engine

}

/******************************************************************************/
/*                                  MAIN                                      */
/******************************************************************************/
int main( int argc, char **argv )
{
   srand(time(0));
   parse_arguements( argc, argv, options );
   init( argc, argv );

   cout << "================ Camera Controls ================" << endl
        << "============ (w,a,s,d,q,e) movement =============" << endl
        << " w - Move Forward          s - Move Backwards    " << endl
        << " a - Move Left             d - Move Right        " << endl
        << " q - Move Up               e - Move Down         " << endl
        << "============== (u,h,j,k,y,i) look ===============" << endl
        << " i - Look Up               k - Look Down         " << endl
        << " j - Look Left             l - Look Right        " << endl
        << "== Twist works only if camera is in free mode ===" << endl
        << " u - Twist CCW             o - Twist CW          " << endl
        << "=================== lighting model ==============" << endl 
        << " n - move light -z axis   m - move light +z axis " << endl
        << " , - move light -x axis   . - move light +x axis " << endl
        << "=================================================" << endl << endl;

   glutMainLoop();
   return 0;
}
