#pragma once


namespace wf
{
	class Fps
	{
	public:
		Fps();
		Fps( const Fps& );
		~Fps();

		void Initialize();
		void Frame();
		int GetFPS() const;

	private:
		int m_fps{ 0 };
		int m_count{ 0 };
		unsigned long m_last_time{ 0 };
	};
}