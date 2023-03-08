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
		int32_t length{ 0 };
		int32_t t1{ 0 };
		int32_t l1{ 0 };
		int32_t b1{ 0 };
		int32_t r1{ 0 };
		int8_t color{ 0 };
		int8_t flags{ 0 };
		int8_t mask_parameters{};

		// mask parameters bit flags present
		int8_t user_mask_density{ 0 };
		double user_mask_feather{ 0.0 };
		int8_t vector_mask_density{ 0 };
		double vector_mask_feather{ 0.0 };

		int8_t _flags{ 0 };
		int8_t user_mask_background{ 0 };

		int32_t t2{ 0 };
		int32_t l2{ 0 };
		int32_t b2{ 0 };
		int32_t r2{ 0 };

		void ReadLayerMaskData( std::istream& stream );
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

		std::string name_mbcs{};
		std::wstring name_wbcs{};
		int32_t section_divider{ 0 };

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