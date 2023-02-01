#pragma once

NAMESPACE_WF_S

struct BoxColorVertex
{
	XMFLOAT3 pos;
	XMFLOAT4 color;
};

struct ObjectConstants
{
	XMFLOAT4X4 wvp = MathHelper::Identity4x4();
};

class Tutorial6BoxApp
	: public AppBase
{
public:
	Tutorial6BoxApp( HINSTANCE _instance );
	virtual ~Tutorial6BoxApp();

	virtual bool Initialize() override;
	virtual void Terminate() override;
private:
	virtual void Update( const GameTimer& _game_timer ) override;
	virtual void Render( const GameTimer& _game_timer ) override;

	virtual void OnMouseDown( WPARAM _btn, int _x, int _y ) override;
	virtual void OnMouseUp( WPARAM _btn, int _x, int _y ) override;
	virtual void OnMouseMove( WPARAM _btn, int _x, int _y ) override;

	bool BuildDescriptorHeaps();
	bool BuildConstantBuffers();
	bool BuildRootSignature();
	bool BuildShaderAndInputLaytouts();
	bool BuildGeometry();
	bool BuildPipelineState();

private:
	ID3D12RootSignature* m_root_signature{ nullptr };
	ID3D12DescriptorHeap* m_cbv_heap{ nullptr };

	std::unique_ptr<UploadBuffer<ObjectConstants>> m_object_cb{ nullptr };
	std::unique_ptr<MeshGeometry> m_box{ nullptr };

	std::vector<D3D12_INPUT_ELEMENT_DESC> m_input_laytouts;
	ID3DBlob* m_vs_byte_code{ nullptr };
	ID3DBlob* m_ps_byte_code{ nullptr };

	ID3D12PipelineState* m_pipeline_state{ nullptr };

	XMFLOAT4X4 m_world{ MathHelper::Identity4x4() };
	XMFLOAT4X4 m_view{ MathHelper::Identity4x4() };
	XMFLOAT4X4 m_projection{ MathHelper::Identity4x4() };

	float m_theta{ 1.5f * XM_PI };
	float m_phi{ XM_PIDIV4 };
	float m_radius{ 5.0f };

	POINT m_last_mouse_pos{ 0, 0 };
};

NAMESPACE_WF_E