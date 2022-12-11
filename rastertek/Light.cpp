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

}