#include "model.h"

Engine::Model::Mesh Engine::Model::LoadMesh(std::string Path) {
    std::filesystem::path FullPath = std::filesystem::path(Util::GetExecutablePath()) / Path;
    std::string FullPathStr = FullPath.string();
    const char* FullPathString = FullPathStr.c_str();
    Assimp::Importer Importer;
    const aiScene* Scene = Importer.ReadFile(FullPathString, aiProcess_Triangulate);

    Mesh ModelMesh;

    if (!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode) {
        return ModelMesh;
    }

    for (unsigned int MeshIndex = 0; MeshIndex < Scene->mNumMeshes; MeshIndex++) {
        aiMesh* AssimpMesh = Scene->mMeshes[MeshIndex];
        aiMaterial* AssimpMaterial = Scene->mMaterials[AssimpMesh->mMaterialIndex];  // Get the material

        MeshData Mesh;
        std::vector<float> Vertices;
        std::vector<unsigned int> Indices;

        // Extracting vertex data
        for (unsigned int i = 0; i < AssimpMesh->mNumVertices; i++) {
            aiVector3D Pos = AssimpMesh->mVertices[i];
            aiVector3D Normal = AssimpMesh->HasNormals() ? AssimpMesh->mNormals[i] : aiVector3D(0, 0, 0);
            aiVector3D TexCoords = AssimpMesh->HasTextureCoords(0) ? AssimpMesh->mTextureCoords[0][i] : aiVector3D(0, 0, 0);

            Vertices.insert(Vertices.end(), {
                Pos.x, Pos.y, Pos.z,
                TexCoords.x, TexCoords.y,
                Normal.x, Normal.y, Normal.z,
                1.0f, 1.0f, 1.0f
            });
        }

        // Extracting face data
        for (unsigned int i = 0; i < AssimpMesh->mNumFaces; i++) {
            aiFace Face = AssimpMesh->mFaces[i];
            for (unsigned int j = 0; j < Face.mNumIndices; j++) {
                Indices.push_back(Face.mIndices[j]);
            }
        }

        glGenVertexArrays(1, &Mesh.VAO);
        glGenBuffers(1, &Mesh.VBO);
        glGenBuffers(1, &Mesh.EBO);

        glBindVertexArray(Mesh.VAO);
        glBindBuffer(GL_ARRAY_BUFFER, Mesh.VBO);
        glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(float), Vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Mesh.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int), Indices.data(), GL_STATIC_DRAW);

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
        ModelMesh.Meshes.push_back(Mesh);

        // Extract and assign material data
        ModelMesh.MaterialData.push_back(AssimpMaterial);
    }
    return ModelMesh;
}

void Engine::Model::DrawModel(const MeshData& Mesh, Material* MaterialPtr, const glm::mat4& ModelMatrix, Camera* MainCamera) {
    if (!MaterialPtr || !MainCamera) {
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
    if (Materials.empty() || !MainCamera) {
        return;
    }

    std::vector<std::pair<int, size_t>> SortedMeshes;

    for (size_t i = 0; i < ModelMesh.Meshes.size(); ++i) {
        Material* MaterialPtr = (i < Materials.size()) ? Materials[i] : Materials.back();

        int SortIndex = MaterialPtr->GetSortOrder();
        
        SortedMeshes.emplace_back(SortIndex, i);
    }

    std::sort(SortedMeshes.begin(), SortedMeshes.end(), 
              [](const std::pair<int, size_t>& A, const std::pair<int, size_t>& B) {
                  return A.first < B.first;
              });

    for (const auto& [_, i] : SortedMeshes) {
        Material* MaterialPtr = (i < Materials.size()) ? Materials[i] : Materials.back();
        DrawModel(ModelMesh.Meshes[i], MaterialPtr, ModelMatrix, MainCamera);
    }
}

void Engine::Model::DrawModelInstances(const std::vector<ModelInstance>& ModelInstances, Camera* MainCamera) {
    if (!MainCamera) {
        return;
    }

    std::vector<std::tuple<int, const MeshData*, Material*, glm::mat4>> SortedMeshes;

    for (const auto& Instance : ModelInstances) {
        const Mesh& ModelMesh = Instance.ModelMesh;
        const glm::mat4& ModelMatrix = Instance.Transform;

        for (size_t i = 0; i < ModelMesh.Meshes.size(); ++i) {
            Material* MaterialPtr = (i < Instance.Materials.size()) ? Instance.Materials[i] : Instance.Materials.back();

            int SortIndex = MaterialPtr->GetSortOrder();

            SortedMeshes.emplace_back(SortIndex, &ModelMesh.Meshes[i], MaterialPtr, ModelMatrix);
        }
    }

    std::sort(SortedMeshes.begin(), SortedMeshes.end(), 
              [MainCamera](const auto& A, const auto& B) {
                  int SortIndexA = std::get<0>(A);
                  int SortIndexB = std::get<0>(B);
                  
                  // First, compare the SortIndex
                  if (SortIndexA != SortIndexB) {
                      return SortIndexA < SortIndexB;
                  }
                  
                  // If SortIndex is the same, compare by distance to the camera
                  glm::vec3 ModelPosA = glm::vec3(std::get<3>(A)[3]);
                  glm::vec3 ModelPosB = glm::vec3(std::get<3>(B)[3]);
                  float DistanceA = glm::length(ModelPosA - MainCamera->GetPosition());
                  float DistanceB = glm::length(ModelPosB - MainCamera->GetPosition());

                  return DistanceA < DistanceB;
              });

    for (const auto& [_, Mesh, MaterialPtr, ModelMatrix] : SortedMeshes) {
        DrawModel(*Mesh, MaterialPtr, ModelMatrix, MainCamera);
    }
}

void Engine::Model::UnloadModelInstance(ModelInstance& instance) {
    UnloadMesh(instance.ModelMesh);
    instance.Materials.clear();
    delete &instance;
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
    delete &ModelMesh;
}
