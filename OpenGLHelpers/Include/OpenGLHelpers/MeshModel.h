//------------------------------------------------------------------------------------------
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 09/03/2016
//            `)  ('                                                    Author: Nghia Truong
//         ,  /::::\  ,
//         |'.\::::/.'|
//        _|  ';::;'  |_
//       (::)   ||   (::)                       _.
//        "|    ||    |"                      _(:)
//         '.   ||   .'                       /::\
//           '._||_.'                         \::/
//            /::::\                         /:::\
//            \::::/                        _\:::/
//             /::::\_.._  _.._  _.._  _.._/::::\
//             \::::/::::\/::::\/::::\/::::\::::/
//               `""`\::::/\::::/\::::/\::::/`""`
//                    `""`  `""`  `""`  `""`
//------------------------------------------------------------------------------------------
#ifndef MESHMODEL_H
#define MESHMODEL_H

#include <Mango/Core/MangoDefinition.h>
#include <Mango/Core/Camera.h>
#include <glm/glm.hpp>

#include <tiny_obj_loader/tiny_obj_loader.h>
#include <tinyply/tinyply.h>

//------------------------------------------------------------------------------------------
class MeshModel
{
public:
    MeshModel(): mesh_ready(false)
    {
        clear_data();
    }

    MeshModel(std::string mesh_file):
        mesh_ready(false)
    {
        load_mesh(mesh_file);
    }

    bool load_mesh(std::string mesh_file);
    void set_default_camera(Camera& camera, float fov = 45);
    glm::vec3 get_mesh_center();
    float get_camera_distance(float fov);

    std::vector<float>& get_vertices();
    std::vector<float>& get_vnormals();
    std::vector<float>& get_vcolors();
    std::vector<float>& get_texcoord_2d();
    std::vector<float>& get_texcoord_3d();

    unsigned int num_triangles;
    bool mesh_ready;
private:
    void check_filetype(std::string mesh_file);
    void clear_data();
    bool load_obj(std::string mesh_file);
    bool load_ply(std::string mesh_file);
    void compute_normal(float N[3], float v0[3], float v1[3], float v2[3]);

    MeshFileType meshFileType;
    std::string loading_error;

    std::vector<float> vertices;
    std::vector<float> vnormals;
    std::vector<float> vcolors;
    std::vector<float> texcoord_2d;
    std::vector<float> texcoord_3d;
    glm::vec3 bmin;
    glm::vec3 bmax;

};

#endif // MESHMODEL_H
