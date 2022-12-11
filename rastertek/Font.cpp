#include "pch.h"
#include "Font.h"

#include "Texture.h"

namespace wf
{
	Font::Font()
	{
	}

	Font::Font( const Font& )
	{
	}

	Font::~Font()
	{
	}

	bool Font::Initialize( ID3D11Device* _device, ID3D11DeviceContext* _context, const char* _font_path, const char* _texture_path )
	{
		if ( !LoadFontData( _font_path ) )
		{
			return false;
		}

		if ( !LoadTexture( _device, _context, _texture_path ) )
		{
			return false;
		}

		return true;
	}

	void Font::Shutdown()
	{
		ReleaseTexture();

		ReleaseFontData();
	}

	ID3D11ShaderResourceView* Font::GetTexture()
	{
		return m_texture->GetTexture();
	}

	void Font::BuildVertexArray( void* _vertices, const char* _text, float _x, float _y )
	{
		VertexType* vertices = (VertexType*)_vertices;

		int index = 0;
		size_t letter_count = strlen( _text );
		for ( size_t i = 0; i < letter_count; ++i )
		{
			int letter = ( (int)_text[ i ] ) - 32;

			if ( 0 == letter )
			{
				_x += 3.0f;
			}
			else
			{
				float l = m_font[ letter ].left;
				float r = m_font[ letter ].right;
				int size = m_font[ letter ].size;

				vertices[ index ].pos = XMFLOAT3( _x, _y, 0.0f );					// lt
				vertices[ index ].tex = XMFLOAT2( l, 0.0f );
				++index;

				vertices[ index ].pos = XMFLOAT3( _x + size, _y - 16.0f, 0.0f);		// rb
				vertices[ index ].tex = XMFLOAT2( r, 1.0f );
				++index;

				vertices[ index ].pos = XMFLOAT3( _x, _y - 16.0f, 0.0f );			// lb
				vertices[ index ].tex = XMFLOAT2( l, 1.0f);
				++index;

				vertices[ index ].pos = XMFLOAT3( _x, _y, 0.0f );					// lt
				vertices[ index ].tex = XMFLOAT2( l, 0.0f );
				++index;

				vertices[ index ].pos = XMFLOAT3( _x + size, _y, 0.0f );			// rt
				vertices[ index ].tex = XMFLOAT2( r, 0.0f );
				++index;

				vertices[ index ].pos = XMFLOAT3( _x + size, _y - 16.0f, 0.0f );	// rb
				vertices[ index ].tex = XMFLOAT2( r, 1.0f );
				++index;

				_x += size + 1.0f;
			}
		}
	}

	bool Font::LoadFontData( const char* _font_path )
	{
		std::ifstream file( _font_path );
		if ( !file.is_open() )
		{
			return false;
		}

		char temp;
		for ( int i = 0; i < 95; ++i )
		{
			file.get( temp );
			while ( temp != ' ' )
			{
				file.get( temp );
			}

			file.get( temp );
			while ( temp != ' ' )
			{
				file.get( temp );
			}

			file >> m_font[ i ].left;
			file >> m_font[ i ].right;
			file >> m_font[ i ].size;
		}

		file.close();

		return true;
	}

	void Font::ReleaseFontData()
	{

	}

	bool Font::LoadTexture( ID3D11Device* _device, ID3D11DeviceContext* _context, const char* _texture_path )
	{
		m_texture = new Texture;
		if ( !m_texture->Initialize( _device, _context, _texture_path ) )
		{
			return false;
		}

		return true;
	}

	void Font::ReleaseTexture()
	{
		SAFE_SHUTDOWN( m_texture );
	}
}