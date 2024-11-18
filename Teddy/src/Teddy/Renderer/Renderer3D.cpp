#include "Teddy/Renderer/Renderer3D.h"
#include "Teddy/Renderer/VertexArray.h"
#include "Teddy/Renderer/Shader.h"
#include "Teddy/Renderer/RenderCommand.h"
#include "Teddy/Scene/SceneCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Teddy {

    struct Vertex3D {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoord;
        glm::vec4 Color;
        float TexIndex;
        float TilingFactor;
        int EntityID;
    };

    struct MeshData {
        std::vector<Vertex3D> Vertices;
        std::vector<uint32_t> Indices;
    };

    struct Renderer3DData {
        static const uint32_t MaxVertices = 20000;
        static const uint32_t MaxIndices = MaxVertices * 3;
        static const uint32_t MaxTextureSlots = 32;

        Ref<VertexArray> MeshVertexArray;
        Ref<VertexBuffer> MeshVertexBuffer;
        Ref<Shader> DefaultShader;
        Ref<Texture2D> WhiteTexture;

        uint32_t IndexCount = 0;
        Vertex3D* VertexBufferBase = nullptr;
        Vertex3D* VertexBufferPtr = nullptr;

        std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
        uint32_t TextureSlotIndex = 1;

        glm::mat4 ViewProjection = glm::mat4(1.0f);
        glm::vec3 LightPosition = glm::vec3(0.0f, 5.0f, -5.0f);
        glm::vec3 LightColor = glm::vec3(1.0f);
        float AmbientStrength = 0.1f;

        struct Statistics {
            uint32_t DrawCalls = 0;
            uint32_t MeshCount = 0;
            uint32_t VertexCount = 0;
        } Stats;
    };

    static Renderer3DData s_Data;

    void Renderer3D::Init() {
        s_Data.MeshVertexArray = VertexArray::Create();

        s_Data.MeshVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(Vertex3D));
        s_Data.MeshVertexBuffer->SetLayout({
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float3, "a_Normal" },
            { ShaderDataType::Float2, "a_TexCoord" },
            { ShaderDataType::Float4, "a_Color" },
            { ShaderDataType::Float,  "a_TexIndex" },
            { ShaderDataType::Float,  "a_TilingFactor" },
            { ShaderDataType::Int,    "a_EntityID" }
            });
        s_Data.MeshVertexArray->AddVertexBuffer(s_Data.MeshVertexBuffer);

        s_Data.VertexBufferBase = new Vertex3D[s_Data.MaxVertices];

        // Create index buffer for maximum possible indices
        uint32_t* indices = new uint32_t[s_Data.MaxIndices];
        uint32_t offset = 0;
        for (uint32_t i = 0; i < s_Data.MaxIndices; i += 3) {
            indices[i + 0] = offset + 0;
            indices[i + 1] = offset + 1;
            indices[i + 2] = offset + 2;
            offset += 3;
        }

        Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices, s_Data.MaxIndices);
        s_Data.MeshVertexArray->SetIndexBuffer(indexBuffer);
        delete[] indices;

        // Create white texture
        s_Data.WhiteTexture = Texture2D::Create(1, 1);
        uint32_t whiteTextureData = 0xffffffff;
        s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

        // Setup default shader
        s_Data.DefaultShader = Shader::Create("assets/shaders/Default3D.glsl");
        s_Data.DefaultShader->Bind();

        // Set texture slots
        int32_t samplers[s_Data.MaxTextureSlots];
        for (uint32_t i = 0; i < s_Data.MaxTextureSlots; i++)
            samplers[i] = i;
        s_Data.DefaultShader->SetIntArray("u_Textures", samplers, s_Data.MaxTextureSlots);

        s_Data.TextureSlots[0] = s_Data.WhiteTexture;
    }

    void Renderer3D::Shutdown() {
        delete[] s_Data.VertexBufferBase;
    }

    void Renderer3D::BeginScene(const Camera& camera, const glm::mat4& transform) {
        glm::mat4 viewProj = camera.GetProjection() * glm::inverse(transform);
        s_Data.ViewProjection = viewProj;

        s_Data.DefaultShader->Bind();
        s_Data.DefaultShader->SetMat4("u_ViewProjection", viewProj);
        s_Data.DefaultShader->SetFloat3("u_LightPosition", s_Data.LightPosition);
        s_Data.DefaultShader->SetFloat3("u_LightColor", s_Data.LightColor);
        s_Data.DefaultShader->SetFloat("u_AmbientStrength", s_Data.AmbientStrength);

        StartBatch();
    }

    void Renderer3D::EndScene() {
        Flush();
    }

    void Renderer3D::StartBatch() {
        s_Data.IndexCount = 0;
        s_Data.VertexBufferPtr = s_Data.VertexBufferBase;
        s_Data.TextureSlotIndex = 1;
    }

    void Renderer3D::Flush() {
        if (s_Data.IndexCount == 0)
            return;

        uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.VertexBufferPtr - (uint8_t*)s_Data.VertexBufferBase);
        s_Data.MeshVertexBuffer->SetData(s_Data.VertexBufferBase, dataSize);

        for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
            s_Data.TextureSlots[i]->Bind(i);

        RenderCommand::DrawIndexed(s_Data.MeshVertexArray, s_Data.IndexCount);
        s_Data.Stats.DrawCalls++;
    }

    void Renderer3D::DrawMesh(const MeshData& mesh, const glm::mat4& transform, const glm::vec4& color, int entityID) {
        if (s_Data.IndexCount >= Renderer3DData::MaxIndices)
            NextBatch();

        for (const auto& vertex : mesh.Vertices) {
            s_Data.VertexBufferPtr->Position = transform * glm::vec4(vertex.Position, 1.0f);
            s_Data.VertexBufferPtr->Normal = glm::mat3(glm::transpose(glm::inverse(transform))) * vertex.Normal;
            s_Data.VertexBufferPtr->TexCoord = vertex.TexCoord;
            s_Data.VertexBufferPtr->Color = color;
            s_Data.VertexBufferPtr->TexIndex = 0.0f;
            s_Data.VertexBufferPtr->TilingFactor = 1.0f;
            s_Data.VertexBufferPtr->EntityID = entityID;
            s_Data.VertexBufferPtr++;
        }

        s_Data.IndexCount += mesh.Indices.size();
        s_Data.Stats.MeshCount++;
        s_Data.Stats.VertexCount += mesh.Vertices.size();
    }

    void Renderer3D::DrawCube(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color, int entityID) {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
            * glm::scale(glm::mat4(1.0f), size);

        static MeshData cubeMesh = CreateCubeMesh();
        DrawMesh(cubeMesh, transform, color, entityID);
    }

    void Renderer3D::DrawSphere(const glm::vec3& position, float radius, const glm::vec4& color, int entityID) {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
            * glm::scale(glm::mat4(1.0f), glm::vec3(radius));

        static MeshData sphereMesh = CreateSphereMesh();
        DrawMesh(sphereMesh, transform, color, entityID);
    }

    void Renderer3D::SetLightPosition(const glm::vec3& position) {
        s_Data.LightPosition = position;
    }

    void Renderer3D::SetLightColor(const glm::vec3& color) {
        s_Data.LightColor = color;
    }

    void Renderer3D::SetAmbientStrength(float strength) {
        s_Data.AmbientStrength = strength;
    }

    MeshData Renderer3D::CreateCubeMesh() {
        // Implementation of cube mesh creation
        // Returns vertices and indices for a cube
        // ... (detailed implementation omitted for brevity)
    }

    MeshData Renderer3D::CreateSphereMesh(uint32_t segments) {
        // Implementation of sphere mesh creation
        // Returns vertices and indices for a sphere with given segments
        // ... (detailed implementation omitted for brevity)
    }

    void Renderer3D::NextBatch() {
        Flush();
        StartBatch();
    }

    void Renderer3D::ResetStats() {
        memset(&s_Data.Stats, 0, sizeof(Renderer3DData::Statistics));
    }

    Renderer3D::Statistics Renderer3D::GetStats() {
        return s_Data.Stats;
    }

}