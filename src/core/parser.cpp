/**
   @file  Parser.cpp
   @brief Implementation of Parser functions

   @author Marvin Smith
*/
#include "parser.h"

void parse_arguements( int argc, char* argv[], Parameters& options ){
   
   //basic variables
   string default_config_filename, gwin_name, fname;
   int gwx, gwy, gix, giy;

   /***********************/
   /*    CREATE PARSERS   */
   /***********************/
   //create parser for generic use, command-line only
   po::options_description generic("Allowed options");
   //create parser for config file and command line related
   po::options_description config_file("Configuration");

   /******************************/
   /*   ADD OPTIONS TO PARSERS   */
   /******************************/
   //construct generic options
   generic.add_options()
      ("help,h","produce help message")
      ("config,c",po::value<string>(&default_config_filename)->default_value("data/options.cfg"), "name of the configuration file ( default is data/options.cfg )");

   /***********************************/
   /*  construct config file options  */
   /***********************************/
   config_file.add_options()

   ("PLAYER_NAME",po::value<string>(&options.name)->default_value("PLAYER"), " name of the player")
   
   ///GLUT WINDOW Parameters
   ("GLUT_WIN_X",po::value<int>(&gwx)->default_value(-1),"default width of GLUT window")
   ("GLUT_WIN_Y",po::value<int>(&gwy)->default_value(-1),"default height of GLUT window")
   ("GLUT_INIT_X",po::value<int>(&gix)->default_value(-1),"initial position of the GLUT window x coord")
   ("GLUT_INIT_Y",po::value<int>(&giy)->default_value(-1),"initial position of the GLUT window y corrd")
   ("GLUT_WIN_NAME",po::value<string>(&gwin_name)->default_value("pong"),"Name of GLUT Window");
   
   /*********************************************************************************************/
   /*     DO NOT PUT ANY MORE OPTIONS AFTER DEBUG. THERE NEEDS TO BE A SEMICOLON AFTER DEBUG    */
   /*                                                                                           */
   /*     DO NOT CHANGE!!!!!!!!!!!!!                                                            */
   /*********************************************************************************************/

   //this is a new description to allow us to combine the command line and config file 
   //  inputs for use in the command-line only options. Should also contain hidded once
   //  they become relevant
   po::options_description cmdline_options;
   cmdline_options.add(generic).add(config_file);

   //this is a new description which will add hidden descriptions once the hidden options
   //   are deemed necessary.  Check multiple_sources.cpp in the boost program options example
   //   code to learn how to integrate this
   po::options_description config_file_options;
   config_file_options.add(config_file);

   //This is a new description which will show visible options not hidded. This is important as 
   //   will be what gets printed to the screen when the help gets called
   po::options_description visible("Allowed options");
   visible.add(generic).add(config_file);

   //create variable map and map the command line arguements to it
   po::variables_map vm;
   store(po::parse_command_line(argc, argv,cmdline_options), vm);
   notify(vm);

   /****************************************/
   /*   CHECK FOR CONFIG FILE ARGUEMENTS   */
   /****************************************/
   ifstream ifs(default_config_filename.c_str());
   if (!ifs)
   {
      cout << "can not open config file: " << default_config_filename << "\n";
      exit(0);
   }
   else
   {
      //if the filestream does exist, then load config arguements and parse
      store(parse_config_file(ifs, config_file_options), vm);
      notify(vm);
   }
   /**************************/
   /*   PRINT HELP OPTIONS   */
   /**************************/
   if (vm.count("help")) {
      cout << visible << "\n";
      exit(0);
   }


   /******************************************************************/
   /*   CONVERT STRING INPUTS INTO APPROPRIATE CONFIG FILE OPTIONS   */
   /******************************************************************/
   if( gwx < 0 || gwy < 0 )
      cout << "ERROR: GLUT_WIN_X and GLUT_WIN_Y must be greater than 0 " << __FILE__ << ", line: " << __LINE__ << endl;
   options.glut_window_x = gwx;
   options.glut_window_y = gwy;

   if( gix < 0 || giy < 0 )
      cout << "ERROR: GLUT_INIT_X and GLUT_INIT_Y must be greater than 0 " << __FILE__ << ", line: " << __LINE__ << endl;
   options.glut_initial_x = gix;
   options.glut_initial_y = giy;
   
   options.glut_window_name = gwin_name;

}
