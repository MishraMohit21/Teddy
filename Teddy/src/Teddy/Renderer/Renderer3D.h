#pragma once


#include "Teddy/Renderer/Camera.h"
#include "Teddy/Renderer/Texture.h"
#include "Teddy/Core/Core.h"

#include <glm/glm.hpp>

namespace Teddy {

    struct MeshData;

    class Renderer3D {
    public:
        static void Init();
        static void Shutdown();

        static void BeginScene(const Camera& camera, const glm::mat4& transform);
        static void EndScene();

        // Primitive Drawing
        static void DrawCube(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color, int entityID = -1);
        static void DrawSphere(const glm::vec3& position, float radius, const glm::vec4& color, int entityID = -1);

        // Generic Mesh Drawing
        static void DrawMesh(const MeshData& mesh, const glm::mat4& transform, const glm::vec4& color, int entityID = -1);

        // Lighting Control
        static void SetLightPosition(const glm::vec3& position);
        static void SetLightColor(const glm::vec3& color);
        static void SetAmbientStrength(float strength);

        // Statistics
        struct Statistics {
            uint32_t DrawCalls = 0;
            uint32_t MeshCount = 0;
            uint32_t VertexCount = 0;
        };
        static void ResetStats();
        static Statistics GetStats();

    private:
        static void StartBatch();
        static void NextBatch();
        static void Flush();

        static MeshData CreateCubeMesh();
        static MeshData CreateSphereMesh(uint32_t segments = 32);
    };

}