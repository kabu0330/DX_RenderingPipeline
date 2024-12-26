#pragma once
#include <list>

// ���� :
class ULevel
{
public:
	// constrcuter destructer
	ULevel();
	~ULevel();

	// delete Function
	ULevel(const ULevel& _Other) = delete;
	ULevel(ULevel&& _Other) noexcept = delete;
	ULevel& operator=(const ULevel& _Other) = delete;
	ULevel& operator=(ULevel&& _Other) noexcept = delete;

	void Tick(float _DeltaTime);
	void Render(float _DeltaTime);
	void Release();
	void BeginPlayCheck();

	template<typename ActorType>
	ActorType* SpawnActor()
	{
		static_assert(std::is_base_of_v<AActor, ActorType>, "���͸� ������ �� �ֽ��ϴ�.");

		AActor* NewActor = new ActorType();
		BeginPlayActors.push_back(NewActor);

		return static_cast<ActorType*>(NewActor);
	}

protected:

private:
	std::list<class AActor*> AllActors;
	std::list<class AActor*> BeginPlayActors;

};

