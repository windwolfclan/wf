#include "pch.h"
#include "GameTimer.h"

#undef max

NAMESPACE_WF_S

GameTimer::GameTimer()
{
}

bool GameTimer::Initialize()
{
	if ( !QueryPerformanceFrequency( (LARGE_INTEGER*)&m_frequency ) )
	{
		return false;
	}

	m_second_per_count = 1.0 / (double)m_frequency;

	return true;
}

float GameTimer::GetTotalTime() const
{
	__int64 time = m_stopped ? m_stop_time : m_current_time;

	return (float)( ( time - m_base_time ) * m_second_per_count );
}

float GameTimer::GetDeltaTime() const
{
	return (float)m_delta_time;
}

void GameTimer::Reset()
{
	__int64 current_time;
	QueryPerformanceCounter( (LARGE_INTEGER*)&current_time );

	m_base_time = current_time;
	m_previous_time = current_time;
	m_stop_time = 0;
	m_stopped = false;
}

void GameTimer::Start()
{
	__int64 current_time;
	QueryPerformanceCounter( (LARGE_INTEGER*)&current_time );

	if ( m_stopped )
	{
		m_paused_time += ( current_time - m_stop_time );

		m_previous_time = current_time;
		m_stop_time = 0;
		m_stopped = false;
	}
}

void GameTimer::Stop()
{
	if ( m_stopped )
	{
		return;
	}

	__int64 current_time;
	QueryPerformanceCounter( (LARGE_INTEGER*)&current_time );

	m_stop_time = current_time;
	m_stopped = true;
}

void GameTimer::Tick()
{
	if ( m_stopped )
	{
		m_delta_time = 0.0;
		return;
	}

	QueryPerformanceCounter( (LARGE_INTEGER*)&m_current_time );
	m_delta_time = std::max( ( m_current_time - m_previous_time ) * m_second_per_count, 0.0 );
	m_previous_time = m_current_time;
}

NAMESPACE_WF_E