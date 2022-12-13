#include "pch.h"
#include "Fps.h"

namespace wf
{
	Fps::Fps()
	{
		::timeBeginPeriod( 1000 );
	}

	Fps::Fps( const Fps& )
	{
	}

	Fps::~Fps()
	{
		::timeEndPeriod( 0 );
	}

	void Fps::Initialize()
	{
		m_last_time = ::timeGetTime();
	}

	void Fps::Frame()
	{
		++m_count;

		const unsigned long current = ::timeGetTime();
		if ( m_last_time + 1000 <= current )
		{
			m_fps = m_count;
			m_count = 0;
			m_last_time = current;
		}
	}

	int Fps::GetFPS() const
	{
		return m_fps;
	}
}