#include "PreCompile.h"
#include "Renderer.h"
#include <assert.h>

void URenderer::CreateDeviceAndContext()
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

void URenderer::CreateBackBuffer()
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

void URenderer::RenderStart()
{
	FVector ClearColor;
	ClearColor = FVector(0.0f, 0.0f, 1.0f, 1.0f); // ��� ����Ʈ
	Context->ClearRenderTargetView(RTV, ClearColor.Arr1D);
}

void URenderer::RenderEnd()
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

void URenderer::BeginPlay()
{
	// 1. Create
	CreateDeviceAndContext();
	CreateBackBuffer();

	// 2. Rendering Pipeline
	InputAssembler1Init();
	VertexShaderInit();
	InputAssembler2Init();
	RasterizerInit();
	PixelShaderInit();
	ShaderResInit();
}

void URenderer::Render(float _DeltaTime)
{

	// 2. Rendering Pipeline
	ShaderResSetting();
	InputAssembler1Setting();
	VertexShaderSetting();
	InputAssembler2Setting();
	RasterizerSetting();
	PixelShaderSetting();
	OutputMergeSetting();

	// �ε��� ���۸� ���ؼ� �׸��ڴ�.
	Context->DrawIndexed(6, 0, 0);
}

void URenderer::InputAssembler1Init()
{
	std::vector<VertexData> Vertices;
	Vertices.resize(4);

	Vertices[0] = VertexData{ FVector(-0.5f,  0.5f, 0.0f), {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f} };
	Vertices[1] = VertexData{ FVector(0.5f,  0.5f, 0.0f), {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f} };
	Vertices[2] = VertexData{ FVector(-0.5f, -0.5f, 0.0f), {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f} };
	Vertices[3] = VertexData{ FVector(0.5f, -0.5f, 0.0f), {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f} };
	// 0   1
	// 
	// 2   3

	D3D11_BUFFER_DESC BufferInfo = { 0, };
	BufferInfo.ByteWidth = sizeof(VertexData) * static_cast<int>(Vertices.size());
	BufferInfo.BindFlags = D3D11_BIND_VERTEX_BUFFER; // ���ؽ� ���۷� ���
	BufferInfo.CPUAccessFlags = 0;                   // CPU ���� ����
	BufferInfo.Usage = D3D11_USAGE_DEFAULT;          // GPU������ ���

	D3D11_SUBRESOURCE_DATA Data; // ������ �����ּ� �־��ִ� ����ü
	Data.pSysMem = &Vertices[0];

	HRESULT HR = Device->CreateBuffer(&BufferInfo, &Data, &VertexBuffer);
	if (S_OK != HR)
	{
		MessageBoxA(nullptr, "���ؽ� ���� ������ �����߽��ϴ�.", "Error", MB_OK);
		assert(false);
		return;
	}
}

void URenderer::InputAssembler2Init()
{
	std::vector<unsigned int> Indices;
	Indices.resize(6);

	Indices.push_back(0);
	Indices.push_back(1);
	Indices.push_back(2);

	Indices.push_back(1);
	Indices.push_back(3);
	Indices.push_back(2);
	// 0    1
	//
	// 2    3

	D3D11_BUFFER_DESC IndexBufferInfo = { 0, };
	IndexBufferInfo.ByteWidth = sizeof(unsigned int) * static_cast<int>(Indices.size());
	IndexBufferInfo.BindFlags = D3D11_BIND_INDEX_BUFFER; // �ε��� ���۷� ���
	IndexBufferInfo.CPUAccessFlags = 0;                  // CPU ���� ����
	IndexBufferInfo.Usage = D3D11_USAGE_DEFAULT;         // GPU�� ���

	D3D11_SUBRESOURCE_DATA Data;
	Data.pSysMem = &Indices[0];

	HRESULT HR = Device->CreateBuffer(&IndexBufferInfo, &Data, &IndexBuffer);
	if (S_OK != HR)
	{
		MSGASSERT("�ε��� ���� ������ �����߽��ϴ�.");
		return;
	}
}

