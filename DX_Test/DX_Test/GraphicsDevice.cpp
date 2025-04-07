#include "PreCompile.h"
#include "GraphicsDevice.h"

UGraphicsDevice::UGraphicsDevice()
{
}

UGraphicsDevice::~UGraphicsDevice()
{
}

void UGraphicsDevice::Init()
{
	CreateDeviceAndContext();
	CreateBackBuffer();
}

void UGraphicsDevice::CreateDeviceAndContext()
{
	// 1. 그래픽카드(GPU)가 여러 개일 경우를 대비하여 가장 좋은 그래픽카드를 먼저 찾는다.
	IDXGIFactory* Factory = nullptr; // 팩토리 : 그래픽 카드 정보 열거, 출력 장치 관리

	HRESULT HR = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&Factory));
	if (S_OK != HR) // S_OK == long 0 // S_False == long 1 
	{
		// 생성에 실패했다면 메시지 박스를 띄우고 프로그램을 터뜨린다.
		MessageBoxA(nullptr, "팩토리 객체 생성에 실패했습니다.", "Error", MB_OK);
		assert(false);
		return;
	}

	// 가장 높은 성능의 기준을 사용자가 정한다.
	unsigned __int64 MaxVideoMemory = 0; // 비디오 램이 제일 큰 애가 성능 좋은 그래픽카드겠지.
	IDXGIAdapter* ResultAdapter = nullptr; // 가장 성능 좋은 그래픽카드 정보를 담을 객체

	// 그래픽 카드를 열거하여 가장 성능이 좋은 그래픽 카드를 선택한다.
	for (int Index = 0; ; ++Index)
	{
		IDXGIAdapter* CurAdapter = nullptr;
		Factory->EnumAdapters(Index, &CurAdapter); // 그래픽카드를 순차적으로 열거

		if (nullptr == CurAdapter)
		{
			// 더 이상 열거할 GPU가 없으면 반복문 종료
			break;
		}

		DXGI_ADAPTER_DESC Desc;
		CurAdapter->GetDesc(&Desc); // GPU의 정보를 가져와 Desc에 저장

		// 가장 큰 VRAM을 가진 GPU 선택
		if (MaxVideoMemory <= Desc.DedicatedVideoMemory)
		{
			MaxVideoMemory = Desc.DedicatedVideoMemory;

			// 이전에 저장된 GPU 객체가 있다면 해제
			if (nullptr != ResultAdapter)
			{
				ResultAdapter->Release();
			}

			// VRAM이 이전 GPU보다 큰 녀석을 최적의 어댑터(GPU)로 설정
			ResultAdapter = CurAdapter;
			continue;
		}

		CurAdapter->Release();
	}

	if (nullptr == ResultAdapter)
	{
		MessageBoxA(nullptr, "그래픽 카드가 존재하지 않는 PC입니다.", "Error", MB_OK);
		assert(false);
		return;
	}

	if (nullptr != Factory) // 최적의 그래픽카드를 찾았으므로 팩토리 객체는 해제
	{
		Factory->Release();
	}

	DXGI_ADAPTER_DESC AdapterDesc;
	ResultAdapter->GetDesc(&AdapterDesc); // PC에서 제일 좋은 그래픽카드를 추출했음을 검증

	// 제일 좋은 그래픽카드를 MainAdapter에 정보를 저장한다.
	MainAdapter = ResultAdapter;

	//----------------------------------------------------------------------------
	// 2. 최적의 GPU와 상호작용 할 Device 와 DeviceContext 객체를 생성한다.

	int iFlag = 0;

#ifdef _DEBUG
	iFlag = D3D11_CREATE_DEVICE_DEBUG; // 디버그 모드에서는 디버깅 기능 사용
