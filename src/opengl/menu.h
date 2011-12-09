#ifndef _THE_POPUP_MENU_FOR_PLINKO__
#define _THE_POPUP_MENU_FOR_PLINKO__

#include "GL.h"

// ../structures
#include "Parameters.h"

void init_menu();

void processMenuEvents( int option );
void processStateMenuEvents( int option );
void processShapeMenuEvents( int option );
void processAIMenuEvents( int option );
void processDiffMenuEvents( int option );

#endif // _THE_POPUP_MENU_FOR_PLINKO__

