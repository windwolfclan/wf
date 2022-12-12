#include "pch.h"
#include "Sound.h"
#include <mmreg.h>

namespace wf
{
	Sound::Sound()
	{
	}

	Sound::Sound( const Sound& )
	{
	}

	Sound::~Sound()
	{
	}

	bool Sound::Initialize( HWND _hwnd )
	{
		if ( !InitializeDirectSound( _hwnd ) )
		{
			return false;
		}

		if ( !LoadWaveFile_PCM( "./resources/sound1.wav", &m_secondary_buffer_1 ) )
		{
			return false;
		}

		/*if ( !LoadWaveFile_test( "./resources/sound2.wav", &m_secondary_buffer_2 ) )
		{
			return false;
		}*/

		if ( !LoadWaveFile_Ext( "./resources/sound2.wav", &m_secondary_buffer_2 ) )
		{
			return false;
		}

		return true;
	}

	void Sound::Shutdown()
	{
		ShutdownWaveFile( &m_secondary_buffer_2 );

		ShutdownWaveFile( &m_secondary_buffer_1 );

		ShutdownDirectSound();
	}

	bool Sound::Play()
	{
		HRESULT hr = m_secondary_buffer_1->SetCurrentPosition( 0 );
		if ( FAILED( hr ) )
		{
			return false;
		}

		hr = m_secondary_buffer_1->SetVolume( DSBVOLUME_MAX );
		if ( FAILED( hr ) )
		{
			return false;
		}

		hr = m_secondary_buffer_1->Play( 0, 0, 0 );
		if ( FAILED( hr ) )
		{
			return false;
		}

		return true;
	}

	bool Sound::InitializeDirectSound( HWND _hwnd )
	{
		HRESULT hr = DirectSoundCreate8( nullptr, &m_direct_sound, nullptr );
		if ( FAILED( hr ) )
		{
			return false;
		}

		hr = m_direct_sound->SetCooperativeLevel( _hwnd, DSSCL_PRIORITY );
		if ( FAILED( hr ) )
		{
			return false;
		}

		DSBUFFERDESC buffer_desc{};
		ZeroMemory( &buffer_desc, sizeof( buffer_desc ) );
		buffer_desc.dwSize = sizeof( DSBUFFERDESC );
		buffer_desc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
		buffer_desc.dwBufferBytes = 0;
		buffer_desc.dwReserved = 0;
		buffer_desc.lpwfxFormat = nullptr;
		buffer_desc.guid3DAlgorithm = GUID_NULL;

		hr = m_direct_sound->CreateSoundBuffer( &buffer_desc, &m_primary_buffer, nullptr );
		if ( FAILED( hr ) )
		{
			return false;
		}

		WAVEFORMATEX wave_format{ };
		ZeroMemory( &wave_format, sizeof( wave_format ) );
		wave_format.wFormatTag = WAVE_FORMAT_PCM;
		wave_format.nSamplesPerSec = 44100;
		wave_format.wBitsPerSample = 16;
		wave_format.nChannels = 2;
		wave_format.nBlockAlign = ( wave_format.wBitsPerSample / 8 ) * wave_format.nChannels;
		wave_format.nAvgBytesPerSec = wave_format.nSamplesPerSec * wave_format.nBlockAlign;
		wave_format.cbSize = 0;

		hr = m_primary_buffer->SetFormat( &wave_format );
		if ( FAILED( hr ) )
		{
			return false;
		}

		return true;
	}

	void Sound::ShutdownDirectSound()
	{
		SAFE_RELEASE( m_primary_buffer );
		SAFE_RELEASE( m_direct_sound );
	}

