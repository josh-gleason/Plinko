/**
 * @file timer.cpp
 * Implementation of timer handler
 * @author Marvin Smith
 */
#include "timer.h"

void timerHandle( int state ){

   // timed pause length
   if( options.timed_pause > 0 )
   {
     options.timed_pause--;
     glutTimerFunc( options.world_timerStep, &timerHandle, 0);
     return;
   }
   
   if( options.view_mode == 0 ){ //setup mode
      options.tick_counter = 0;
      options.puck_pos = options.physics.getPuckTranslation();
   }
   if( options.view_mode == 1 ){
      
      // Game mode
      options.tick_counter++;
      options.physics.stepSimulation(1./60.);

      options.puck->set_translation(options.physics.getPuckTranslation());
      options.puck->set_rotation(options.physics.getPuckRotation());
   
      //check for end of run
      double weight = 0.06;
      double dist = vdistance( options.puck_pos, options.physics.getPuckTranslation());
      options.avg_travel = weight*dist + (1-weight)*options.avg_travel;
      options.puck_pos = options.physics.getPuckTranslation();
   
      if( options.avg_travel < 0.004 && options.tick_counter > 100 ){ //the puck has stopped moving
         
         //compute money won, 
         double amt;
         double xpos = options.physics.getPuckTranslation().x;
         if( fabs(xpos) < 0.5 ) //$10,000
            amt = 10000;
         else if( fabs(xpos) < 1.0 ) // $0
            amt = 0;
         else if( fabs(xpos) < 1.5 ) // $1000
            amt = 1000;
         else 
            amt = 500;

         //show the screen 
         options.hud.increment_score( amt );
         options.hud.decrement_turns( 1 );

         //reset the puck position
         options.physics.setPuckTranslation(vec3(0.3,0.0, 0.1));
         options.puck->set_translation(options.physics.getPuckTranslation());
         options.view_mode = 0;

         //check for ending condition
         if( options.hud.get_turns() < 0 ){
   
            options.winner->rebind(options.hud.get_score());

            options.view_mode = 2;
            options.hud.reset_turns( );
            options.hud.reset_score( );

         }
      }
   }

   glutPostRedisplay(); 
   glutTimerFunc( options.world_timerStep, &timerHandle, 0);
}
