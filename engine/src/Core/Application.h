#pragma once

#include "CoreDefines.h"
struct Game;
class Platform;

typedef struct ApplicationConfig
{
	int startWidth;

	int startHeight;

	char* name;
};

class Application
{
public:
	JE_API bool Init(struct Game* gameInst);
	JE_API void Tick();

private:
	Platform* platform;
	Game* gameInst;
	bool is_running;
	bool is_suspended;
	unsigned int width;
	unsigned int height;
	bool initialized = false;
};
