
#include "print_data.h"

#include <iomanip>

void printVector(obj_vector *v, std::ostream& out)
{
   out << "(" << std::setprecision(2) << std::fixed << v->e[0] << ",";
   out << std::setprecision(2) << std::fixed << v->e[1] << ",";
   out << std::setprecision(2) << std::fixed << v->e[2] << ") ";
}

void printObjData(const objLoader& loader, std::ostream& out)
{
   out << "Vertex Count: " << loader.vertexCount << std::endl
        << "Vertex Normals: " << loader.normalCount << std::endl
        << "Texture Coords: " << loader.textureCount << std::endl << std::endl;

   out << "Faces: " << loader.faceCount << std::endl;
   for ( int i = 0; i < loader.faceCount; ++i )
   {
      obj_face *o = loader.faceList[i];
      out << " face ";
      for ( int j = 0; j < 3; ++j )
         printVector(loader.vertexList[ o->vertex_index[j] ], out);
      out << std::endl;
   }
   out << std::endl;

   out << "Number of Spheres: " << loader.sphereCount << std::endl;
   out << "Number of Planes: " << loader.planeCount << std::endl;
   out << "Number of Point Lights: " << loader.lightPointCount << std::endl;
   out << "Number of Disc Lights: " << loader.lightDiscCount << std::endl;
   out << "Number of Quad Lights: " << loader.lightQuadCount << std::endl;
   out << std::endl;

   if ( loader.camera == NULL )
      out << "No Camera Found" << std::endl;
   else
   {
      out << "Camera Found" << std::endl
           << " position: ";
      printVector(loader.vertexList[loader.camera->camera_pos_index], out);
      out << "\n looking at: ";
      printVector(loader.vertexList[loader.camera->camera_look_point_index], out);
      out << "\n up normal: ";
      printVector(loader.vertexList[loader.camera->camera_up_norm_index], out);
      out << std::endl;
   }

   out << std::endl;

   out << "Number of materials: " << loader.materialCount << std::endl;
   for ( int i = 0; i < loader.materialCount; ++i )
   {
      obj_material *mtl = loader.materialList[i];
      out << " name: " << mtl->name;
      out << " amb: " << std::setprecision(2) << std::fixed << mtl->amb[0] << ' ';
      out << std::setprecision(2) << std::fixed << mtl->amb[1] << ' ';
      out << std::setprecision(2) << std::fixed << mtl->amb[2] << std::endl;

      out << " diff: " << std::setprecision(2) << std::fixed << mtl->diff[0] << ' ';
      out << std::setprecision(2) << std::fixed << mtl->diff[1] << ' ';
      out << std::setprecision(2) << std::fixed << mtl->diff[2] << std::endl;

      out << " spec: " << std::setprecision(2) << std::fixed << mtl->spec[0] << ' ';
      out << std::setprecision(2) << std::fixed << mtl->spec[1] << ' ';
      out << std::setprecision(2) << std::fixed << mtl->spec[2] << std::endl;

      out << " reflect: " << std::setprecision(2) << std::fixed << mtl->reflect << std::endl;
      out << " trans: " << std::setprecision(2) << std::fixed << mtl->trans << std::endl;
      out << " glossy: " << std::setprecision(2) << std::fixed << mtl->glossy << std::endl;
      out << " shiny: " << std::setprecision(2) << std::fixed << mtl->shiny << std::endl;
      out << " refract: " << std::setprecision(2) << std::fixed << mtl->refract_index << std::endl;
      
      out << " texture: " << mtl->texture_filename << std::endl;
      out << std::endl;
   }

   out << std::endl;

   if ( loader.textureCount > 2 && loader.normalCount > 2 && loader.faceCount > 2 )
   {
      out << "Detailed face data:" << std::endl;

      for ( int i = 0; i < 3; ++i )
      {
         obj_face *o = loader.faceList[i];
         out << " face ";
         for ( int j = 0; j < 3; ++j )
         {
            out << o->vertex_index[i] << "/";
            out << o->texture_index[i] << "/";
            out << o->normal_index[i] << "/";
         }
         out << std::endl;
      }
   }
}
