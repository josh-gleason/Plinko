#ifndef __SRC_STRUCTURES_TEX_H__
#define __SRC_STRUCTURES_TEX_H__

#include "vec.h"
#include "mat.h"
#include "GL.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class TEX{

   public:

      TEX( );
      
      void init( GLuint const& prog  );

      void draw_shape( );

   private:
      
      void build_anchor_points();

      unsigned char* read_image(char* fname, int& rows, int& cols );
      
      // attribute types
      GLuint vao;
      GLuint vbo;
      GLuint program;
      GLuint vPosition;
      GLuint vTexCoord; 
      GLuint buffer;
      GLuint drawmode;

      GLuint texture;

      GLuint num_points_banner;
      GLuint vex_size_banner;      //size of vertex array (BANNER ONLY)
      GLuint tex_size_banner;      //size of texture array (BANNER ONLY) 
      vector<vec4> points_banner;        //list of vertices (BANNER ONLY)
      vector<vec2> texture_banner;    //list of texture coordinates (BANNER ONLY)
      
      int banner_rows;
      int banner_cols;
      unsigned char* banner_data;   //character image data (data/characters_flip.ppm)
};

#endif
