#pragma once

NAMESPACE_WF_S

class BasicApp
	: public AppBase
{
public:
	BasicApp( HINSTANCE _instance );
	virtual ~BasicApp();

	virtual bool Initialize() override;

private:
	virtual void OnResize()override;
	virtual void Update( const GameTimer& _game_timer ) override;
	virtual void Render( const GameTimer& _game_timer ) override;
};

NAMESPACE_WF_E