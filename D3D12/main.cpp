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

	try
	{
		if ( !app->Initialize() )
		{
			return -2;
		}

		int ret = app->Run();

	}
	catch ( wf::DxException& e )
	{
		MessageBox( nullptr, e.ToString().c_str(), L"HR Failed", MB_OK );

		SAFE_DELETE( app );
		return -3;
	}

	SAFE_DELETE( app );

	return 0;
}