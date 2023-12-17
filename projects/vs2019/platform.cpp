
#include "Platform.h"
#include <windows.h>

static LARGE_INTEGER g_PerformanceFrequency;
static LARGE_INTEGER g_MSPerformanceFrequency;
static LARGE_INTEGER g_ClockStart;
static bool s_bTimeInitted = false;
static void InitTime()
{
	if ( !s_bTimeInitted )
	{
		s_bTimeInitted = true;
		QueryPerformanceFrequency(&g_PerformanceFrequency);
		g_MSPerformanceFrequency.QuadPart = g_PerformanceFrequency.QuadPart / 1000;
		QueryPerformanceCounter(&g_ClockStart);
	}
}

float Plat_PlatformTime()
{
	if ( !s_bTimeInitted )
		InitTime();
	LARGE_INTEGER CurrentTime;
	QueryPerformanceCounter( &CurrentTime );
	double fRawSeconds = (double)( CurrentTime.QuadPart - g_ClockStart.QuadPart ) / (double)(g_PerformanceFrequency.QuadPart);
	return fRawSeconds;
}
