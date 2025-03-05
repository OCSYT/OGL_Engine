#include "model.h"

Engine::Model::Mesh Engine::Model::LoadMesh(std::string Path) {
    std::filesystem::path FullPath = std::filesystem::path(Util::GetExecutablePath()) / Path;
    std::string FullPathStr = FullPath.string();
    const char* FullPathString = FullPathStr.c_str();
    Assimp::Importer Importer;
    const aiScene* Scene = Importer.ReadFile(FullPathString, aiProcess_Triangulate);

    Mesh ModelMesh;

    if (!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << Importer.GetErrorString() << std::endl;
        return ModelMesh;
    }

    // Loop through all meshes in the scene
    for (unsigned int MeshIndex = 0; MeshIndex < Scene->mNumMeshes; MeshIndex++) {
        aiMesh* AssimpMesh = Scene->mMeshes[MeshIndex];

        MeshData Mesh;
        std::vector<float> Vertices;
        std::vector<unsigned int> Indices;

        for (unsigned int i = 0; i < AssimpMesh->mNumVertices; i++) {
            aiVector3D Pos = AssimpMesh->mVertices[i];
            aiVector3D Normal = AssimpMesh->HasNormals() ? AssimpMesh->mNormals[i] : aiVector3D(0, 0, 0);
            aiVector3D TexCoords = AssimpMesh->HasTextureCoords(0) ? AssimpMesh->mTextureCoords[0][i] : aiVector3D(0, 0, 0);

            // Push position, texcoords, normal, extra vertex color data (default white)
            Vertices.insert(Vertices.end(), {
                Pos.x, Pos.y, Pos.z,       // Position
                TexCoords.x, TexCoords.y,  // Texture Coordinates
                Normal.x, Normal.y, Normal.z, // Normal
                1.0f, 1.0f, 1.0f            // Default Vertex Color (white)
            });
        }

        for (unsigned int i = 0; i < AssimpMesh->mNumFaces; i++) {
            aiFace Face = AssimpMesh->mFaces[i];
            for (unsigned int j = 0; j < Face.mNumIndices; j++) {
                Indices.push_back(Face.mIndices[j]);
            }
        }

        // OpenGL buffer setup
        glGenVertexArrays(1, &Mesh.VAO);
        glGenBuffers(1, &Mesh.VBO);
        glGenBuffers(1, &Mesh.EBO);

        glBindVertexArray(Mesh.VAO);

        glBindBuffer(GL_ARRAY_BUFFER, Mesh.VBO);
        glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(float), Vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Mesh.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int), Indices.data(), GL_STATIC_DRAW);

        // Vertex attributes
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(5 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
        glEnableVertexAttribArray(3);

        glBindVertexArray(0);

        Mesh.IndexCount = static_cast<unsigned int>(Indices.size());

        // Add the processed mesh data to the ModelMesh object
        ModelMesh.Meshes.push_back(Mesh);
    }

    return ModelMesh;
}


void Engine::Model::DrawModel(const MeshData& Mesh, Material* MaterialPtr, const glm::mat4& ModelMatrix, Camera* MainCamera) {
    if (!MaterialPtr) {
        std::cerr << "DrawModel: Material pointer is null!" << std::endl;
        return;
    }

    if (!MainCamera) {
        std::cerr << "DrawModel: Camera pointer is null!" << std::endl;
        return;
    }

    MaterialPtr->Bind();
    MaterialPtr->SetUniform("Model", ModelMatrix);
    MaterialPtr->SetUniform("View", MainCamera->GetViewMatrix());
    MaterialPtr->SetUniform("Projection", MainCamera->GetProjectionMatrix());

    glBindVertexArray(Mesh.VAO);
    glDrawElements(GL_TRIANGLES, Mesh.IndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Engine::Model::DrawMesh(const Mesh& ModelMesh, const std::vector<Material*>& Materials, const glm::mat4& ModelMatrix, Camera* MainCamera) {
    if (Materials.empty()) {
        std::cerr << "DrawMesh: Material list is empty!" << std::endl;
        return;
    }

    if (!MainCamera) {
        std::cerr << "DrawMesh: Camera pointer is null!" << std::endl;
        return;
    }

    for (size_t i = 0; i < ModelMesh.Meshes.size(); ++i) {
        Material* MaterialPtr = (i < Materials.size()) ? Materials[i] : Materials.back();
        DrawModel(ModelMesh.Meshes[i], MaterialPtr, ModelMatrix, MainCamera);
    }
}


void Engine::Model::UnloadMesh(Mesh& ModelMesh) {
    for (MeshData& Mesh : ModelMesh.Meshes) {
        glDeleteVertexArrays(1, &Mesh.VAO);
        glDeleteBuffers(1, &Mesh.VBO);
        glDeleteBuffers(1, &Mesh.EBO);
        Mesh.VAO = Mesh.VBO = Mesh.EBO = 0;
        Mesh.IndexCount = 0;
    }
    ModelMesh.Meshes.clear();
}
