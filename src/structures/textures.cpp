#include "textures.h"

/*****************************************/
/*              Load an image            */
/*****************************************/
unsigned char* TEX::read_image(char* fname, int& rows, int& cols)
{
   int Q;
   unsigned char *charImage;
   char header [100], *ptr; 
   ifstream ifp;

   ifp.open(fname, ios::in | ios::binary);

   if (!ifp) {
      cout << "Can't read image: " << fname << endl;
      exit(1);
   }

   // read header

   ifp.getline(header,100,'\n');

   if ( (header[0]!=80) ||    /* 'P' */
         (header[1]!=54) ) {   /* '6' */
      cout << header[0] << ", " << header[1] << endl;
      cout << "Image " << fname << " is not PPM" << endl;
      exit(1);
   }

   ifp.getline(header,100,'\n');
   while(header[0]=='#')
      ifp.getline(header,100,'\n');

   rows=strtol(header,&ptr,0);
   cols=atoi(ptr);

   ifp.getline(header,100,'\n');
   Q=strtol(header,&ptr,0);

   charImage = new unsigned char [3*rows*cols];
   unsigned char* tImage    = new unsigned char [3*rows*cols];

   ifp.read( reinterpret_cast<char *>(charImage), (3*cols*rows)*sizeof(unsigned char));

   if (ifp.fail()) {
      cout << "Image " << fname << " has wrong size" << endl;
      exit(1);
   }

   ifp.close();

   for(int i=0; i<(3*rows*cols); i++)
      tImage[i] = charImage[(3*rows*cols-1)-i];
   for(int i=0; i<(3*rows*cols); i++)
      charImage[i] = tImage[i];
   
   delete tImage;
   return charImage;
}
// Draw the normals for the object
//#define DEBUG_DRAW_NORMALS

/*****************************************/
/*             Constructors              */
/*****************************************/
TEX::TEX( ){
   
}

void TEX::init( const GLuint& prog ){
   

   // Load shaders and use the resulting shader program
   program = prog;

   //read image from file
   banner_data = read_image( "data/PlinkgoBannerAlone.ppm", banner_rows, banner_cols );

   //set anchor points
   build_anchor_points(); 
   
   /*****************/

   // Initialize texture objects
   glGenTextures( 1, &texture );
   glBindTexture( GL_TEXTURE_2D, texture );
   glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, banner_rows, banner_cols, 0, GL_BGR, GL_UNSIGNED_BYTE, banner_data );
   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
   glActiveTexture( GL_TEXTURE0 );
   
   // generate and bind the vao
#ifdef __APPLE__
   glGenVertexArraysAPPLE( 1, &vao );
   glBindVertexArrayAPPLE( vao );
#else
   glGenVertexArrays(1, &vao);
   glBindVertexArray( vao );
#endif

   // Create and initialize a buffer object
   glGenBuffers( 1, &buffer );

   
   //build points and tex_coords arrays
   glBindBuffer( GL_ARRAY_BUFFER, buffer );
   glBufferData( GL_ARRAY_BUFFER, vex_size_banner + tex_size_banner, NULL, GL_DYNAMIC_DRAW );

   // Specify an offset to keep track of where we're placing data in our
   //   vertex array buffer.  We'll use the same technique when we
   //   associate the offsets with vertex attribute pointers.
   GLintptr offset = 0;
   glBufferSubData( GL_ARRAY_BUFFER, offset, vex_size_banner, &points_banner[0] );
   offset += vex_size_banner;

   glBufferSubData( GL_ARRAY_BUFFER, offset, tex_size_banner, &texture_banner[0] );
   offset += tex_size_banner;

   // set up vertex arrays
   offset = 0;
   GLuint vPosition = glGetAttribLocation( program, "vPosition" );
   glEnableVertexAttribArray( vPosition );
   glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset) );
   offset += vex_size_banner;

   GLuint vTexCoord = glGetAttribLocation( program, "vTexCoord" );
   glEnableVertexAttribArray( vTexCoord );
   glVertexAttribPointer( vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset) );
   
   worldview = glGetUniformLocation(      program, "worldview" );
   projection = glGetUniformLocation(     program, "projection" );

}

   
void TEX::draw_shape( mat4 const& worldviewMat, mat4 const& proj, vec4 const& l_pos ){

#ifdef __APPLE__
   glBindVertexArrayAPPLE( vao );
#else
   glBindVertexArray( vao ); 
#endif

   glUniform1i( drawmode, 3 );
   
   glUniformMatrix4fv( projection, 1, GL_TRUE, proj );
   glUniformMatrix4fv( worldview, 1, GL_TRUE, worldviewMat);
   glBindBuffer( GL_ARRAY_BUFFER, buffer );
   glBindTexture( GL_TEXTURE_2D, texture );
   glDrawArrays( GL_TRIANGLES, 0, num_points_banner);
   

}


