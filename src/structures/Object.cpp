#include "Object.h"

// Draw the normals for the object
//#define DEBUG_DRAW_NORMALS

Object::Object( const string& fname, const double& sf, const GLuint prog ){

   program  = prog;
   scale_factor = sf;
   model_name = fname;
   velocity = vec4(0.0,0.0,0.0,1.0);
   vertices = NULL;
   normals  = NULL;
   ambients = NULL;
   diffuses = NULL;
   speculars= NULL;
   shininess= NULL;

}

Object::~Object(){
   
   make_empty();

}

void Object::make_empty(){
   if( vertices != NULL )
      delete [] vertices;
   if( normals != NULL )
      delete [] normals;
   if( ambients != NULL )
      delete [] ambients;
   if( diffuses != NULL )
      delete [] diffuses;
   if( speculars != NULL )
      delete [] speculars;
   if( shininess != NULL )
      delete [] shininess;
}


void Object::init_buffers( const vec4& lpos, const vec4& l_amb, const vec4& l_diff, const vec4& l_spec, bool load_shader, int rotate){


   //load data from model
   model_data.load(model_name.c_str());

   //load model sizes
   vertex_count = model_data.faceCount*3;
   
   radians = rotate * M_PI / 180.0;
   
   //compute shininess and vertex array size
#ifdef DEBUG_DRAW_NORMALS
   vertices_size = sizeof(vec4)*vertex_count*3;
   shininess_size = sizeof(GLfloat)*vertex_count*3;
   
   //assign memory to arrays
   vertices  = new vec4[vertex_count*3];
   normals   = new vec4[vertex_count*3];
   ambients  = new vec4[vertex_count*3];
   diffuses  = new vec4[vertex_count*3];
   speculars = new vec4[vertex_count*3];
   shininess = new GLfloat[vertex_count*3];
#else
   vertices_size = sizeof(vec4)*vertex_count;
   shininess_size = sizeof(GLfloat)*vertex_count;
   
   //assign memory to arrays
   vertices  = new vec4[vertex_count];
   normals   = new vec4[vertex_count];
   ambients  = new vec4[vertex_count];
   diffuses  = new vec4[vertex_count];
   speculars = new vec4[vertex_count];
   shininess = new GLfloat[vertex_count];
#endif
   
   //Copy model data into arrays
   build_arrays( lpos, l_amb, l_diff, l_spec);

   ///Get width of object
   double x_min = vertices[0].x;
   double x_max = vertices[0].x;
   double y_min = vertices[0].y;
   double y_max = vertices[0].y;
   double z_min = vertices[0].z;
   double z_max = vertices[0].z;
   for(size_t i=1; i<vertex_count; i++){
      if( vertices[i].x < x_min )
         x_min = vertices[i].x;
      if( vertices[i].x > x_max )
         x_max = vertices[i].x;
      if( vertices[i].y < y_min )
         y_min = vertices[i].y;
      if( vertices[i].y > y_max )
         y_max = vertices[i].y;
      if( vertices[i].z < z_min )
         z_min = vertices[i].z;
      if( vertices[i].z > z_max )
         z_max = vertices[i].z;
   }
   x_width = x_max - x_min;
   y_width = y_max - y_min;
   z_width = z_max - z_min;
   centroid = vec3(  (x_max-x_min)/2.0+x_min,
                     (y_max-y_min)/2.0+y_min,
                     (z_max-z_min)/2.0+z_min);

   // in case we just want to read the model file
   if ( !load_shader )
      return;

   /// Initialize Shader
   // Create a vertex array object
#ifdef __APPLE__
   glGenVertexArraysAPPLE( 1, &vao );
   glBindVertexArrayAPPLE( vao );
#else
   glGenVertexArrays( 1, &vao );
   glBindVertexArray( vao ); 
#endif


   // Create and initialize a buffer object
   glGenBuffers( 1, &vbo );
   glBindBuffer( GL_ARRAY_BUFFER, vbo );
   glBufferData( GL_ARRAY_BUFFER, 5*vertices_size, NULL, GL_STATIC_DRAW );
   glBufferSubData( GL_ARRAY_BUFFER, 0*vertices_size, vertices_size, vertices );
   glBufferSubData( GL_ARRAY_BUFFER, 1*vertices_size, vertices_size, normals );
   glBufferSubData( GL_ARRAY_BUFFER, 2*vertices_size, vertices_size, ambients );
   glBufferSubData( GL_ARRAY_BUFFER, 3*vertices_size, vertices_size, diffuses );
   glBufferSubData( GL_ARRAY_BUFFER, 4*vertices_size, vertices_size, speculars );
   glBufferSubData( GL_ARRAY_BUFFER, 5*vertices_size, shininess_size, shininess );

   //load program
   glUseProgram( program );

   // get ids of and set up attribute variables
   GLuint vPosition = glGetAttribLocation( program, "vPosition" );
   glEnableVertexAttribArray( vPosition );
   glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
         BUFFER_OFFSET(0) );

   GLuint vNormal = glGetAttribLocation( program, "vNormal" ); 
   glEnableVertexAttribArray( vNormal );
   glVertexAttribPointer( vNormal, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET((1*vertices_size)));

   GLuint vAmbient = glGetAttribLocation( program, "vAmbient" ); 
   glEnableVertexAttribArray( vAmbient );
   glVertexAttribPointer( vAmbient, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET((2*vertices_size)));

   GLuint vDiffuse = glGetAttribLocation( program, "vDiffuse" ); 
   glEnableVertexAttribArray( vDiffuse );
   glVertexAttribPointer( vDiffuse, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET((3*vertices_size)));

   GLuint vSpecular = glGetAttribLocation( program, "vSpecular" ); 
   glEnableVertexAttribArray( vSpecular );
   glVertexAttribPointer( vSpecular, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET((4*vertices_size)));

   GLuint vShiny    = glGetAttribLocation( program, "vShiny" ); 
   glEnableVertexAttribArray( vShiny );
   glVertexAttribPointer( vShiny, 1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET((5*vertices_size)));

   // get ids of uniform variables
   worldview = glGetUniformLocation( program, "worldview" );
   projection = glGetUniformLocation( program, "projection" );
   light_position = glGetUniformLocation( program, "light_position");
   drawmode = glGetUniformLocation( program, "drawmode");
   translation_id = glGetUniformLocation( program, "translation");
   rotation_id = glGetUniformLocation( program, "rotation" );

   translation = vec4(0,0,0,1);
}

