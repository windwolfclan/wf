#pragma once

namespace wf
{
	class Font;
	class FontShader;

	class Text
	{
		struct SentenceType
		{
			ID3D11Buffer* vertex_buffer{ nullptr };
			ID3D11Buffer* index_buffer{ nullptr };
			int vertex_count{ 0 };
			int index_count{ 0 };
			int max_length{ 0 };
			float r{ 0.0f };
			float g{ 0.0f };
			float b{ 0.0f };
		};

		struct VertexType
		{
			XMFLOAT3 pos{ 0.0f, 0.0f, 0.0f };
			XMFLOAT2 tex{ 0.0f, 0.0f };
		};

	public:
		Text();
		Text( const Text& );
		~Text();

		bool Initialize( ID3D11Device* _device, ID3D11DeviceContext* _context, HWND _hwnd, int _screen_width, int _screen_height, XMMATRIX _view );
		void Shutdown();
		bool Render( ID3D11DeviceContext* _context, XMMATRIX _world, XMMATRIX _ortho );


	private:
		bool InitializeSentence( SentenceType** _sentence, int _max_length, ID3D11Device* _device );
		void ReleaseSentence( SentenceType** _sentence );
		bool UpdateSentence( SentenceType* _sentence, const char* _text, int _x, int _y, float _r, float _g, float _b, ID3D11DeviceContext* _context );
		bool RenderSentence( ID3D11DeviceContext* _context, SentenceType* _sentence, XMMATRIX _world, XMMATRIX _ortho );

	private:
		Font* m_font{ nullptr };
		FontShader* m_font_shader{ nullptr };
		int m_screen_width{ 0 };
		int m_screen_height{ 0 };
		XMMATRIX m_view{};

		SentenceType* m_sentence1{ nullptr };
		SentenceType* m_sentence2{ nullptr };

	};
}