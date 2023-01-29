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

void BasicApp::OnResize()
{
	__super::OnResize();
}

void BasicApp::Update( const GameTimer& _game_timer )
{
}

void BasicApp::Render( const GameTimer& _game_timer )
{
}

NAMESPACE_WF_E