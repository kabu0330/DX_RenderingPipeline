#include "EngineSerializer.h"

UEngineSerializer::UEngineSerializer()
{
}

UEngineSerializer::~UEngineSerializer()
{
}

void UEngineSerializer::Write(void* _Data, unsigned int _Size)
{
	// vector밖에 없다.
	// 

	// 최초의 시리얼라이저는 앞으로 얼마를 저장하게 될지 알수가 없다.

	if (WriteOffset + _Size >= Data.size())
	{
		Data.resize(Data.size() * 2 + _Size);
	}

	// 굉장히 많이 쓰이는 함수가 나오는데 
	// 자료형이 의미가 있어?
	// int 4바이트
	// char Arr[4]
	// 주소값과 크기만 알면 어떤 메모리영역이든 복사할수 있잖아.

	// [C][V][V][V][V][]
	memcpy_s(&Data[WriteOffset], _Size, _Data, _Size);
	WriteOffset += _Size;
}

void UEngineSerializer::operator<<(ISerializObject& _Data)
{
	_Data.Serialize(*this);
}

void UEngineSerializer::Read(void* _Data, unsigned int _Size)
{
	memcpy_s(_Data, _Size, &Data[ReadOffset], _Size);
	ReadOffset += _Size;
}

void UEngineSerializer::operator>>(ISerializObject& _Data)
{
	_Data.DeSerialize(*this);
}
