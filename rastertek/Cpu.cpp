#include "pch.h"
#include "Cpu.h"

namespace wf
{
	Cpu::Cpu()
	{
	}

	Cpu::Cpu( const Cpu& )
	{
	}

	Cpu::~Cpu()
	{
	}

	void Cpu::Initialize()
	{
		PDH_STATUS status = ::PdhOpenQuery( nullptr, 0, &m_query_handle );
		if ( ERROR_SUCCESS != status )
		{
			m_can_read_cpu = false;
		}

		status = ::PdhAddCounter( m_query_handle, TEXT( "\\Processor(_Total)\\% processor time" ), 0, &m_counter_handle );
		if ( ERROR_SUCCESS != status )
		{
			m_can_read_cpu = false;
		}

		m_last_time = ::GetTickCount64();
	}

	void Cpu::Shutdown()
	{
		if ( m_can_read_cpu )
		{
			::PdhCloseQuery( m_query_handle );
		}
	}

	void Cpu::Frame()
	{
		if ( !m_can_read_cpu )
		{
			return;
		}


		PDH_FMT_COUNTERVALUE value;
		ULONGLONG current = ::GetTickCount64();
		if ( m_last_time + 1000 < current )
		{
			m_last_time = current;

			::PdhCollectQueryData( m_query_handle );

			::PdhGetFormattedCounterValue( m_counter_handle, PDH_FMT_LONG, nullptr, &value );

			m_usage = value.longValue;
		}
	}

	int Cpu::GetCpuPercentage() const
	{
		return m_can_read_cpu ? m_usage : 0;
	}

}