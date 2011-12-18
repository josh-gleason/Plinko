#include "assimp_utilities.h"

void set_float4(float f[4], float a, float b, float c, float d)
{
	f[0] = a;
	f[1] = b;
	f[2] = c;
	f[3] = d;
}

/** 
   Load the scene using the scene loader  
*/
int  read_model( const string& filename, const aiScene*& scene ){
   
   /** There should be a check here that the scene is already null */
   scene = aiImportFile( filename.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);

   if( scene == NULL ){
      cout << "ERROR: Model not loaded" << endl;
      exit(0);
   }
   return 1;
}

void apply_material(const aiMaterial *mtl, vec4& diffuse, vec4& ambient, vec4& specular, double& shininess){
	
   aiColor3D color;

   //set a default
   if(AI_SUCCESS == mtl->Get(AI_MATKEY_COLOR_DIFFUSE, color))
      diffuse = vec4(color.r, color.g, color.b, 1);
   else
      diffuse = vec4(0.8, 0.8, 0.8, 1.0);
   
   if(AI_SUCCESS == mtl->Get(AI_MATKEY_COLOR_AMBIENT, color))
      ambient = vec4(color.r, color.g, color.b, 1);
   else
      ambient = vec4(0.2, 0.2, 0.2, 1.0);
   
   if(AI_SUCCESS == mtl->Get(AI_MATKEY_COLOR_SPECULAR, color))
      specular = vec4(color.r, color.g, color.b, 1);
   else
      specular = vec4(0.0, 0.0, 0.0, 1.0);
   
   if( AI_SUCCESS == mtl->Get(AI_MATKEY_SHININESS, shininess ));
   else 
      shininess = 0;

   /*
   set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
   if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &emission))
      color4_to_float4(&emission, c);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, c);

   max = 1;
   ret1 = aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &shininess, &max);
   if(ret1 == AI_SUCCESS) {
      max = 1;
      ret2 = aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS_STRENGTH, &strength, &max);
      if(ret2 == AI_SUCCESS)
         glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess * strength);
      else
         glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
   }
   else {
      glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);
      set_float4(c, 0.0f, 0.0f, 0.0f, 0.0f);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c);
   }

   max = 1;
   if(AI_SUCCESS == aiGetMaterialIntegerArray(mtl, AI_MATKEY_ENABLE_WIREFRAME, &wireframe, &max))
      fill_mode = wireframe ? GL_LINE : GL_FILL;
   else
      fill_mode = GL_FILL;
   glPolygonMode(GL_FRONT_AND_BACK, fill_mode);

   max = 1;
   if((AI_SUCCESS == aiGetMaterialIntegerArray(mtl, AI_MATKEY_TWOSIDED, &two_sided, &max)) && two_sided)
      glDisable(GL_CULL_FACE);
   else 
      glEnable(GL_CULL_FACE);
      */
}




/** 
 * Load the scene and return the vbo's
 *
 * @param[in] filename - name of model to load
 * @param[out] vertices  - vertex buffer object
 * @param[out] normals   - face normal buffer object
 * @param[out] ambients  - ambient light buffer object
 * @param[out] diffuses  - diffuse light buffer object
 * @param[out] speculars  - specular light buffer object
 * @param[out] shininess - shininess coefficient buffer object
 * @param[out] vertex_count - number of vertices in object
 */
