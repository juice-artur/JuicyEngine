#pragma once

#include "CoreDefines.h"

class BaseWindow;

class Platform
{
public:
	virtual bool Startup(const char* applicationName, int width, int height) = 0;
	virtual void Shutdown() = 0;
	virtual bool PumpMessages() = 0;

	virtual double GetAbsoluteTime() = 0;
	virtual const BaseWindow& const GetWindow() = 0;
};
