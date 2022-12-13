#include "pch.h"
#include "Text.h"

#include "Font.h"
#include "FontShader.h"

namespace wf
{
	Text::Text()
	{
	}

	Text::Text( const Text& )
	{
	}

	Text::~Text()
	{
	}

	bool Text::Initialize( ID3D11Device* _device, ID3D11DeviceContext* _context, HWND _hwnd, int _screen_width, int _screen_height, XMMATRIX _view )
	{
		m_screen_width = _screen_width;
		m_screen_height = _screen_height;

		m_view = _view;

		m_font = new Font;
		if ( !m_font->Initialize( _device, _context, "./resources/fontdata.txt", "./resources/font.tga" ) )
		{
			return false;
		}

		m_font_shader = new FontShader;
		if ( !m_font_shader->Initialize( _device, _hwnd ) )
		{
			return false;
		}

		if ( !InitializeSentence( &m_mouse_x, 32, _device ) )
		{
			return false;
		}

		if ( !InitializeSentence( &m_mouse_y, 32, _device ) )
		{
			return false;
		}

		if ( !InitializeSentence( &m_fps, 32, _device ) )
		{
			return false;
		}

		if ( !InitializeSentence( &m_usage, 32, _device ) )
		{
			return false;
		}

		return true;
	}

	void Text::Shutdown()
	{
		ReleaseSentence( &m_usage );
		ReleaseSentence( &m_fps );
		ReleaseSentence( &m_mouse_y );
		ReleaseSentence( &m_mouse_x );

		SAFE_SHUTDOWN( m_font_shader );
		SAFE_SHUTDOWN( m_font );
	}

	bool Text::Render( ID3D11DeviceContext* _context, XMMATRIX _world, XMMATRIX _ortho )
	{
		if ( !RenderSentence( _context, m_fps, _world, _ortho ) )
		{
			return false;
		}

		if ( !RenderSentence( _context, m_usage, _world, _ortho ) )
		{
			return false;
		}

		if ( !RenderSentence( _context, m_mouse_x, _world, _ortho ) )
		{
			return false;
		}

		if ( !RenderSentence( _context, m_mouse_y, _world, _ortho ) )
		{
			return false;
		}

		return true;
	}

	bool Text::SetMousePosition( int _mouse_x, int _mouse_y, ID3D11DeviceContext* _context )
	{
		char temp[ 16 ];
		char mouse[ 16 ];

		_itoa_s( _mouse_x, temp, 10 );
		strcpy_s( mouse, "Mouse X: " );
		strcat_s( mouse, temp );
		if ( !UpdateSentence( m_mouse_x, mouse, 20, 60, 1.0f, 1.0f, 1.0f, _context ) )
		{
			return false;
		}

		_itoa_s( _mouse_y, temp, 10 );
		strcpy_s( mouse, "Mouse Y: " );
		strcat_s( mouse, temp );
		if ( !UpdateSentence( m_mouse_y, mouse, 20, 80, 1.0f, 1.0f, 1.0f, _context ) )
		{
			return false;
		}

		return true;
	}

	bool Text::SetFps( int _fps, ID3D11DeviceContext* _context )
	{
		char temp[ 32 ];
		char fps[ 32 ];

		float r{ 0.0f };
		float g{ 0.0f };
		float b{ 0.0f };

		_itoa_s( _fps, temp, 10 );
		strcpy_s( fps, "FPS: " );
		strcat_s( fps, temp );

		if ( _fps >= 60 ) 
		{
			g = 1.0f;
		}
		else if ( _fps >= 30 ) 
		{
			r = 1.0f;
			g = 1.0f;
		}
		else
		{
			r = 1.0f;
		}

		if ( !UpdateSentence( m_fps, fps, 20, 20, r, g, b, _context ) )
		{
			return false;
		}

		return true;
	}

	bool Text::SetCpu( int _usage, ID3D11DeviceContext* _context )
	{
		char temp[ 32 ];
		char usage[ 32 ];

		float r{ 0.0f };
		float g{ 1.0f };
		float b{ 0.0f };

		_itoa_s( _usage, temp, 10 );
		strcpy_s( usage, "CPU: " );
		strcat_s( usage, temp );
		strcat_s( usage, "%" );

		if ( !UpdateSentence( m_usage, usage, 20, 40, r, g, b, _context ) )
		{
			return false;
		}

		return true;
	}

