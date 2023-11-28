#include "pch.h"
#include "Log.h"

namespace BJEngine {


	Log* Log::m_instance = nullptr;
	std::ofstream Log::file;

	Log::Log()
	{
		if (!m_instance)
		{
			m_instance = this;
			m_init();
		}
		else
			Err("Log was created");
	}

	Log::~Log()
	{
		m_close();
		m_instance = nullptr;
	}

	void Log::m_init()
	{
		file.open(LOGNAME);

		if (file.is_open())
		{
			char timer[9];
			_strtime_s(timer, 9);
			char date[9];
			_strdate_s(date, 9);
			file << "Log is create: " << date << " " << timer << ".\n" << "---------------------------------------\n\n";
		}
		else
		{
			printf("Creating log error\n");
			
		}
	}

	void Log::m_close()
	{
		if (!file.is_open())
			return;

		char timer[9];
		_strtime_s(timer, 9);
		char date[9];
		_strdate_s(date, 9);

		file << "\n---------------------------------------\n" << "Logs end: " << date << " " << timer << ".";
		file.close();
	}

	void Log::Print(const char* message, ...)
	{
		va_list args;
		va_start(args, message);
		int len = _vscprintf(message, args) + 1;
		char* buffer = static_cast<char*>(malloc(len * sizeof(char)));
		vsprintf_s(buffer, len, message, args);
		m_print("", buffer);
		va_end(args);
		free(buffer);
	}

	void Log::Debug(const char* message, ...)
	{
#ifdef _DEBUG
		va_list args;
		va_start(args, message);
		int len = _vscprintf(message, args) + 1;
		char* buffer = static_cast<char*>(malloc(len * sizeof(char)));
		vsprintf_s(buffer, len, message, args);
		m_print("*DEBUG: ", buffer);
		va_end(args);
		free(buffer);
#endif
	}
	void Log::Err(const char* message, ...)
	{
		va_list args;
		va_start(args, message);
		int len = _vscprintf(message, args) + 1;
		char* buffer = static_cast<char*>(malloc(len * sizeof(char)));
		vsprintf_s(buffer, len, message, args);
		m_print("*ERROR: ", buffer);
		va_end(args);
		free(buffer);
	}

	void Log::m_print(const char* levtext, const char* text)
	{
		char timer[9];
		_strtime_s(timer, 9);
		clock_t cl = clock();

		printf("%s::%d: %s%s\n", timer, cl, levtext, text);
		if (!file.is_open()) {
			m_init();
		}

		if (file.is_open())
		{
			file << "" << timer << "::" << cl << ": " << levtext << " " << text << "\n";
			file.flush();
		}
	}


}