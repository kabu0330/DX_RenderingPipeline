#include "EngineMath.h"

const double UEngineMath::DPI = 3.14159265358979323846264338327950288419716939937510;
const double UEngineMath::DPI2 = DPI * 2.0;

const float UEngineMath::PI = 3.14159265358979323846264f;
const float UEngineMath::PI2 = PI * 2.0f;

// 디그리를 라디안으로 바꾸는 값이 된다.
const float UEngineMath::D2R = UEngineMath::PI / 180.0f;
const float UEngineMath::R2D = 180.0f / UEngineMath::PI;

const FVector FVector::ZERO = { 0.0f, 0.0f };
const FVector FVector::LEFT = { -1.0f, 0.0f };
const FVector FVector::RIGHT = { 1.0f, 0.0f };
const FVector FVector::UP = { 0.0f, 1.0f };
const FVector FVector::DOWN = { 0.0f, -1.0f };
const FVector FVector::FORWARD = { 0.0f, 0.0f, 1.0f };
const FVector FVector::BACK = { 0.0f, 0.0f , -1.0f };

// const FVector FVector::BLUE = {0.0f, 0.0f, 1.0f, 1.0f};


const FIntPoint FIntPoint::LEFT = { -1, 0 };
const FIntPoint FIntPoint::RIGHT = { 1, 0 };
const FIntPoint FIntPoint::UP = { 0, -1 };
const FIntPoint FIntPoint::DOWN = { 0, 1 };



const UColor UColor::WHITE = { 255, 255, 255, 0 };
const UColor UColor::BLACK = { 0, 0, 0, 0 };

FIntPoint FVector::ConvertToPoint() const
{
	return { iX(), iY() };
}


std::function<bool(const FTransform&, const FTransform&)> FTransform::AllCollisionFunction[static_cast<int>(ECollisionType::Max)][static_cast<int>(ECollisionType::Max)];

class CollisionFunctionInit
{
public:
	CollisionFunctionInit()
	{
		// 데이터 영역이 초기화 될때 초기화하는 일을 자동으로 수행할수 있다.
		// 데이터 영역이 만들어질때 이 작업은 자동으로 실행된다.
		FTransform::AllCollisionFunction[static_cast<int>(ECollisionType::Rect)][static_cast<int>(ECollisionType::Rect)] = FTransform::RectToRect;
		FTransform::AllCollisionFunction[static_cast<int>(ECollisionType::CirCle)][static_cast<int>(ECollisionType::CirCle)] = FTransform::CirCleToCirCle;
		FTransform::AllCollisionFunction[static_cast<int>(ECollisionType::Rect)][static_cast<int>(ECollisionType::CirCle)] = FTransform::RectToCirCle;
		FTransform::AllCollisionFunction[static_cast<int>(ECollisionType::CirCle)][static_cast<int>(ECollisionType::Rect)] = FTransform::CirCleToRect;

	}
};

// 데이터 영역
CollisionFunctionInit Inst = CollisionFunctionInit();


bool FTransform::Collision(ECollisionType _LeftType, const FTransform& _Left, ECollisionType _RightType, const FTransform& _Right)
{
	return FTransform::AllCollisionFunction[static_cast<int>(_LeftType)][static_cast<int>(_RightType)](_Left, _Right);
}

bool FTransform::PointToCirCle(const FTransform& _Left, const FTransform& _Right)
{
	FTransform LeftTrans = _Left;
	LeftTrans.Scale = FVector::ZERO;
	return CirCleToCirCle(LeftTrans, _Right);
}

// 점 vs 사각형
bool FTransform::PointToRect(const FTransform& _Left, const FTransform& _Right)
{
	FTransform LeftTrans = _Left;
	LeftTrans.Scale = FVector::ZERO;
	return RectToRect(LeftTrans, _Right);
}

bool FTransform::CirCleToCirCle(const FTransform& _Left, const FTransform& _Right)
{
	FVector Len = _Left.Location - _Right.Location;

	// 트랜스폼을 원으로 봤을때 반지름은 x의 절반크기를 반지름으로 보겠습니다.
	// 두원의 반지름의 합이 벡터의 길이보다 크다면 
	if (Len.Length() < _Left.Scale.hX() + _Right.Scale.hX())
	{
		return true;
	}

	return false;
}

