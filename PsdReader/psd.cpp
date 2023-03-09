#include "pch.h"
#include "psd.h"

NAMESPACE_WF_S

// https://www.adobe.com/devnet-apps/photoshop/fileformatashtml/#50577409_13084

void check( std::istream& stream )
{
    if ( !stream )
    {
        std::cerr << "stream is consumed :(" << std::endl;
    }

    if ( stream.gcount() == 0 )
    {
        std::cerr << "no data read :(" << std::endl;
    }
}

void read( int8_t& val, std::istream& stream )
{
    stream.read( reinterpret_cast<char*>( &val ), 1 );
    check( stream );
}

void read( uint8_t& val, std::istream& stream )
{
    stream.read( reinterpret_cast<char*>( &val ), 1 );
    check( stream );
}

void read( int16_t& val, std::istream& stream )
{
    auto bytes = reinterpret_cast<char*>( &val );
    stream.read( bytes, sizeof( val ) );
    std::reverse( bytes, bytes + sizeof( val ) );
    check( stream );
}

void read( uint16_t& val, std::istream& stream )
{
    auto bytes = reinterpret_cast<char*>( &val );
    stream.read( bytes, sizeof( val ) );
    std::reverse( bytes, bytes + sizeof( val ) );
    check( stream );
}

void read( uint32_t& val, std::istream& stream )
{
    auto bytes = reinterpret_cast<char*>( &val );
    stream.read( bytes, sizeof( val ) );
    std::reverse( bytes, bytes + sizeof( val ) );
    check( stream );
}

void read( int32_t& val, std::istream& stream )
{
    auto bytes = reinterpret_cast<char*>( &val );
    stream.read( bytes, sizeof( val ) );
    std::reverse( bytes, bytes + sizeof( val ) );
    check( stream );
}

void read( std::vector<uint8_t>& val, std::istream& stream )
{
    stream.read( reinterpret_cast<char*>( val.data() ), val.size() );
    check( stream );
}

void read( unsigned char* val, std::size_t size, std::istream& stream )
{
    stream.read( reinterpret_cast<char*>( val ), size );
    check( stream );
}

template<std::size_t  size>
void read( std::array<uint8_t, size>& val, std::istream& stream )
{
    stream.read( reinterpret_cast<char*>( val.data() ), size );
    check( stream );
}

////////////////////////////////////////////////////////////////////////////////////////////////

namespace psd
{
	void Header::ReadData( std::istream& stream )
	{
        read< sizeof( signature )>( signature, stream );
        read( version, stream );
        read< sizeof( reserved )>( reserved, stream );
        read( channels, stream );
        read( height, stream );
        read( width, stream );
        read( depth, stream );
        read( colormode, stream );
	}

    void ColorModeData::ReadData( std::istream& stream )
    {
        read( length, stream );
        stream.ignore( length );
    }

    void ImageResources::ReadData( std::istream& stream )
    {
        read( length, stream );
        stream.ignore( length );

        // ImageResourceBlocks
    }

    void Channel::Initialize( int16_t id, int32_t height, int32_t width )
    {
        this->id = id;
        datas.resize( height * width, 0xff );
    }

    void Channel::ReadChannelData( std::istream& stream )
    {
        read( id, stream );
        read( length, stream );
    }

    void Channel::ReadRawData( std::istream& stream, int32_t height, int32_t width )
    {
        stride = width;
        datas.resize( height * width );
        read( datas, stream );
    }

    void Channel::ReadRleData( std::istream& stream, int32_t height, int32_t width )
    {
        std::vector<uint16_t> scanline_byte_counts;
        for ( auto y = 0; y < height; ++y )
        {
            uint16_t value{ 0 };
            read( value, stream );
            scanline_byte_counts.push_back( value );
        }

        stride = width;
        datas.resize( height * width );

        for ( auto y = 0; y < height; ++y )
        {
            const auto bytes_per_scanline = scanline_byte_counts[ y ];
            const auto offset = y * stride;

            UnpackBits( datas, offset, bytes_per_scanline, stream );
        }
    }

    void Channel::UnpackBits( std::vector<uint8_t>& dst, int32_t offset, size_t bytes_per_scanline, std::istream& stream )
    {
        size_t read_bytes = 0u;

        while ( read_bytes < bytes_per_scanline )
        {
            uint8_t control_byte = 0;
            read( control_byte, stream );
            ++read_bytes;

            if ( 0x80 == control_byte )
            {
                for ( auto j = 0; j < control_byte + 1; ++j )
                {
                    uint8_t single_value{ 0 };
                    read( single_value, stream );
                    ++read_bytes;

                    dst[ offset ] = single_value;
                    ++offset;
                }
            }
            else
            {
                uint8_t span_value{ 0 };
                read( span_value, stream );
                ++read_bytes;

                auto count = ( 256 - control_byte ) + 1;
                for ( auto j = 0; j < count; ++j )
                {
                    dst[ offset ] = span_value;
                    ++offset;
                }
            }
        }
    }

    uint8_t* Channel::GetScanline( int32_t y ) const
    {
        return const_cast<uint8_t*>( datas.data() + y * stride );
    }