void URenderer::VertexShaderInit()
{
	UEngineDirectory ShaderDirectory;
	ShaderDirectory.MoveParentToDirectory("Shader");
	UEngineFile File = ShaderDirectory.GetFile("VertexAndPixelShader.fx");

	std::string Path = File.GetPathToString();
	std::wstring WPath = UEngineString::AnsiToUnicode(Path);

	std::string Version = "vs_5_0";
	int Flag0 = 0;
	int Flag1 = 0;

#ifdef _DEBUG
	Flag0 = D3D10_SHADER_DEBUG;
#endif // _DEBUG
	Flag0 |= D3DCOMPILE_PACK_MATRIX_ROW_MAJOR; // ����� �����

	D3DCompileFromFile(
		WPath.c_str(),
		nullptr,
		nullptr,
		"VertexToWorld", // ���̴� ���� ���� �Լ�
		Version.c_str(),
		Flag0,
		Flag1,
		&VSShaderCodeBlob,
		&VSErrorCodeBlob
	);

	if (nullptr == VSShaderCodeBlob)
	{
		std::string ErrString = reinterpret_cast<char*>(VSErrorCodeBlob->GetBufferPointer());
		MSGASSERT("���ؽ� ���̴� �ʱ� ������ �����߽��ϴ�.\n" + ErrString);
		return;
	}

	HRESULT Result = Device->CreateVertexShader(
		VSShaderCodeBlob->GetBufferPointer(),
		VSShaderCodeBlob->GetBufferSize(),
		nullptr,
		&VertexShader
	);

	if (S_OK != Result)
	{
		MSGASSERT("���ؽ� ���̴� ������ �����߽��ϴ�.");
		return;
	}

	InputAssembler1Layout();
}

void URenderer::InputAssembler1Layout()
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> InputLayoutData;

	D3D11_INPUT_ELEMENT_DESC PositionDesc;
	PositionDesc.SemanticName = "POSITION";
	PositionDesc.AlignedByteOffset = 0; // �ش� ������ ���� ����Ʈ (0 + sizeof(float4))
	PositionDesc.InputSlot = 0;
	PositionDesc.SemanticIndex = 0;
	PositionDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	PositionDesc.InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	PositionDesc.InstanceDataStepRate = 0;
	InputLayoutData.push_back(PositionDesc);


	D3D11_INPUT_ELEMENT_DESC UVDesc;
	UVDesc.SemanticName = "TEXCOORD";
	UVDesc.AlignedByteOffset = 16; // �ش� ������ ���� ����Ʈ (0 + sizeof(float4))
	UVDesc.InputSlot = 0;
	UVDesc.SemanticIndex = 0;
	UVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	UVDesc.InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	UVDesc.InstanceDataStepRate = 0;
	InputLayoutData.push_back(UVDesc);


	D3D11_INPUT_ELEMENT_DESC ColorDesc;
	ColorDesc.SemanticName = "COLOR";
	ColorDesc.AlignedByteOffset = 32; // �ش� ������ ���� ����Ʈ (0 + sizeof(float4))
	ColorDesc.InputSlot = 0;
	ColorDesc.SemanticIndex = 0;
	ColorDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	ColorDesc.InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	ColorDesc.InstanceDataStepRate = 0;
	InputLayoutData.push_back(ColorDesc);


	HRESULT HR = Device->CreateInputLayout(
		&InputLayoutData[0],
		static_cast<unsigned int>(InputLayoutData.size()),
		VSShaderCodeBlob->GetBufferPointer(),
		VSShaderCodeBlob->GetBufferSize(),
		&InputLayout
	);

	if (S_OK != HR)
	{
		MessageBoxA(nullptr, "��ǲ ���̾ƿ� ������ �����߽��ϴ�.", "Error", MB_OK);
		assert(false);
		return;
	}
}

