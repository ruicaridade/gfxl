#include <gfxl_common.h>

namespace gfxl
{
	static void(*errorFunc)(const char*);

	void ErrorSetCallback(void(*callback)(const char*))
	{
		errorFunc = callback;
	}

	void Error(const char* info)
	{
		if (errorFunc != nullptr)
			errorFunc(info);
	}
}