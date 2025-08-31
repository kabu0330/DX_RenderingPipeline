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


	// 디버깅
	ID3D11DeviceContext* Context = UCore::GraphicsDevice.Context;
	ID3D11RenderTargetView* CurrentRTV = nullptr;
	Context->OMGetRenderTargets(1, &CurrentRTV, nullptr);
	assert(CurrentRTV != nullptr);
	if (CurrentRTV)
	{
		CurrentRTV->Release();
	}

	// 인덱스 버퍼를 통해서 그리겠다.
	UCore::GraphicsDevice.Context->DrawIndexed(6, 0, 0);
}

void URenderer::ShaderResInit()
{
	D3D11_BUFFER_DESC BufferInfo = { 0, };
	BufferInfo.ByteWidth = sizeof(FTransform);
	BufferInfo.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // 상수 버퍼로 사용하겠다.
	BufferInfo.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE; // CPU 쓰기
	BufferInfo.Usage = D3D11_USAGE_DYNAMIC; // CPU가 동적으로 버퍼 업데이트, GPU 읽기 전용

	if (S_OK != UCore::GraphicsDevice.Device->CreateBuffer(&BufferInfo, nullptr, &TransformConstBuffer))
	{
		MSGASSERT("상수 버퍼 생성에 실패했습니다.");
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
	BufferInfo.BindFlags = D3D11_BIND_VERTEX_BUFFER; // 버텍스 버퍼로 사용
	BufferInfo.CPUAccessFlags = 0;                   // CPU 접근 불허
	BufferInfo.Usage = D3D11_USAGE_DEFAULT;          // GPU에서만 사용

	D3D11_SUBRESOURCE_DATA Data; // 정점의 시작주소 넣어주는 구조체
	Data.pSysMem = &Vertices[0];

	HRESULT HR = UCore::GraphicsDevice.Device->CreateBuffer(&BufferInfo, &Data, &VertexBuffer);
	if (S_OK != HR)
	{
		MessageBoxA(nullptr, "버텍스 버퍼 생성에 실패했습니다.", "Error", MB_OK);
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
	IndexBufferInfo.BindFlags = D3D11_BIND_INDEX_BUFFER; // 인덱스 버퍼로 사용
	IndexBufferInfo.CPUAccessFlags = 0;                  // CPU 접근 불허
	IndexBufferInfo.Usage = D3D11_USAGE_DEFAULT;         // GPU만 사용

	D3D11_SUBRESOURCE_DATA Data;
	Data.pSysMem = &Indices[0];

	HRESULT HR = UCore::GraphicsDevice.Device->CreateBuffer(&IndexBufferInfo, &Data, &IndexBuffer);
	if (S_OK != HR)
	{
		MSGASSERT("인덱스 버퍼 생성에 실패했습니다.");
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
	Flag0 |= D3DCOMPILE_PACK_MATRIX_ROW_MAJOR; // 행렬은 행기준

	D3DCompileFromFile(
		WPath.c_str(),
		nullptr,
		nullptr,
		"VertexToWorld", // 셰이더 파일 진입 함수
		Version.c_str(),
		Flag0,
		Flag1,
		&VSShaderCodeBlob,
		&VSErrorCodeBlob
	);

	if (nullptr == VSShaderCodeBlob)
	{
		std::string ErrString = reinterpret_cast<char*>(VSErrorCodeBlob->GetBufferPointer());
		MSGASSERT("버텍스 셰이더 초기 설정에 실패했습니다.\n" + ErrString);
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
		MSGASSERT("버텍스 셰이더 생성에 실패했습니다.");
		return;
	}

	InputAssembler1Layout();
}

void URenderer::InputAssembler1Layout()
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> InputLayoutData;

	D3D11_INPUT_ELEMENT_DESC PositionDesc;
	PositionDesc.SemanticName = "POSITION";
	PositionDesc.AlignedByteOffset = 0; // 해당 데이터 시작 바이트 (0 + sizeof(float4))
	PositionDesc.InputSlot = 0;
	PositionDesc.SemanticIndex = 0;
	PositionDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	PositionDesc.InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	PositionDesc.InstanceDataStepRate = 0;
	InputLayoutData.push_back(PositionDesc);


	//D3D11_INPUT_ELEMENT_DESC UVDesc;
	//UVDesc.SemanticName = "TEXCOORD";
	//UVDesc.AlignedByteOffset = 16; // 해당 데이터 시작 바이트 (0 + sizeof(float4))
	//UVDesc.InputSlot = 0;
	//UVDesc.SemanticIndex = 0;
	//UVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	//UVDesc.InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	//UVDesc.InstanceDataStepRate = 0;
	//InputLayoutData.push_back(UVDesc);


	D3D11_INPUT_ELEMENT_DESC ColorDesc;
	ColorDesc.SemanticName = "COLOR";
	ColorDesc.AlignedByteOffset = 16; // 해당 데이터 시작 바이트 (0 + sizeof(float4))
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
		MessageBoxA(nullptr, "인풋 레이아웃 생성에 실패했습니다.", "Error", MB_OK);
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
	Flag0 |= D3DCOMPILE_PACK_MATRIX_ROW_MAJOR; // 행렬은 행기준

	D3DCompileFromFile(
		WPath.c_str(),
		nullptr,
		nullptr,
		"PixelToWorld", // 셰이더 파일 진입 함수
		Version.c_str(),
		Flag0,
		Flag1,
		&PSShaderCodeBlob,
		&PSErrorCodeBlob
	);

	if (nullptr == PSShaderCodeBlob)
	{
		std::string ErrString = reinterpret_cast<char*>(PSErrorCodeBlob->GetBufferPointer());
		MSGASSERT("픽셀 셰이더 초기 설정에 실패했습니다.\n" + ErrString);
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
		MSGASSERT("픽셀 셰이더 생성에 실패했습니다.");
		return;
	}

}

void URenderer::InputAssembler1Setting()
{
	UINT VertexSize = sizeof(VertexData);
	UINT Offset = 0; // 몇번째 정점부터 세팅할건지?

	// 0번 슬롯에 VertexBuffer를 연결하고 1개의 슬롯만 사용하여, GPU가 Offest 정점 인덱스부터 VertexSize 단위로 메모리를 읽을 수 있도록 한다.
	UCore::GraphicsDevice.Context->IASetVertexBuffers(0, 1, &VertexBuffer, &VertexSize, &Offset);
	UCore::GraphicsDevice.Context->IASetInputLayout(InputLayout); // InputLayout를 통해 데이터의 의미(위치, UV, 색상)를 지정
}

void URenderer::InputAssembler2Setting()
{
	int Offset = 0;
	UCore::GraphicsDevice.Context->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, Offset); // 정점 인덱스 정보 제공
	UCore::GraphicsDevice.Context->IASetPrimitiveTopology(Topology); // 어떤 형태로 그릴거냐
}

void URenderer::VertexShaderSetting()
{
	UCore::GraphicsDevice.Context->VSSetShader(VertexShader, nullptr, 0);
}

void URenderer::RasterizerSetting()
{
	UCore::GraphicsDevice.Context->RSSetViewports(1, &ViewportInfo); // 뷰포트는 1개
	UCore::GraphicsDevice.Context->RSSetState(RasterizerState);
}

void URenderer::PixelShaderSetting()
{
	UCore::GraphicsDevice.Context->PSSetShader(PixelShader, nullptr, 0);
}

void URenderer::OutputMergeSetting()
{
	ID3D11RenderTargetView* RenderTargetView = UCore::GraphicsDevice.RTV; // RTV는 백버퍼를 렌더 타겟으로 정했다.
	UCore::GraphicsDevice.Context->OMSetRenderTargets(1, &RenderTargetView, nullptr); // 렌더링 데이터를 그릴 타겟이 어디냐
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
	FTransform& RendererTrans = GetTransformRef(); // 수정할 데이터

	D3D11_MAPPED_SUBRESOURCE Data = {}; 

	UCore::GraphicsDevice.Context->Map( // Map : GPU 메모리를 CPU에서 조작할 수 있는 포인터 제공
		TransformConstBuffer,    // GPU 메모리 버퍼
		0,                       // 서브 리소스 인덱스
		D3D11_MAP_WRITE_DISCARD, // 이전 데이터는 버리고 새 데이터로 교체
		0,                       // 플래그
		&Data					 // GPU 메모리의 정보를 담을 구조체
	);

	if (nullptr == Data.pData) // pData : GPU 메모리 버퍼 접근용 포인터
	{
		MSGASSERT("그래픽 카드 매핑에 실패했습니다. (Map returned nullptr)");
		return;
	}
	
	memcpy_s(
		Data.pData,			// 복사받을 데이터(Ctrl + V)
		sizeof(FTransform), // 복사받을 메모리 크기 == 복사할 메모리 크기
		&RendererTrans,		// 복사할 데이터(Ctrl + C)
		sizeof(FTransform)  // 복사할 메모리 크기 
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
