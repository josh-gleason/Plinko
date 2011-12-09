#include "Winner.h"

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

/*****************************************/
/*              Load an image            */
/*****************************************/
unsigned char* Winner::read_image(char* fname)
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
Winner::Winner( ){


}

Winner::~Winner( ){

   make_empty();

}

void Winner::make_empty(){
   if( points_main != NULL )
      delete [] points_main;
   if( tex_coords_main != NULL )
      delete [] tex_coords_main;
   if( points_p != NULL )
      delete [] points_p;
   if( tex_coords_p != NULL )
      delete [] tex_coords_p;
}


void Winner::init( const GLuint& prog, bool const& winner ){
   
   // Load shaders and use the resulting shader program
   program = prog;
   
   //read image from file
   img_data = read_image( "data/characters_flip.ppm" );
   
   /*********************************/
   /*       High Score Banner       */
   /*********************************/
   //build points and tex_coords arrays
   points_main = NULL;
   tex_coords_main = NULL;
   vec4 tl_tex(-0.75, 0.9, 0, 1);
   vec4 br_tex( 0.75, 0.5, 0, 1);
   build_texture_values( tl_tex, br_tex, string("WINNER"), points_main, tex_coords_main, vex_size_main, tex_size_main);

   num_points_main = tex_size_main/sizeof(vec2);

   /****************/
   /*    PLAYER    */
   /****************/
   //build points and tex_coords arrays
   points_p = NULL;
   tex_coords_p = NULL;
   tl_tex = vec4(-0.75, 0.0, 0, 1);
   br_tex = vec4( 0.75,-0.4, 0, 1);
   if( winner == false )
      build_texture_values( tl_tex, br_tex, string("PLAYER 1"), points_p, tex_coords_p, vex_size_p, tex_size_p);
   else
      build_texture_values( tl_tex, br_tex, string("PLAYER 2"), points_p, tex_coords_p, vex_size_p, tex_size_p);

   num_points_p = tex_size_p/sizeof(vec2);

   /*****************/
   /*    PLAYER 2   */
   /*****************/
   //build points and tex_coords arrays
   points_p2 = NULL;
   tex_coords_p2 = NULL;
   tl_tex = vec4(-0.75,-0.5, 0, 1);
   br_tex = vec4( 0.75,-0.7, 0, 1);
   build_texture_values( tl_tex, br_tex, string("PRESS ANY KEY TO CONTINUE"), points_p2, tex_coords_p2, vex_size_p2, tex_size_p2);

   num_points_p2 = tex_size_p2/sizeof(vec2); 

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
   glBindBuffer( GL_ARRAY_BUFFER, buffer );
   glBufferData( GL_ARRAY_BUFFER, vex_size_main + vex_size_p + vex_size_p2 + 
                                  tex_size_main + tex_size_p + tex_size_p2 , NULL, GL_STATIC_DRAW );

   // Specify an offset to keep track of where we're placing data in our
   //   vertex array buffer.  We'll use the same technique when we
   //   associate the offsets with vertex attribute pointers.
   GLintptr offset = 0;
   glBufferSubData( GL_ARRAY_BUFFER, offset, vex_size_main, points_main );
   offset += vex_size_main;
   glBufferSubData( GL_ARRAY_BUFFER, offset, vex_size_p, points_p );
   offset += vex_size_p;
   glBufferSubData( GL_ARRAY_BUFFER, offset, vex_size_p2, points_p2 );
   offset += vex_size_p2;

   glBufferSubData( GL_ARRAY_BUFFER, offset, tex_size_main, tex_coords_main );
   offset += tex_size_main;
   glBufferSubData( GL_ARRAY_BUFFER, offset, tex_size_p, tex_coords_p );
   offset += tex_size_p;
   glBufferSubData( GL_ARRAY_BUFFER, offset, tex_size_p2, tex_coords_p2 );

   // set up vertex arrays
   offset = 0;
   GLuint vPosition = glGetAttribLocation( program, "vPosition" );
   glEnableVertexAttribArray( vPosition );
   glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset) );
   offset += vex_size_main + vex_size_p + vex_size_p2;

   GLuint vTexCoord = glGetAttribLocation( program, "vTexCoord" );
   glEnableVertexAttribArray( vTexCoord );
   glVertexAttribPointer( vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset) );

   drawmode = glGetUniformLocation( program, "drawmode");

}

void Winner::draw_shape(  ){
   
#ifdef __APPLE__
   glBindVertexArrayAPPLE( vao );
#else
   glBindVertexArray( vao ); 
#endif

   glUniform1i( drawmode, 2 );

   glBindBuffer( GL_ARRAY_BUFFER, buffer );
   glBindTexture( GL_TEXTURE_2D, texture );
   glDrawArrays( GL_TRIANGLES, 0, num_points_main + num_points_p + num_points_p2);


}

size_t Winner::get_index( char const& letter ){
   if( letter >= '0' && letter <= '9' )
      return ((letter-'0')+26);
   else if( letter >= 'A' && letter <= 'Z' )
      return (letter-'A');
   else if( letter >= 'a' && letter <= 'z' )
      return (letter-'a');
   else
      return 36;

}

void Winner::build_texture_values( vec4 const& tl, vec4 const& br, string const& tag, vec4*& loc, vec2*& char_data, GLuint& vex_size, GLuint& tex_size){

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
      char_data[i*6+0] = vec2( char_ran[idx].x, 0.0);//bl
      char_data[i*6+1] = vec2( char_ran[idx].y, 0.0);//br
      char_data[i*6+2] = vec2( char_ran[idx].y, 1.0);//tr
      char_data[i*6+3] = vec2( char_ran[idx].x, 0.0);//bl
      char_data[i*6+4] = vec2( char_ran[idx].y, 1.0);//tr
      char_data[i*6+5] = vec2( char_ran[idx].x, 1.0);//tl

   }
   tex_size = sizeof(vec2)*6*tag.size();

}