void Object::draw_shape( mat4 const& worldviewMat, mat4 const& proj, vec4 const& l_pos ){

   glUseProgram(program);
#ifdef __APPLE__
   glBindVertexArrayAPPLE( vao );
#else
   glBindVertexArray( vao ); 
#endif
   
   glUniform4fv( translation_id, 1, translation );
   glUniformMatrix4fv( rotation_id, 1, GL_TRUE, rotation );
   glUniform4fv( light_position,   1, l_pos );
   glUniformMatrix4fv( projection, 1, GL_TRUE, proj );
   glUniformMatrix4fv( worldview, 1, GL_TRUE, worldviewMat);

   glUniform1i( drawmode, (GLint)TRIANGLES );
   glDrawArrays( GL_TRIANGLES, 0, vertex_count );

#ifdef DEBUG_DRAW_NORMALS
   glUniform1i( drawmode, (GLint)LINES );
   glDrawArrays( GL_LINES, vertex_count, vertex_count*2 );
#endif

}
      
double Object::get_x_width()const{
   return x_width;
}
double Object::get_y_width()const{
   return y_width;
}
double Object::get_z_width()const{
   return z_width;
}

vec3 Object::get_dimensions()const{
   return vec3(get_x_width(), get_y_width(), get_z_width());
}

vec3 Object::get_centroid()const{
   return centroid+get_translation();  
}

