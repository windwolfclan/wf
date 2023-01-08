#include "pch.h"
#include "Light.h"

namespace wf
{
	Light::Light()
	{
	}

	Light::Light( const Light& )
	{
	}

	Light::~Light()
	{
	}

	void Light::SetAmbient( float _r, float _g, float _b, float _a )
	{
		m_ambient.x = _r;
		m_ambient.y = _g;
		m_ambient.z = _b;
		m_ambient.w = _a;
	}

	void Light::SetDiffuse( float _r, float _g, float _b, float _a )
	{
		m_diffuse.x = _r;
		m_diffuse.y = _g;
		m_diffuse.z = _b;
		m_diffuse.w = _a;
	}

	void Light::SetDirection( float _x, float _y, float _z )
	{
		m_direction.x = _x;
		m_direction.y = _y;
		m_direction.z = _z;
	}

	void Light::SetSpecular( float _r, float _g, float _b, float _a )
	{
		m_specular.x = _r;
		m_specular.y = _g;
		m_specular.z = _b;
		m_specular.w = _a;
	}

	void Light::SetSpecularPower( float _power )
	{
		m_specular_power = _power;
	}

	XMFLOAT4 Light::GetAmbient() const
	{
		return m_ambient;
	}

	XMFLOAT4 Light::GetDiffuse() const
	{
		return m_diffuse;
	}

	XMFLOAT3 Light::GetDirection() const
	{
		return m_direction;
	}

	XMFLOAT4 Light::GetSpecular() const
	{
		return m_specular;
	}

	float Light::GetSpecularPower() const
	{
		return m_specular_power;
	}

	void Light::SetPosition( float _x, float _y, float _z )
	{
		m_position.x = _x;
		m_position.y = _y;
		m_position.z = _z;
	}

	XMFLOAT3 Light::GetPosition() const
	{
		return m_position;
	}

	void Light::SetLookAt( float _x, float _y, float _z )
	{
		m_look_at.x = _x;
		m_look_at.y = _y;
		m_look_at.z = _z;
	}

	XMFLOAT3 Light::GetLookAt() const
	{
		return m_look_at;
	}

	void Light::GenerateViewMatrix()
	{
		XMFLOAT3 up{ 0.0f, 1.0f, 0.0f };

		XMVECTOR p = XMLoadFloat3( &m_position );
		XMVECTOR l = XMLoadFloat3( &m_look_at );
		XMVECTOR u = XMLoadFloat3( &up );

		m_view = XMMatrixLookAtLH( p, l, u );
	}

	void Light::GenerateProjectionMatrix( float _near_z, float _far_z )
	{
		float fov = XM_PIDIV2;
		float aspect = 1.0f;

		m_projection = XMMatrixPerspectiveFovLH( fov, aspect, _near_z, _far_z );
	}

	XMMATRIX Light::GetViewMatrix() const
	{
		return m_view;
	}

	XMMATRIX Light::GetProjectionMatrix() const
	{
		return m_projection;
	}

}