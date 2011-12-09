/**
  * @file timer.h
  * Timer functions for opengl Pong
  * @author Marvin Smith
*/
#ifndef __SRC_OPENGL_MANAGEMENT_TIMER_H__
#define __SRC_OPENGL_MANAGEMENT_TIMER_H__

#include "display.h"
#include "../structures/Parameters.h"

#include "Physics.h"

// bullet physics engine
#include <btBulletDynamicsCommon.h>

extern Parameters options;

void timerHandle( int state );

#endif
