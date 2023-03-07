#include "pch.h"
#include "psd.h"

#include <iostream>
#include <sstream>

NAMESPACE_WF_S

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
        read( extra_length, stream );
        stream.ignore( extra_length );
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