	bool Sound::LoadWaveFile_PCM( const char* _sound_path, IDirectSoundBuffer8** _sound_buffer )
	{
		FILE* fp{ nullptr };

		int error = fopen_s( &fp, _sound_path, "rb" );
		if ( 0 != error )
		{
			return false;
		}

		size_t count{ 0 };
		WaveHeader_PCM wave_header{};
		ZeroMemory( &wave_header, sizeof( wave_header ) );
		count = fread( &wave_header, sizeof( wave_header ), 1, fp );
		if ( 1 != count )
		{
			fclose( fp );
			return false;
		}

		if ( wave_header.riff[ 0 ] != 'R' ||
			wave_header.riff[ 1 ] != 'I' ||
			wave_header.riff[ 2 ] != 'F' ||
			wave_header.riff[ 3 ] != 'F'
			)
		{
			fclose( fp );
			return false;
		}

		if ( wave_header.wave[ 0 ] != 'W' ||
			wave_header.wave[ 1 ] != 'A' ||
			wave_header.wave[ 2 ] != 'V' ||
			wave_header.wave[ 3 ] != 'E'
			)
		{
			fclose( fp );
			return false;
		}

		if ( wave_header.fmt[ 0 ] != 'f' ||
			wave_header.fmt[ 1 ] != 'm' ||
			wave_header.fmt[ 2 ] != 't' ||
			wave_header.fmt[ 3 ] != ' '
			)
		{
			fclose( fp );
			return false;
		}

		if ( wave_header.data[ 0 ] != 'd' ||
			wave_header.data[ 1 ] != 'a' ||
			wave_header.data[ 2 ] != 't' ||
			wave_header.data[ 3 ] != 'a'
			)
		{
			return false;
		}

		WAVEFORMATEX wave_format{};
		ZeroMemory( &wave_format, sizeof( wave_format ) );
		wave_format.wFormatTag = wave_header.audio_format;;
		wave_format.nSamplesPerSec = wave_header.sample_per_sec;
		wave_format.wBitsPerSample = wave_header.bits_per_sample;
		wave_format.nChannels = wave_header.channels;
		wave_format.nBlockAlign = wave_header.block_align;
		wave_format.nAvgBytesPerSec = wave_header.avg_bytes_per_sec;
		wave_format.cbSize = 0;

		DSBUFFERDESC buffer_desc{};
		ZeroMemory( &buffer_desc, sizeof( buffer_desc ) );
		buffer_desc.dwSize = sizeof( DSBUFFERDESC );
		buffer_desc.dwFlags = DSBCAPS_CTRLVOLUME;
		buffer_desc.dwBufferBytes = wave_header.data_size;
		buffer_desc.dwReserved = 0;
		buffer_desc.lpwfxFormat = &wave_format;
		buffer_desc.guid3DAlgorithm = GUID_NULL;

		IDirectSoundBuffer* temp_buffer{ nullptr };
		HRESULT hr = m_direct_sound->CreateSoundBuffer( &buffer_desc, &temp_buffer, nullptr );
		if ( FAILED( hr ) )
		{
			fclose( fp );
			return false;
		}

		hr = temp_buffer->QueryInterface( IID_IDirectSoundBuffer8, (void**)&*_sound_buffer );
		if ( FAILED( hr ) )
		{
			fclose( fp );
			return false;
		}

		SAFE_RELEASE( temp_buffer );


		fseek( fp, sizeof( WaveHeader_PCM ), SEEK_SET );
		unsigned char* wave_data = new unsigned char[ wave_header.data_size ];
		if ( !wave_data )
		{
			fclose( fp );
			return false;
		}

		count = fread( wave_data, 1, wave_header.data_size, fp );
		if ( count != wave_header.data_size )
		{
			SAFE_DELETE_ARRAY( wave_data );
			fclose( fp );
			return false;
		}

		fclose( fp );

		unsigned char* buffer_ptr{};
		unsigned long buffer_size{ 0 };
		hr = ( *_sound_buffer )->Lock( 0, wave_header.data_size, (void**)&buffer_ptr, (DWORD*)&buffer_size, nullptr, 0, 0 );
		if ( FAILED( hr ) )
		{
			SAFE_DELETE_ARRAY( wave_data );
			return false;
		}

		memcpy( buffer_ptr, wave_data, wave_header.data_size );

		hr = ( *_sound_buffer )->Unlock( (void*)buffer_ptr, buffer_size, nullptr, 0 );
		if ( FAILED( hr ) )
		{
			SAFE_DELETE_ARRAY( wave_data );
			return false;
		}

		SAFE_DELETE_ARRAY( wave_data );

		return true;
	}

	bool Sound::LoadWaveFile( const char* _sound_path, IDirectSoundBuffer8** _sound_buffer )
	{
		if ( LoadWaveFile_PCM( _sound_path, _sound_buffer ) )
		{
			return true;
		}

		if ( LoadWaveFile_Ext( _sound_path, _sound_buffer ) )
		{
			return true;
		}

		return false;
	}

	void Sound::ShutdownWaveFile( IDirectSoundBuffer8** _sound_buffer )
	{
		SAFE_RELEASE( ( *_sound_buffer ) );
	}

