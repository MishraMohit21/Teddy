#pragma once
#include <Teddy.h>
#include <Teddy/Renderer/Renderer2D.h>
#include <Teddy/Core/Timestep.h>

#define MAX_POINTS 10000

#define PI = 3.14159265359

namespace DemoScene
{
	typedef struct
	{
		glm::vec3 points[MAX_POINTS];
		int count  = 0;
	} CurveBuffer;


	class Simulation
	{

	public:

		Simulation();

		CurveBuffer run(Teddy::Timestep ts);

	private:

		void GenerateLissajous(CurveBuffer* curve, float A, float B, float C, float a, float b, float c, float deltaX, float deltaY, float deltaZ, float tMax, float tStep);


		CurveBuffer curve;
		float tMax = 10.0f, tStep = 0.01f;

		float A = 5.0f, B = 5.0f, C = 5.0f;
		float a = 3.0f, b = 2.0f, c = 1.0f;
		float deltaX = 0.0f, deltaY = PI / 2.0f, deltaZ = PI / 4.0f;


	};

}