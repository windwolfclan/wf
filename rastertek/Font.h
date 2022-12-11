#pragma once

namespace wf
{
	class Texture;

	class Font
	{
		struct FontType
		{
			float left{ 0.0f };
			float right{ 0.0f };
			int size{ 0 };
		};

		struct VertexType
		{
			XMFLOAT3 pos{ 0.0f, 0.0f, 0.0f };
			XMFLOAT2 tex{ 0.0f, 0.0f };
		};

	public:
		Font();
		Font( const Font& );
		~Font();

		bool Initialize( ID3D11Device* _device, ID3D11DeviceContext* _context, const char* _font_path, const char* _texture_path );
		void Shutdown();

		ID3D11ShaderResourceView* GetTexture();

		void BuildVertexArray( void* _vertices, const char* _text, float _x, float _y );

	private:
		bool LoadFontData( const char* _font_path );
		void ReleaseFontData();

		bool LoadTexture( ID3D11Device* _device, ID3D11DeviceContext* _context, const char* _texture_path );
		void ReleaseTexture();


	private:
		std::array<FontType, 95> m_font{};
		Texture* m_texture{ nullptr };
	};
}