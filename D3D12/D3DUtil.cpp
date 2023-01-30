#include "pch.h"
#include "D3DUtil.h"
#include <comdef.h>

NAMESPACE_WF_S

DxException::DxException(HRESULT _hr, const std::wstring & _func, const std::wstring & _file, int _line)
	: hr( _hr ), func( _func ), file( _file ), line( _line )
{

}

std::wstring DxException::ToString() const
{
	_com_error err( hr );
	std::wstring msg = err.ErrorMessage();

	return func + L" failed in " + file + L"\nline : " + std::to_wstring( line ) + L"\n Error : " + msg;
}

NAMESPACE_WF_E