#pragma once

namespace wf
{
	class Sound
	{
		struct WaveHeader_PCM
		{
			char				riff[ 4 ];
			unsigned long		riff_size;
			char				wave[ 4 ];
			char				fmt[ 4 ];
			unsigned long		fmt_size;
			unsigned short		audio_format;
			unsigned short		channels;
			unsigned long		sample_per_sec;
			unsigned long		avg_bytes_per_sec;
			unsigned short		block_align;
			unsigned short		bits_per_sample;
			char				data[ 4 ];
			unsigned long		data_size;
		};

		struct WaveHeader_Ext
		{
			char				riff[ 4 ];
			unsigned long		riff_size{ };
			char				wave[ 4 ];
			char				fmt[ 4 ];
			unsigned long		fmt_size{};
			unsigned short		fmt_tag;
			unsigned short		channels;
			unsigned long		sample_per_sec;
			unsigned long		avg_bytes_per_sec;
			unsigned short		block_align;
			unsigned short		bits_per_sample;
			unsigned short		cb_size{};
			unsigned short		valid_bits_per_sample;
			unsigned			channel_mask;
			char				sub_format[ 16 ];
			char				fact[ 4 ];
			unsigned long		fact_size;
			unsigned long		sample_length;
			char				data[ 4 ];
			unsigned long		data_size;
		};

	public:
		Sound();
		Sound( const Sound& );
		~Sound();

		bool Initialize( HWND _hwnd );
		void Shutdown();

		bool Play();

	private:
		bool InitializeDirectSound( HWND _hwnd );
		void ShutdownDirectSound();

		bool LoadWaveFile( const char* _sound_path, IDirectSoundBuffer8** _sound_buffer );
		void ShutdownWaveFile( IDirectSoundBuffer8** _sound_buffer );
		bool LoadWaveFile_PCM( const char* _sound_path, IDirectSoundBuffer8** _sound_buffer );
		bool LoadWaveFile_Ext( const char* _sound_path, IDirectSoundBuffer8** _sound_buffer );

	private:
		IDirectSound8* m_direct_sound{ nullptr };
		IDirectSoundBuffer* m_primary_buffer{ nullptr };
		IDirectSoundBuffer8* m_secondary_buffer_1{ nullptr };
		IDirectSoundBuffer8* m_secondary_buffer_2{ nullptr };
	};
}