	bool Text::InitializeSentence( SentenceType** _sentence, int _max_length, ID3D11Device* _device )
	{
		*_sentence = new SentenceType;
		if ( !*_sentence )
		{
			return false;
		}
		
		int count = _max_length * 6;

		( *_sentence )->max_length = _max_length;
		( *_sentence )->vertex_count = count;
		( *_sentence )->index_count = count;
		
		VertexType* vertices = new VertexType[ count ];
		UINT* indices = new UINT[ count ];
		if ( !vertices || !indices )
		{
			SAFE_DELETE_ARRAY( vertices );
			SAFE_DELETE_ARRAY( indices );
			return false;
		}

		memset( vertices, 0, sizeof( VertexType ) * count );

		for ( int i = 0; i < count; ++i )
		{
			indices[ i ] = i;
		}

		D3D11_BUFFER_DESC buffer_desc{};
		ZeroMemory( &buffer_desc, sizeof( buffer_desc ) );
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		buffer_desc.ByteWidth = sizeof( VertexType ) * count;
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA data{};
		ZeroMemory( &data, sizeof( data ) );
		data.pSysMem = vertices;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		HRESULT hr = _device->CreateBuffer( &buffer_desc, &data, &( *_sentence )->vertex_buffer );
		if ( FAILED( hr ) )
		{
			SAFE_DELETE_ARRAY( vertices );
			SAFE_DELETE_ARRAY( indices );
			return false;
		}

		ZeroMemory( &buffer_desc, sizeof( buffer_desc ) );
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.ByteWidth = sizeof( UINT ) * count;
		buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;

		ZeroMemory( &data, sizeof( data ) );
		data.pSysMem = indices;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		hr = _device->CreateBuffer( &buffer_desc, &data, &( *_sentence )->index_buffer );
		if ( FAILED( hr ) )
		{
			SAFE_DELETE_ARRAY( vertices );
			SAFE_DELETE_ARRAY( indices );
			return false;
		}

		SAFE_DELETE_ARRAY( vertices );
		SAFE_DELETE_ARRAY( indices );

		return true;
	}

	void Text::ReleaseSentence( SentenceType** _sentence )
	{
		if ( *_sentence )
		{
			SAFE_RELEASE( ( *_sentence )->index_buffer );
			SAFE_RELEASE( ( *_sentence )->vertex_buffer );

			SAFE_DELETE( *_sentence );
		}
	}

	bool Text::UpdateSentence( SentenceType* _sentence, const char* _text, int _x, int _y, float _r, float _g, float _b, ID3D11DeviceContext* _context )
	{
		_sentence->r = _r;
		_sentence->g = _g;
		_sentence->b = _b;

		int letter_count = static_cast<int>( strlen( _text ) );
		if ( _sentence->max_length < letter_count )
		{
			return false;
		}

		VertexType* vertices = new VertexType[ _sentence->vertex_count ];
		memset( vertices, 0, sizeof( VertexType ) * _sentence->vertex_count );

		float x = static_cast<float>( -m_screen_width / 2.0f + _x );
		float y = static_cast<float>( m_screen_height / 2.0f - _y );

		m_font->BuildVertexArray( (void*)vertices, _text, x, y );


		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		HRESULT hr = _context->Map( _sentence->vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource );
		if ( FAILED( hr ) )
		{
			return false;
		}

		VertexType* data = (VertexType*)mappedSubresource.pData;

		memcpy( data, vertices, sizeof( VertexType ) * _sentence->vertex_count );

		_context->Unmap( _sentence->vertex_buffer, 0 );

		SAFE_DELETE_ARRAY( vertices );

		return true;
	}

	bool Text::RenderSentence( ID3D11DeviceContext* _context, SentenceType* _sentence, XMMATRIX _world, XMMATRIX _ortho )
	{
		UINT stride{ sizeof( VertexType ) };
		UINT offset{ 0 };

		XMFLOAT4 pixel_color{ _sentence->r, _sentence->g, _sentence->b, 1.0f };

		_context->IASetVertexBuffers( 0, 1, &_sentence->vertex_buffer, &stride, &offset );

		_context->IASetIndexBuffer( _sentence->index_buffer, DXGI_FORMAT_R32_UINT, 0 );

		_context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

		if ( !m_font_shader->Render( _context, _sentence->index_count, _world, m_view, _ortho, m_font->GetTexture(), pixel_color ) )
		{
			return false;
		}

		return true;
	}

}