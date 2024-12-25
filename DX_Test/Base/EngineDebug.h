#pragma once
#include "EngineDefine.h"
#include <Windows.h>
#include <assert.h>


#define MSGASSERT(VALUE) std::string ErrorText = VALUE; MessageBoxA(nullptr, ErrorText.c_str(), "치명적 에러", MB_OK); assert(false);

namespace UEngineDebug
{
	ENGINEAPI void LeakCheck();

	// 분리해야 하겠네요
	ENGINEAPI void OutPutString(const std::string& Text);


	ENGINEAPI void StartConsole();

	// AllocConsole();
	// 이제 콘솔창과 연결된 입력 출력 핸들을 내가 지정한 것으로 바꿔라.
	// 그런데 우리는 지금 멀티바이트로 처리했다.
	// 아래함수 써서 콘솔 입출력을 변경해줘야 하는데.
	// _tfreopen_s 함수로 변경되었는데 사용법을 전혀 인지하지 못함.
	// 인터넷 검색을 더해서 알아내야 할것 같다.
	// 아래 함수를 사용하는법을 더 숙력하고 와야할것 같다.
	//_tfreopen(_T("CONOUT$"), _T("w"), stdout);
	//_tfreopen(_T("CONIN$"), _T("r"), stdin);
	//_tfreopen(_T("CONERR$"), _T("w"), stderr);
	ENGINEAPI void EndConsole();
	// FreeConsole();

}

