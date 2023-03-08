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

	stream.close();

	return 0;
}