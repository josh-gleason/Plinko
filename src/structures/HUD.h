#ifndef __SRC_STRUCTURES_HUD_H__
#define __SRC_STRUCTURES_HUD_H__

#include "vec.h"
#include "mat.h"
#include "GL.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

#define MAX_TURNS 1

class HUD{

   public:

      HUD( );
      
      void init( GLuint const& prog, string const& pname );

      void draw_shape( );

      void increment_score( const int& amt = 1);
      void decrement_score( const int& amt = 1);
      void increment_turns( const int& amt = 1);
      void decrement_turns( const int& amt = 1);
      void reset_turns();
      void reset_score();

      int get_turns( )const;
      int get_score( )const;

   private:

      unsigned char* read_image(char* fname);
      size_t get_index( char const& letter );
      void build_texture_values( vec4 const& tl, vec4 const& br, string const& tag, vec4*& loc, vec2*& char_data, GLuint& vex_size, GLuint& tex_size);
      
      void rebind_data();

      // attribute types
      GLuint vao;
      GLuint vbo;
      GLuint program;
      GLuint vPosition;
      GLuint vTexCoord; 
      GLuint buffer;
      GLuint drawmode;

      GLuint texture;

      GLuint num_points;
      GLuint vex_size;      //size of vertex array (BANNER ONLY)
      GLuint tex_size;      //size of texture array (BANNER ONLY) 
      vector<vec4> points_name;        //list of vertices (BANNER ONLY)
      vector<vec2> texture_name;    //list of texture coordinates (BANNER ONLY)
      
      GLuint player_score;
      GLuint num_points_score;
      GLuint vex_size_score;
      GLuint tex_size_score;
      vector<vec4> points_score;
      vector<vec2> texture_score;
      
      GLuint player_turns;
      GLuint num_points_turns;
      GLuint vex_size_turns;
      GLuint tex_size_turns;
      vector<vec4> points_turns;
      vector<vec2> texture_turns;
      
      double end1, end2, end3;

      GLfloat z_dist;
      float z_trans;

      string player_name;

      int img_rows;
      int img_cols;
      unsigned char* img_data;   //character image data (data/characters_flip.ppm)
};

#endif