void Load_Model_Data( const string& filename, const mat4& rotation, const double& scale, 
      vector<vec4>& vertices, 
      vector<vec4>& normals, 
      vector<vec4>& ambients, 
      vector<vec4>& diffuses, 
      vector<vec4>& speculars, 
      vector<GLfloat>& shininess, 
      GLuint& vertex_count ){

   //clear all arrays
   vertices.clear();
   normals.clear();
   ambients.clear();
   diffuses.clear();
   speculars.clear();
   shininess.clear();

   //create list of indeces
   vector<int> indeces;

   //create scene object
   const aiScene* scene;

   //read the model 
   read_model( filename, scene);

   //iterate over each mesh
   for( size_t i=0; i < scene->mNumMeshes; i++){

      //pull the address for the mesh
      const aiMesh * mesh = scene->mMeshes[i];

      //pull the material properties for the element
      vec4 diff, amb, spec;
      double shin;
      if( scene->mMaterials[mesh->mMaterialIndex] != NULL )
         apply_material(scene->mMaterials[mesh->mMaterialIndex], diff, amb, spec, shin);


      for (size_t t = 0; t < mesh->mNumFaces; ++t) {
         const aiFace* face = &mesh->mFaces[t];

         for(size_t k = 0; k < face->mNumIndices; k++) {
            int index = face->mIndices[k];
            if(mesh->mNormals != NULL) 
               normals.push_back(  rotation * vec4( mesh->mNormals[index].x, mesh->mNormals[index].y, mesh->mNormals[index].z, 1));
            if(mesh->mVertices != NULL )   
               vertices.push_back( rotation * vec4( mesh->mVertices[index].x*scale, mesh->mVertices[index].y*scale, mesh->mVertices[index].z*scale, 1));
            diffuses.push_back(diff);
            ambients.push_back(amb);
            speculars.push_back(spec);
            shininess.push_back(shin);
         }
      }
   }

   vertex_count = vertices.size();
   /*

      unsigned int *faceArray;
      faceArray = (unsigned int *)malloc(sizeof(unsigned int) * mesh->mNumFaces * 3);
      unsigned int faceIndex = 0;

      for (unsigned int t = 0; t < mesh->mNumFaces; ++t) {
      const struct aiFace* face = &mesh->mFaces[t];

      memcpy(&faceArray[faceIndex], face->mIndices,3 * sizeof(float));
      faceIndex += 3;
      }
      aMesh.numFaces = sc->mMeshes[n]->mNumFaces;

   // generate Vertex Array for mesh
   glGenVertexArrays(1,&(aMesh.vao));
   glBindVertexArray(aMesh.vao);

   // buffer for faces
   glGenBuffers(1, &buffer);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh->mNumFaces * 3, faceArray, GL_STATIC_DRAW);

   // buffer for vertex positions
   if (mesh->HasPositions()) {
   glGenBuffers(1, &buffer);
   glBindBuffer(GL_ARRAY_BUFFER, buffer);
   glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*mesh->mNumVertices, mesh->mVertices, GL_STATIC_DRAW);
   glEnableVertexAttribArray(vertexLoc);
   glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, 0, 0, 0);
   }

   // buffer for vertex normals
   if (mesh->HasNormals()) {
   glGenBuffers(1, &buffer);
   glBindBuffer(GL_ARRAY_BUFFER, buffer);
   glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*mesh->mNumVertices, mesh->mNormals, GL_STATIC_DRAW);
   glEnableVertexAttribArray(normalLoc);
   glVertexAttribPointer(normalLoc, 3, GL_FLOAT, 0, 0, 0);
   }

   // buffer for vertex texture coordinates
   if (mesh->HasTextureCoords(0)) {
   float *texCoords = (float *)malloc(sizeof(float)*2*mesh->mNumVertices);
   for (unsigned int k = 0; k < mesh->mNumVertices; ++k) {

   texCoords[k*2]   = mesh->mTextureCoords[0][k].x;
   texCoords[k*2+1] = mesh->mTextureCoords[0][k].y;

   }
   glGenBuffers(1, &buffer);
   glBindBuffer(GL_ARRAY_BUFFER, buffer);
   glBufferData(GL_ARRAY_BUFFER, sizeof(float)*2*mesh->mNumVertices, texCoords, GL_STATIC_DRAW);
   glEnableVertexAttribArray(texCoordLoc);
   glVertexAttribPointer(texCoordLoc, 2, GL_FLOAT, 0, 0, 0);
   }

   // unbind buffers
   glBindVertexArray(0);
   glBindBuffer(GL_ARRAY_BUFFER,0);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

   // create material uniform buffer
   struct aiMaterial *mtl = sc->mMaterials[mesh->mMaterialIndex];

   aiString texPath;   //contains filename of texture
   if(AI_SUCCESS == mtl->GetTexture(aiTextureType_DIFFUSE, 0, &texPath)){
   //bind texture
   unsigned int texId = textureIdMap[texPath.data];
   aMesh.texIndex = texId;
   aMat.texCount = 1;
   }
   else
      aMat.texCount = 0;

   float c[4];
   set_float4(c, 0.8f, 0.8f, 0.8f, 1.0f);
   aiColor4D diffuse;
   if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
      color4_to_float4(&diffuse, c);
   memcpy(aMat.diffuse, c, sizeof(c));

   set_float4(c, 0.2f, 0.2f, 0.2f, 1.0f);
   aiColor4D ambient;
   if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &ambient))
      color4_to_float4(&ambient, c);
   memcpy(aMat.ambient, c, sizeof(c));

   set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
   aiColor4D specular;
   if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular))
      color4_to_float4(&specular, c);
   memcpy(aMat.specular, c, sizeof(c));

   set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
   aiColor4D emission;
   if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &emission))
      color4_to_float4(&emission, c);
   memcpy(aMat.emissive, c, sizeof(c));

   float shininess = 0.0;
   unsigned int max;
   aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &shininess, &max);
   aMat.shininess = shininess;

   glGenBuffers(1,&(aMesh.uniformBlockIndex));
   glBindBuffer(GL_UNIFORM_BUFFER,aMesh.uniformBlockIndex);
   glBufferData(GL_UNIFORM_BUFFER, sizeof(aMat), (void *)(&aMat), GL_STATIC_DRAW);

   myMeshes.push_back(aMesh);
}

*/

}


