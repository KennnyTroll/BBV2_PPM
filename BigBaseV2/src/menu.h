#pragma once
#include "common.hpp"

class Timeeer
{
private:
	signed int TargetTime = 0;
	
public:

//	Timeeer()
//	{
//		TargetTime = 0;
//	}

	clock_t curClock;

	void Delay(int MilliSeconds)
	{
		TargetTime = curClock + MilliSeconds;
	}

	bool IsTimerGood()
	{
		if (TargetTime <= curClock)
			return true;
		else
			return false;
	}
};



class Menu
{
//private:
//	static signed int TargetTime;
public:
	
	//static clock_t	curClock;
	//static void TimerDelay(int MilliSeconds)
	//{
	//	TargetTime = curClock + MilliSeconds;
	//}
	//static bool IsTimerGood()
	//{
	//	if (TargetTime <= curClock)
	//		return true;
	//	else
	//		return false;
	//}
	static char	le_DLL_Path[MAX_PATH];//MAX_PATH = 260
	static void randomm_str(char* str, uint32_t len);
	static bool _ShowDemoWindow;	
};

