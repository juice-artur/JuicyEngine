#pragma once

#include "CoreDefines.h"

class Platform
{
public:
	JE_API virtual bool Startup(const char* applicationName, int x, int y, int width, int height) = 0;
	JE_API virtual void Shutdown() = 0;
	JE_API virtual bool PumpMessages()= 0;

	JE_API virtual double GetAbsoluteTime() = 0;

private:
};
