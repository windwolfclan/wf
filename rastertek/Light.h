#pragma once

namespace wf
{
	class Light
	{
	public:
		Light();
		Light( const Light& );
		~Light();

		void SetDiffuse( float _r, float _g, float _b, float _a );
		void SetDirection( float _x, float _y, float _z );

		XMFLOAT4 GetDiffuse() const;
		XMFLOAT3 GetDirection() const;

	private:
		XMFLOAT4 m_diffuse{ 1.0f, 1.0f, 1.0f, 1.0f };
		XMFLOAT3 m_direction{ 0.0f, 0.0f, 0.0f };
	};
}