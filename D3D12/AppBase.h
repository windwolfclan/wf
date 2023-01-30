#pragma once

NAMESPACE_WF_S


class AppBase
{
	// static
public:
	static AppBase* GetApp();
protected:
	static AppBase* m_app;

protected:
	AppBase( HINSTANCE _instance );
	AppBase( const AppBase& ) = delete;
	AppBase& operator = ( const AppBase& ) = delete;
	virtual ~AppBase();

public:
	HINSTANCE GetInstance() const;
	HWND GetWindowHandle() const;
	float GetAspectRatio() const;

	int Run();

	virtual bool Initialize();
	virtual LRESULT MsgProc( HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam );

protected:
	virtual void Update( const GameTimer& _game_timer ) = 0;
	virtual void Render( const GameTimer& _game_timer ) = 0;

	virtual void OnMouseDown( WPARAM _btn, int _x, int _y );
	virtual void OnMouseUp( WPARAM _btn, int _x, int _y );
	virtual void OnMouseMove( WPARAM _btn, int _x, int _y );

protected:
	bool InitializeMainWindow();
	void CalculateFrameStats();

protected:
	HINSTANCE m_instance{ nullptr };
	HWND m_wnd{ nullptr };

	bool m_app_paused{ false };
	bool m_minimized{ false };
	bool m_maximized{ false };
	bool m_resizing{ false };
	bool m_fullscreen{ false };

	std::wstring m_window_caption{ L"WindWolf - D3D12" };
	int m_client_width{ 1600 };
	int m_client_height{ 900 };

	GameTimer m_game_timer;

	std::shared_ptr<D3D12Context> context{ nullptr };

};

NAMESPACE_WF_E