#include "pch.h"
#include "TextureArray.h"

namespace wf
{
	TextureArray::TextureArray()
	{
	}

	TextureArray::TextureArray( const TextureArray& )
	{
	}

	TextureArray::~TextureArray()
	{
	}

	bool TextureArray::Intialize( ID3D11Device* _device, ID3D11DeviceContext* _context, const wchar_t* _texture_path1, const wchar_t* _texture_path2 )
	{
		if ( !LoadTexture( _device, _context, _texture_path1, m_textures[ 0 ], m_views[ 0 ] ) )
		{
			return false;
		}

		if ( !LoadTexture( _device, _context, _texture_path2, m_textures[ 1 ], m_views[ 1 ] ) )
		{
			return false;
		}

		return true;
	}

	bool TextureArray::Intialize( ID3D11Device* _device, ID3D11DeviceContext* _context, const wchar_t* _texture_path1, const wchar_t* _texture_path2, const wchar_t* _texture_path3 )
	{
		if ( nullptr == _texture_path3 )
		{
			return Intialize( _device, _context, _texture_path1, _texture_path2 );
		}

		if ( !LoadTexture( _device, _context, _texture_path1, m_textures[ 0 ], m_views[ 0 ] ) )
		{
			return false;
		}

		if ( !LoadTexture( _device, _context, _texture_path2, m_textures[ 1 ], m_views[ 1 ] ) )
		{
			return false;
		}

		if ( !LoadTexture( _device, _context, _texture_path3, m_textures[ 2 ], m_views[ 2 ] ) )
		{
			return false;
		}

		return true;
	}

	void TextureArray::Shutdown()
	{
		SAFE_RELEASE( m_views[ 2 ] );
		SAFE_RELEASE( m_views[ 1 ] );
		SAFE_RELEASE( m_views[ 0 ] );

		SAFE_RELEASE( m_textures[ 2 ] );
		SAFE_RELEASE( m_textures[ 1 ] );
		SAFE_RELEASE( m_textures[ 0 ] );
	}

	ID3D11ShaderResourceView** TextureArray::GetTextureArray()
	{
		return m_views;
	}

	bool TextureArray::LoadTexture( ID3D11Device* _device, ID3D11DeviceContext* _context, const wchar_t* _texture_path, ID3D11Texture2D*& _texture, ID3D11ShaderResourceView*& _srv )
	{
		unsigned char* buffer{ nullptr };
		int width{ 0 };
		int height{ 0 };
		if ( !LoadTarga( _texture_path, width, height, buffer ) )
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

		HRESULT hr = _device->CreateTexture2D( &texture_desc, nullptr, &_texture );
		if ( FAILED( hr ) )
		{
			SAFE_DELETE_ARRAY( buffer );
			return false;
		}

		unsigned int pitch = ( width * 4 ) * sizeof( unsigned char );
		_context->UpdateSubresource( _texture, 0, nullptr, buffer, pitch, 0 );

		D3D11_SHADER_RESOURCE_VIEW_DESC view_desc{};
		ZeroMemory( &view_desc, sizeof( view_desc ) );
		view_desc.Format = texture_desc.Format;
		view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		view_desc.Texture2D.MostDetailedMip = 0;
		view_desc.Texture2D.MipLevels = 1;

		hr = _device->CreateShaderResourceView( _texture, &view_desc, &_srv );
		if ( FAILED( hr ) )
		{
			SAFE_DELETE_ARRAY( buffer );
			return false;
		}

		_context->GenerateMips( _srv );

		SAFE_DELETE_ARRAY( buffer );

		return true;
	}

}