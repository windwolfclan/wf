#include "pch.h"
#include "Tutorial6BoxApp.h"

NAMESPACE_WF_S

Tutorial6BoxApp::Tutorial6BoxApp( HINSTANCE _instance )
	: AppBase( _instance )
{
}

Tutorial6BoxApp::~Tutorial6BoxApp()
{
}

bool Tutorial6BoxApp::Initialize()
{
	if ( !__super::Initialize() )
	{
		return false;
	}

	ID3D12GraphicsCommandList* command_list = context->command_list;
	ID3D12CommandAllocator* command_allocator = context->command_allocator;

	HRESULT hr = command_list->Reset( command_allocator, nullptr );
	if ( FAILED( hr ) )
	{
		return false;
	}


	if ( !BuildDescriptorHeaps() )
	{
		return false;
	}

	if ( !BuildConstantBuffers() )
	{
		return false;
	}

	if ( !BuildRootSignature() )
	{
		return false;
	}

	if ( !BuildShaderAndInputLaytouts() )
	{
		return false;
	}

	if ( !BuildGeometry() )
	{
		return false;
	}

	if ( !BuildPipelineState() )
	{
		return false;
	}

	hr = command_list->Close();
	if ( FAILED( hr ) )
	{
		return false;
	}

	ID3D12CommandList* lists[]{ command_list };
	ID3D12CommandQueue* command_queue = context->command_queue;
	command_queue->ExecuteCommandLists( _countof( lists ), lists );

	context->FlushCommandQueue();

	return true;
}

void Tutorial6BoxApp::Terminate()
{
	SAFE_RELEASE( m_pipeline_state );
	SAFE_RELEASE( m_ps_byte_code );
	SAFE_RELEASE( m_vs_byte_code );
	SAFE_RELEASE( m_root_signature );
	SAFE_RELEASE( m_cbv_heap );

	__super::Terminate();
}

void Tutorial6BoxApp::Update( const GameTimer& _game_timer )
{
	float x = m_radius * std::sinf( m_phi ) * std::cosf( m_theta );
	float y = m_radius * std::sinf( m_phi ) * std::sinf( m_theta );
	float z = m_radius * std::cosf( m_phi );

	XMVECTOR pos = XMVectorSet( x, y, z, 1.0f );
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );

	XMMATRIX v = XMMatrixLookAtLH( pos, target, up );
	XMStoreFloat4x4( &m_view, v );

	XMMATRIX p = XMMatrixPerspectiveFovLH( 0.25f * MathHelper::Pi, GetAspectRatio(), 1.0f, 1000.0f );
	XMStoreFloat4x4( &m_projection, p );

	XMMATRIX world = XMLoadFloat4x4( &m_world );
	XMMATRIX view = XMLoadFloat4x4( &m_view );
	XMMATRIX projection = XMLoadFloat4x4( &m_projection );


	XMMATRIX wvp = world * view * projection;

	ObjectConstants object_constants;
	XMStoreFloat4x4( &object_constants.wvp, XMMatrixTranspose( wvp ) );
	m_object_cb->CopyData( 0, object_constants );
}

void Tutorial6BoxApp::Render( const GameTimer& _game_timer )
{
	ID3D12GraphicsCommandList* command_list = context->command_list;
	ID3D12CommandAllocator* command_allocator = context->command_allocator;
	ID3D12CommandQueue* command_queue = context->command_queue;


	HRESULT hr = command_allocator->Reset();
	if ( FAILED( hr ) )
	{
		return;
	}

	hr = command_list->Reset( command_allocator, m_pipeline_state );
	if ( FAILED( hr ) )
	{
		return;
	}

	context->SetScreenViewport();
	context->SetScissorRect();

	CD3DX12_RESOURCE_BARRIER barrier1 = CD3DX12_RESOURCE_BARRIER::Transition( context->GetBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET );
	command_list->ResourceBarrier( 1, &barrier1 );

	context->ClearRenderTargetView( 0.0f, 0.0f, 0.0f, 1.0f );
	context->ClearDepthStencilView();
	context->SetBackBufferRenderTarget();

	ID3D12DescriptorHeap* descriptor_hepas[]{ m_cbv_heap };
	command_list->SetDescriptorHeaps( _countof( descriptor_hepas ), descriptor_hepas );

	command_list->SetGraphicsRootSignature( m_root_signature );

	D3D12_VERTEX_BUFFER_VIEW vbv = m_box->VertexBufferView();
	command_list->IASetVertexBuffers( 0, 1, &vbv );

	D3D12_INDEX_BUFFER_VIEW ibv = m_box->IndexBufferView();
	command_list->IASetIndexBuffer( &ibv );

	command_list->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	command_list->SetGraphicsRootDescriptorTable( 0, m_cbv_heap->GetGPUDescriptorHandleForHeapStart() );

	command_list->DrawIndexedInstanced(
		m_box->draw_arguments[ "box" ].index_count,
		1,
		0,
		0,
		0
	);

	CD3DX12_RESOURCE_BARRIER barrier2 = CD3DX12_RESOURCE_BARRIER::Transition( context->GetBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT );
	command_list->ResourceBarrier( 1, &barrier2 );

	hr = command_list->Close();
	if ( FAILED( hr ) )
	{
		return;
	}

	ID3D12CommandList* lists[]{ command_list };
	command_queue->ExecuteCommandLists( _countof( lists ), lists );

	context->Present();

	context->FlushCommandQueue();
}

