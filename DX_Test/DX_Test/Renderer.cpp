#include "PreCompile.h"
#include "Renderer.h"
#include <assert.h>

void URenderer::BeginPlay()
{
	// 2. Rendering Pipeline
	ShaderResInit();

	InputAssembler1Init();
	VertexShaderInit();
	InputAssembler2Init();
	RasterizerInit();
	PixelShaderInit();
}

void URenderer::Render(float _DeltaTime)
{
	// 2. Rendering Pipeline
	WorldViewProjection();
	ShaderResSetting();

	InputAssembler1Setting();
	VertexShaderSetting();
	InputAssembler2Setting();
	RasterizerSetting();
	PixelShaderSetting();
	OutputMergeSetting();


	// �����
	ID3D11DeviceContext* Context = UCore::GraphicsDevice.Context;
	ID3D11RenderTargetView* CurrentRTV = nullptr;
	Context->OMGetRenderTargets(1, &CurrentRTV, nullptr);
	assert(CurrentRTV != nullptr);
	if (CurrentRTV)
	{
		CurrentRTV->Release();
	}

	// �ε��� ���۸� ���ؼ� �׸��ڴ�.
	UCore::GraphicsDevice.Context->DrawIndexed(6, 0, 0);
}

void URenderer::ShaderResInit()
{
	D3D11_BUFFER_DESC BufferInfo = { 0, };
	BufferInfo.ByteWidth = sizeof(FTransform);
	BufferInfo.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // ��� ���۷� ����ϰڴ�.
	BufferInfo.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE; // CPU ����
	BufferInfo.Usage = D3D11_USAGE_DYNAMIC; // CPU�� �������� ���� ������Ʈ, GPU �б� ����

	if (S_OK != UCore::GraphicsDevice.Device->CreateBuffer(&BufferInfo, nullptr, &TransformConstBuffer))
	{
		MSGASSERT("��� ���� ������ �����߽��ϴ�.");
		return;
	}
}