/*




// the global Assimp scene object
const struct aiScene* scene = NULL;
GLuint scene_list = 0;
struct aiVector3D scene_min, scene_max, scene_center;

// current rotation angle
static float angle = 0.f;

#define aisgl_min(x,y) (x<y?x:y)
#define aisgl_max(x,y) (y>x?y:x)

// ----------------------------------------------------------------------------
void reshape(int width, int height)
{
const double aspectRatio = (float) width / height, fieldOfView = 45.0;

glMatrixMode(GL_PROJECTION);
glLoadIdentity();
gluPerspective(fieldOfView, aspectRatio,
1.0, 1000.0);  * Znear and Zfar *
glViewport(0, 0, width, height);
}

// ----------------------------------------------------------------------------
void get_bounding_box_for_node (const struct aiNode* nd, struct aiVector3D* min, struct aiVector3D* max, struct aiMatrix4x4* trafo ){
struct aiMatrix4x4 prev;
unsigned int n = 0, t;

prev = *trafo;
aiMultiplyMatrix4(trafo,&nd->mTransformation);

for (; n < nd->mNumMeshes; ++n) {
const struct aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];
for (t = 0; t < mesh->mNumVertices; ++t) {

struct aiVector3D tmp = mesh->mVertices[t];
aiTransformVecByMatrix4(&tmp,trafo);

min->x = aisgl_min(min->x,tmp.x);
min->y = aisgl_min(min->y,tmp.y);
min->z = aisgl_min(min->z,tmp.z);

max->x = aisgl_max(max->x,tmp.x);
max->y = aisgl_max(max->y,tmp.y);
max->z = aisgl_max(max->z,tmp.z);
}
}

for (n = 0; n < nd->mNumChildren; ++n) {
get_bounding_box_for_node(nd->mChildren[n],min,max,trafo);
}
 *trafo = prev;
 }

// ----------------------------------------------------------------------------
void get_bounding_box (struct aiVector3D* min, struct aiVector3D* max)
{
struct aiMatrix4x4 trafo;
aiIdentityMatrix4(&trafo);

min->x = min->y = min->z =  1e10f;
max->x = max->y = max->z = -1e10f;
get_bounding_box_for_node(scene->mRootNode,min,max,&trafo);
}

// ----------------------------------------------------------------------------
void color4_to_float4(const struct aiColor4D *c, float f[4])
{
   f[0] = c->r;
   f[1] = c->g;
   f[2] = c->b;
   f[3] = c->a;
}

// ----------------------------------------------------------------------------
void set_float4(float f[4], float a, float b, float c, float d)
{
   f[0] = a;
   f[1] = b;
   f[2] = c;
   f[3] = d;
}

// ----------------------------------------------------------------------------
void apply_material(const struct aiMaterial *mtl)
{
   float c[4];

   GLenum fill_mode;
   int ret1, ret2;
   struct aiColor4D diffuse;
   struct aiColor4D specular;
   struct aiColor4D ambient;
   struct aiColor4D emission;
   float shininess, strength;
   int two_sided;
   int wireframe;
   int max;

   set_float4(c, 0.8f, 0.8f, 0.8f, 1.0f);
   if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
      color4_to_float4(&diffuse, c);
   glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, c);

   set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
   if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular))
      color4_to_float4(&specular, c);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c);

   set_float4(c, 0.2f, 0.2f, 0.2f, 1.0f);
   if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &ambient))
      color4_to_float4(&ambient, c);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, c);

   set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
   if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &emission))
      color4_to_float4(&emission, c);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, c);

   max = 1;
   ret1 = aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &shininess, &max);
   if(ret1 == AI_SUCCESS) {
      max = 1;
      ret2 = aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS_STRENGTH, &strength, &max);
      if(ret2 == AI_SUCCESS)
         glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess * strength);
      else
         glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
   }
   else {
      glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);
      set_float4(c, 0.0f, 0.0f, 0.0f, 0.0f);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c);
   }

   max = 1;
   if(AI_SUCCESS == aiGetMaterialIntegerArray(mtl, AI_MATKEY_ENABLE_WIREFRAME, &wireframe, &max))
      fill_mode = wireframe ? GL_LINE : GL_FILL;
   else
      fill_mode = GL_FILL;
   glPolygonMode(GL_FRONT_AND_BACK, fill_mode);

   max = 1;
   if((AI_SUCCESS == aiGetMaterialIntegerArray(mtl, AI_MATKEY_TWOSIDED, &two_sided, &max)) && two_sided)
      glDisable(GL_CULL_FACE);
   else 
      glEnable(GL_CULL_FACE);
}

// ----------------------------------------------------------------------------
void recursive_render (const struct aiScene *sc, const struct aiNode* nd)
{
   int i;
   unsigned int n = 0, t;
   struct aiMatrix4x4 m = nd->mTransformation;

   // update transform
   aiTransposeMatrix4(&m);
   glPushMatrix();
   glMultMatrixf((float*)&m);

   // draw all meshes assigned to this node
   for (; n < nd->mNumMeshes; ++n) {
      const struct aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];

      apply_material(sc->mMaterials[mesh->mMaterialIndex]);

      if(mesh->mNormals == NULL) {
         glDisable(GL_LIGHTING);
      } else {
         glEnable(GL_LIGHTING);
      }

      for (t = 0; t < mesh->mNumFaces; ++t) {
         const struct aiFace* face = &mesh->mFaces[t];
         GLenum face_mode;

         switch(face->mNumIndices) {
            case 1: face_mode = GL_POINTS; break;
            case 2: face_mode = GL_LINES; break;
            case 3: face_mode = GL_TRIANGLES; break;
            default: face_mode = GL_POLYGON; break;
         }

         glBegin(face_mode);

         for(i = 0; i < face->mNumIndices; i++) {
            int index = face->mIndices[i];
            if(mesh->mColors[0] != NULL)
               glColor4fv((GLfloat*)&mesh->mColors[0][index]);
            if(mesh->mNormals != NULL) 
               glNormal3fv(&mesh->mNormals[index].x);
            glVertex3fv(&mesh->mVertices[index].x);
         }

         glEnd();
      }

   }

   // draw all children
   for (n = 0; n < nd->mNumChildren; ++n) {
      recursive_render(sc, nd->mChildren[n]);
   }

   glPopMatrix();
}

// ----------------------------------------------------------------------------
void do_motion (void)
{
   static GLint prev_time = 0;

   int time = glutGet(GLUT_ELAPSED_TIME);
   angle += (time-prev_time)*0.01;
   prev_time = time;

   glutPostRedisplay ();
}

// ----------------------------------------------------------------------------
void display(void)
{
   float tmp;

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(0.f,0.f,3.f,0.f,0.f,-5.f,0.f,1.f,0.f);

   // rotate it around the y axis
   glRotatef(angle,0.f,1.f,0.f);

   // scale the whole asset to fit into our view frustum 
   tmp = scene_max.x-scene_min.x;
   tmp = aisgl_max(scene_max.y - scene_min.y,tmp);
   tmp = aisgl_max(scene_max.z - scene_min.z,tmp);
   tmp = 1.f / tmp;
   glScalef(tmp, tmp, tmp);

   // center the model
   glTranslatef( -scene_center.x, -scene_center.y, -scene_center.z );

   // if the display list has not been made yet, create a new one and
   // fill it with scene contents
   if(scene_list == 0) {
      scene_list = glGenLists(1);
      glNewList(scene_list, GL_COMPILE);
      // now begin at the root node of the imported data and traverse
      // the scenegraph by multiplying subsequent local transforms
      // together on GL's matrix stack.
      recursive_render(scene, scene->mRootNode);
      glEndList();
   }

   glCallList(scene_list);

   glutSwapBuffers();

   do_motion();
}

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
int main(int argc, char **argv)
{
   struct aiLogStream stream;

   glutInitWindowSize(900,600);
   glutInitWindowPosition(100,100);
   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
   glutInit(&argc, argv);

   glutCreateWindow("Assimp - Very simple OpenGL sample");
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);

   // get a handle to the predefined STDOUT log stream and attach
   // it to the logging system. It remains active for all further
   // calls to aiImportFile(Ex) and aiApplyPostProcessing.
   stream = aiGetPredefinedLogStream(aiDefaultLogStream_STDOUT,NULL);
   aiAttachLogStream(&stream);

   // ... same procedure, but this stream now writes the
   // log messages to assimp_log.txt
   stream = aiGetPredefinedLogStream(aiDefaultLogStream_FILE,"assimp_log.txt");
   aiAttachLogStream(&stream);

   // the model name can be specified on the command line. If none
   // is specified, we try to locate one of the more expressive test 
   // models from the repository (/models-nonbsd may be missing in 
   // some distributions so we need a fallback from /models!).
   if( 0 != loadasset( argc >= 2 ? argv[1] : "AirHockey.obj")) {
      if( argc != 1 || 0 != loadasset( "../../../../test/models-nonbsd/X/dwarf.x") && 0 != loadasset( "../../test/models/X/Testwuson.X")) { 
         return -1;
      }
   }

   glClearColor(0.1f,0.1f,0.1f,1.f);

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);    // Uses default lighting parameters

   glEnable(GL_DEPTH_TEST);

   glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
   glEnable(GL_NORMALIZE);

   // XXX docs say all polygons are emitted CCW, but tests show that some aren't.
   if(getenv("MODEL_IS_BROKEN"))  
      glFrontFace(GL_CW);

   glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);

   glutGet(GLUT_ELAPSED_TIME);
   glutMainLoop();

   // cleanup - calling 'aiReleaseImport' is important, as the library 
   // keeps internal resources until the scene is freed again. Not 
   // doing so can cause severe resource leaking.
   aiReleaseImport(scene);

   // We added a log stream to the library, it's our job to disable it
   // again. This will definitely release the last resources allocated
   // by Assimp.
   aiDetachAllLogStreams();
   return 0;
}







const struct aiMesh* mesh = sc->mMeshes[n];

// create array with faces
// have to convert from Assimp format to array
unsigned int *faceArray;
faceArray = (unsigned int *)malloc(sizeof(unsigned int) * mesh->mNumFaces * 3);
unsigned int faceIndex = 0;

for (unsigned int t = 0; t < mesh->mNumFaces; ++t) {
   const struct aiFace* face = &mesh->mFaces[t];

   memcpy(&faceArray[faceIndex], face->mIndices,3 * sizeof(float));
   faceIndex += 3;
}
aMesh.numFaces = sc->mMeshes[n]->mNumFaces;

// generate Vertex Array for mesh
glGenVertexArrays(1,&(aMesh.vao));
glBindVertexArray(aMesh.vao);

// buffer for faces
glGenBuffers(1, &buffer);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh->mNumFaces * 3, faceArray, GL_STATIC_DRAW);

// buffer for vertex positions
if (mesh->HasPositions()) {
   glGenBuffers(1, &buffer);
   glBindBuffer(GL_ARRAY_BUFFER, buffer);
   glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*mesh->mNumVertices, mesh->mVertices, GL_STATIC_DRAW);
   glEnableVertexAttribArray(vertexLoc);
   glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, 0, 0, 0);
}

// buffer for vertex normals
if (mesh->HasNormals()) {
   glGenBuffers(1, &buffer);
   glBindBuffer(GL_ARRAY_BUFFER, buffer);
   glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*mesh->mNumVertices, mesh->mNormals, GL_STATIC_DRAW);
   glEnableVertexAttribArray(normalLoc);
   glVertexAttribPointer(normalLoc, 3, GL_FLOAT, 0, 0, 0);
}

// buffer for vertex texture coordinates
if (mesh->HasTextureCoords(0)) {
   float *texCoords = (float *)malloc(sizeof(float)*2*mesh->mNumVertices);
   for (unsigned int k = 0; k < mesh->mNumVertices; ++k) {

      texCoords[k*2]   = mesh->mTextureCoords[0][k].x;
      texCoords[k*2+1] = mesh->mTextureCoords[0][k].y;

   }
   glGenBuffers(1, &buffer);
   glBindBuffer(GL_ARRAY_BUFFER, buffer);
   glBufferData(GL_ARRAY_BUFFER, sizeof(float)*2*mesh->mNumVertices, texCoords, GL_STATIC_DRAW);
   glEnableVertexAttribArray(texCoordLoc);
   glVertexAttribPointer(texCoordLoc, 2, GL_FLOAT, 0, 0, 0);
}

// unbind buffers
glBindVertexArray(0);
glBindBuffer(GL_ARRAY_BUFFER,0);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

// create material uniform buffer
struct aiMaterial *mtl = sc->mMaterials[mesh->mMaterialIndex];

aiString texPath;   //contains filename of texture
if(AI_SUCCESS == mtl->GetTexture(aiTextureType_DIFFUSE, 0, &texPath)){
   //bind texture
   unsigned int texId = textureIdMap[texPath.data];
   aMesh.texIndex = texId;
   aMat.texCount = 1;
}
else
aMat.texCount = 0;

float c[4];
set_float4(c, 0.8f, 0.8f, 0.8f, 1.0f);
aiColor4D diffuse;
if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
   color4_to_float4(&diffuse, c);
   memcpy(aMat.diffuse, c, sizeof(c));

   set_float4(c, 0.2f, 0.2f, 0.2f, 1.0f);
   aiColor4D ambient;
if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &ambient))
   color4_to_float4(&ambient, c);
   memcpy(aMat.ambient, c, sizeof(c));

   set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
   aiColor4D specular;
if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular))
   color4_to_float4(&specular, c);
   memcpy(aMat.specular, c, sizeof(c));

   set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
   aiColor4D emission;
if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &emission))
   color4_to_float4(&emission, c);
   memcpy(aMat.emissive, c, sizeof(c));

   float shininess = 0.0;
   unsigned int max;
   aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &shininess, &max);
   aMat.shininess = shininess;

   glGenBuffers(1,&(aMesh.uniformBlockIndex));
   glBindBuffer(GL_UNIFORM_BUFFER,aMesh.uniformBlockIndex);
   glBufferData(GL_UNIFORM_BUFFER, sizeof(aMat), (void *)(&aMat), GL_STATIC_DRAW);

   myMeshes.push_back(aMesh);
   }

*/
