#include "Texture.h"

namespace wf
{
	Texture::Texture()
	{
	}

	Texture::Texture( const Texture& )
	{
	}

	Texture::~Texture()
	{
	}

	bool Texture::Initialize( ID3D11Device* _device, ID3D11DeviceContext* _context, const char* _path )
	{
		int width{ 0 };
		int height{ 0 };
		if ( !LoadTarga( _path, width, height ) )
		{
			return false;
		}

		D3D11_TEXTURE2D_DESC texture_desc{};
		ZeroMemory( &texture_desc, sizeof( texture_desc ) );
		texture_desc.Width = width;
		texture_desc.Height = height;
		texture_desc.MipLevels = 0;
		texture_desc.ArraySize = 1;
		texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		texture_desc.SampleDesc.Count = 1;
		texture_desc.SampleDesc.Quality = 0;
		texture_desc.Usage = D3D11_USAGE_DEFAULT;
		texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		texture_desc.CPUAccessFlags = 0;
		texture_desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		HRESULT hr = _device->CreateTexture2D( &texture_desc, nullptr, &m_texture );
		if ( FAILED( hr ) )
		{
			return false;
		}

		unsigned int pitch = ( width * 4 ) * sizeof( unsigned char );
		_context->UpdateSubresource( m_texture, 0, nullptr, m_targa_data, pitch, 0 );

		D3D11_SHADER_RESOURCE_VIEW_DESC view_desc{};
		ZeroMemory( &view_desc, sizeof( view_desc ) );
		view_desc.Format = texture_desc.Format;
		view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		view_desc.Texture2D.MostDetailedMip = 0;
		view_desc.Texture2D.MipLevels = 1;

		hr = _device->CreateShaderResourceView( m_texture, &view_desc, &m_texture_view );
		if ( FAILED( hr ) )
		{
			return false;
		}

		_context->GenerateMips( m_texture_view );

		delete[] m_targa_data;
		m_targa_data = nullptr;

		return true;
	}

	void Texture::Shutdown()
	{
		if ( m_texture_view )
		{
			m_texture_view->Release();
			m_texture_view = nullptr;
		}

		if ( m_texture )
		{
			m_texture->Release();
			m_texture = nullptr;
		}

		if ( m_targa_data )
		{
			delete[] m_targa_data;
			m_targa_data = nullptr;
		}
	}

	ID3D11ShaderResourceView* Texture::GetTexture()
	{
		return m_texture_view;
	}

	bool Texture::LoadTarga( const  char* _path, int& _width, int& _height )
	{
		FILE* file{ nullptr };
		int error = fopen_s( &file, _path, "rb" );
		if ( 0 != error )
		{
			return false;
		}

		TargaHeader header{};
		unsigned int count = (unsigned int)fread( &header, sizeof( TargaHeader ), 1, file );
		if ( 1 != count )
		{
			return false;
		}

		_width = (int)header.width;
		_height = (int)header.height;
		int bpp = (int)header.bpp;
		if ( 32 != bpp )
		{
			return false;
		}

		int image_size = _width * _height * 4;
		unsigned char* targa = new unsigned char[ image_size ];
		count = (unsigned int)fread( targa, 1, image_size, file );
		if ( image_size != count )
		{
			return false;
		}

		error = fclose( file );
		if ( 0 != error )
		{
			return false;
		}

		m_targa_data = new unsigned char[ image_size ];
		int index = 0;
		int k = ( _width * _height * 4 ) - ( _width * 4 );

		for ( int j = 0; j < _height; ++j )
		{
			for ( int i = 0; i < _width; ++i )
			{
				m_targa_data[ index + 0 ] = targa[ k + 2 ];	// r
				m_targa_data[ index + 1 ] = targa[ k + 1 ];	// g
				m_targa_data[ index + 2 ] = targa[ k + 0 ];	// b
				m_targa_data[ index + 3 ] = targa[ k + 3 ];	// a

				k += 4;
				index += 4;
			}

			k -= ( _width * 8 );
		}

		delete[] targa;
		targa = nullptr;

		return true;
	}
}