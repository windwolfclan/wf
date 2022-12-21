#include "pch.h"
#include "Camera.h"

namespace wf
{
	Camera::Camera()
	{
	}

	Camera::Camera( const Camera& )
	{
	}

	Camera::~Camera()
	{
	}

	void Camera::SetPosition( float _x, float _y, float _z )
	{
		m_x = _x;
		m_y = _y;
		m_z = _z;
	}

	void Camera::SetRotation( float _pitch, float _yaw, float _roll )
	{
		m_pitch = _pitch;
		m_yaw = _yaw;
		m_roll = _roll;
	}

	XMFLOAT3 Camera::GetPosition() const
	{
		return XMFLOAT3{ m_x, m_y, m_z };
	}

	XMFLOAT3 Camera::GetRotation() const
	{
		return XMFLOAT3{ m_pitch, m_yaw, m_roll };
	}

	void Camera::Render()
	{
		XMFLOAT3 pos{ m_x, m_y, m_z };
		XMFLOAT3 up{ 0.0f, 1.0f, 0.0f };
		XMFLOAT3 focus{ 0.0f, 0.0f, 1.0f };

		XMVECTOR pos_vector = XMLoadFloat3( &pos );
		XMVECTOR up_vector = XMLoadFloat3( &up );
		XMVECTOR focus_vector = XMLoadFloat3( &focus );

		float pitch = m_pitch * 0.0174532925f;
		float yaw = m_yaw * 0.0174532925f;
		float roll = m_roll * 0.0174532925f;
		XMMATRIX rotate_matrix = XMMatrixRotationRollPitchYaw( pitch, yaw, roll );
		
		focus_vector = XMVector3TransformCoord( focus_vector, rotate_matrix );
		up_vector = XMVector3TransformCoord( up_vector, rotate_matrix );

		focus_vector = XMVectorAdd( pos_vector, focus_vector );

		m_view_matrix = XMMatrixLookAtLH( pos_vector, focus_vector, up_vector );
	}

	void Camera::GetViewMatrix( XMMATRIX& _m )
	{
		_m = m_view_matrix;
	}

	void Camera::RenderReflect( float _height )
	{
		XMFLOAT3 pos{ m_x, -m_y + ( _height * 2.0f ), m_z};
		XMFLOAT3 up{ 0.0f, 1.0f, 0.0f };
		XMFLOAT3 focus{ 0.0f, 0.0f, 1.0f };

		XMVECTOR pos_vector = XMLoadFloat3( &pos );
		XMVECTOR up_vector = XMLoadFloat3( &up );
		XMVECTOR focus_vector = XMLoadFloat3( &focus );

		float pitch = m_pitch * 0.0174532925f;
		float yaw = m_yaw * 0.0174532925f;
		float roll = m_roll * 0.0174532925f;
		XMMATRIX rotate_matrix = XMMatrixRotationRollPitchYaw( pitch, yaw, roll );

		focus_vector = XMVector3TransformCoord( focus_vector, rotate_matrix );
		up_vector = XMVector3TransformCoord( up_vector, rotate_matrix );

		focus_vector = XMVectorAdd( pos_vector, focus_vector );

		m_reflect_matrix = XMMatrixLookAtLH( pos_vector, focus_vector, up_vector );
	}

	void Camera::GetReflectionMatrix( XMMATRIX& _m )
	{
		_m = m_reflect_matrix;
	}
}