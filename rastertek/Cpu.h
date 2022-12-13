#pragma once

#pragma comment( lib, "pdh.lib" )

#include <Pdh.h>

namespace wf
{
	class Cpu
	{
	public:
		Cpu();
		Cpu( const Cpu& );
		~Cpu();

		void Initialize();
		void Shutdown();
		void Frame();
		int GetCpuPercentage() const;

	private:
		bool m_can_read_cpu{ true };
		HQUERY m_query_handle{ nullptr };
		HCOUNTER m_counter_handle{ nullptr };
		ULONGLONG m_last_time{ 0 };
		long m_usage{ 0 };
	};
}