#endif 

	D3D_FEATURE_LEVEL ResultLevel;

	//HRESULT WINAPI D3D11CreateDevice(
	//	IDXGIAdapter * pAdapter,                  // 기본 어댑터
	//	D3D_DRIVER_TYPE DriverType,               // 하드웨어 드라이버
	//	HMODULE Software,                         // 소프트웨어 드라이버
	//	UINT Flags,                               // 디버깅 플래그
	//	CONST D3D_FEATURE_LEVEL * pFeatureLevels, // 지원할 기능 수준 배열
	//	UINT FeatureLevels,                       // 배열 크기
	//	UINT SDKVersion,                          // SDK 버전
	//	ID3D11Device** ppDevice,                  // 디바이스 객체 정보 반환
	//	D3D_FEATURE_LEVEL* pFeatureLevel,         // DX 버전 반환
	//	ID3D11DeviceContext** ppImmediateContext  // 디바이스 컨텍스트 객체 정보 반환
	// ); 

	HRESULT Result = D3D11CreateDevice(
		MainAdapter,                              // 선택된 GPU를 사용
		D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_UNKNOWN, // GPU 지정시 UNKNOWN
		nullptr,                                  // 소프트웨어 드라이버 사용 안함
		iFlag,                                    // 디버깅 또는 기타 옵션 플래그
		nullptr,                                  // DX 3D 기능 수준 배열(nullptr이면 기본 수준)
		0,                                        // 기능 수준 배열 크기 (nullptr이면 0)
		D3D11_SDK_VERSION,                        // 항상 이 버전을 사용해야 함.
		&Device,                                  // 생성된 Device 객체
		&ResultLevel,                             // 디바이스가 지원하는 DirectX 버전 반환
		&Context);                                // 생성된 DeviceContext 객체

	if (nullptr == Device)
	{
		MessageBoxA(nullptr, "그래픽 디바이스 생성에 실패했습니다.", "Error", MB_OK);
		assert(false);
		return;
	}

	if (nullptr == Context)
	{
		MessageBoxA(nullptr, "그래픽 디바이스 컨텍스트 생성에 실패했습니다.", "Error", MB_OK);
		assert(false);
		return;
	}

	if (S_OK != Result)
	{
		MessageBoxA(nullptr, "그래픽 디바이스 및 컨텍스트 생성 작업이 실패했습니다. \n매개변수를 확인해주세요.", "Error", MB_OK);
		assert(false);
		return;
	}

	if (ResultLevel != D3D_FEATURE_LEVEL_11_0 && ResultLevel != D3D_FEATURE_LEVEL_11_1)
	{
		MessageBoxA(nullptr, "해당 그래픽카드는 DirectX 11 버전을 지원하지 않는 장치입니다.", "Error", MB_OK);
		assert(false);
		return;
	}

	Result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);	// 멀티스레드 환경

	if (S_OK != Result)
	{
		MessageBoxA(nullptr, "스레드 안정성 적용에 실패했습니다.", "Error", MB_OK);
		assert(false);
		return;
	}
}

