#pragma once

namespace wf
{
	class Input
	{
	public:
		Input();
		Input(const Input&);
		~Input();

		void Initialize();

		void KeyDown(unsigned int key);
		void KeyUp(unsigned int key);

		bool IsKeyDown(unsigned int key);

	private:
		bool m_keys[256];
	};
}