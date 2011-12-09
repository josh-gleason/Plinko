#include "Scoreboard.h"

/*****************************************/
/*              Load an image            */
/*****************************************/
unsigned char* Scoreboard::read_image(char* fname)
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
Scoreboard::Scoreboard( ){


}

Scoreboard::~Scoreboard( ){

   make_empty();

   ofstream fout;
   
   string name, tname;
   int temp;
   if( p1w > sc3 || p2w > sc3 ){
      cout << "Please enter a three letter name for the record books" << endl;
      cin >> name;
      usr3 = name; 
   
      if( p1w > p2w ){
         sc3 = p1w;
      }
      else{
         sc3 = p2w;
      }

      if(sc3 > sc2){
         temp = sc3;
         sc3 = sc2;
         sc2 = temp;
         tname = usr3;
         usr3 = usr2;
         usr2 = tname; 
      }
      if(sc2 > sc1){
         temp = sc2;
         sc2 = sc1;
         sc1 = temp;
         tname = usr2;
         usr2 = usr1;
         usr1 = tname; 
      }
   }
   
   fout.open("data/scores.txt");
   fout << usr1 << "  " << sc1 << endl;
   fout << usr2 << "  " << sc2 << endl;
   fout << usr3 << "  " << sc3 << endl;
   fout.close();


}

void Scoreboard::make_empty(){
   if( points_main != NULL )
      delete [] points_main;
   if( tex_coords_main != NULL )
      delete [] tex_coords_main;
}

void Scoreboard::win_count( const int p1, const int p2 ){
   p1w = p1;
   p2w = p2;
}

void Scoreboard::init( const GLuint& prog ){
   
   // Load shaders and use the resulting shader program
   program = prog;
   
   //read image from file
   img_data = read_image( "data/characters_flip.ppm" );
   
   ifstream fin;
   fin.open("data/scores.txt");
   fin >> usr1 >> sc1 >> usr2 >> sc2 >> usr3 >> sc3;
   fin.close();
   
   stringstream sin;

   sin.clear();
   sin.str("");
   sin << sc1;
   sin >> scp1;

   sin.clear();
   sin.str("");
   sin << sc2;
   sin >> scp2;

   sin.clear();
   sin.str("");
   sin << sc3;
   sin >> scp3;
   
   /*********************************/
   /*       High Score Banner       */
   /*********************************/
   //build points and tex_coords arrays
   points_main = NULL;
   tex_coords_main = NULL;
   vec4 tl_tex(-0.75, 0.9, 0, 1);
   vec4 br_tex( 0.75, 0.7, 0, 1);
   build_texture_values( tl_tex, br_tex, string("HIGH SCORES"), points_main, tex_coords_main, vex_size_main, tex_size_main);

   num_points_main = tex_size_main/sizeof(vec2);

   /*****************/
   /*    PLAYER 1   */
   /*****************/
   //build points and tex_coords arrays
   points_p1 = NULL;
   tex_coords_p1 = NULL;
   tl_tex = vec4(-0.75, 0.4, 0, 1);
   br_tex = vec4( 0.75, 0.2, 0, 1);
   build_texture_values( tl_tex, br_tex, string(usr1+string("  ")+scp1), points_p1, tex_coords_p1, vex_size_p1, tex_size_p1);

   num_points_p1 = tex_size_p1/sizeof(vec2);
  
   /*****************/
   /*    PLAYER 2   */
   /*****************/
   //build points and tex_coords arrays
   points_p2 = NULL;
   tex_coords_p2 = NULL;
   tl_tex = vec4(-0.75, 0.2, 0, 1);
   br_tex = vec4( 0.75, 0.0, 0, 1);
   build_texture_values( tl_tex, br_tex, string(usr2+string("  ")+scp2), points_p2, tex_coords_p2, vex_size_p2, tex_size_p2);

   num_points_p2 = tex_size_p2/sizeof(vec2); 
   
   /*****************/
   /*    PLAYER 3   */
   /*****************/
   //build points and tex_coords arrays
   points_p3 = NULL;
   tex_coords_p3 = NULL;
   tl_tex = vec4(-0.75, 0.0, 0, 1);
   br_tex = vec4( 0.75,-0.2, 0, 1);
   build_texture_values( tl_tex, br_tex, string(usr3+string("  ")+scp3), points_p3, tex_coords_p3, vex_size_p3, tex_size_p3);

   num_points_p3 = tex_size_p3/sizeof(vec2);
   
   
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
   glBufferData( GL_ARRAY_BUFFER, vex_size_main + vex_size_p1 + vex_size_p2 + vex_size_p3 + 
                                  tex_size_main + tex_size_p1 + tex_size_p2 + tex_size_p3, NULL, GL_STATIC_DRAW );

   // Specify an offset to keep track of where we're placing data in our
   //   vertex array buffer.  We'll use the same technique when we
   //   associate the offsets with vertex attribute pointers.
   GLintptr offset = 0;
   glBufferSubData( GL_ARRAY_BUFFER, offset, vex_size_p1, points_p1 );
   offset += vex_size_p1;
   glBufferSubData( GL_ARRAY_BUFFER, offset, vex_size_p2, points_p2 );
   offset += vex_size_p2;
   glBufferSubData( GL_ARRAY_BUFFER, offset, vex_size_p3, points_p3 );
   offset += vex_size_p3;
   glBufferSubData( GL_ARRAY_BUFFER, offset, vex_size_main, points_main );
   offset += vex_size_main;

   glBufferSubData( GL_ARRAY_BUFFER, offset, tex_size_main, tex_coords_p1 );
   offset += tex_size_p1;
   glBufferSubData( GL_ARRAY_BUFFER, offset, tex_size_main, tex_coords_p2 );
   offset += tex_size_p2;
   glBufferSubData( GL_ARRAY_BUFFER, offset, tex_size_main, tex_coords_p3 );
   offset += tex_size_p3;



   glBufferSubData( GL_ARRAY_BUFFER, offset, tex_size_main, tex_coords_main );

   // set up vertex arrays
   offset = 0;
   GLuint vPosition = glGetAttribLocation( program, "vPosition" );
   glEnableVertexAttribArray( vPosition );
   glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset) );
   offset += vex_size_main + vex_size_p1 + vex_size_p2 + vex_size_p3;

   GLuint vTexCoord = glGetAttribLocation( program, "vTexCoord" );
   glEnableVertexAttribArray( vTexCoord );
   glVertexAttribPointer( vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset) );

   drawmode = glGetUniformLocation( program, "drawmode");
   glEnable( GL_DEPTH_TEST );

}



void Scoreboard::draw_shape(  ){

#ifdef __APPLE__
   glBindVertexArrayAPPLE( vao );
#else
   glBindVertexArray( vao ); 
#endif

   glUniform1i( drawmode, 2 );

   glBindBuffer( GL_ARRAY_BUFFER, buffer );
   glBindTexture( GL_TEXTURE_2D, texture );
   glDrawArrays( GL_TRIANGLES, 0, num_points_main+num_points_p1+num_points_p2+num_points_p3);


}

size_t Scoreboard::get_index( char const& letter ){
   if( letter >= '0' && letter <= '9' )
      return ((letter-'0')+26);
   else if( letter >= 'A' && letter <= 'Z' )
      return (letter-'A');
   else if( letter >= 'a' && letter <= 'z' )
      return (letter-'a');
   else
      return 36;

}

void Scoreboard::build_texture_values( vec4 const& tl, vec4 const& br, string const& tag, vec4*& loc, vec2*& char_data, GLuint& vex_size, GLuint& tex_size){

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


