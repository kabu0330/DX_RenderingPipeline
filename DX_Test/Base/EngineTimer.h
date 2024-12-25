#pragma once
#include <Windows.h>

// 설명 :
class UEngineTimer
{
public:
	// constrcuter destructer
	UEngineTimer();
	~UEngineTimer();

	// delete Function
	UEngineTimer(const UEngineTimer& _Other) = delete;
	UEngineTimer(UEngineTimer&& _Other) noexcept = delete;
	UEngineTimer& operator=(const UEngineTimer& _Other) = delete;
	UEngineTimer& operator=(UEngineTimer&& _Other) noexcept = delete;

	void TimeCheck();

	float GetDeltaTime()
	{
		// 0.0000001
		// 0.000000000000001
		return fDeltaTime;
	}

	double GetDoubleDeltaTime()
	{
		// 0.0000001
		// 0.000000000000001
		return DeltaTime;
	}

	void TimeStart();

	float End();

	double DEnd();

	void SetTimeScale(float _Scale)
	{
		TimeScale = _Scale;
	}

protected:

private:
	// 시간을 재는 방식은 일반적으로 카운팅이다.
	// 그런데 초당 100만을 재는 녀석이 있다고 치자.
	// int정도는 over해버린다.
	// 그래서 당연히 8바이트 정수가 필요하다.
	// 예전에는 

	//typedef union _LARGE_INTEGER {
	//	struct {
	//		DWORD LowPart;
	//		LONG HighPart;
	//	} DUMMYSTRUCTNAME;
	//	struct {
	//		DWORD LowPart;
	//		LONG HighPart;
	//	} u;
	//	LONGLONG QuadPart;
	//} LARGE_INTEGER;


	// LONGLONG QuadPart => __int64
	LARGE_INTEGER Count = LARGE_INTEGER(); // 나 1초에 xxx만큼 샐수 있어요.
	// QuadPart => 1 => 나는 1초에 1을 샐수 있어요.
	// QuadPart => 100 => 나는 1초에 100을 샐수 있어요.

	LARGE_INTEGER PrevTime = LARGE_INTEGER();
	// LONGLONG QuadPart => 10000 / 1 기준으로 부터 10000초 지났어요

	LARGE_INTEGER CurTime = LARGE_INTEGER();
	// LONGLONG QuadPart => 10002 

	// 메모리 써서
	double TimeCounter = 0.0f;

	double DeltaTime = 0.0;

	float fDeltaTime = 0.0f;

	float TimeScale = 1.0f;
};

