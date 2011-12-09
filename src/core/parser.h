/**
   @file  Parser.h
   @brief Header for parsing functions

   @author Marvin Smith
   
   Handles all parsing of the configuration file
*/
#ifndef __SRC_CORE_PARSER_H__
#define __SRC_CORE_PARSER_H__

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

// ../structures
#include "Parameters.h"

using namespace std;

namespace po = boost::program_options;
namespace bf = boost::filesystem;

/**
   @brief  parses arguements from command-line as well as config file
   
   @param[in] argc pass number of arguements straight from main
   @param[in] argv pass arguement list straight from main
   @param[in,out] params Parameter struct which stores relevant program variables
   @return void
*/
void parse_arguements( int argc, char* argv[], Parameters& params );

#endif