bool FTransform::RectToRect(const FTransform& _Left, const FTransform& _Right)
{

	if (_Left.ZAxisCenterLeft() > _Right.ZAxisCenterRight())
	{
		return false;
	}

	if (_Left.ZAxisCenterRight() < _Right.ZAxisCenterLeft())
	{
		return false;
	}

	if (_Left.ZAxisCenterTop() > _Right.ZAxisCenterBottom())
	{
		return false;
	}

	if (_Left.ZAxisCenterBottom() < _Right.ZAxisCenterTop())
	{
		return false;
	}
	// 공식 만들면 된다.
	return true;
}

bool FTransform::RectToCirCle(const FTransform& _Left, const FTransform& _Right)
{
	return CirCleToRect(_Right, _Left);
}


bool FTransform::CirCleToRect(const FTransform& _Left, const FTransform& _Right)
{
	// 좌우로 반지름 확장한 트랜스폼
	FTransform WTransform = _Right;
	WTransform.Scale.X += _Left.Scale.X;

	// 위아래로 반지름 만큼 확장한 트랜스폼
	FTransform HTransform = _Right;
	HTransform.Scale.Y += _Left.Scale.X;

	if (true == PointToRect(_Left, WTransform) || true == PointToRect(_Left, HTransform))
	{
		return true;
	}

	// 비용 절약을 위해서 static으로 만드는 방법도 있는데.
	// static FVector ArrPoint[4];
	// 쓰레드에서는 못쓴다.
	FVector ArrPoint[4];

	ArrPoint[0] = _Right.ZAxisCenterLeftTop();
	ArrPoint[1] = _Right.ZAxisCenterLeftBottom();
	ArrPoint[2] = _Right.ZAxisCenterRightTop();
	ArrPoint[3] = _Right.ZAxisCenterRightBottom();

	FTransform PointCirCle;
	PointCirCle.Scale = _Left.Scale;
	for (size_t i = 0; i < 4; i++)
	{
		PointCirCle.Location = ArrPoint[i];
		if (true == PointToCirCle(_Left, PointCirCle))
		{
			return true;
		}
	}

	return false;
}

FVector FVector::Transform(const FVector& _Vector, const class FMatrix& _Matrix)
{
	return _Vector * _Matrix;
}

FVector FVector::TransformCoord(const FVector& _Vector, const class FMatrix& _Matrix)
{
	FVector Copy = _Vector;
	Copy.W = 1.0f;
	return Copy * _Matrix;
}

FVector FVector::TransformNormal(const FVector& _Vector, const class FMatrix& _Matrix)
{
	FVector Copy = _Vector;
	Copy.W = 0.0f;
	return Copy * _Matrix;
}

