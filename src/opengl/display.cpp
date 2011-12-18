#include "display.h"

void display_function( void )
{
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   switch (options.view_mode)
   {
      case 0:
      case 4:
         /******************************************/
         /*               Game Display             */
         /******************************************/
         glClearColor( 0.5, 0.5, 0.5, 1.0 );
         options.board->draw_shape( options.camera.getTransform(), options.projectionMatrix, options.light.m_position); 
         options.pegs->draw_shape( options.camera.getTransform(), options.projectionMatrix, options.light.m_position); 
         options.puck->draw_shape( options.camera.getTransform(), options.projectionMatrix, options.light.m_position);

         options.hud.draw_shape();
         break;
      
      case 1:  // scoreboard
         
         /******************************************/
         /*          Scoreboard Display            */
         /******************************************/
         
         glClearColor( 1.0, 1.0, 1.0, 1.0 );
         options.scoreboard.draw_shape();
         
         break;
      case 2:  // win!
         glClearColor( 1.0, 1.0, 1.0, 1.0 );
         options.winner->draw_shape();
   }
   glutSwapBuffers();

}

