#pragma once

namespace wf
{
	class Camera
	{
	public:
		Camera();
		Camera( const Camera& );
		~Camera();

		void SetPosition( float _x, float _y, float _z );
		void SetRotation( float _pitch, float _yaw, float _roll );

		XMFLOAT3 GetPosition() const;
		XMFLOAT3 GetRotation() const;

		void Render();
		void GetViewMatrix( XMMATRIX& _m );

	private:
		float m_x{ 0.0f };
		float m_y{ 0.0f };
		float m_z{ 0.0f };

		float m_pitch{ 0.0f };
		float m_yaw{ 0.0f };
		float m_roll{ 0.0f };

		XMMATRIX m_view_matrix{};
	};
}