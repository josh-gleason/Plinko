#include "HUD.h"

/*****************************************/
/*              Load an image            */
/*****************************************/
unsigned char* HUD::read_image(char* fname)
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

   img_rows=strtol(header,&ptr,0);
   img_cols=atoi(ptr);

   ifp.getline(header,100,'\n');
   Q=strtol(header,&ptr,0);

   charImage = new unsigned char [3*img_rows*img_cols];
   unsigned char* tImage    = new unsigned char [3*img_rows*img_cols];

   ifp.read( reinterpret_cast<char *>(charImage), (3*img_cols*img_rows)*sizeof(unsigned char));

   if (ifp.fail()) {
      cout << "Image " << fname << " has wrong size" << endl;
      exit(1);
   }

   ifp.close();

   for(int i=0; i<(3*img_rows*img_cols); i++)
      tImage[i] = charImage[(3*img_rows*img_cols-1)-i];
   for(int i=0; i<(3*img_rows*img_cols); i++)
      charImage[i] = tImage[i];
   
   delete tImage;
   return charImage;
}
// Draw the normals for the object
//#define DEBUG_DRAW_NORMALS

/*****************************************/
/*             Constructors              */
/*****************************************/
HUD::HUD( ){
   
   player1_score = 0;
   player2_score = 0;
   player1_wins = 0;
   player2_wins = 0;
}

HUD::~HUD( ){

   make_empty();

}

void HUD::make_empty(){
   if( points_p1 != NULL )
      delete [] points_p1;
   if( points_p2 != NULL )
      delete [] points_p2;
   if( texture_p1 != NULL )
      delete [] texture_p1;
   if( texture_p2 != NULL )
      delete [] texture_p2;
}


void HUD::init( const GLuint& prog, const string& p1name, const string& p2name ){

   vec4 tl_tex;
   vec4 br_tex;

   // Load shaders and use the resulting shader program
   program = prog;

   //read image from file
   img_data = read_image( "data/characters_flip.ppm" );

   //create names
   p1_name = p1name;
   p2_name = p2name;

   //build points and tex_coords arrays
   points_p1 = NULL;
   texture_p1 = NULL;
   points_p2 = NULL;
   texture_p2 = NULL;

   tl_tex = vec4(-0.99, 0.99, 0, 1);
   br_tex = vec4(-0.60, 0.95, 0, 1);
   build_texture_values( tl_tex, br_tex, string(p1_name + string("   ")), points_p1, texture_p1, vex_size_p1, tex_size_p1);
   tl_tex = vec4( 0.35, 0.99, 0, 1);
   br_tex = vec4( 0.74, 0.95, 0, 1);
   build_texture_values( tl_tex, br_tex, string(p2_name + string("   ")), points_p2, texture_p2, vex_size_p2, tex_size_p2);

   num_points_p1 = tex_size_p1/sizeof(vec2);
   num_points_p2 = tex_size_p2/sizeof(vec2);

   //build points and tex_coords arrays
   string p1s, p2s;

   stringstream sin;

   sin.clear();
   sin.str("");
   sin << player1_score;
   sin >> p1s;

   sin.clear();
   sin.str("");
   sin << player2_score;
   sin >> p2s;


   points_p1s = NULL;
   texture_p1s = NULL;
   points_p2s = NULL;
   texture_p2s = NULL;

   tl_tex = vec4(-0.60, 0.99, 0, 1);
   br_tex = vec4(-0.50, 0.95, 0, 1);
   build_texture_values( tl_tex, br_tex, string(p1s), points_p1s, texture_p1s, vex_size_p1s, tex_size_p1s);
   tl_tex = vec4( 0.74, 0.99, 0, 1);
   br_tex = vec4( 0.84, 0.95, 0, 1);
   build_texture_values( tl_tex, br_tex, string(p2s), points_p2s, texture_p2s, vex_size_p2s, tex_size_p2s);

   num_points_p1s = tex_size_p1s/sizeof(vec2);
   num_points_p2s = tex_size_p2s/sizeof(vec2);

   //build wins and tex_coords arrays
   string p1w, p2w;

   sin.clear();
   sin.str("");
   sin << player1_wins;
   sin >> p1w;

   sin.clear();
   sin.str("");
   sin << player2_wins;
   sin >> p2w;


   points_p1w = NULL;
   texture_p1w = NULL;
   points_p2w = NULL;
   texture_p2w = NULL;

   tl_tex = vec4(-0.45, 0.99, 0, 1);
   br_tex = vec4(-0.35, 0.95, 0, 1);
   build_texture_values( tl_tex, br_tex, string(p1w), points_p1w, texture_p1w, vex_size_p1w, tex_size_p1w);
   tl_tex = vec4( 0.89, 0.99, 0, 1);
   br_tex = vec4( 0.99, 0.95, 0, 1);
   build_texture_values( tl_tex, br_tex, string(p2w), points_p2w, texture_p2w, vex_size_p2w, tex_size_p2w);

   num_points_p1w = tex_size_p1w/sizeof(vec2);
   num_points_p2w = tex_size_p2w/sizeof(vec2);

   /*****************/

   // Initialize texture objects
   glGenTextures( 1, &texture );

   //bind and build data
   glBindTexture( GL_TEXTURE_2D, texture );

   //load texture image data
   glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, img_rows, img_cols, 0, GL_RGB, GL_UNSIGNED_BYTE, img_data );

   //set texture wrap mode x axis
   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );

   //set texture wrap mode y axis
   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

   //set magnification mode
   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

   //set minification mode
   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

   //activate single texture
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

   rebind_data();

}


