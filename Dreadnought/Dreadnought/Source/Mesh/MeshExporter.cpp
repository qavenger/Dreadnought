#include "pch.h"
#include "MeshExporter.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Exporter.hpp>

void IMeshExporter::ExportMesh(const std::string& Filename, std::vector<uint16>& Indices,
    std::vector<float3>& Vertices, std::vector<float3>& Normals, std::vector<float2>& UVs1, std::vector<float2>& UVs2)
{
    // create vertices and faces, then pack into an aiMesh

    aiVector3D* V = new aiVector3D[Vertices.size()];
    aiVector3D* N= new aiVector3D[Vertices.size()];
    for (int VertexId = 0; VertexId < Vertices.size(); ++VertexId)
    {
        V[VertexId].x = Vertices[VertexId].x;
        V[VertexId].y = Vertices[VertexId].y;
        V[VertexId].z = Vertices[VertexId].z;
        N[VertexId].x = Normals[VertexId].x;
        N[VertexId].y = Normals[VertexId].y;
        N[VertexId].z = Normals[VertexId].z;
    }

    int NumTriangles = Indices.size() / 3;
    aiFace* Faces = new aiFace[NumTriangles];
    for (int Index = 0; Index < NumTriangles; ++Index)
    {
        Faces[Index].mNumIndices = 3;
        Faces[Index].mIndices = new unsigned [] {Indices[3 * Index + 0], Indices[3 * Index + 1], Indices[3 * Index + 2]};
    }  

    aiMesh* Mesh = new aiMesh();                       
    Mesh->mNumVertices = Vertices.size();
    Mesh->mVertices = V;
    Mesh->mNormals = N;
    Mesh->mNumFaces = NumTriangles;
    Mesh->mFaces = Faces;
    Mesh->mPrimitiveTypes = aiPrimitiveType_TRIANGLE; // workaround, issue #3778
    // a valid material is needed, even if its empty

    aiMaterial* Material = new aiMaterial();           

    // a root node with the mesh list is needed; if you have multiple meshes, this must match.

    aiNode* Root = new aiNode();                      
    Root->mNumMeshes = 1;
    Root->mMeshes = new unsigned [] { 0 };            

    // pack mesh(es), material, and root node into a new minimal aiScene

    aiScene* Out = new aiScene();                     
    Out->mNumMeshes = 1;
    Out->mMeshes = new aiMesh * [] { Mesh };
    Out->mNumMaterials = 1;
    Out->mMaterials = new aiMaterial * [] { Material };
    Out->mRootNode = Root;
    Out->mMetaData = new aiMetadata(); // workaround, issue #3781

    // and we're good to go. do whatever:

    Assimp::Exporter Exporter;
    if (Exporter.Export(Out, "objnomtl", Filename.c_str()) != AI_SUCCESS)
        printf("%s", Exporter.GetErrorString());

    // deleting the scene will also take care of the vertices, faces, meshes, materials, nodes, etc.

    delete Out;
}