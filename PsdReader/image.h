#pragma once

NAMESPACE_WF_S

namespace psd
{
	class Image
	{
	public:
		Image() = default;
		Image( const std::string& filename );
		Image( const Image& image );

		const Image& operator = ( const Image& image );

		void init( int32_t x, int32_t y );
		void clear( uint32_t argb );
		void save( const std::string& filename ) const;

		int32_t getWidth() const;
		int32_t getHeight() const;
		uint32_t* getScanline( int32_t y ) const;
		const std::vector<uint32_t>& getData() const;
		Image downsample() const;
		void scaled( const Image& image ) const;
		void load( const std::string& filename );
		void copy( int32_t x, int32_t y, const Image& source, int32_t clamp = 0 );
		void premultiplyAlpha();
		void minimum( const Image& image );
		uint32_t getPixel( float u, float v ) const;
		void buildNormalMap( int32_t z );
		void buildDeltaMap();
		const std::string& path() const;
		const std::string& filename() const;

	private:
		std::vector<uint32_t> m_data{};
		int32_t m_width{ 0 };
		int32_t m_height{ 0 };
		std::string m_path{};
		std::string m_filename{};
	};
}

NAMESPACE_WF_E