    void LayerMask::ReadLayerMaskData( std::istream& stream )
    {
    }

    void LayerRecord::ReadLayerRecordData( std::istream& stream )
    {
        channels.clear();

        read( t, stream );
        read( l, stream );
        read( b, stream );
        read( r, stream );

        read( channel_count, stream );
        for ( int16_t i = 0; i < channel_count; ++i )
        {
            Channel channel;
            channel.ReadChannelData( stream );
            channels.emplace_back( channel );
        }

        read( blend_signature, stream );
        read( blend_key, stream );
        read( opacity, stream );
        read( clipping, stream );
        read( flags, stream );
        read( filter, stream );

        // Extra data
        read( extra_length, stream );
        std::streampos layer_mask_position = stream.tellg();
        
        // Layer mask / adjustment layer data
        {
            int32_t data_length{ 0 };
            read( data_length, stream );
            stream.ignore( data_length );
        }

        // Layer blending ranges data
        {
            int32_t data_length{ 0 };
            read( data_length, stream );
            stream.ignore( data_length );
        }

        // Layer name
        {
            uint8_t length{ 0 };
            read( length, stream );

            char buf[ 256 ]{ 0, };
            read( reinterpret_cast<unsigned char*>( buf ), length, stream );

            name_mbcs = buf;
        }

        // Additional Layer Information
        int32_t additional_layer_signature{ 0 };
        while ( extra_length - ( stream.tellg() - layer_mask_position ) > 4 )
        {
            if ( 0 == additional_layer_signature )
            {
                read( additional_layer_signature, stream );
            }
            else
            {
                uint8_t byte{ 0 };
                read( byte, stream );
                additional_layer_signature = ( additional_layer_signature << 8 ) | byte;
            }

            if ( '8BIM' != additional_layer_signature )
            {
                continue;
            }

			int32_t additional_layer_key{ 0 };
			read( additional_layer_key, stream );

            int32_t additional_layer_data_length{ 0 };
			read( additional_layer_data_length, stream );

			std::streampos block_position = stream.tellg();

			if ( 'lsct' == additional_layer_key )
			{
				read( section_divider, stream );
			}

			if ( 'luni' == additional_layer_key )
			{
				uint32_t length = 0;
				read( length, stream );

                wchar_t name[ 256 ]{ 0, };
				for ( auto i = 0u; i < length; i++ )
				{
					uint16_t word = 0;
					read( word, stream );
					name[ i ] = word;
				}

                name_wbcs = name;
			}

			stream.ignore( additional_layer_data_length - ( stream.tellg() - block_position ) );
			additional_layer_signature = 0;
		}

        stream.ignore( extra_length - ( stream.tellg() - layer_mask_position ) );
    }

    void LayerRecord::ReadImageData( std::istream& stream )
    {
        int32_t height = b - t;
        int32_t width = r - l;

        for ( Channel& channel : channels )
        {
            uint16_t compression{ 0 };
            read( compression, stream );

            switch ( compression )
            {
                // Raw image data
                case 0:
                {
                    channel.ReadRawData( stream, height, width );
                    break;
                }

                case 1:
                {
                    channel.ReadRleData( stream, height, width );
                    break;
                }
            }
        }

        // rgb
        if ( 3 == channel_count )
        {
            Channel channel;
            channel.Initialize( -1, height, width );
            channels.push_back( channel );
            ++channel_count;
        }

        image.init( width, height );

        for ( int32_t y = 0; y < height; ++y )
        {
            uint32_t* dst = image.getScanline( y );

            uint8_t* red = GetChannel( 0 ).GetScanline( y );
            uint8_t* green = GetChannel( 1 ).GetScanline( y );
            uint8_t* blue = GetChannel( 2 ).GetScanline( y );
            uint8_t* alpha = GetChannel( -1 ).GetScanline( y );

            for ( int32_t x = 0; x < width; ++x )
            {
                uint8_t a = alpha[ x ];
                uint8_t r = a > 0 ? red[ x ] : 0;
                uint8_t g = a > 0 ? green[ x ] : 0;
                uint8_t b = a > 0 ? blue[ x ] : 0;


                // ÀÏ´Ü ARGB
                dst[ x ] = ( a << 24 ) | ( r << 16 ) | ( g << 8 ) | b;
            }
        }
    }

    Channel& LayerRecord::GetChannel( int32_t id )
    {
        auto iter = std::find_if( channels.begin(), channels.end(), [id]( const Channel& channel ) { return channel.id == id; } );
        return *iter;
    }

    void LayerAndMask::ReadData( std::istream& stream )
    {
        read( total, stream );
        
        // layer info
        layer_records.clear();

        read( info_length, stream );
        read( layer_count, stream );
        for ( int16_t i = 0; i < layer_count; ++i )
        {
            LayerRecord layer_record;
            layer_record.ReadLayerRecordData( stream );
            layer_records.emplace_back( layer_record );
        }

        for ( int16_t i = 0; i < layer_count; ++i )
        {
            LayerRecord& layer_record = layer_records[ i ];
            layer_record.ReadImageData( stream );
        }
       

        // stream.ignore( length );
    }
}

NAMESPACE_WF_E