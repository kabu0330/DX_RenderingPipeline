#include "EngineString.h"
#include "EngineDebug.h"

UEngineString::UEngineString()
{
}

UEngineString::~UEngineString()
{
}


std::string UEngineString::ToUpper(std::string_view _string)
{
	std::string Result = _string.data();
	Result.resize(_string.size());

	for (size_t i = 0; i < _string.size(); i++)
	{
		Result[i] = std::toupper(_string[i]);
	}
	
	return Result;
}

std::wstring UEngineString::AnsiToUnicode(std::string_view _Name)
{
	// 윈도우가 지원해줄까?
	// 기본적으로 대부분의 운영체제의 문자인코딩 방식은
	// 국가코드에 의해서 결정된다.
	// 대부분 운영체제가 2바이트를 문자표현의 한계로 봅니다.
	// 국가코드를 관리하는것은 => OS

	// 멀티바이트를 와이드 바이트로 바꿔주는 클래스입니다.
	// CP_ACP 현재 국가코드를 기반으로 변경해주세요.

	// 멀티바이트 200바이트 => 1~2바이트로 표현하는 녀석 => 와이드 바이트로 바꾼다고
	//  400바이트
	// 바꾸려면 몇바이트가 필요한지도 알아내주는 함수도 필요하겠네요?

	//이 함수가 2가지 역할을 다합니다.
	// std::wstring => std::vector<wchar_t> ;

	// 마지막 2가지 인자인
	// 받기위한 와이드바이트 메모리와
	// 그 메모리의 크기를 넣어주면
	// 변환시 크기가 얼마가 되는지 계산해주는 함수로 변경됩니다.
	int Size = MultiByteToWideChar(CP_ACP, 0, _Name.data(), static_cast<int>(_Name.size()), nullptr, 0);

	if (0 == Size)
	{
		MSGASSERT("MultiByteToWideChar 문자열 변환에 실패했습니다" + std::string(_Name));
		return L"";
	}

	std::wstring Result;
	Result.resize(Size);

	Size = MultiByteToWideChar(CP_ACP, 0, _Name.data(), static_cast<int>(_Name.size()), &Result[0], Size);

	if (0 == Size)
	{
		MSGASSERT("MultiByteToWideChar 문자열 변환에 실패했습니다" + std::string(_Name));
		return L"";
	}

	return Result;
}

std::string UEngineString::InterString(const std::string& _Text, std::string_view _Start, std::string_view _End, size_t& _Offset)
{
	size_t DataStart = _Text.find(_Start, _Offset);
	size_t DataEnd = _Text.find(_End, DataStart);

	if (DataStart == std::string::npos || DataEnd == std::string::npos)
	{
		return "";
	}


	std::string Result = _Text.substr(DataStart + _Start.size(), (DataEnd - (DataStart + _Start.size()))).data();

	_Offset = DataEnd + 1;
	return Result;
}