void TEX::build_anchor_points(){


   //create banner points
   vec4 tl(-1.7,  0.0, 0.2, 1);
   vec4 tr( 1.7,  0.0, 0.2, 1);
   vec4 br( 1.7, -0.8, 0.2, 1);
   vec4 bl(-1.7, -0.8, 0.2, 1);

   points_banner.clear();        texture_banner.clear();
   points_banner.push_back(tl);  texture_banner.push_back(vec2(0.5,1.0));
   points_banner.push_back(tr);  texture_banner.push_back(vec2(1.0,1.0));
   points_banner.push_back(br);  texture_banner.push_back(vec2(1.0,0.6));
   points_banner.push_back(tl);  texture_banner.push_back(vec2(0.5,1.0));
   points_banner.push_back(br);  texture_banner.push_back(vec2(1.0,0.6));
   points_banner.push_back(bl);  texture_banner.push_back(vec2(0.5,0.6));

   //create 10k points
   tl = vec4(-0.2, -7.0, 0.2, 1);
   tr = vec4( 0.2, -7.0, 0.2, 1);
   br = vec4( 0.2, -8.0, 0.2, 1);
   bl = vec4(-0.2, -8.0, 0.2, 1);

   points_banner.push_back(tl);  texture_banner.push_back(vec2(0.43,1.0));
   points_banner.push_back(tr);  texture_banner.push_back(vec2(0.48,1.0));
   points_banner.push_back(br);  texture_banner.push_back(vec2(0.48,0.2));
   points_banner.push_back(tl);  texture_banner.push_back(vec2(0.43,1.0));
   points_banner.push_back(br);  texture_banner.push_back(vec2(0.48,0.2));
   points_banner.push_back(bl);  texture_banner.push_back(vec2(0.43,0.2));
   
   //create 0k points
   tl = vec4( 0.3, -7.0, 0.2, 1);
   tr = vec4( 0.8, -7.0, 0.2, 1);
   br = vec4( 0.8, -8.0, 0.2, 1);
   bl = vec4( 0.3, -8.0, 0.2, 1);

   points_banner.push_back(tl);  texture_banner.push_back(vec2(0.33,1.0));
   points_banner.push_back(tr);  texture_banner.push_back(vec2(0.38,1.0));
   points_banner.push_back(br);  texture_banner.push_back(vec2(0.38,0.2));
   points_banner.push_back(tl);  texture_banner.push_back(vec2(0.33,1.0));
   points_banner.push_back(br);  texture_banner.push_back(vec2(0.38,0.2));
   points_banner.push_back(bl);  texture_banner.push_back(vec2(0.33,0.2));

   //create 0k points
   tl = vec4( -0.8, -7.0, 0.2, 1);
   tr = vec4( -0.3, -7.0, 0.2, 1);
   br = vec4( -0.3, -8.0, 0.2, 1);
   bl = vec4( -0.8, -8.0, 0.2, 1);

   points_banner.push_back(tl);  texture_banner.push_back(vec2(0.33,1.0));
   points_banner.push_back(tr);  texture_banner.push_back(vec2(0.38,1.0));
   points_banner.push_back(br);  texture_banner.push_back(vec2(0.38,0.2));
   points_banner.push_back(tl);  texture_banner.push_back(vec2(0.33,1.0));
   points_banner.push_back(br);  texture_banner.push_back(vec2(0.38,0.2));
   points_banner.push_back(bl);  texture_banner.push_back(vec2(0.33,0.2));
   
   //create 1k points
   tl = vec4( 0.9, -7.0, 0.2, 1);
   tr = vec4( 1.25, -7.0, 0.2, 1);
   br = vec4( 1.25, -8.0, 0.2, 1);
   bl = vec4( 0.9, -8.0, 0.2, 1);

   points_banner.push_back(tl);  texture_banner.push_back(vec2(0.23,1.0));
   points_banner.push_back(tr);  texture_banner.push_back(vec2(0.28,1.0));
   points_banner.push_back(br);  texture_banner.push_back(vec2(0.28,0.2));
   points_banner.push_back(tl);  texture_banner.push_back(vec2(0.23,1.0));
   points_banner.push_back(br);  texture_banner.push_back(vec2(0.28,0.2));
   points_banner.push_back(bl);  texture_banner.push_back(vec2(0.23,0.2));

   //create 0k points
   tl = vec4( -1.25, -7.0, 0.2, 1);
   tr = vec4( -0.85, -7.0, 0.2, 1);
   br = vec4( -0.85, -8.0, 0.2, 1);
   bl = vec4( -1.25, -8.0, 0.2, 1);

   points_banner.push_back(tl);  texture_banner.push_back(vec2(0.23,1.0));
   points_banner.push_back(tr);  texture_banner.push_back(vec2(0.28,1.0));
   points_banner.push_back(br);  texture_banner.push_back(vec2(0.28,0.2));
   points_banner.push_back(tl);  texture_banner.push_back(vec2(0.23,1.0));
   points_banner.push_back(br);  texture_banner.push_back(vec2(0.28,0.2));
   points_banner.push_back(bl);  texture_banner.push_back(vec2(0.23,0.2));
   
   //create 1k points
   tl = vec4( 1.31, -7.0, 0.2, 1);
   tr = vec4( 1.78, -7.0, 0.2, 1);
   br = vec4( 1.78, -8.0, 0.2, 1);
   bl = vec4( 1.31, -8.0, 0.2, 1);

   points_banner.push_back(tl);  texture_banner.push_back(vec2(0.11,1.0));
   points_banner.push_back(tr);  texture_banner.push_back(vec2(0.18,1.0));
   points_banner.push_back(br);  texture_banner.push_back(vec2(0.18,0.2));
   points_banner.push_back(tl);  texture_banner.push_back(vec2(0.11,1.0));
   points_banner.push_back(br);  texture_banner.push_back(vec2(0.18,0.2));
   points_banner.push_back(bl);  texture_banner.push_back(vec2(0.11,0.2));

   //create 0k points
   tl = vec4( -1.78, -7.0, 0.2, 1);
   tr = vec4( -1.31, -7.0, 0.2, 1);
   br = vec4( -1.31, -8.0, 0.2, 1);
   bl = vec4( -1.78, -8.0, 0.2, 1);

   points_banner.push_back(tl);  texture_banner.push_back(vec2(0.11,1.0));
   points_banner.push_back(tr);  texture_banner.push_back(vec2(0.18,1.0));
   points_banner.push_back(br);  texture_banner.push_back(vec2(0.18,0.2));
   points_banner.push_back(tl);  texture_banner.push_back(vec2(0.11,1.0));
   points_banner.push_back(br);  texture_banner.push_back(vec2(0.18,0.2));
   points_banner.push_back(bl);  texture_banner.push_back(vec2(0.11,0.2));

   num_points_banner = points_banner.size();
   vex_size_banner = sizeof(vec4)*points_banner.size();
   tex_size_banner = sizeof(vec2)*points_banner.size();
  
}