void Tutorial6BoxApp::OnMouseDown( WPARAM _btn, int _x, int _y )
{
	m_last_mouse_pos.x = _x;
	m_last_mouse_pos.y = _y;

	SetCapture( m_wnd );
}

void Tutorial6BoxApp::OnMouseUp( WPARAM _btn, int _x, int _y )
{
	ReleaseCapture();
}

void Tutorial6BoxApp::OnMouseMove( WPARAM _btn, int _x, int _y )
{
	if ( ( _btn & MK_LBUTTON ) != 0 )
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians( 0.25f * static_cast<float>( _x - m_last_mouse_pos.x ) );
		float dy = XMConvertToRadians( 0.25f * static_cast<float>( _y - m_last_mouse_pos.y ) );

		// Update angles based on input to orbit camera around box.
		m_theta += dx;
		m_phi += dy;

		// Restrict the angle mPhi.
		m_phi = MathHelper::Clamp( m_phi, 0.1f, MathHelper::Pi - 0.1f );
	}
	else if ( ( _btn & MK_RBUTTON ) != 0 )
	{
		// Make each pixel correspond to 0.005 unit in the scene.
		float dx = 0.005f * static_cast<float>( _x - m_last_mouse_pos.x );
		float dy = 0.005f * static_cast<float>( _y - m_last_mouse_pos.y );

		// Update the camera radius based on input.
		m_radius += dx - dy;

		// Restrict the radius.
		m_radius = MathHelper::Clamp( m_radius, 3.0f, 15.0f );
	}

	m_last_mouse_pos.x = _x;
	m_last_mouse_pos.y = _y;
}

bool Tutorial6BoxApp::BuildDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC cbv_heap_desc;
	ZeroMemory( &cbv_heap_desc, sizeof( cbv_heap_desc ) );
	cbv_heap_desc.NumDescriptors = 1;
	cbv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbv_heap_desc.NodeMask = 0;

	HRESULT hr = GetDevice()->CreateDescriptorHeap( &cbv_heap_desc, IID_PPV_ARGS( &m_cbv_heap ) );
	if ( FAILED( hr ) )
	{
		return false;
	}

	return true;
}

bool Tutorial6BoxApp::BuildConstantBuffers()
{
	m_object_cb = std::make_unique<UploadBuffer<ObjectConstants>>( GetDevice(), 1, true );
	UINT object_cb_byte_size = utility::CalculateConstantBufferByteSize( sizeof( ObjectConstants ) );

	D3D12_GPU_VIRTUAL_ADDRESS address = m_object_cb->GetResource()->GetGPUVirtualAddress();
	int box_cb_index{ 0 };
	address += box_cb_index * object_cb_byte_size;

	D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
	ZeroMemory( &desc, sizeof( desc ) );
	desc.BufferLocation = address;
	desc.SizeInBytes = object_cb_byte_size;

	GetDevice()->CreateConstantBufferView( &desc, m_cbv_heap->GetCPUDescriptorHandleForHeapStart() );

	return true;
}

bool Tutorial6BoxApp::BuildRootSignature()
{
	CD3DX12_ROOT_PARAMETER slot_root_parameter[ 1 ];
	CD3DX12_DESCRIPTOR_RANGE cbv_table;
	cbv_table.Init( D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0 );
	slot_root_parameter[ 0 ].InitAsDescriptorTable( 1, &cbv_table );

	CD3DX12_ROOT_SIGNATURE_DESC root_signature_desc( 
		1, 
		slot_root_parameter, 
		0, 
		nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT 
	);

	ID3DBlob* serialized_root_signature{ nullptr };
	ID3DBlob* error_blob{ nullptr };

	HRESULT hr = D3D12SerializeRootSignature(
		&root_signature_desc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&serialized_root_signature,
		&error_blob
	);

	if ( nullptr != error_blob )
	{
		::OutputDebugStringA( (char*)error_blob->GetBufferPointer() );
		SAFE_RELEASE( error_blob );
		return false;
	}

	hr = GetDevice()->CreateRootSignature(
		0,
		serialized_root_signature->GetBufferPointer(),
		serialized_root_signature->GetBufferSize(),
		IID_PPV_ARGS( &m_root_signature )
	);
	if ( FAILED( hr ) )
	{
		return false;
	}

	return true;
}

