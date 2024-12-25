#pragma once
#include <vector>
#include <string>
#include "EngineMath.h"

// 설명 :
class UEngineSerializer
{
public:
	// constrcuter destructer
	UEngineSerializer();
	~UEngineSerializer();

	// delete Function
	UEngineSerializer(const UEngineSerializer& _Other) = delete;
	UEngineSerializer(UEngineSerializer&& _Other) noexcept = delete;
	UEngineSerializer& operator=(const UEngineSerializer& _Other) = delete;
	UEngineSerializer& operator=(UEngineSerializer&& _Other) noexcept = delete;

	// 데이터의 크기
	void Write(void* _Data, unsigned int _Size);

	void operator<<(int& _Data)
	{
		Write(&_Data, sizeof(int));
	}

	void operator<<(bool& _Data)
	{
		Write(&_Data, sizeof(bool));
	}

	void operator<<(FVector& _Data)
	{
		Write(&_Data, sizeof(FVector));
	}

	void operator<<(FIntPoint& _Data)
	{
		Write(&_Data, sizeof(FIntPoint));
	}

	void operator<<(std::string& _Data)
	{
		// int하나랑 포인터 하나 들고 있죠?
		// 길이도 같이 저장해야 한다.
		int Size = static_cast<int>(_Data.size());
		operator<<(Size);
		if (0 != Size)
		{
			Write(&_Data[0], static_cast<int>(_Data.size()));
		}
	}

	void operator<<(class ISerializObject& _Data);

	template<typename DataType>
	void operator<<(std::vector<DataType>& _vector)
	{
		int Size = static_cast<int>(_vector.size());
		operator<<(Size);

		for (size_t i = 0; i < _vector.size(); i++)
		{
			// 자료형 변환이 안된다는 것이다.
			operator<<(_vector[i]);
		}
	}

	void Read(void* _Data, unsigned int _Size);

	void operator>>(int& _Data)
	{
		Read(&_Data, sizeof(int));
	}

	void operator>>(bool& _Data)
	{
		Read(&_Data, sizeof(bool));
	}

	void operator>>(FVector& _Data)
	{
		Read(&_Data, sizeof(FVector));
	}

	void operator>>(FIntPoint& _Data)
	{
		Read(&_Data, sizeof(FIntPoint));
	}

	void operator>>(std::string& _Data)
	{
		// int하나랑 포인터 하나 들고 있죠?
		// 길이도 같이 저장해야 한다.
		int Size;
		operator>>(Size);
		// 사이즈를 읽어낸다.
		_Data.resize(Size);

		Read(&_Data[0], static_cast<int>(_Data.size()));
	}

	void operator>>(class ISerializObject& _Data);

	template<typename DataType>
	void operator>>(std::vector<DataType>& _vector)
	{
		int Size = 0;
		operator>>(Size);
		_vector.resize(Size);

		for (size_t i = 0; i < _vector.size(); i++)
		{
			// 자료형 변환이 안된다는 것이다.
			operator>>(_vector[i]);
		}
	}

	void* GetDataPtr()
	{
		return &Data[0];
	}

	int GetWriteOffset()
	{
		return WriteOffset;
	}

	void DataResize(int _Value)
	{
		Data.resize(_Value);
	}

	size_t GetDataSize()
	{
		return Data.size();
	}

protected:

private:
	// 모든 데이터는 바이트 덩어리이다.
	// 그대신 클래스의 경우에는 중간에 바이트 패딩등등을 때문에 
	// 빈공간이 생긴다.

	// 얼마나 데이터가 채워졌냐?
	int WriteOffset = 0;

	// 얼마나 데이터가 채워졌냐?
	int ReadOffset = 0;

	// 자료형이라는게 존재하지 않죠?
	std::vector<char> Data;
};

// 언리얼 에서 인터페이스라고 불리는 존재를 만들때
// 앞에 I를 붙입니다.
class ISerializObject
{
	// 인터페이스가 뭐지?
	// 1. 맴버변수가 없음
	// 크기를 가지지 않아. 가상함수 테이블의 크기만 가지는 경우.
	// 2. 구현이 없음
	// 오로지 함수가 존재할거다라는 인터페이스로만 이루어져 있다.
	// 이 조건을 만족하면 
	// 다중상속에 부담이 없다. 
	// *. C++로 치자면 오로지 순수가상함수로만 구성된 클래스를 
	// 언리얼은 아닙니다. 순수가상함수를 막아서 못쓰거든요.
	// 3. 내부 가상함수들은 Private이 존재할수 없다.

public:
	// 데이터를 직렬화(압축)
	virtual void Serialize(UEngineSerializer& _Ser) = 0;
	// 데이터를 복구(할때)
	virtual void DeSerialize(UEngineSerializer& _Ser) = 0;
};

//class Test
//{
//	// 정한다고 하면
//	// Data 5 바이트 할당
//	//             [C][V][V][V][V]
//	// 직렬화의 첫번째 의미
//	// 데이터의 최적화
//
//	bool Check; // [C][ ][ ][ ][v][v][v][v]
//	int Value;  // 
//};
