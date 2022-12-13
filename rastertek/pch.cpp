#include "pch.h"

namespace wf
{
	bool LoadTarga( const wchar_t* _path, int& _width, int& _height, unsigned char*& _buffer )
	{
		FILE* file{ nullptr };
		int error = _wfopen_s( &file, _path, L"rb" );
		if ( 0 != error )
		{
			return false;
		}

		TargaHeader header{};
		unsigned int count = (unsigned int)fread( &header, sizeof( TargaHeader ), 1, file );
		if ( 1 != count )
		{
			return false;
		}

		_width = (int)header.width;
		_height = (int)header.height;
		int bpp = (int)header.bpp;
		if ( 32 != bpp )
		{
			return false;
		}

		int image_size = _width * _height * 4;
		unsigned char* targa = new unsigned char[ image_size ];
		count = (unsigned int)fread( targa, 1, image_size, file );
		if ( image_size != count )
		{
			return false;
		}

		error = fclose( file );
		if ( 0 != error )
		{
			return false;
		}

		SAFE_DELETE_ARRAY( _buffer );
		_buffer = new unsigned char[ image_size ];
		int index = 0;
		int k = ( _width * _height * 4 ) - ( _width * 4 );

		for ( int j = 0; j < _height; ++j )
		{
			for ( int i = 0; i < _width; ++i )
			{
				_buffer[ index + 0 ] = targa[ k + 2 ];	// r
				_buffer[ index + 1 ] = targa[ k + 1 ];	// g
				_buffer[ index + 2 ] = targa[ k + 0 ];	// b
				_buffer[ index + 3 ] = targa[ k + 3 ];	// a

				k += 4;
				index += 4;
			}

			k -= ( _width * 8 );
		}

		SAFE_DELETE_ARRAY( targa );
		
		return true;
	}
}