bool Tutorial6BoxApp::BuildShaderAndInputLaytouts()
{
	m_vs_byte_code = utility::CompileShader( L"shaders\\color.hlsl", nullptr, "vs", "vs_5_0" );
	if ( !m_vs_byte_code )
	{
		return false;
	}

	m_ps_byte_code = utility::CompileShader( L"shaders\\color.hlsl", nullptr, "ps", "ps_5_0" );
	if ( !m_ps_byte_code )
	{
		return false;
	}

	m_input_laytouts =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	return true;
}

bool Tutorial6BoxApp::BuildGeometry()
{
	std::array<BoxColorVertex, 8> vertices =
	{
		BoxColorVertex( { XMFLOAT3( -1.0f, -1.0f, -1.0f ), XMFLOAT4( Colors::White ) } ),
		BoxColorVertex( { XMFLOAT3( -1.0f, +1.0f, -1.0f ), XMFLOAT4( Colors::Black ) } ),
		BoxColorVertex( { XMFLOAT3( +1.0f, +1.0f, -1.0f ), XMFLOAT4( Colors::Red ) } ),
		BoxColorVertex( { XMFLOAT3( +1.0f, -1.0f, -1.0f ), XMFLOAT4( Colors::Green ) } ),
		BoxColorVertex( { XMFLOAT3( -1.0f, -1.0f, +1.0f ), XMFLOAT4( Colors::Blue ) } ),
		BoxColorVertex( { XMFLOAT3( -1.0f, +1.0f, +1.0f ), XMFLOAT4( Colors::Yellow ) } ),
		BoxColorVertex( { XMFLOAT3( +1.0f, +1.0f, +1.0f ), XMFLOAT4( Colors::Cyan ) } ),
		BoxColorVertex( { XMFLOAT3( +1.0f, -1.0f, +1.0f ), XMFLOAT4( Colors::Magenta ) } )
	};

	std::array<std::uint16_t, 36> indices =
	{
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};

	const UINT vb_byte_size = (UINT)vertices.size() * sizeof( BoxColorVertex );
	const UINT ib_byte_size = (UINT)indices.size() * sizeof( std::uint16_t );

	m_box = std::make_unique<MeshGeometry>();
	m_box->name = "box";

	HRESULT hr;
	hr = D3DCreateBlob( vb_byte_size, &m_box->cpu_vertex_buffer );
	if ( FAILED( hr ) )
	{
		return false;
	}

	hr = D3DCreateBlob( ib_byte_size, &m_box->cpu_index_buffer );
	if ( FAILED( hr ) )
	{
		return false;
	}

	CopyMemory( m_box->cpu_vertex_buffer->GetBufferPointer(), vertices.data(), vb_byte_size );
	CopyMemory( m_box->cpu_index_buffer->GetBufferPointer(), indices.data(), ib_byte_size );

	ID3D12Device* device = GetDevice();

	m_box->gpu_vertex_buffer = utility::CreateDefaultBuffer( device, context->command_list, vertices.data(), vb_byte_size, m_box->vertex_buffer_uploader );
	if ( !m_box->gpu_vertex_buffer )
	{
		return false;
	}

	m_box->gpu_index_buffer = utility::CreateDefaultBuffer( device, context->command_list, indices.data(), ib_byte_size, m_box->index_buffer_uploader );
	if ( !m_box->gpu_index_buffer )
	{
		return false;
	}

	m_box->vertex_byte_stride = sizeof( BoxColorVertex );
	m_box->vertex_buffer_byte_size = vb_byte_size;
	m_box->index_format = DXGI_FORMAT_R16_UINT;
	m_box->index_buffer_byte_size = ib_byte_size;

	SubmeshGeometry submesh;
	submesh.index_count = (UINT)indices.size();
	submesh.start_index_location = 0;
	submesh.base_vertex_location = 0;

	m_box->draw_arguments[ "box" ] = submesh;

	return true;
}

bool Tutorial6BoxApp::BuildPipelineState()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc;
	ZeroMemory( &desc, sizeof( desc ) );
	desc.InputLayout = { m_input_laytouts.data(), (UINT)m_input_laytouts.size() };
	desc.pRootSignature = m_root_signature;
	desc.VS =
	{
		reinterpret_cast<BYTE*>( m_vs_byte_code->GetBufferPointer() ),
		m_vs_byte_code->GetBufferSize()
	};
	desc.PS =
	{
		reinterpret_cast<BYTE*>( m_ps_byte_code->GetBufferPointer() ),
		m_ps_byte_code->GetBufferSize()
	};

	desc.RasterizerState = CD3DX12_RASTERIZER_DESC( D3D12_DEFAULT );
	desc.BlendState = CD3DX12_BLEND_DESC( D3D12_DEFAULT );
	desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC( D3D12_DEFAULT );
	desc.SampleMask = UINT_MAX;
	desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	desc.NumRenderTargets = 1;
	desc.RTVFormats[ 0 ] = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	HRESULT hr = GetDevice()->CreateGraphicsPipelineState( &desc, IID_PPV_ARGS( &m_pipeline_state ) );
	if ( FAILED( hr ) )
	{
		return false;
	}

	return true;
}

NAMESPACE_WF_E