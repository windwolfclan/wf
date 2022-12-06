#include "Graphics.h"

namespace wf
{
	Graphics::Graphics()
	{
	}

	Graphics::Graphics(const Graphics&)
	{
	}

	Graphics::~Graphics()
	{
	}

	bool Graphics::Initialize(int _width, int _height, HWND _hwnd)
	{
		return true;
	}

	void Graphics::Shutdown()
	{
	}

	bool Graphics::Frame()
	{
		return true;
	}

	bool Graphics::Render()
	{
		return true;
	}
}