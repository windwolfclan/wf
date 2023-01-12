#include "pch.h"
#include "DefferedBuffer.h"

namespace wf
{
	bool DefferedBuffer::Initialize( ID3D11Device* _device, int _width, int _height, float _near_z, float _far_z )
	{
		m_width = _width;
		m_height = _height;

		D3D11_TEXTURE2D_DESC texture_desc{};
		ZeroMemory( &texture_desc, sizeof( texture_desc ) );
		texture_desc.Width = (UINT)m_width;
		texture_desc.Height = (UINT)m_height;
		texture_desc.MipLevels = 1;
		texture_desc.ArraySize = 1;
		texture_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		texture_desc.Usage = D3D11_USAGE_DEFAULT;
		texture_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texture_desc.CPUAccessFlags = 0;
		texture_desc.MiscFlags = 0;

		D3D11_RENDER_TARGET_VIEW_DESC rtv_desc{};
		ZeroMemory( &rtv_desc, sizeof( rtv_desc ) );
		rtv_desc.Format = texture_desc.Format;
		rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtv_desc.Texture2D.MipSlice = 0;

		D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc{};
		ZeroMemory( &srv_desc, sizeof( srv_desc ) );
		srv_desc.Format = texture_desc.Format;
		srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srv_desc.Texture2D.MostDetailedMip = 0;
		srv_desc.Texture2D.MipLevels = 1;

		HRESULT hr;
		for ( int i = 0; i < BUFFER_COUNT; ++i )
		{
			hr = _device->CreateTexture2D( &texture_desc, nullptr, &m_render_target_textures[ i ] );
			if ( FAILED( hr ) )
			{
				return false;
			}

			hr = _device->CreateRenderTargetView( m_render_target_textures[ i ], &rtv_desc, &m_render_target_views[ i ] );
			if ( FAILED( hr ) )
			{
				return false;
			}

			hr = _device->CreateShaderResourceView( m_render_target_textures[ i ], &srv_desc, &m_shader_resource_views[ i ] );
			if ( FAILED( hr ) )
			{
				return false;
			}
		}

		D3D11_TEXTURE2D_DESC depth_desc{};
		ZeroMemory( &depth_desc, sizeof( depth_desc ) );
		depth_desc.Width = (UINT)m_width;
		depth_desc.Height = (UINT)m_height;
		depth_desc.MipLevels = 1;
		depth_desc.ArraySize = 1;
		depth_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depth_desc.SampleDesc.Count = 1;
		depth_desc.SampleDesc.Quality = 0;
		depth_desc.Usage = D3D11_USAGE_DEFAULT;
		depth_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depth_desc.CPUAccessFlags = 0;
		depth_desc.MiscFlags = 0;

		hr = _device->CreateTexture2D( &depth_desc, nullptr, &m_depth_stencil_texture );
		if ( FAILED( hr ) )
		{
			return false;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc{};
		ZeroMemory( &dsv_desc, sizeof( dsv_desc ) );
		dsv_desc.Format = depth_desc.Format;
		dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsv_desc.Texture2D.MipSlice = 0;

		hr = _device->CreateDepthStencilView( m_depth_stencil_texture, &dsv_desc, &m_depth_stencil_view );
		if ( FAILED( hr ) )
		{
			return false;
		}

		m_viewport.Width = (float)m_width;
		m_viewport.Height = (float)m_height;
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;
		m_viewport.TopLeftX = 0.0f;
		m_viewport.TopLeftY = 0.0f;

		return true;
	}

	void DefferedBuffer::Shutdown()
	{
		SAFE_RELEASE( m_depth_stencil_view );
		SAFE_RELEASE( m_depth_stencil_texture );

		for ( int i = 0; i < BUFFER_COUNT; ++i )
		{
			SAFE_RELEASE( m_shader_resource_views[ i ] );
			SAFE_RELEASE( m_render_target_views[ i ] );
			SAFE_RELEASE( m_render_target_textures[ i ] );
		}
	}

	void DefferedBuffer::SetRenderTargets( ID3D11DeviceContext* _context )
	{
		_context->OMSetRenderTargets( BUFFER_COUNT, m_render_target_views, m_depth_stencil_view );
		_context->RSSetViewports( 1, &m_viewport );
	}

	void DefferedBuffer::ClearRenderTargets( ID3D11DeviceContext* _context, float _r, float _g, float _b, float _a )
	{
		const float color[]{ _r, _g, _b, _a };

		for ( int i = 0; i < BUFFER_COUNT; ++i )
		{
			_context->ClearRenderTargetView( m_render_target_views[ i ], color );
		}

		_context->ClearDepthStencilView( m_depth_stencil_view, D3D11_CLEAR_DEPTH, 1.0f, 0 );
	}

	ID3D11ShaderResourceView* DefferedBuffer::GetShaderResourceView( int _index )
	{
		return m_shader_resource_views[ _index ];
	}

}