#pragma once

#include <d3d11.h>
#include <stdio.h>

namespace wf
{
	class Texture
	{
	private:
		struct TargaHeader
		{
			unsigned char data1[ 12 ]{ 0, };
			unsigned short width{ 0 };
			unsigned short height{ 0 };
			unsigned char bpp{ 0 };
			unsigned char data2{ 0 };
		};

	public:
		Texture();
		Texture( const Texture& );
		~Texture();

		bool Initialize( ID3D11Device* _device, ID3D11DeviceContext* _context, const char* _path );
		void Shutdown();
		ID3D11ShaderResourceView* GetTexture();

	private:
		bool LoadTarga( const  char* _path, int& _width, int& _height );

	private:
		unsigned char* m_targa_data{ nullptr };
		ID3D11Texture2D* m_texture{ nullptr };
		ID3D11ShaderResourceView* m_texture_view{ nullptr };
	};
}