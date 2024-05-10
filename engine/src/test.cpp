#include "test.h"
#include <stdio.h>
#include "core/log.h"

void print_int(int i)
{
	Log::Init();
	LOG_TRACE("The number is: {0}", i);
}