void URenderer::InputAssembler1Init()
{
	std::vector<VertexData> Vertices;
	Vertices.resize(4);

	Vertices[0] = VertexData{ FVector(-0.5f,  0.5f, 0.0f), /*{0.0f, 0.0f},*/ {1.0f, 0.0f, 0.0f, 1.0f} };
	Vertices[1] = VertexData{ FVector(0.5f,  0.5f, 0.0f), /*{1.0f, 0.0f},*/ {1.0f, 0.0f, 0.0f, 1.0f} };
	Vertices[2] = VertexData{ FVector(-0.5f, -0.5f, 0.0f), /*{0.0f, 1.0f},*/ {1.0f, 0.0f, 0.0f, 1.0f} };
	Vertices[3] = VertexData{ FVector(0.5f, -0.5f, 0.0f), /*{1.0f, 1.0f},*/ {1.0f, 0.0f, 0.0f, 1.0f} };
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

	HRESULT HR = UCore::GraphicsDevice.Device->CreateBuffer(&BufferInfo, &Data, &VertexBuffer);
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

	//Indices.push_back(0);
	//Indices.push_back(1);
	//Indices.push_back(2);

	//Indices.push_back(1);
	//Indices.push_back(3);
	//Indices.push_back(2);

	Indices[0] = 0;
	Indices[1] = 1;
	Indices[2] = 2;

	Indices[3] = 1;
	Indices[4] = 3;
	Indices[5] = 2;

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

	HRESULT HR = UCore::GraphicsDevice.Device->CreateBuffer(&IndexBufferInfo, &Data, &IndexBuffer);
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

	HRESULT Result = UCore::GraphicsDevice.Device->CreateVertexShader(
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


	//D3D11_INPUT_ELEMENT_DESC UVDesc;
	//UVDesc.SemanticName = "TEXCOORD";
	//UVDesc.AlignedByteOffset = 16; // �ش� ������ ���� ����Ʈ (0 + sizeof(float4))
	//UVDesc.InputSlot = 0;
	//UVDesc.SemanticIndex = 0;
	//UVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	//UVDesc.InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	//UVDesc.InstanceDataStepRate = 0;
	//InputLayoutData.push_back(UVDesc);


	D3D11_INPUT_ELEMENT_DESC ColorDesc;
	ColorDesc.SemanticName = "COLOR";
	ColorDesc.AlignedByteOffset = 16; // �ش� ������ ���� ����Ʈ (0 + sizeof(float4))
	ColorDesc.InputSlot = 0;
	ColorDesc.SemanticIndex = 0;
	ColorDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	ColorDesc.InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	ColorDesc.InstanceDataStepRate = 0;
	InputLayoutData.push_back(ColorDesc);


	HRESULT HR = UCore::GraphicsDevice.Device->CreateInputLayout(
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
	UCore::GraphicsDevice.Device->CreateRasterizerState(&RasterizerDesc, &RasterizerState);

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

	HRESULT Result = UCore::GraphicsDevice.Device->CreatePixelShader(
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
	UCore::GraphicsDevice.Context->IASetVertexBuffers(0, 1, &VertexBuffer, &VertexSize, &Offset);
	UCore::GraphicsDevice.Context->IASetInputLayout(InputLayout); // InputLayout�� ���� �������� �ǹ�(��ġ, UV, ����)�� ����
}

void URenderer::InputAssembler2Setting()
{
	int Offset = 0;
	UCore::GraphicsDevice.Context->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, Offset); // ���� �ε��� ���� ����
	UCore::GraphicsDevice.Context->IASetPrimitiveTopology(Topology); // � ���·� �׸��ų�
}

void URenderer::VertexShaderSetting()
{
	UCore::GraphicsDevice.Context->VSSetShader(VertexShader, nullptr, 0);
}

void URenderer::RasterizerSetting()
{
	UCore::GraphicsDevice.Context->RSSetViewports(1, &ViewportInfo); // ����Ʈ�� 1��
	UCore::GraphicsDevice.Context->RSSetState(RasterizerState);
}

void URenderer::PixelShaderSetting()
{
	UCore::GraphicsDevice.Context->PSSetShader(PixelShader, nullptr, 0);
}

void URenderer::OutputMergeSetting()
{
	ID3D11RenderTargetView* RenderTargetView = UCore::GraphicsDevice.RTV; // RTV�� ����۸� ���� Ÿ������ ���ߴ�.
	UCore::GraphicsDevice.Context->OMSetRenderTargets(1, &RenderTargetView, nullptr); // ������ �����͸� �׸� Ÿ���� ����
}

void URenderer::WorldViewProjection()
{
	FTransform& RendererTrans = GetTransformRef();
	FVector CameraPos = { 0.0f, 0.0f, -1000.0f, 1.0f };
	FTransform CameraMatrix;
	CameraMatrix.Location = CameraPos;
	RendererTrans.View = CameraMatrix.View;
	RendererTrans.Projection = CameraMatrix.Projection;

	RendererTrans.WVP = RendererTrans.World * RendererTrans.View * RendererTrans.Projection;
}

void URenderer::ShaderResSetting()
{
	FTransform& RendererTrans = GetTransformRef(); // ������ ������

	D3D11_MAPPED_SUBRESOURCE Data = {}; 

	UCore::GraphicsDevice.Context->Map( // Map : GPU �޸𸮸� CPU���� ������ �� �ִ� ������ ����
		TransformConstBuffer,    // GPU �޸� ����
		0,                       // ���� ���ҽ� �ε���
		D3D11_MAP_WRITE_DISCARD, // ���� �����ʹ� ������ �� �����ͷ� ��ü
		0,                       // �÷���
		&Data					 // GPU �޸��� ������ ���� ����ü
	);

	if (nullptr == Data.pData) // pData : GPU �޸� ���� ���ٿ� ������
	{
		MSGASSERT("�׷��� ī�� ���ο� �����߽��ϴ�. (Map returned nullptr)");
		return;
	}
	
	memcpy_s(
		Data.pData,			// ������� ������(Ctrl + V)
		sizeof(FTransform), // ������� �޸� ũ�� == ������ �޸� ũ��
		&RendererTrans,		// ������ ������(Ctrl + C)
		sizeof(FTransform)  // ������ �޸� ũ�� 
	);

	UCore::GraphicsDevice.Context->Unmap(TransformConstBuffer, 0);

	UCore::GraphicsDevice.Context->VSSetConstantBuffers(0, 1, &TransformConstBuffer);

}

void URenderer::Release()
{
	if (nullptr != SamplerState)
	{
		SamplerState->Release();
		SamplerState = nullptr;
	}

	if (nullptr != TransformConstBuffer)
	{
		TransformConstBuffer->Release();
		TransformConstBuffer = nullptr;
	}

	if (nullptr != VertexBuffer)
	{
		VertexBuffer->Release();
		VertexBuffer = nullptr;
	}

	if (nullptr != InputLayout)
	{
		InputLayout->Release();
		InputLayout = nullptr;
	}

	if (nullptr != IndexBuffer)
	{
		IndexBuffer->Release();
		IndexBuffer = nullptr;
	}

	if (nullptr != VSShaderCodeBlob)
	{
		VSShaderCodeBlob->Release();
		VSShaderCodeBlob = nullptr;
	}

	if (nullptr != VSErrorCodeBlob)
	{
		VSErrorCodeBlob->Release();
		VSErrorCodeBlob = nullptr;
	}

	if (nullptr != VertexShader)
	{
		VertexShader->Release();
		VertexShader = nullptr;
	}

	if (nullptr != RasterizerState)
	{
		RasterizerState->Release();
		RasterizerState = nullptr;
	}

	if (nullptr != PSShaderCodeBlob)
	{
		PSShaderCodeBlob->Release();
		PSShaderCodeBlob = nullptr;
	}

	if (nullptr != PSErrorCodeBlob)
	{
		PSErrorCodeBlob->Release();
		PSErrorCodeBlob = nullptr;
	}

	if (nullptr != PixelShader)
	{
		PixelShader->Release();
		PixelShader = nullptr;
	}
}

URenderer::URenderer()
{
}

URenderer::~URenderer()
{
}
