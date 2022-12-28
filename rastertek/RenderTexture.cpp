#include "pch.h"
#include "RenderTexture.h"

namespace wf
{
	RenderTexture::RenderTexture()
	{

	}

	RenderTexture::RenderTexture( const RenderTexture& )
	{

	}

	RenderTexture::~RenderTexture()
	{

	}

	bool RenderTexture::Initialize( ID3D11Device* _device, int _width, int _height )
	{
		m_width = _width;
		m_height = _height;

		D3D11_TEXTURE2D_DESC texture_desc{};
		ZeroMemory( &texture_desc, sizeof( texture_desc ) );
		texture_desc.Width = static_cast<UINT>( _width );
		texture_desc.Height = static_cast<UINT>( _height );
		texture_desc.MipLevels = 1;
		texture_desc.ArraySize = 1;
		texture_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		texture_desc.SampleDesc.Count = 1;
		texture_desc.Usage = D3D11_USAGE_DEFAULT;
		texture_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texture_desc.CPUAccessFlags = 0;
		texture_desc.MiscFlags = 0;

		HRESULT hr = _device->CreateTexture2D( &texture_desc, nullptr, &m_render_target_texture );
		if ( FAILED( hr ) )
		{
			return false;
		}

		D3D11_RENDER_TARGET_VIEW_DESC rtv_desc{};
		ZeroMemory( &rtv_desc, sizeof( rtv_desc ) );
		rtv_desc.Format = texture_desc.Format;
		rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtv_desc.Texture2D.MipSlice = 0;

		hr = _device->CreateRenderTargetView( m_render_target_texture, &rtv_desc, &m_render_target_view );
		if ( FAILED( hr ) )
		{
			return false;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc{};
		ZeroMemory( &srv_desc, sizeof( srv_desc ) );
		srv_desc.Format = texture_desc.Format;
		srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srv_desc.Texture2D.MostDetailedMip = 0;
		srv_desc.Texture2D.MipLevels = 1;

		hr = _device->CreateShaderResourceView( m_render_target_texture, &srv_desc, &m_shader_resource_view );
		if ( FAILED( hr ) )
		{
			return false;
		}

		D3D11_TEXTURE2D_DESC depth_desc{};
		ZeroMemory( &depth_desc, sizeof( depth_desc ) );
		depth_desc.Width = _width;
		depth_desc.Height = _height;
		depth_desc.MipLevels = 1;
		depth_desc.ArraySize = 1;
		depth_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depth_desc.SampleDesc.Count = 1;
		depth_desc.SampleDesc.Quality = 0;
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
		dsv_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsv_desc.Texture2D.MipSlice = 0;

		hr = _device->CreateDepthStencilView( m_depth_stencil_texture, &dsv_desc, &m_depth_stencil_view );
		if ( FAILED( hr ) )
		{
			return false;
		}

		m_viewport.Width = (float)_width;
		m_viewport.Height = (float)_height;
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;
		m_viewport.TopLeftX = 0.0f;
		m_viewport.TopLeftY = 0.0f;

		m_projection = XMMatrixPerspectiveFovLH( XM_PIDIV4, _width / (float)_height, 0.1f, 1000.0f );

		m_ortho = XMMatrixOrthographicLH( (float)_width, (float)_height, 0.1f, 1000.0f );

		return true;
	}

	void RenderTexture::Shutdown()
	{
		SAFE_RELEASE( m_depth_stencil_view );
		SAFE_RELEASE( m_depth_stencil_texture );
		SAFE_RELEASE( m_shader_resource_view );
		SAFE_RELEASE( m_render_target_view );
		SAFE_RELEASE( m_render_target_texture );
	}

	void RenderTexture::SetRenderTarget( ID3D11DeviceContext* _context )
	{
		ID3D11ShaderResourceView* srv{ nullptr };
		_context->PSSetShaderResources( 0, 1, &srv );

		_context->OMSetRenderTargets( 1, &m_render_target_view, m_depth_stencil_view );

		_context->RSSetViewports( 1, &m_viewport );
	}

	void RenderTexture::ClearRenderTarget( ID3D11DeviceContext* _context, float _r, float _g, float _b, float _a )
	{
		const float color[ 4 ]{ _r, _g, _b, _a };

		_context->ClearRenderTargetView( m_render_target_view, color );

		_context->ClearDepthStencilView( m_depth_stencil_view, D3D11_CLEAR_DEPTH, 1.0f, 0 );
	}

	ID3D11ShaderResourceView* RenderTexture::GetShaderResourceView() const
	{
		return m_shader_resource_view;
	}

	XMMATRIX RenderTexture::GetProjectionMatrix() const
	{
		return m_projection;
	}

	XMMATRIX RenderTexture::GetOrthoMatrix() const
	{
		return m_ortho;
	}

	int RenderTexture::GetWidth() const
	{
		return m_width;
	}

	int RenderTexture::GetHeight() const
	{
		return m_height;
	}

}