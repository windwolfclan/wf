#pragma once

NAMESPACE_WF_S

namespace psd
{
	struct Header
	{
		std::array< uint8_t, 4> signature{ 0, 0, 0, 0 };
		uint16_t version{ 0 };
		std::array< uint8_t, 6> reserved{ 0, 0, 0, 0, 0, 0 };
		uint16_t channels{ 0 };
		int32_t height{ 0 };
		int32_t width{ 0 };
		uint16_t depth{ 0 };
		uint16_t colormode{ 0 };

		void ReadData( std::istream& stream );
	};

	struct ColorModeData
	{
		int32_t length{ 0 };
		void ReadData( std::istream& stream );
	};

	struct ImageResources
	{
		int32_t length{ 0 };
		void ReadData( std::istream& stream );
	};

	struct Channel
	{
		int16_t id{ 0 };
		int32_t length{};	// data length

		void ReadChannelData( std::istream& stream );
	};

	struct LayerMask
	{

	};

	struct LayerRecord
	{
		int32_t l{ 0 };
		int32_t t{ 0 };
		int32_t r{ 0 };
		int32_t b{ 0 };
		int16_t channel_count{ 0 };
		std::vector<Channel> channels{};
		std::array< uint8_t, 4 > blend_signature;
		std::array< uint8_t, 4 > blend_key;
		uint8_t opacity{ 0 };
		uint8_t clipping{ 0 };
		uint8_t flags{ 0 };
		uint8_t filter{ 0 };
		uint32_t extra_length{ 0 };

		void ReadLayerRecordData( std::istream& stream );
	};

	struct LayerAndMask
	{
		int32_t total{ 0 };
		int32_t info_length{ 0 };
		int16_t layer_count{ 0 };

		std::vector<LayerRecord> layer_records{};

		void ReadData( std::istream& stream );
	};
}

NAMESPACE_WF_E