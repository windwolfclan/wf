#pragma once

namespace wf
{
	class TextureArray
	{
	public:
		TextureArray();
		TextureArray( const TextureArray& );
		~TextureArray();

		bool Intialize( ID3D11Device* _device, ID3D11DeviceContext* _context, const wchar_t* _texture_path1, const wchar_t* _texture_path2 );
		void Shutdown();

		ID3D11ShaderResourceView** GetTextureArray();

	private:
		bool LoadTexture( ID3D11Device* _device, ID3D11DeviceContext* _context, const wchar_t* _texture_path, ID3D11Texture2D*& _texture, ID3D11ShaderResourceView*& _srv );

	private:
		ID3D11Texture2D* m_textures[ 2 ]{ nullptr, nullptr };
		ID3D11ShaderResourceView* m_views[ 2 ]{ nullptr, nullptr };
	};
}