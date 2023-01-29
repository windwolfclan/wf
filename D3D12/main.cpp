#include "pch.h"
#include "BasicApp.h"


int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd )
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	wf::BasicApp* app = new wf::BasicApp( hInstance );
	if ( !app )
	{
		return - 1;
	}

	if ( !app->Initialize() )
	{
		return -2;
	}

	int ret = app->Run();

	delete app;
	app = nullptr;

	return 0;
}