#pragma once

NAMESPACE_WF_S

class GameTimer
{
public:
	GameTimer();

	bool Initialize();

	float GetTotalTime() const;
	float GetDeltaTime() const;

	void Reset();
	void Start();
	void Stop();
	void Tick();

private:
	__int64 m_frequency{ 0 };
	double m_second_per_count{ 0.0 };
	double m_delta_time{ 0.0 };

	__int64 m_base_time{ 0 };
	__int64 m_paused_time{ 0 };
	__int64 m_stop_time{ 0 };
	__int64 m_previous_time{ 0 };
	__int64 m_current_time{ 0 };

	bool m_stopped{ false };
};

NAMESPACE_WF_E