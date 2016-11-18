#pragma once
#include "stdinclude.h"

class SRTimer
{
public:
	SRTimer();
	~SRTimer();

	void Initialize();
	void Start();
	double Stop();

public:

	LARGE_INTEGER mPerfFreq;
	LARGE_INTEGER mStartTime;
	LARGE_INTEGER mEndTime;
};