void HUD::rebind_data( ){

   glBindBuffer( GL_ARRAY_BUFFER, buffer );
   glBufferData( GL_ARRAY_BUFFER, vex_size_p1 + vex_size_p2 + vex_size_p1s + vex_size_p2s + vex_size_p1w + vex_size_p2w +
                                  tex_size_p1 + tex_size_p2 + tex_size_p1s + tex_size_p2s + tex_size_p1w + tex_size_p2s, NULL, GL_STATIC_DRAW );

   // Specify an offset to keep track of where we're placing data in our
   //   vertex array buffer.  We'll use the same technique when we
   //   associate the offsets with vertex attribute pointers.
   GLintptr offset = 0;
   glBufferSubData( GL_ARRAY_BUFFER, offset, vex_size_p1, points_p1 );
   offset += vex_size_p1;
   glBufferSubData( GL_ARRAY_BUFFER, offset, vex_size_p2, points_p2 );
   offset += vex_size_p2;
   glBufferSubData( GL_ARRAY_BUFFER, offset, vex_size_p1s, points_p1s );
   offset += vex_size_p1s;
   glBufferSubData( GL_ARRAY_BUFFER, offset, vex_size_p2s, points_p2s );
   offset += vex_size_p2s;
   glBufferSubData( GL_ARRAY_BUFFER, offset, vex_size_p1w, points_p1w );
   offset += vex_size_p1w;
   glBufferSubData( GL_ARRAY_BUFFER, offset, vex_size_p2w, points_p2w );
   offset += vex_size_p2w;

   glBufferSubData( GL_ARRAY_BUFFER, offset, tex_size_p1, texture_p1 );
   offset += tex_size_p1;
   glBufferSubData( GL_ARRAY_BUFFER, offset, tex_size_p2, texture_p2 );
   offset += tex_size_p2;
   glBufferSubData( GL_ARRAY_BUFFER, offset, tex_size_p1s, texture_p1s );
   offset += tex_size_p1s;
   glBufferSubData( GL_ARRAY_BUFFER, offset, tex_size_p2s, texture_p2s );
   offset += tex_size_p2s;
   glBufferSubData( GL_ARRAY_BUFFER, offset, tex_size_p1w, texture_p1w );
   offset += tex_size_p1w;
   glBufferSubData( GL_ARRAY_BUFFER, offset, tex_size_p2w, texture_p2w );
   offset += tex_size_p2w;

   // set up vertex arrays
   offset = 0;
   GLuint vPosition = glGetAttribLocation( program, "vPosition" );
   glEnableVertexAttribArray( vPosition );
   glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset) );
   offset += vex_size_p1 + vex_size_p2 + vex_size_p1s + vex_size_p2s + vex_size_p1w + vex_size_p2w;

   GLuint vTexCoord = glGetAttribLocation( program, "vTexCoord" );
   glEnableVertexAttribArray( vTexCoord );
   glVertexAttribPointer( vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset) );

   drawmode = glGetUniformLocation( program, "drawmode");


}

void HUD::p1_score( const GLuint& score ){
   vec4 tl_tex;
   vec4 br_tex;

   player1_score = score;
   string p1s;

   stringstream sin;

   sin.clear();
   sin.str("");
   sin << player1_score;
   sin >> p1s;

   if( points_p1s != NULL ) delete points_p1s;
   points_p1s = NULL;
   if( texture_p1s != NULL ) delete texture_p1s;
   texture_p1s = NULL;

   tl_tex = vec4(-0.60, 0.99, 0, 1);
   br_tex = vec4(-0.50, 0.95, 0, 1);
   build_texture_values( tl_tex, br_tex, string(p1s), points_p1s, texture_p1s, vex_size_p1s, tex_size_p1s);

   num_points_p1s = tex_size_p1s/sizeof(vec2);
   rebind_data();
}

void HUD::p2_score( const GLuint& score ){
   vec4 tl_tex;
   vec4 br_tex;

   player2_score = score;
   string p2s;

   stringstream sin;
   sin.clear();
   sin.str("");
   sin << player2_score;
   sin >> p2s;
   
   if( points_p2s != NULL ) delete points_p2s;
   points_p2s = NULL;
   if( texture_p2s != NULL ) delete texture_p2s;
   texture_p2s = NULL;

   tl_tex = vec4( 0.74, 0.99, 0, 1);
   br_tex = vec4( 0.84, 0.95, 0, 1);
   build_texture_values( tl_tex, br_tex, string(p2s), points_p2s, texture_p2s, vex_size_p2s, tex_size_p2s);

   num_points_p2s = tex_size_p2s/sizeof(vec2);
   rebind_data();

}

