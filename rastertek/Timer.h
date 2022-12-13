#pragma once

namespace wf
{
	class Timer
	{
	public:
		Timer();
		Timer( const Timer& );
		~Timer();

		bool Initialize();
		void Frame();

		float GetTime() const;

	private:
		INT64 m_frequency{ 0 };
		INT64 m_last_time{ 0 };
		float m_tick_per_ms{ 0.0f };
		float m_frame_time{ 0.0f };
	};
}