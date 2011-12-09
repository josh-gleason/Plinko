#ifndef __SRC_STRUCTURES_SCOREBOARD_H__
#define __SRC_STRUCTURES_SCOREBOARD_H__

#include "vec.h"
#include "mat.h"
#include "GL.h"

#include <fstream>
#include <sstream>
#include <string>

using namespace std;

const vec2 char_ran[37] = { vec2(0.003, 0.022), //a - 0
                      vec2(0.031, 0.048), //b - 1
                      vec2(0.057, 0.077), //c - 2
                      vec2(0.085, 0.108), //d - 3
                      vec2(0.116, 0.131), //e - 4 
                      vec2(0.142, 0.156), //f - 5 
                      vec2(0.165, 0.187), //g - 6
                      vec2(0.198, 0.215), //h - 7
                      vec2(0.225, 0.234), //i - 8
                      vec2(0.240, 0.250), //j - 9
                      vec2(0.260, 0.280), //k - 10
                      vec2(0.287, 0.304), //l - 11
                      vec2(0.311, 0.335), //m - 12
                      vec2(0.345, 0.362), //n - 13
                      vec2(0.372, 0.395), //o - 14
                      vec2(0.404, 0.421), //p - 15
                      vec2(0.427, 0.451), //q - 16
                      vec2(0.459, 0.478), //r - 17
                      vec2(0.488, 0.506), //s - 18
                      vec2(0.512, 0.532), //t - 19
                      vec2(0.538, 0.562), //u - 20
                      vec2(0.565, 0.594), //v - 21
                      vec2(0.594, 0.626), //w - 22
                      vec2(0.632, 0.654), //x - 23
                      vec2(0.659, 0.679), //y - 24
                      vec2(0.685, 0.709), //z - 25
                      vec2(0.712, 0.735), //0 - 26
                      vec2(0.740, 0.758), //1 - 27
                      vec2(0.765, 0.784), //2 - 28
                      vec2(0.791, 0.812), //3 - 29
                      vec2(0.815, 0.837), //4 - 30
                      vec2(0.842, 0.864), //5 - 31
                      vec2(0.868, 0.891), //6 - 32
                      vec2(0.895, 0.914), //7 - 33
                      vec2(0.920, 0.942), //8 - 34
                      vec2(0.946, 0.969), //9 - 35
                      vec2(0.972, 0.999)  // <space> - 36
};



class Scoreboard{

   public:

      Scoreboard( );
      ~Scoreboard( );
      
      void make_empty();

      void init( GLuint const& prog );

      void draw_shape( );
      void win_count( const int p1, const int p2);

   private:

      unsigned char* read_image(char* fname);
      size_t get_index( char const& letter );
      void build_texture_values( vec4 const& tl, vec4 const& br, string const& tag, vec4*& loc, vec2*& char_data, GLuint& vex_size, GLuint& tex_size);
      
      // attribute types
      GLuint vao;
      GLuint vbo;
      GLuint program;
      GLuint vPosition;
      GLuint vTexCoord; 
      GLuint buffer;
      GLuint drawmode;

      GLuint texture;

      GLuint num_points_main;
      GLuint vex_size_main;      //size of vertex array (BANNER ONLY)
      GLuint tex_size_main;      //size of texture array (BANNER ONLY) 
      vec4 * points_main;        //list of vertices (BANNER ONLY)
      vec2 * tex_coords_main;    //list of texture coordinates (BANNER ONLY)
      
      GLuint num_points_p1;
      GLuint vex_size_p1;      //size of vertex array (PLAYER 1)
      GLuint tex_size_p1;      //size of texture array (PLAYER 1) 
      vec4 * points_p1;        //list of vertices (PLAYER 1)
      vec2 * tex_coords_p1;    //list of texture coordinates (PLAYER 1)
      
      GLuint num_points_p2;
      GLuint vex_size_p2;      //size of vertex array (PLAYER 2)
      GLuint tex_size_p2;      //size of texture array (PLAYER 2) 
      vec4 * points_p2;        //list of vertices (PLAYER 2)
      vec2 * tex_coords_p2;    //list of texture coordinates (PLAYER 2)
      
      GLuint num_points_p3;
      GLuint vex_size_p3;      //size of vertex array (BANNER ONLY)
      GLuint tex_size_p3;      //size of texture array (PLAYER 1) 
      vec4 * points_p3;        //list of vertices (PLAYER 3)
      vec2 * tex_coords_p3;    //list of texture coordinates (PLAYER 3)
      
      string usr1, usr2, usr3, scp1, scp2, scp3;
      int sc1, sc2, sc3;
     
      int p1w, p2w;

      GLfloat z_dist;
      float z_trans;
      
      int img_rows;
      int img_cols;
      unsigned char* img_data;   //character image data (data/characters_flip.ppm)
};

#endif