FVector FVector::operator*(const class FMatrix& _Matrix) const
{
	FVector Result;
	// 나머지 완성하고 곱해서 결과 확인해보세요.

	// x y z w가 다 곱해져야 한다.
	Result.X = Arr2D[0][0] * _Matrix.Arr2D[0][0] + Arr2D[0][1] * _Matrix.Arr2D[1][0] + Arr2D[0][2] * _Matrix.Arr2D[2][0] + Arr2D[0][3] * _Matrix.Arr2D[3][0];
	Result.Y = Arr2D[0][0] * _Matrix.Arr2D[0][1] + Arr2D[0][1] * _Matrix.Arr2D[1][1] + Arr2D[0][2] * _Matrix.Arr2D[2][1] + Arr2D[0][3] * _Matrix.Arr2D[3][1];
	Result.Z = Arr2D[0][0] * _Matrix.Arr2D[0][2] + Arr2D[0][1] * _Matrix.Arr2D[1][2] + Arr2D[0][2] * _Matrix.Arr2D[2][2] + Arr2D[0][3] * _Matrix.Arr2D[3][2];
	Result.W = Arr2D[0][0] * _Matrix.Arr2D[0][3] + Arr2D[0][1] * _Matrix.Arr2D[1][3] + Arr2D[0][2] * _Matrix.Arr2D[2][3] + Arr2D[0][3] * _Matrix.Arr2D[3][3];


	//std::cout << "X : " << Arr2D[0][0] << "*" << _Matrix.Arr2D[0][0] << "+" << Arr2D[0][1] << "*" << _Matrix.Arr2D[1][0] << "+" << Arr2D[0][2] << "*" << _Matrix.Arr2D[2][0] << "+" << Arr2D[0][3] << "*" << _Matrix.Arr2D[3][0] << std::endl;
	//std::cout << "Y : " << Arr2D[0][0] << "*" << _Matrix.Arr2D[0][1] << "+" << Arr2D[0][1] << "*" << _Matrix.Arr2D[1][1] << "+" << Arr2D[0][2] << "*" << _Matrix.Arr2D[2][1] << "+" << Arr2D[0][3] << "*" << _Matrix.Arr2D[3][1] << std::endl;
	//std::cout << "Z : " << Arr2D[0][0] << "*" << _Matrix.Arr2D[0][2] << "+" << Arr2D[0][1] << "*" << _Matrix.Arr2D[1][2] << "+" << Arr2D[0][2] << "*" << _Matrix.Arr2D[2][2] << "+" << Arr2D[0][3] << "*" << _Matrix.Arr2D[3][2] << std::endl;
	//std::cout << "W : " << Arr2D[0][0] << "*" << _Matrix.Arr2D[0][3] << "+" << Arr2D[0][1] << "*" << _Matrix.Arr2D[1][3] << "+" << Arr2D[0][2] << "*" << _Matrix.Arr2D[2][3] << "+" << Arr2D[0][3] << "*" << _Matrix.Arr2D[3][3] << std::endl;

	//std::cout << Result.ToString() << std::endl;


	return Result;
}

FVector& FVector::operator*=(const FMatrix& _Matrix)
{
	FVector Copy = *this;

	this->X = Copy.X * _Matrix.Arr2D[0][0] + Copy.Y * _Matrix.Arr2D[1][0] + Copy.Z * _Matrix.Arr2D[2][0] + Copy.W * _Matrix.Arr2D[3][0];
	this->Y = Copy.X * _Matrix.Arr2D[0][1] + Copy.Y * _Matrix.Arr2D[1][1] + Copy.Z * _Matrix.Arr2D[2][1] + Copy.W * _Matrix.Arr2D[3][1];
	this->Z = Copy.X * _Matrix.Arr2D[0][2] + Copy.Y * _Matrix.Arr2D[1][2] + Copy.Z * _Matrix.Arr2D[2][2] + Copy.W * _Matrix.Arr2D[3][2];
	this->W = Copy.X * _Matrix.Arr2D[0][3] + Copy.Y * _Matrix.Arr2D[1][3] + Copy.Z * _Matrix.Arr2D[2][3] + Copy.W * _Matrix.Arr2D[3][3];


	return *this;
}

