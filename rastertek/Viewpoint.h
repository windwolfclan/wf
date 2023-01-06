#pragma once

namespace wf
{
	class Viewpoint
	{
	public:
		void SetPosition( float _x, float _y, float _z );
		void SetLookAt( float _x, float _y, float _z );
		void SetProjectionParameters( float _field_of_view, float _aspect_ratio, float _near_plane, float _far_plane );

		void GenerateViewMatrix();
		void GenerateProjectionMatrix();

		XMMATRIX GetViewMatrix() const;
		XMMATRIX GetProjectionMatrix() const;

	private:
		XMFLOAT3 position;
		XMFLOAT3 look_at;

		XMMATRIX view;
		XMMATRIX projection;

		float field_of_view;
		float aspect_ratio;
		float near_plane;
		float far_plane;
	};
}