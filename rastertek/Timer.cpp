#include "pch.h"
#include "Timer.h"

namespace wf
{
	Timer::Timer()
	{

	}

	Timer::Timer( const Timer& )
	{

	}

	Timer::~Timer()
	{
	}

	bool Timer::Initialize()
	{
		::QueryPerformanceFrequency( (LARGE_INTEGER*)&m_frequency );
		if ( 0 == m_frequency )
		{
			return false;
		}

		m_tick_per_ms = (float)( m_frequency / 1000.0f );

		::QueryPerformanceCounter( (LARGE_INTEGER*)&m_last_time );

		return true;
	}

	void Timer::Frame()
	{
		INT64 current;
		::QueryPerformanceCounter( (LARGE_INTEGER*)&current );

		float difference = (float)( current - m_last_time );

		m_frame_time = difference / m_tick_per_ms;

		m_last_time = current;
	}

	float Timer::GetTime() const
	{
		return m_frame_time;
	}
}