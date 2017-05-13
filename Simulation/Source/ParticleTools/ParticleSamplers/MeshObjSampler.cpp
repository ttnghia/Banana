//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//  Copyright (c) 2017 by
//       __      _     _         _____
//    /\ \ \__ _| |__ (_) __ _  /__   \_ __ _   _  ___  _ __   __ _
//   /  \/ / _` | '_ \| |/ _` |   / /\/ '__| | | |/ _ \| '_ \ / _` |
//  / /\  / (_| | | | | | (_| |  / /  | |  | |_| | (_) | | | | (_| |
//  \_\ \/ \__, |_| |_|_|\__,_|  \/   |_|   \__,_|\___/|_| |_|\__, |
//         |___/                                              |___/
//
//  <nghiatruong.vn@gmail.com>
//  All rights reserved.
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#include "..\Include\ParticleSampler\MeshSampler.h"



MeshSampler::MeshSampler()
{}


MeshSampler::~MeshSampler()
{}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//  Copyright (c) 2017 by
//       __      _     _         _____
//    /\ \ \__ _| |__ (_) __ _  /__   \_ __ _   _  ___  _ __   __ _
//   /  \/ / _` | '_ \| |/ _` |   / /\/ '__| | | |/ _ \| '_ \ / _` |
//  / /\  / (_| | | | | | (_| |  / /  | |  | |_| | (_) | | | | (_| |
//  \_\ \/ \__, |_| |_|_|\__,_|  \/   |_|   \__,_|\___/|_| |_|\__, |
//         |___/                                              |___/
//
//  <nghiatruong.vn@gmail.com>
//  All rights reserved.
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#include <Noodle/Core/Particle/ObjectBuilders/TriMeshBuilder.h>
#include <Noodle/Core/Mesh/OBJLoader.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void TriMeshBuilder::set_mesh_params(const std::string& mesh_file_,
                                     const Vec3& mesh_center_, Real mesh_scale_)
{
    mesh_file = mesh_file_;
    mesh_center = mesh_center_;
    mesh_scale = mesh_scale_;

    ////////////////////////////////////////////////////////////////////////////////
    monitor.printLog("Set mesh file: " + mesh_file);
    monitor.printLogIndent("Mesh center: " + NumberHelpers::toString(
        mesh_center) + ", scale: " +
        NumberHelpers::formatWithCommas(mesh_scale, 5));

    mesh_initialized = true;
    invalidate_data();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void TriMeshBuilder::compute_aabb()
{
    if(!mesh_loaded)
    {
        load_mesh(); // aabb is calculated here
        mesh_loaded = true;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void TriMeshBuilder::compute_sdf(bool negative_inside)
{
    __NOODLE_ASSERT(sdf_grid.m_SizeX > 0 && sdf_grid.m_SizeY > 0 && sdf_grid.m_SizeZ > 0);

    if(!mesh_loaded)
    {
        load_mesh();
        mesh_loaded = true;
    }

    Timer timer;
    timer.tick();

    SignDistanceField::distance_field_tri_mesh(face_list, vertex_list,
                                               Vec3(domainParams->domainBMin[0],
                                               domainParams->domainBMin[1],
                                               domainParams->domainBMin[2]),
                                               sdf_cell_size,
                                               sdf_grid.m_SizeX, sdf_grid.m_SizeY, sdf_grid.m_SizeZ,
                                               sdf_grid, negative_inside);


    timer.tock();
    monitor.printLog("Sign distance field: [" +
                     NumberHelpers::formatWithCommas(sdf_grid.m_SizeX) + "x" +
                     NumberHelpers::formatWithCommas(sdf_grid.m_SizeY) + "x" +
                     NumberHelpers::formatWithCommas(sdf_grid.m_SizeZ) + "]" +
                     ". Compute: " + timer.getRunTime());
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void TriMeshBuilder::load_mesh()
{
    __NOODLE_ASSERT(mesh_initialized);

    Timer timer;
    timer.tick();

    OBJLoader loader;
    loader.loadFromFileObj(mesh_file.c_str());
    loader.computeBoundingBox();

    Vec3 bbmin = loader.getBoundMin();
    Vec3 bbmax = loader.getBoundMax();

    Vec3 diff = bbmax - bbmin;
    Real max_size = fmaxf(fmaxf(fabs(diff[0]), fabs(diff[1])), fabs(diff[2]));
    Real scale = fminf(fminf(domainParams->domainBMax[0] - domainParams->domainBMin[0],
                       domainParams->domainBMax[1] - domainParams->domainBMin[1]),
                       domainParams->domainBMax[2] - domainParams->domainBMin[2]) / max_size;

    // how small the mesh object will be:
    scale *= mesh_scale;

    bbmin *= scale;
    bbmax *= scale;

    Vec3 obj_center = (Real)0.5 * (bbmax - bbmin);

    // translate the object if needed
    Vec3 shift = domainParams->domainBMin - bbmin + mesh_center - obj_center;
    monitor.printLog("Mesh scale/shift: " +
                     NumberHelpers::toStringWithPrecision(scale) + "/" +
                     NumberHelpers::toString(shift));

    obj_bbmin = bbmin + shift;
    obj_bbmax = bbmax + shift;
    vertex_list.clear();
    face_list.clear();

    for(int i = 0; i < loader.getNumVertices(); ++i)
    {
        Vec3 v = loader.getVertex(i);
        Vec3 v_new = v * scale + shift;

        vertex_list.push_back(v_new);
    }

    for(int i = 0; i < loader.getNumFaces(); ++i)
    {
        OBJLoader::Face f = loader.getFace(i);
        Vec3ui          face(f.vertices[0], f.vertices[1], f.vertices[2]);

        face_list.push_back(face);
    }

    ////////////////////////////////////////////////////////////////////////////////
    timer.tock();
    monitor.printLog(timer.getRunTime("Load mesh file: "));
}





//    TriMeshBuilder* objBuilder = new TriMeshBuilder(simParams, globalParams);
//    objBuilder->setMeshObject(globalParams.mesh_object, Vec3(1, 1.0, 1), 0.8);
//    objBuilder->initPDObject(pdSimulator.pdPosition,
//                             pdSimulator.pdVelocity);
