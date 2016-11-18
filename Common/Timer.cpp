#include "Timer.h"

SRTimer::SRTimer()
{
	mStartTime.QuadPart = 0;
	mEndTime.QuadPart = 0;
}
SRTimer::~SRTimer()
{

}

void SRTimer::Initialize()
{
	QueryPerformanceFrequency(&mPerfFreq);
}
void SRTimer::Start()
{
	QueryPerformanceCounter(&mStartTime);
}
double SRTimer::Stop()
{
	if(mStartTime.QuadPart)
		QueryPerformanceCounter(&mEndTime);

	return (mEndTime.QuadPart - mStartTime.QuadPart) / double(mPerfFreq.QuadPart);
}