void URenderer::RasterizerInit()
{
	D3D11_RASTERIZER_DESC RasterizerDesc = {};
	RasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
	RasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	Device->CreateRasterizerState(&RasterizerDesc, &RasterizerState);

	ViewportInfo.Width = UCore::WindowSize.X;
	ViewportInfo.Height = UCore::WindowSize.Y;
	ViewportInfo.TopLeftX = 0.0f;
	ViewportInfo.TopLeftY = 0.0f;
	ViewportInfo.MinDepth = 0.0f;
	ViewportInfo.MaxDepth = 1.0f;
}

void URenderer::PixelShaderInit()
{
	UEngineDirectory ShaderDirectory;
	ShaderDirectory.MoveParentToDirectory("Shader");
	UEngineFile File = ShaderDirectory.GetFile("VertexAndPixelShader.fx");

	std::string Path = File.GetPathToString();
	std::wstring WPath = UEngineString::AnsiToUnicode(Path);

	std::string Version = "ps_5_0";
	int Flag0 = 0;
	int Flag1 = 0;

#ifdef _DEBUG
	Flag0 = D3D10_SHADER_DEBUG;
#endif // _DEBUG
	Flag0 |= D3DCOMPILE_PACK_MATRIX_ROW_MAJOR; // ����� �����

	D3DCompileFromFile(
		WPath.c_str(),
		nullptr,
		nullptr,
		"PixelToWorld", // ���̴� ���� ���� �Լ�
		Version.c_str(),
		Flag0,
		Flag1,
		&PSShaderCodeBlob,
		&PSErrorCodeBlob
	);

	if (nullptr == PSShaderCodeBlob)
	{
		std::string ErrString = reinterpret_cast<char*>(PSErrorCodeBlob->GetBufferPointer());
		MSGASSERT("�ȼ� ���̴� �ʱ� ������ �����߽��ϴ�.\n" + ErrString);
		return;
	}

	HRESULT Result = Device->CreatePixelShader(
		PSShaderCodeBlob->GetBufferPointer(),
		PSShaderCodeBlob->GetBufferSize(),
		nullptr,
		&PixelShader
	);

	if (S_OK != Result)
	{
		MSGASSERT("�ȼ� ���̴� ������ �����߽��ϴ�.");
		return;
	}

}

void URenderer::InputAssembler1Setting()
{
	UINT VertexSize = sizeof(VertexData);
	UINT Offset = 0; // ���° �������� �����Ұ���?

	// 0�� ���Կ� VertexBuffer�� �����ϰ� 1���� ���Ը� ����Ͽ�, GPU�� Offest ���� �ε������� VertexSize ������ �޸𸮸� ���� �� �ֵ��� �Ѵ�.
	Context->IASetVertexBuffers(0, 1, &VertexBuffer, &VertexSize, &Offset);
	Context->IASetInputLayout(InputLayout); // InputLayout�� ���� �������� �ǹ�(��ġ, UV, ����)�� ����
}

void URenderer::InputAssembler2Setting()
{
	int Offset = 0;
	Context->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, Offset); // ���� �ε��� ���� ����
	Context->IASetPrimitiveTopology(Topology); // � ���·� �׸��ų�
}

void URenderer::VertexShaderSetting()
{
	Context->VSSetShader(VertexShader, nullptr, 0);
}

void URenderer::RasterizerSetting()
{
	Context->RSSetViewports(1, &ViewportInfo); // ����Ʈ�� 1��
	Context->RSSetState(RasterizerState);
}

void URenderer::PixelShaderSetting()
{
	Context->PSSetShader(PixelShader, nullptr, 0);
}

void URenderer::OutputMergeSetting()
{
	ID3D11RenderTargetView* RenderTargetView = RTV; // RTV�� ����۸� ���� Ÿ������ ���ߴ�.
	Context->OMSetRenderTargets(1, &RenderTargetView, nullptr); // ������ �����͸� �׸� Ÿ���� ����
}

void URenderer::ShaderResInit()
{

}

void URenderer::ShaderResSetting()
{
}

URenderer::URenderer()
{
}

URenderer::~URenderer()
{
	Release();
}

void URenderer::Release()
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
}