#pragma once

namespace BJEngine {

	class Log
	{
	public:
		Log();
		~Log();

		static Log* Get() { return m_instance; }

		void Print(const char* message, ...);
		void Debug(const char* message, ...);
		void Err(const char* message, ...);

		enum
		{
			WNDCLASSERRORS = -1101,
			DIERROR = -1102,

		};

	private:
		static Log* m_instance;

		void m_init();
		void m_close();
		void m_print(const char* levtext, const char* text);

		static FILE* m_file;





	};



}