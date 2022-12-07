#include "Input.h"

namespace wf
{
	Input::Input()
	{
	}

	Input::Input( const Input& )
	{
	}

	Input::~Input()
	{
	}

	void Input::Initialize()
	{
		for ( bool& key : m_keys )
		{
			key = false;
		}
	}

	void Input::KeyDown( unsigned int key )
	{
		m_keys[ key ] = true;
	}

	void Input::KeyUp( unsigned int key )
	{
		m_keys[ key ] = false;
	}

	bool Input::IsKeyDown( unsigned int key )
	{
		return m_keys[ key ];
	}
}