#pragma once

#include "CoreDefines.h"
class Platform;

class Engine
{
public:
	JE_API bool Init();
	JE_API void Tick();

private:
	Platform* platform;
};
