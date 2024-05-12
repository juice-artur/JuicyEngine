#pragma once

#include "CoreDefines.h"

class Platform
{
public:
	virtual bool Startup(const char* applicationName, int x, int y, int width, int height) = 0;
	virtual void Shutdown() = 0;
	virtual bool PumpMessages() = 0;

	virtual double GetAbsoluteTime() = 0;

private:
};
