#include <Teddy/Renderer/Renderer.h>
#include "Simulation.h"


namespace DemoScene
{
	Simulation::Simulation()
	{
		GenerateLissajous(&curve, A, B, C, a, b, c, deltaX, deltaY, deltaZ, tMax, tStep);
	}

	CurveBuffer Simulation::run(Teddy::Timestep time)
	{


		A = 5.0f + 2.0f * sinf(time * 0.5f); // Amplitude A oscillates
		B = 5.0f + 2.0f * cosf(time * 0.6f); // Amplitude B oscillates
		C = 5.0f + 2.0f * sinf(time * 0.7f); // Amplitude C oscillates

		a = 3.0f + sinf(time * 0.8f); // Frequency a oscillates
		b = 2.0f + cosf(time * 0.9f); // Frequency b oscillates
		c = 1.0f + sinf(time);        // Frequency c oscillates

		GenerateLissajous(&curve, A, B, C, a, b, c, deltaX, deltaY, deltaZ, tMax, tStep);

		glm::vec3 minBound = { -5.0f, -5.0f, -5.0f };
		glm::vec3 maxBound = { 5.0f, 5.0f, 5.0f };
		glm::vec3 center = { (minBound.x + maxBound.x) / 2.0f, (minBound.y + maxBound.y) / 2.0f,		(minBound.z + maxBound.z) / 2.0f };
		glm::vec3 size = { maxBound.x - minBound.x, maxBound.y - minBound.y, maxBound.z - minBound.z };

		return curve;

	}

	void Simulation::GenerateLissajous(CurveBuffer* curve, float A, float B, float C, float a, float b, float c, float deltaX, float deltaY, float deltaZ, float tMax, float tStep)
	{

		curve->count = 0;
		for (float t = 0.0f; t < tMax && curve->count < MAX_POINTS; t += tStep) {
			float x = A * sinf(a * t + deltaX);
			float y = B * sinf(b * t + deltaY);
			float z = C * sinf(c * t + deltaZ);
			curve->points[curve->count++] = glm::vec3{ x, y, z };
		}

	}

}