void HUD::p1_win( const GLuint& score ){
   vec4 tl_tex;
   vec4 br_tex;

   player1_wins = score;
   string p1w;

   stringstream sin;
   sin.clear();
   sin.str("");
   sin << player1_wins;
   sin >> p1w;
   
   if( points_p1w != NULL ) delete points_p1w;
   points_p1w = NULL;
   if( texture_p1w != NULL ) delete texture_p1w;
   texture_p1w = NULL;

   tl_tex = vec4(-0.45, 0.99, 0, 1);
   br_tex = vec4(-0.35, 0.95, 0, 1);
   build_texture_values( tl_tex, br_tex, string(p1w), points_p1w, texture_p1w, vex_size_p1w, tex_size_p1w);

   num_points_p1w = tex_size_p1w/sizeof(vec2);
   rebind_data();

}



void HUD::p2_win( const GLuint& score ){
   vec4 tl_tex;
   vec4 br_tex;

   player2_wins = score;
   string p2w;

   stringstream sin;
   sin.clear();
   sin.str("");
   sin << player2_wins;
   sin >> p2w;
   
   if( points_p2w != NULL ) delete points_p2w;
   points_p2w = NULL;
   if( texture_p2w != NULL ) delete texture_p2w;
   texture_p2w = NULL;

   tl_tex = vec4( 0.89, 0.99, 0, 1);
   br_tex = vec4( 0.99, 0.95, 0, 1);
   build_texture_values( tl_tex, br_tex, string(p2w), points_p2w, texture_p2w, vex_size_p2w, tex_size_p2w);

   num_points_p2w = tex_size_p2w/sizeof(vec2);
   rebind_data();

}


void HUD::draw_shape(  ){
   
#ifdef __APPLE__
   glBindVertexArrayAPPLE( vao );
#else
   glBindVertexArray( vao ); 
#endif

   glUniform1i( drawmode, 2 );

   glBindBuffer( GL_ARRAY_BUFFER, buffer );
   glBindTexture( GL_TEXTURE_2D, texture );
   glDrawArrays( GL_TRIANGLES, 0, num_points_p1 + num_points_p2 + num_points_p1s + num_points_p2s + num_points_p1w + num_points_p2w);


}

size_t HUD::get_index( char const& letter ){
   if( letter >= '0' && letter <= '9' )
      return ((letter-'0')+26);
   else if( letter >= 'A' && letter <= 'Z' )
      return (letter-'A');
   else if( letter >= 'a' && letter <= 'z' )
      return (letter-'a');
   else
      return 36;

}

void HUD::build_texture_values( vec4 const& tl, vec4 const& br, string const& tag, vec4*& loc, vec2*& char_data, GLuint& vex_size, GLuint& tex_size){

   //find intervals for letters
   double ranX = (br.x-tl.x)/(double)tag.size();

   //create array of vertices corresponding to locations
   size_t size_loc = 6 * tag.size(); // 6 verts per char, then the number of letters
   loc = new vec4[size_loc];

   //iterate over interval to set in place the pixel locations
   double beg_x, end_x, z_val;
   for( size_t i=0; i<tag.size(); i++){
      //compute x ranges
      beg_x = i*ranX+tl.x; end_x = beg_x + ranX;

      //interpolate z value for step
      z_val = (1 - (i/(double)tag.size()))*tl.z + (i/(double)tag.size())*br.z;

      //load values into vertex array
      loc[i*6+0] = vec4( beg_x, br.y, z_val, 1);//bl
      loc[i*6+1] = vec4( end_x, br.y, z_val, 1);//br
      loc[i*6+2] = vec4( end_x, tl.y, z_val, 1);//tr
      loc[i*6+3] = vec4( beg_x, br.y, z_val, 1);//bl
      loc[i*6+4] = vec4( end_x, tl.y, z_val, 1);//tr
      loc[i*6+5] = vec4( beg_x, tl.y, z_val, 1);//tl

   }
   vex_size = sizeof(vec4)*6*tag.size();

   //now that we have the vertex locations built, lets go ahead
   //and figure out which characters go into the overall texture

   //initialize char_data array to store texture locations
   size_t size_char = 6 * tag.size();
   char_data = new vec2[size_char];

   //iterate over interval to set in place the texture locations
   size_t idx;
   for( size_t i=0; i<tag.size(); i++){

      //figure out which letter we are dealing with
      idx = get_index(tag[i]);

      //load values into vertex array
      char_data[i*6+0] = vec2( chr_ran[idx].x, 0.0);//bl
      char_data[i*6+1] = vec2( chr_ran[idx].y, 0.0);//br
      char_data[i*6+2] = vec2( chr_ran[idx].y, 1.0);//tr
      char_data[i*6+3] = vec2( chr_ran[idx].x, 0.0);//bl
      char_data[i*6+4] = vec2( chr_ran[idx].y, 1.0);//tr
      char_data[i*6+5] = vec2( chr_ran[idx].x, 1.0);//tl

   }
   tex_size = sizeof(vec2)*6*tag.size();

}


