#pragma once
#include <list>

// Ό³Έν :
class AActor
{
public:
	// constrcuter destructer
	AActor();
	~AActor();

	// delete Function
	AActor(const AActor& _Other) = delete;
	AActor(AActor&& _Other) noexcept = delete;
	AActor& operator=(const AActor& _Other) = delete;
	AActor& operator=(AActor&& _Other) noexcept = delete;

	virtual void BeginPlay();
	virtual void Tick(float _DeltaTime) {}

	void Render(float _DeltaTime);


	template<typename ComponetType>
	ComponetType* CreateDefaultSubObject()
	{
		URenderer* NewComponent = new ComponetType();

		Renderers.push_back(NewComponent);
		BeginPlayRenderers.push_back(NewComponent);
		return static_cast<ComponetType*>(NewComponent);
	}

	void SetActorLocation(const FVector& _Value)
	{
		Transform.Location = _Value;
		Transform.TransformUpdate();
	}

	void SetActorRelativeScale3D(const FVector& _Scale)
	{
		Transform.Scale = _Scale;
		Transform.TransformUpdate();
	}

	void AddActorLocation(const FVector& _Value)
	{
		Transform.Location += _Value;
		Transform.TransformUpdate();
	}
	

protected:
	FTransform Transform;
private:
	void ComponentBeginPlay();
	static std::list<class URenderer*> Renderers;
	std::list<class URenderer*> BeginPlayRenderers;

};

typedef AActor Super;
