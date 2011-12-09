#ifndef __SRC_OPENGL_MANAGEMENT_MOUSE_H__
#define __SRC_OPENGL_MANAGEMENT_MOUSE_H__

#include <cstdio>
#include <cstdlib>

// ../math/
#include "vec.h"

// ../structures
#include "Parameters.h"

extern Parameters options;

void mouseEvent(int button, int state, int x, int y);

void mouseEntered(int state);

void mouseMove(int x, int y);

#endif

