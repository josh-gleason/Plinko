#ifndef __SRC_STRUCTURES_WINNER_H__
#define __SRC_STRUCTURES_WINNER_H__

#include "vec.h"
#include "mat.h"
#include "GL.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class Winner{

   public:

      Winner( const int sc, const string nm, const GLuint& prog );

      void rebind( const int sc );
      
      void draw_shape( );

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
      vector<vec4> points_main;        //list of vertices (BANNER ONLY)
      vector<vec2> tex_coords_main;    //list of texture coordinates (BANNER ONLY)
      
      GLuint num_points_p;
      GLuint vex_size_p;      //size of vertex array (PLAYER 1)
      GLuint tex_size_p;      //size of texture array (PLAYER 1) 
      vector<vec4> points_p;        //list of vertices (PLAYER 1)
      vector<vec2> tex_coords_p;    //list of texture coordinates (PLAYER 1)
      
      GLuint num_points_p2;
      GLuint vex_size_p2;      //size of vertex array (PLAYER 1)
      GLuint tex_size_p2;      //size of texture array (PLAYER 1) 
      vector<vec4> points_p2;        //list of vertices (PLAYER 1)
      vector<vec2> tex_coords_p2;    //list of texture coordinates (PLAYER 1)
      
      
      int img_rows;
      int img_cols;
      unsigned char* img_data;   //character image data (data/characters_flip.ppm)
};

#endif
