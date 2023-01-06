#include "pch.h"
#include "Viewpoint.h"

namespace wf
{
	void Viewpoint::SetPosition( float _x, float _y, float _z )
	{
		position.x = _x;
		position.y = _y;
		position.z = _z;
	}

	void Viewpoint::SetLookAt( float _x, float _y, float _z )
	{
		look_at.x = _x;
		look_at.y = _y;
		look_at.z = _z;
	}

	void Viewpoint::SetProjectionParameters( float _field_of_view, float _aspect_ratio, float _near_plane, float _far_plane )
	{
		field_of_view = _field_of_view;
		aspect_ratio = _aspect_ratio;
		near_plane = _near_plane;
		far_plane = _far_plane;
	}

	void Viewpoint::GenerateViewMatrix()
	{
		XMFLOAT3 up{ 0.0f, 1.0f, 0.0f };

		XMVECTOR v_position = XMLoadFloat3( &position );
		XMVECTOR v_up = XMLoadFloat3( &up );
		XMVECTOR v_look_at = XMLoadFloat3( &look_at );

		view = XMMatrixLookAtLH( v_position, v_look_at, v_up );
	}

	void Viewpoint::GenerateProjectionMatrix()
	{
		projection = XMMatrixPerspectiveFovLH( field_of_view, aspect_ratio, near_plane, far_plane );
	}

	XMMATRIX Viewpoint::GetViewMatrix() const
	{
		return view;
	}

	XMMATRIX Viewpoint::GetProjectionMatrix() const
	{
		return projection;
	}
}