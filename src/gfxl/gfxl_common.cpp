#include <gfxl_common.h>
#include <cstdarg>

namespace gfxl
{
	static int(*messageCallback)(const char*, va_list);

	void SetMessageCallback(int(*callback)(const char*, va_list))
	{
		messageCallback = callback;
	}

	void Message(const char* msg, ...)
	{
		if (messageCallback)
		{
			va_list args;
			va_start(args, msg);
			messageCallback(msg, args);
			va_end(args);
		}
	}
}