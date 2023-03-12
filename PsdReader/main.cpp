#include "pch.h"
#include "psd.h"


int main( int argc, char* argv[] )
{
	std::string psd_file( "sample.psd" );

	std::ifstream stream;
	stream.open( psd_file, std::ios_base::binary );

	wf::psd::Header header;
	header.ReadData( stream );

	wf::psd::ColorModeData colormode;
	colormode.ReadData( stream );

	wf::psd::ImageResources imageresources;
	imageresources.ReadData( stream );

	wf::psd::LayerAndMask layerandmask;
	layerandmask.ReadData( stream );


	int i = 0;
	for ( wf::psd::LayerRecord& layer_record : layerandmask.layer_records )
	{
		if ( layer_record.image.getWidth() <= 0 || layer_record.image.getHeight() <= 0 )
		{
			continue;
		}

		std::string filename = std::to_string(i++);
		filename += ".tga";
		layer_record.image.save( filename );
	}

	stream.close();

	return 0;
}