	bool Sound::LoadWaveFile_Ext( const char* _sound_path, IDirectSoundBuffer8** _sound_buffer )
	{

		FILE* fp{ nullptr };

		int error = fopen_s( &fp, _sound_path, "rb" );
		if ( 0 != error )
		{
			return false;
		}

		size_t count{ 0 };
		WaveHeader_Ext wave_header{};
		ZeroMemory( &wave_header, sizeof( wave_header ) );
		count = fread( &wave_header, sizeof( wave_header ), 1, fp );
		if ( 1 != count )
		{
			fclose( fp );
			return false;
		}

		if ( wave_header.riff[ 0 ] != 'R' ||
			wave_header.riff[ 1 ] != 'I' ||
			wave_header.riff[ 2 ] != 'F' ||
			wave_header.riff[ 3 ] != 'F'
			)
		{
			fclose( fp );
			return false;
		}

		if ( wave_header.wave[ 0 ] != 'W' ||
			wave_header.wave[ 1 ] != 'A' ||
			wave_header.wave[ 2 ] != 'V' ||
			wave_header.wave[ 3 ] != 'E'
			)
		{
			fclose( fp );
			return false;
		}

		if ( wave_header.fmt[ 0 ] != 'f' ||
			wave_header.fmt[ 1 ] != 'm' ||
			wave_header.fmt[ 2 ] != 't' ||
			wave_header.fmt[ 3 ] != ' '
			)
		{
			fclose( fp );
			return false;
		}

		if ( wave_header.data[ 0 ] != 'd' ||
			wave_header.data[ 1 ] != 'a' ||
			wave_header.data[ 2 ] != 't' ||
			wave_header.data[ 3 ] != 'a'
			)
		{
			return false;
		}

		if ( wave_header.fact[ 0 ] != 'f' ||
			wave_header.fact[ 1 ] != 'a' ||
			wave_header.fact[ 2 ] != 'c' ||
			wave_header.fact[ 3 ] != 't'
			)
		{
			return false;
		}


		WAVEFORMATEX wave_format{};
		ZeroMemory( &wave_format, sizeof( wave_format ) );
		wave_format.wFormatTag = WAVE_FORMAT_PCM;	// WAVE_FORMAT_EXTENSIBLE
		wave_format.nSamplesPerSec = wave_header.sample_per_sec;
		wave_format.wBitsPerSample = wave_header.bits_per_sample;
		wave_format.nChannels = wave_header.channels;
		wave_format.nBlockAlign = wave_header.block_align;
		wave_format.nAvgBytesPerSec = wave_header.avg_bytes_per_sec;
		wave_format.cbSize = wave_header.cb_size;

		DSBUFFERDESC buffer_desc{};
		ZeroMemory( &buffer_desc, sizeof( buffer_desc ) );
		buffer_desc.dwSize = sizeof( DSBUFFERDESC );
		buffer_desc.dwFlags = DSBCAPS_CTRLVOLUME;
		buffer_desc.dwBufferBytes = wave_header.data_size;
		buffer_desc.dwReserved = 0;
		buffer_desc.lpwfxFormat = &wave_format;
		buffer_desc.guid3DAlgorithm = GUID_NULL;

		IDirectSoundBuffer* temp_buffer{ nullptr };
		HRESULT hr = m_direct_sound->CreateSoundBuffer( &buffer_desc, &temp_buffer, nullptr );
		if ( FAILED( hr ) )
		{
			DWORD dwErr = hr;
			if ( dwErr == DSERR_INVALIDPARAM )
			{
				int a;
				a = 1;
			}

			fclose( fp );
			return false;
		}

		hr = temp_buffer->QueryInterface( IID_IDirectSoundBuffer8, (void**)&*_sound_buffer );
		if ( FAILED( hr ) )
		{
			fclose( fp );
			return false;
		}

		SAFE_RELEASE( temp_buffer );


		fseek( fp, sizeof( WaveHeader_Ext ), SEEK_SET );
		unsigned char* wave_data = new unsigned char[ wave_header.data_size ];
		if ( !wave_data )
		{
			fclose( fp );
			return false;
		}

		count = fread( wave_data, 1, wave_header.data_size, fp );
		if ( count != wave_header.data_size )
		{
			SAFE_DELETE_ARRAY( wave_data );
			fclose( fp );
			return false;
		}

		fclose( fp );

		unsigned char* buffer_ptr{};
		unsigned long buffer_size{ 0 };
		hr = ( *_sound_buffer )->Lock( 0, wave_header.data_size, (void**)&buffer_ptr, (DWORD*)&buffer_size, nullptr, 0, 0 );
		if ( FAILED( hr ) )
		{
			SAFE_DELETE_ARRAY( wave_data );
			return false;
		}

		memcpy( buffer_ptr, wave_data, wave_header.data_size );

		hr = ( *_sound_buffer )->Unlock( (void*)buffer_ptr, buffer_size, nullptr, 0 );
		if ( FAILED( hr ) )
		{
			SAFE_DELETE_ARRAY( wave_data );
			return false;
		}

		SAFE_DELETE_ARRAY( wave_data );

		return true;
	}

}