#pragma once


namespace Teddy
{

	class GraphicContext
	{
	public:
		virtual void init() = 0;
		virtual void swapbuffer() = 0;
	};

}