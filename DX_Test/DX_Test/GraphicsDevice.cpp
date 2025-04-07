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
	// 1. �׷���ī��(GPU)�� ���� ���� ��츦 ����Ͽ� ���� ���� �׷���ī�带 ���� ã�´�.
	IDXGIFactory* Factory = nullptr; // ���丮 : �׷��� ī�� ���� ����, ��� ��ġ ����

	HRESULT HR = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&Factory));
	if (S_OK != HR) // S_OK == long 0 // S_False == long 1 
	{
		// ������ �����ߴٸ� �޽��� �ڽ��� ���� ���α׷��� �Ͷ߸���.
		MessageBoxA(nullptr, "���丮 ��ü ������ �����߽��ϴ�.", "Error", MB_OK);
		assert(false);
		return;
	}

	// ���� ���� ������ ������ ����ڰ� ���Ѵ�.
	unsigned __int64 MaxVideoMemory = 0; // ���� ���� ���� ū �ְ� ���� ���� �׷���ī�����.
	IDXGIAdapter* ResultAdapter = nullptr; // ���� ���� ���� �׷���ī�� ������ ���� ��ü

	// �׷��� ī�带 �����Ͽ� ���� ������ ���� �׷��� ī�带 �����Ѵ�.
	for (int Index = 0; ; ++Index)
	{
		IDXGIAdapter* CurAdapter = nullptr;
		Factory->EnumAdapters(Index, &CurAdapter); // �׷���ī�带 ���������� ����

		if (nullptr == CurAdapter)
		{
			// �� �̻� ������ GPU�� ������ �ݺ��� ����
			break;
		}

		DXGI_ADAPTER_DESC Desc;
		CurAdapter->GetDesc(&Desc); // GPU�� ������ ������ Desc�� ����

		// ���� ū VRAM�� ���� GPU ����
		if (MaxVideoMemory <= Desc.DedicatedVideoMemory)
		{
			MaxVideoMemory = Desc.DedicatedVideoMemory;

			// ������ ����� GPU ��ü�� �ִٸ� ����
			if (nullptr != ResultAdapter)
			{
				ResultAdapter->Release();
			}

			// VRAM�� ���� GPU���� ū �༮�� ������ �����(GPU)�� ����
			ResultAdapter = CurAdapter;
			continue;
		}

		CurAdapter->Release();
	}

	if (nullptr == ResultAdapter)
	{
		MessageBoxA(nullptr, "�׷��� ī�尡 �������� �ʴ� PC�Դϴ�.", "Error", MB_OK);
		assert(false);
		return;
	}

	if (nullptr != Factory) // ������ �׷���ī�带 ã�����Ƿ� ���丮 ��ü�� ����
	{
		Factory->Release();
	}

	DXGI_ADAPTER_DESC AdapterDesc;
	ResultAdapter->GetDesc(&AdapterDesc); // PC���� ���� ���� �׷���ī�带 ���������� ����

	// ���� ���� �׷���ī�带 MainAdapter�� ������ �����Ѵ�.
	MainAdapter = ResultAdapter;

	//----------------------------------------------------------------------------
	// 2. ������ GPU�� ��ȣ�ۿ� �� Device �� DeviceContext ��ü�� �����Ѵ�.

	int iFlag = 0;

#ifdef _DEBUG
	iFlag = D3D11_CREATE_DEVICE_DEBUG; // ����� ��忡���� ����� ��� ���
#endif 

	D3D_FEATURE_LEVEL ResultLevel;

	//HRESULT WINAPI D3D11CreateDevice(
	//	IDXGIAdapter * pAdapter,                  // �⺻ �����
	//	D3D_DRIVER_TYPE DriverType,               // �ϵ���� ����̹�
	//	HMODULE Software,                         // ����Ʈ���� ����̹�
	//	UINT Flags,                               // ����� �÷���
	//	CONST D3D_FEATURE_LEVEL * pFeatureLevels, // ������ ��� ���� �迭
	//	UINT FeatureLevels,                       // �迭 ũ��
	//	UINT SDKVersion,                          // SDK ����
	//	ID3D11Device** ppDevice,                  // ����̽� ��ü ���� ��ȯ
	//	D3D_FEATURE_LEVEL* pFeatureLevel,         // DX ���� ��ȯ
	//	ID3D11DeviceContext** ppImmediateContext  // ����̽� ���ؽ�Ʈ ��ü ���� ��ȯ
	// ); 

	HRESULT Result = D3D11CreateDevice(
		MainAdapter,                              // ���õ� GPU�� ���
		D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_UNKNOWN, // GPU ������ UNKNOWN
		nullptr,                                  // ����Ʈ���� ����̹� ��� ����
		iFlag,                                    // ����� �Ǵ� ��Ÿ �ɼ� �÷���
		nullptr,                                  // DX 3D ��� ���� �迭(nullptr�̸� �⺻ ����)
		0,                                        // ��� ���� �迭 ũ�� (nullptr�̸� 0)
		D3D11_SDK_VERSION,                        // �׻� �� ������ ����ؾ� ��.
		&Device,                                  // ������ Device ��ü
		&ResultLevel,                             // ����̽��� �����ϴ� DirectX ���� ��ȯ
		&Context);                                // ������ DeviceContext ��ü

	if (nullptr == Device)
	{
		MessageBoxA(nullptr, "�׷��� ����̽� ������ �����߽��ϴ�.", "Error", MB_OK);
		assert(false);
		return;
	}

	if (nullptr == Context)
	{
		MessageBoxA(nullptr, "�׷��� ����̽� ���ؽ�Ʈ ������ �����߽��ϴ�.", "Error", MB_OK);
		assert(false);
		return;
	}

	if (S_OK != Result)
	{
		MessageBoxA(nullptr, "�׷��� ����̽� �� ���ؽ�Ʈ ���� �۾��� �����߽��ϴ�. \n�Ű������� Ȯ�����ּ���.", "Error", MB_OK);
		assert(false);
		return;
	}

	if (ResultLevel != D3D_FEATURE_LEVEL_11_0 && ResultLevel != D3D_FEATURE_LEVEL_11_1)
	{
		MessageBoxA(nullptr, "�ش� �׷���ī��� DirectX 11 ������ �������� �ʴ� ��ġ�Դϴ�.", "Error", MB_OK);
		assert(false);
		return;
	}

	Result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);	// ��Ƽ������ ȯ��

	if (S_OK != Result)
	{
		MessageBoxA(nullptr, "������ ������ ���뿡 �����߽��ϴ�.", "Error", MB_OK);
		assert(false);
		return;
	}
}

