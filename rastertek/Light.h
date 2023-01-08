#pragma once

namespace wf
{
	class Light
	{
	public:
		Light();
		Light( const Light& );
		~Light();

		void SetAmbient( float _r, float _g, float _b, float _a );
		void SetDiffuse( float _r, float _g, float _b, float _a );
		void SetDirection( float _x, float _y, float _z );
		void SetSpecular( float _r, float _g, float _b, float _a );
		void SetSpecularPower( float _power );

		XMFLOAT4 GetAmbient() const;
		XMFLOAT4 GetDiffuse() const;
		XMFLOAT3 GetDirection() const;
		XMFLOAT4 GetSpecular() const;
		float GetSpecularPower() const;

		void SetPosition( float _x, float _y, float _z );
		XMFLOAT3 GetPosition() const;

		void SetLookAt( float _x, float _y, float _z );
		XMFLOAT3 GetLookAt() const;

		void GenerateViewMatrix();
		void GenerateProjectionMatrix( float _near_z, float _far_z );

		XMMATRIX GetViewMatrix() const;
		XMMATRIX GetProjectionMatrix() const;

	private:
		XMFLOAT3 m_position{ 0.0f, 0.0f, 0.0f };
		XMFLOAT4 m_ambient{ 1.0f, 1.0f, 1.0f, 1.0f };
		XMFLOAT4 m_diffuse{ 1.0f, 1.0f, 1.0f, 1.0f };
		XMFLOAT3 m_direction{ 0.0f, 0.0f, 0.0f };
		XMFLOAT4 m_specular{ 0.0f, 0.0f, 0.0f, 0.0f };
		float m_specular_power{ 1.0f };
		
		XMFLOAT3 m_look_at{ 0.0f, 0.0f, 0.0f };
		XMMATRIX m_view{};
		XMMATRIX m_projection{};
	};
}