void UGraphicsDevice::CreateBackBuffer()
{
	// 1. 백버퍼 관련 설정값 입력

	DXGI_SWAP_CHAIN_DESC SwapChainInfo = { 0, }; // 기본값(0) 설정

	SwapChainInfo.BufferCount = 2; // 백버퍼 몇 개 쓸건지?
	SwapChainInfo.BufferDesc.Width = UCore::WindowSize.iX();
	SwapChainInfo.BufferDesc.Height = UCore::WindowSize.iY();
	SwapChainInfo.OutputWindow = hWnd; // 최종 출력 창
	SwapChainInfo.Windowed = true; // 창화면, false : 전체화면

	// 주사율 설정(Refresh Rate) : 초당 화면이 갱신되는 빈도
	SwapChainInfo.BufferDesc.RefreshRate.Denominator = 1; // 분모
	SwapChainInfo.BufferDesc.RefreshRate.Numerator = 60; // 분자 (60Hz)

	SwapChainInfo.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 32비트 색상값, 양수 정규화 값(0.0 ~ 1.0)

	// 픽셀이 갱신되는 순서(Order) 지정 : 그냥 제일 빠른 방법으로
	SwapChainInfo.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainInfo.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// 백버퍼 사용용도                   렌더 타겟으로 사용       및   셰이더에서 접근 가능
	SwapChainInfo.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;

	SwapChainInfo.SampleDesc.Quality = 0;
	SwapChainInfo.SampleDesc.Count = 1;

	// 스왑 체인 효과 : 화면 갱신 시 이전 프레임을 버리고 새로운 프레임으로 교체
	SwapChainInfo.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	SwapChainInfo.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// --------------------------------------------------------------------------
	// 2. 백버퍼 만들기

	IDXGIFactory* ParentFactory = nullptr;

	// 현재 어댑터를 만들어준 팩토리 정보를 ParentFactory에 저장한다.
	MainAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&ParentFactory));

	ParentFactory->CreateSwapChain(Device, &SwapChainInfo, &SwapChain);

	if (nullptr == SwapChain)
	{
		MessageBoxA(nullptr, "스왑 체인 생성에 실패했습니다.", "Error", MB_OK);
		assert(false);
		return;
	}

	// 팩토리와 어댑터는 스왑체인을 생성함으로써 역할을 다 했다.
	ParentFactory->Release();
	MainAdapter->Release();


	// 스왑체인에서 첫 번째 백버퍼 텍스처를 가져온다.
	HRESULT HR = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&BackBufferTexture));

	if (S_OK != HR)
	{
		// 백버퍼 텍스처를 가져오지 못한 경우 : Direct3D 리소스 문제를 의심해야 한다.
		MessageBoxA(nullptr, "백버퍼 텍스처를 가져오는데 실패했습니다.", "Error", MB_OK);
		assert(false);
		return;
	}

	// 백버퍼 텍스처를 렌더 타겟 뷰(RTV)로 생성
	HR = Device->CreateRenderTargetView(BackBufferTexture, nullptr, &RTV);

	if (S_OK != HR)
	{
		MessageBoxA(nullptr, "백버퍼의 렌더 타겟 뷰 생성에 실패했습니다.", "Error", MB_OK);
		assert(false);
		return;
	}
}

void UGraphicsDevice::RenderStart()
{
	FVector ClearColor;
	ClearColor = FVector(0.0f, 0.0f, 1.0f, 1.0f); // 블루 프린트
	Context->ClearRenderTargetView(RTV, ClearColor.Arr1D);
}

void UGraphicsDevice::RenderEnd()
{
	// 백버퍼를 프론트버퍼로 전환
	HRESULT HR = SwapChain->Present(0, 0);
	//UINT SyncInterval, // 동기화 간격, 0 : 수직동기화 비활성화
	//UINT Flags         // 제어 플래그, 일반적으로 0으로 설정

	if (HR == DXGI_ERROR_DEVICE_REMOVED || HR == DXGI_ERROR_DEVICE_RESET)
	{
		MessageBoxA(nullptr, "스왑체인에 실패했습니다.", "Error", MB_OK);
		assert(false);
		return;
	}
}

void UGraphicsDevice::Release()
{
	if (nullptr != Device)
	{
		Device->Release();
		Device = nullptr;
	}

	if (nullptr != Context)
	{
		Context->Release();
		Context = nullptr;
	}

	if (nullptr != BackBufferTexture)
	{
		BackBufferTexture->Release();
		BackBufferTexture = nullptr;
	}

	if (nullptr != SwapChain)
	{
		SwapChain->Release();
		SwapChain = nullptr;
	}

	if (nullptr != RTV)
	{
		RTV->Release();
		RTV = nullptr;
	}

	//if (nullptr != MainAdapter)
	//{
	//	MainAdapter->Release();
	//	MainAdapter = nullptr;
	//}
}