void UGraphicsDevice::CreateBackBuffer()
{
	// 1. ����� ���� ������ �Է�

	DXGI_SWAP_CHAIN_DESC SwapChainInfo = { 0, }; // �⺻��(0) ����

	SwapChainInfo.BufferCount = 2; // ����� �� �� ������?
	SwapChainInfo.BufferDesc.Width = UCore::WindowSize.iX();
	SwapChainInfo.BufferDesc.Height = UCore::WindowSize.iY();
	SwapChainInfo.OutputWindow = hWnd; // ���� ��� â
	SwapChainInfo.Windowed = true; // âȭ��, false : ��üȭ��

	// �ֻ��� ����(Refresh Rate) : �ʴ� ȭ���� ���ŵǴ� ��
	SwapChainInfo.BufferDesc.RefreshRate.Denominator = 1; // �и�
	SwapChainInfo.BufferDesc.RefreshRate.Numerator = 60; // ���� (60Hz)

	SwapChainInfo.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 32��Ʈ ����, ��� ����ȭ ��(0.0 ~ 1.0)

	// �ȼ��� ���ŵǴ� ����(Order) ���� : �׳� ���� ���� �������
	SwapChainInfo.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainInfo.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// ����� ���뵵                   ���� Ÿ������ ���       ��   ���̴����� ���� ����
	SwapChainInfo.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;

	SwapChainInfo.SampleDesc.Quality = 0;
	SwapChainInfo.SampleDesc.Count = 1;

	// ���� ü�� ȿ�� : ȭ�� ���� �� ���� �������� ������ ���ο� ���������� ��ü
	SwapChainInfo.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	SwapChainInfo.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// --------------------------------------------------------------------------
	// 2. ����� �����

	IDXGIFactory* ParentFactory = nullptr;

	// ���� ����͸� ������� ���丮 ������ ParentFactory�� �����Ѵ�.
	MainAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&ParentFactory));

	ParentFactory->CreateSwapChain(Device, &SwapChainInfo, &SwapChain);

	if (nullptr == SwapChain)
	{
		MessageBoxA(nullptr, "���� ü�� ������ �����߽��ϴ�.", "Error", MB_OK);
		assert(false);
		return;
	}

	// ���丮�� ����ʹ� ����ü���� ���������ν� ������ �� �ߴ�.
	ParentFactory->Release();
	MainAdapter->Release();


	// ����ü�ο��� ù ��° ����� �ؽ�ó�� �����´�.
	HRESULT HR = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&BackBufferTexture));

	if (S_OK != HR)
	{
		// ����� �ؽ�ó�� �������� ���� ��� : Direct3D ���ҽ� ������ �ǽ��ؾ� �Ѵ�.
		MessageBoxA(nullptr, "����� �ؽ�ó�� �������µ� �����߽��ϴ�.", "Error", MB_OK);
		assert(false);
		return;
	}

	// ����� �ؽ�ó�� ���� Ÿ�� ��(RTV)�� ����
	HR = Device->CreateRenderTargetView(BackBufferTexture, nullptr, &RTV);

	if (S_OK != HR)
	{
		MessageBoxA(nullptr, "������� ���� Ÿ�� �� ������ �����߽��ϴ�.", "Error", MB_OK);
		assert(false);
		return;
	}
}

void UGraphicsDevice::RenderStart()
{
	FVector ClearColor;
	ClearColor = FVector(0.0f, 0.0f, 1.0f, 1.0f); // ��� ����Ʈ
	Context->ClearRenderTargetView(RTV, ClearColor.Arr1D);
}

void UGraphicsDevice::RenderEnd()
{
	// ����۸� ����Ʈ���۷� ��ȯ
	HRESULT HR = SwapChain->Present(0, 0);
	//UINT SyncInterval, // ����ȭ ����, 0 : ��������ȭ ��Ȱ��ȭ
	//UINT Flags         // ���� �÷���, �Ϲ������� 0���� ����

	if (HR == DXGI_ERROR_DEVICE_REMOVED || HR == DXGI_ERROR_DEVICE_RESET)
	{
		MessageBoxA(nullptr, "����ü�ο� �����߽��ϴ�.", "Error", MB_OK);
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
