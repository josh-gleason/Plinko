#ifndef PRINT_MODEL_INFO_H
#define PRINT_MODEL_INFO_H

// Author: Joshua Gleason
// Purpose: given an objLoader object print all the information through
//          the desired stream

#include <iostream>
#include "objLoader.h"

void printObjData(const objLoader& loader, std::ostream& out = std::cout);

#endif // PRINT_MODEL_INFO_H
