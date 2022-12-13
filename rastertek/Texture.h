#pragma once

namespace wf
{
	class Texture
	{
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