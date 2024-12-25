#include "EngineTimer.h"

UEngineTimer::UEngineTimer()
{
	// 나는 1초에 얼마를 샐수 있는 애에요.
	QueryPerformanceFrequency(&Count);

	// 연산을 빠르게 하기 위해서 8바이트 
	TimeCounter = static_cast<double>(Count.QuadPart);

	// 현재까지 이만큼 샜어요
	QueryPerformanceCounter(&PrevTime);
	QueryPerformanceCounter(&CurTime);

	int a = 0;
}

UEngineTimer::~UEngineTimer()
{
}

void UEngineTimer::TimeStart()
{
	QueryPerformanceCounter(&PrevTime);
}

float UEngineTimer::End()
{
	TimeCheck();
	return GetDeltaTime();
}

double UEngineTimer::DEnd()
{
	TimeCheck();
	return GetDoubleDeltaTime();
}

void UEngineTimer::TimeCheck()
{
	// QueryPerformanceCounter(&PrevTime); 이전에 재었던 시간이 남아있을 것이다.

	QueryPerformanceCounter(&CurTime);

	// 사이간격이 나옵니다.
	double Tick = static_cast<double>(CurTime.QuadPart - PrevTime.QuadPart);
	DeltaTime = Tick / TimeCounter * TimeScale;
	fDeltaTime = static_cast<float>(DeltaTime);
	PrevTime.QuadPart = CurTime.QuadPart;
}
