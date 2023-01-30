#include "pch.h"
#include "BasicApp.h"

NAMESPACE_WF_S

BasicApp::BasicApp( HINSTANCE _instance )
	: AppBase( _instance )
{
}

BasicApp::~BasicApp()
{
}

bool BasicApp::Initialize()
{
	if ( !__super::Initialize() )
	{
		return false;
	}

	return true;
}

void BasicApp::Update( const GameTimer& _game_timer )
{
}

void BasicApp::Render( const GameTimer& _game_timer )
{
	ID3D12CommandQueue*& command_queue = context->command_queue;
	ID3D12CommandAllocator*& command_allocator = context->command_allocator;
	ID3D12GraphicsCommandList*& command_list = context->command_list;

	CHECK_HR( command_allocator->Reset() );
	CHECK_HR( command_list->Reset( command_allocator, nullptr ) );

	CD3DX12_RESOURCE_BARRIER barrier1 = CD3DX12_RESOURCE_BARRIER::Transition(
		context->GetBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	command_list->ResourceBarrier( 1, &barrier1 );

	context->SetScreenViewport();
	context->SetScissorRect();
	context->ClearRenderTargetView( 0.0f, 0.0f, 1.0f, 1.0f );
	context->ClearDepthStencilView();
	context->SetBackBufferRenderTarget();

	CD3DX12_RESOURCE_BARRIER barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(
		context->GetBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);

	command_list->ResourceBarrier( 1, &barrier2 );

	CHECK_HR( command_list->Close() );

	ID3D12CommandList* lists[]{ command_list };
	command_queue->ExecuteCommandLists( _countof( lists ), lists );

	context->Present();

	context->FlushCommandQueue();
}

NAMESPACE_WF_E