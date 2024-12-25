#pragma once
#include <vector>
// 최신 std기능을 이용해볼겁니다.
#include "EnginePath.h"

// 실패가 없는게 여러분들의 가장문제.

// 설명 : 
class UEngineDirectory : public UEnginePath
{
public:
	// constrcuter destructer
	UEngineDirectory();
	UEngineDirectory(std::string_view _Path);
	UEngineDirectory(std::filesystem::path _Path);
	~UEngineDirectory();

	//// delete Function
	//UEngineDirectory(const UEngineDirectory& _Other) = delete;
	//UEngineDirectory(UEngineDirectory&& _Other) noexcept = delete;
	//UEngineDirectory& operator=(const UEngineDirectory& _Other) = delete;
	//UEngineDirectory& operator=(UEngineDirectory&& _Other) noexcept = delete;

	// 시간이 너무 없다고 안만듬. 나중에 만들어야지.
	std::vector<class UEngineFile> GetAllFile(bool _IsRecursive, std::vector<std::string> _Exts);

	std::vector<class UEngineDirectory> GetAllDirectory();

	UEngineFile GetFile(std::string_view _FileName);

protected:

private:
	void GetAllFileRecursive(std::filesystem::path _Path, std::vector<class UEngineFile>& _Result);

};

