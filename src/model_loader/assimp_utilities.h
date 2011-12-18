#ifndef __ASSIMP_UTILITIES_H__
#define __ASSIMP_UTILITIES_H__

#include "GL.h"
#include "mat.h"
#include "vec.h"

// assimp include files. These three are usually needed.
#include <assimp/assimp.h>
#include <assimp/aiPostProcess.h>
#include <assimp/aiScene.h>

#include <iostream>
#include <string>
#include <vector>

using namespace std;

/** 
   Load the scene using the scene loader  
*/
int  read_model( const string& filename, const aiScene*& scene );

/** 
  * Load the scene and return the vbo's
*/
void Load_Model_Data( const string& filename, mat4 const& rotation, 
                                              const double& scale, 
                                              vector<vec4>& vertices, 
                                              vector<vec4>& normals, 
                                              vector<vec4>& ambients, 
                                              vector<vec4>& diffuses, 
                                              vector<vec4>& speculars, 
                                              vector<GLfloat>& shininess, 
                                              GLuint& vertex_count );

#endif
