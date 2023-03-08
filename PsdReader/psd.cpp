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

    void Channel::ReadChannelData( std::istream& stream )
    {
        read( id, stream );
        read( length, stream );
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

        // stream.ignore( length );
    }
}

NAMESPACE_WF_E