void Object::build_arrays( const vec4& lpos, const vec4& l_amb, const vec4& l_dif, const vec4& l_spec ){

   mat4 rotate(cos(radians),  0.0, sin(radians), 0.0,
               0.0,           1.0, 0.0,          0.0,
               -sin(radians), 0.0, cos(radians), 0.0,
               0.0,           0.0, 0.0,          1.0);

   // copy vertex info into vertices and colors
   for ( int i = 0; i < model_data.faceCount; ++i )
   {
      obj_face *o = model_data.faceList[i];
      for ( int j = 0; j < 3; ++j )
      {
         vertices[3*i+j] = rotate*
               vec4(model_data.vertexList[o->vertex_index[j]]->e[0]*scale_factor,
                    model_data.vertexList[o->vertex_index[j]]->e[1]*scale_factor,
                    model_data.vertexList[o->vertex_index[j]]->e[2]*scale_factor,
                    1.0);

         normals[3*i+j] = rotate*
               vec4( model_data.normalList[o->normal_index[j]]->e[0],
                     model_data.normalList[o->normal_index[j]]->e[1],
                     model_data.normalList[o->normal_index[j]]->e[2],
                     1.0);

         diffuses[3*i+j] = 
               vec4(model_data.materialList[o->material_index]->diff[0],
                    model_data.materialList[o->material_index]->diff[1],
                    model_data.materialList[o->material_index]->diff[2],
                    1.0);
         
         ambients[3*i+j] =
               vec4(model_data.materialList[o->material_index]->amb[0],
                    model_data.materialList[o->material_index]->amb[1],
                    model_data.materialList[o->material_index]->amb[2],
                    1.0);
         
         speculars[3*i+j] =
               vec4(model_data.materialList[o->material_index]->spec[0],
                    model_data.materialList[o->material_index]->spec[1],
                    model_data.materialList[o->material_index]->spec[2],
                    1.0);
         
         shininess[3*i+j] = model_data.materialList[o->material_index]->shiny;

#ifdef DEBUG_DRAW_NORMALS
         vertices[vertex_count+(3*i+j)*2] = vertices[3*i+j];
         vertices[vertex_count+(3*i+j)*2+1] =
            vertices[3*i+j]+normals[3*i+j]*0.1*scale_factor;
         vertices[vertex_count+(3*i+j)*2+1].w = 1.0;
         
         diffuses[vertex_count+(3*i+j)*2] = vec4(0.4,1.0,0.4,1.0);
         diffuses[vertex_count+(3*i+j)*2+1] = vec4(0.4,1.0,0.4,1.0);

         ambients[vertex_count+(3*i+j)*2] = l_amb;
         ambients[vertex_count+(3*i+j)*2+1] = l_amb;
         
         speculars[vertex_count+(3*i+j)*2] = l_spec;
         speculars[vertex_count+(3*i+j)*2+1] = l_spec;
         
         shininess[vertex_count+(3*i+j)*2] = 1.0;
         shininess[vertex_count+(3*i+j)*2+1] = 1.0;
#endif      
      }
   }
   
   /*************************************************/
   /*    Iterate through materials, multiplying the */
   /*    light material to the material.            */
   /*************************************************/
   for(size_t i=0; i<vertex_count; i++){
      ambients[i]  *= l_amb;
      diffuses[i]  *= l_dif;
      speculars[i] *= l_spec;
   }
   
}
      
void Object::adjust_translation( const vec3& motion ){

   velocity = vec4(motion.x,motion.y,motion.z,1.0);
   
   translation.x += motion.x;
   translation.y += motion.y;
   translation.z += motion.z;
}

void Object::adjust_rotation( const mat4& rotMat ){
   rotation = rotMat*rotation;
}

void Object::set_rotation( const mat4& rotMat ){
   rotation = rotMat;
}

const mat4& Object::get_rotation() const{
   return rotation;
}

vec3 Object::get_translation()const{
   return vec3( translation.x, translation.y, translation.z);
}

const vec4& Object::get_velocity() const{
   return velocity;
}

void Object::set_translation( const vec3& trans ){
   velocity = vec4(trans.x,trans.y,trans.z,0.0) - translation;
   
   translation.x = trans.x;
   translation.y = trans.y;
   translation.z = trans.z;
}

vec4* Object::get_vertices(){
   return vertices;
}

size_t Object::num_verts(){
   return vertex_count;
}