FMatrix FMatrix::operator*(const FMatrix& _Matrix)
{
	FMatrix Result;

	Result.Arr2D[0][0] = Arr2D[0][0] * _Matrix.Arr2D[0][0] + Arr2D[0][1] * _Matrix.Arr2D[1][0] + Arr2D[0][2] * _Matrix.Arr2D[2][0] + Arr2D[0][3] * _Matrix.Arr2D[3][0];
	Result.Arr2D[0][1] = Arr2D[0][0] * _Matrix.Arr2D[0][1] + Arr2D[0][1] * _Matrix.Arr2D[1][1] + Arr2D[0][2] * _Matrix.Arr2D[2][1] + Arr2D[0][3] * _Matrix.Arr2D[3][1];
	Result.Arr2D[0][2] = Arr2D[0][0] * _Matrix.Arr2D[0][2] + Arr2D[0][1] * _Matrix.Arr2D[1][2] + Arr2D[0][2] * _Matrix.Arr2D[2][2] + Arr2D[0][3] * _Matrix.Arr2D[3][2];
	Result.Arr2D[0][3] = Arr2D[0][0] * _Matrix.Arr2D[0][3] + Arr2D[0][1] * _Matrix.Arr2D[1][3] + Arr2D[0][2] * _Matrix.Arr2D[2][3] + Arr2D[0][3] * _Matrix.Arr2D[3][3];

	Result.Arr2D[1][0] = Arr2D[1][0] * _Matrix.Arr2D[0][0] + Arr2D[1][1] * _Matrix.Arr2D[1][0] + Arr2D[1][2] * _Matrix.Arr2D[2][0] + Arr2D[1][3] * _Matrix.Arr2D[3][0];
	Result.Arr2D[1][1] = Arr2D[1][0] * _Matrix.Arr2D[0][1] + Arr2D[1][1] * _Matrix.Arr2D[1][1] + Arr2D[1][2] * _Matrix.Arr2D[2][1] + Arr2D[1][3] * _Matrix.Arr2D[3][1];
	Result.Arr2D[1][2] = Arr2D[1][0] * _Matrix.Arr2D[0][2] + Arr2D[1][1] * _Matrix.Arr2D[1][2] + Arr2D[1][2] * _Matrix.Arr2D[2][2] + Arr2D[1][3] * _Matrix.Arr2D[3][2];
	Result.Arr2D[1][3] = Arr2D[1][0] * _Matrix.Arr2D[0][3] + Arr2D[1][1] * _Matrix.Arr2D[1][3] + Arr2D[1][2] * _Matrix.Arr2D[2][3] + Arr2D[1][3] * _Matrix.Arr2D[3][3];

	Result.Arr2D[2][0] = Arr2D[2][0] * _Matrix.Arr2D[0][0] + Arr2D[2][1] * _Matrix.Arr2D[1][0] + Arr2D[2][2] * _Matrix.Arr2D[2][0] + Arr2D[2][3] * _Matrix.Arr2D[3][0];
	Result.Arr2D[2][1] = Arr2D[2][0] * _Matrix.Arr2D[0][1] + Arr2D[2][1] * _Matrix.Arr2D[1][1] + Arr2D[2][2] * _Matrix.Arr2D[2][1] + Arr2D[2][3] * _Matrix.Arr2D[3][1];
	Result.Arr2D[2][2] = Arr2D[2][0] * _Matrix.Arr2D[0][2] + Arr2D[2][1] * _Matrix.Arr2D[1][2] + Arr2D[2][2] * _Matrix.Arr2D[2][2] + Arr2D[2][3] * _Matrix.Arr2D[3][2];
	Result.Arr2D[2][3] = Arr2D[2][0] * _Matrix.Arr2D[0][3] + Arr2D[2][1] * _Matrix.Arr2D[1][3] + Arr2D[2][2] * _Matrix.Arr2D[2][3] + Arr2D[2][3] * _Matrix.Arr2D[3][3];

	Result.Arr2D[3][0] = Arr2D[3][0] * _Matrix.Arr2D[0][0] + Arr2D[3][1] * _Matrix.Arr2D[1][0] + Arr2D[3][2] * _Matrix.Arr2D[2][0] + Arr2D[3][3] * _Matrix.Arr2D[3][0];
	Result.Arr2D[3][1] = Arr2D[3][0] * _Matrix.Arr2D[0][1] + Arr2D[3][1] * _Matrix.Arr2D[1][1] + Arr2D[3][2] * _Matrix.Arr2D[2][1] + Arr2D[3][3] * _Matrix.Arr2D[3][1];
	Result.Arr2D[3][2] = Arr2D[3][0] * _Matrix.Arr2D[0][2] + Arr2D[3][1] * _Matrix.Arr2D[1][2] + Arr2D[3][2] * _Matrix.Arr2D[2][2] + Arr2D[3][3] * _Matrix.Arr2D[3][2];
	Result.Arr2D[3][3] = Arr2D[3][0] * _Matrix.Arr2D[0][3] + Arr2D[3][1] * _Matrix.Arr2D[1][3] + Arr2D[3][2] * _Matrix.Arr2D[2][3] + Arr2D[3][3] * _Matrix.Arr2D[3][3];

	return Result;

}

void FTransform::TransformUpdate()
{
	ScaleMat.Scale(Scale);
	RotationMat.RotationDeg(Rotation);
	LocationMat.Position(Location);

	World = ScaleMat * RotationMat * LocationMat;
}