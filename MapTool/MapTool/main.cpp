#include "main.h"


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG msg;
	WNDCLASSEX WndClassEx;
	g_hInst = hInstance;


	
	WndClassEx.cbClsExtra = 0;
	WndClassEx.cbSize = sizeof(WndClassEx);
	WndClassEx.cbWndExtra = 0;
	WndClassEx.hbrBackground = (HBRUSH)GetStockObject(COLOR_WINDOW + 1);
	WndClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClassEx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClassEx.hIconSm = NULL;
	WndClassEx.hInstance = hInstance;
	WndClassEx.lpfnWndProc = WndProc;
	WndClassEx.lpszClassName = lpszClass;
	WndClassEx.lpszMenuName = NULL;
	WndClassEx.style = CS_HREDRAW | CS_VREDRAW;
	
	RegisterClassEx(&WndClassEx);

	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, (HMENU)NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);

	g_hWnd = hWnd;

	InitDevice();
	CreateShader();
	CreateVertexBuffer();
	CreateIndexBuffer();
	CreateConstantBuffer();

	CreateRenderState();


	LoadTexture();
	InitMatrix();
	
	LoadHeightMap();
	CreateHeightMapVB();
	CreateHeightMapIB();

	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				CleanupDevice();
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			static float deltaTime = 0;
			deltaTime += 0.0005f;
			Render(deltaTime);
		}
	}


	CleanupDevice();

	return (int)msg.wParam;
	
}


LRESULT CALLBACK WndProc(HWND hWnd,UINT iMessage,WPARAM wParam, LPARAM lParam)
{
	switch (iMessage)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		break;

		case WM_KEYDOWN:
		{
			switch (wParam)
			{
				case VK_UP:
				{
					if (GetAsyncKeyState(VK_SHIFT))
					{
						g_Camera.Walk(m_MoveSpeed);
					}
					else 
					{
						g_Camera.VerticalMove(m_MoveSpeed);
					}
					break;
				}
				case VK_DOWN:
				{
					if (GetAsyncKeyState(VK_SHIFT))
					{
						g_Camera.Walk(-m_MoveSpeed);
					}
					else
					{
						g_Camera.VerticalMove(-m_MoveSpeed);
					}
					break;
				}
				case VK_LEFT:
				{
					g_Camera.HorizontalMove(-m_MoveSpeed);
					break;
				}
				case VK_RIGHT:
				{
					g_Camera.HorizontalMove(m_MoveSpeed);
					break;
				}

				
			}
			break;
		}
		
		case WM_MOUSEMOVE:
		{
			float x = GET_X_LPARAM(lParam);
			float y = GET_Y_LPARAM(lParam);

			if (GetAsyncKeyState(MK_RBUTTON))
			{
				float dx = XMConvertToRadians(m_RotateSpeed * static_cast<float>(x - m_LastMouseX));
				float dy = XMConvertToRadians(m_RotateSpeed * static_cast<float>(y - m_LastMouseY));

				g_Camera.Pitch(dy);
				g_Camera.RotateY(dx);
			}

			m_LastMouseX = x;
			m_LastMouseY = y;
		
			break;
		}

	}


	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void CreateDepthStencilTexture()
{
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));

	descDepth.Width = 800;
	descDepth.Height = 600;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	g_pd3dDevice->CreateTexture2D(&descDepth, NULL, &g_pDepthStencil);




	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));

	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	descDSV.Flags = 0;

	g_pd3dDevice->CreateDepthStencilView(g_pDepthStencil, &descDSV, &g_pDepthStencilView);

	g_pDepthStencil->Release();
}

HRESULT InitDevice()
{
	HRESULT hr = S_OK;

	// Flag 설정
	UINT createDeviceFlags = 0;

#ifdef _DEBUG
	createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);


	// SWAP CHAIN

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;

	sd.BufferDesc.Width = 800;
	sd.BufferDesc.Height = 600;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = g_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;


	hr = D3D11CreateDeviceAndSwapChain(
		0,
		D3D_DRIVER_TYPE_HARDWARE,
		0,
		createDeviceFlags,
		featureLevels,
		numFeatureLevels,
		D3D11_SDK_VERSION,
		&sd,
		&g_pSwapChain,
		&g_pd3dDevice,
		&g_featureLevel,
		&g_pImmediateContext);

	if (FAILED(hr))
		return hr;


	// RENDER TARGET VIEW

	ID3D11Texture2D*	pBackBuffer = NULL;
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	if (FAILED(hr))
		return hr;

	hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTargetView);
	pBackBuffer->Release();

	if (FAILED(hr))
		return hr;

	CreateDepthStencilTexture();
	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);


	D3D11_VIEWPORT vp;
	vp.Width = 800;
	vp.Height = 600;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_pImmediateContext->RSSetViewports(1, &vp);

	return hr;
}

void CleanupDevice()
{
	if (g_pImmediateContext)				g_pImmediateContext->ClearState();

	if (g_pSamplerLinear)					g_pSamplerLinear->Release();
	if (g_pTextureRV)						g_pTextureRV->Release();

	//if (g_pWireframeRS)						g_pWireframeRS->Release();
	if (g_pSolidRS)							g_pSolidRS->Release();
	if (g_pDepthStencilView)				g_pDepthStencilView->Release();
	if (g_pConstantBuffer)					g_pConstantBuffer->Release();
	if (g_pIndexBuffer)						g_pIndexBuffer->Release();
	if (g_pVertexBuffer)					g_pVertexBuffer->Release();
	if (g_pVertexLayout)					g_pVertexLayout->Release();
	if (g_pVertexShader)					g_pVertexShader->Release();
	if (g_pPixelShader)						g_pPixelShader->Release();
	if (g_pRenderTargetView)				g_pRenderTargetView->Release();
	if (g_pImmediateContext)				g_pImmediateContext->Release();
	if (g_pd3dDevice)						g_pd3dDevice->Release();
	if (g_pSwapChain)						g_pSwapChain->Release();
}


HRESULT CreateShader()
{
	ID3DBlob *pErrorBlob = NULL;
	ID3DBlob *pVSBlob = NULL;
	HRESULT hr = D3DCompileFromFile(L"Shader.fx", 0, 0,
		"VS", "vs_5_0",
		0, 0,
		&pVSBlob, &pErrorBlob);

	if (FAILED(hr))
		return hr;

	hr = g_pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), 0, &g_pVertexShader);

	if (FAILED(hr))
		return hr;

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 	D3D11_INPUT_PER_VERTEX_DATA, 0 },

	};


	UINT numElements = ARRAYSIZE(layout);

	hr = g_pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &g_pVertexLayout);
	if (FAILED(hr))
		return hr;

	pVSBlob->Release();


	ID3DBlob* pPSBlob = NULL;
	hr = D3DCompileFromFile(L"Shader.fx", 0, 0,
		"PS", "ps_5_0",
		0, 0,
		&pPSBlob, &pErrorBlob);

	if (FAILED(hr))
		return hr;

	hr = g_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), 0, &g_pPixelShader);

	if (FAILED(hr))
		return hr;

	pPSBlob->Release();
}

void CreateVertexBuffer()
{
	MyVertex vertices[] =
	{
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f),  XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT3(-0.33f,  0.33f, -0.33f), XMFLOAT2(1.0f,1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f),   XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT3(0.33f,  0.33f, -0.33f) , XMFLOAT2(0.0f,1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f),    XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.33f,  0.33f,  0.33f) , XMFLOAT2(0.0f,0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f),   XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT3(-0.33f,  0.33f,  0.33f), XMFLOAT2(1.0f,0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f), XMFLOAT3(-0.33f, -0.33f, -0.33f), XMFLOAT2(0.0f,0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f),  XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f), XMFLOAT3(0.33f, -0.33f, -0.33f) , XMFLOAT2(1.0f,0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f),   XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.33f, -0.33f,  0.33f) , XMFLOAT2(1.0f,1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f),  XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT3(-0.33f, -0.33f,  0.33f), XMFLOAT2(0.0f,1.0f) },
	};



	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.ByteWidth = sizeof(vertices);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = vertices;

	g_pd3dDevice->CreateBuffer(&bd, &initData, &g_pVertexBuffer);
}

void CreateIndexBuffer()
{
	UINT indices[]
	{
		3,1,0,
		2,1,3,
		0,5,4,
		1,5,0,
		3,4,7,
		0,4,3,
		1,6,5,
		2,6,1,
		2,7,6,
		3,7,2,
		6,4,5,
		7,4,6,
	};

	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));

	ibd.ByteWidth = sizeof(indices);
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA iInitData;
	ZeroMemory(&iInitData, sizeof(iInitData));
	iInitData.pSysMem = indices;

	g_pd3dDevice->CreateBuffer(&ibd, &iInitData, &g_pIndexBuffer);

}

void InitMatrix()
{
	// world 초기화
	g_World = XMMatrixIdentity();
	// view 구성
	/*
	XMVECTOR	pos = XMVectorSet(-20.0f, 45.0f, 10.0f, 1.0f);
	XMVECTOR	target = XMVectorSet(50.0f, 0.0f, 50.0f, 0.0f);
	XMVECTOR	up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	g_View = XMMatrixLookAtLH(pos, target, up);



	// projection
	g_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, 800.0f / (float)600.0f, 0.01f, 1000.0f);
	*/
}

void CreateConstantBuffer()
{
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));

	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.ByteWidth = sizeof(ConstantBuffer);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = 0;

	g_pd3dDevice->CreateBuffer(&cbd, NULL, &g_pConstantBuffer);
}

void CreateRenderState()
{
	D3D11_RASTERIZER_DESC	rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));

	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.FrontCounterClockwise = false;

	g_pd3dDevice->CreateRasterizerState(&rasterizerDesc, &g_pSolidRS);

	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	g_pd3dDevice->CreateRasterizerState(&rasterizerDesc, &g_pWireframeRS);
}

HRESULT LoadTexture()
{
	DirectX::ScratchImage Image = DirectX::LoadTextureFromFile("Texture/heightMap.jpg");
	HRESULT hr = DirectX::CreateShaderResourceView(g_pd3dDevice, Image.GetImages(), Image.GetImageCount(), Image.GetMetadata(), &g_pTextureRV);

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));

	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = g_pd3dDevice->CreateSamplerState(&sampDesc, &g_pSamplerLinear);

	if (FAILED(hr))
		return hr;
}

void CreateHeightMapVB()
{
	MyVertex * heightMapVertex = new MyVertex[numVertices];

	for (int z = 0; z < vertexCount; z++)
	{
		for (int x = 0; x < vertexCount; x++)
		{
			int idx = x + (z* (vertexCount));
			heightMapVertex[idx].pos = XMFLOAT3(x, 0, z);
			heightMapVertex[idx].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			heightMapVertex[idx].tex = XMFLOAT2(x / (float)(vertexCount - 1), z / (float)(vertexCount - 1));
		}
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.ByteWidth = sizeof(MyVertex) * numVertices;
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = heightMapVertex;

	g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pHeightMapVertexBuffer);


}

void CreateHeightMapIB()
{
	int triangleCount = (vertexCount - 1) * (vertexCount - 1) * 2;
	indexSize = triangleCount * 3;
	UINT* indices = new UINT[indexSize];

	int baseIndex = 0;
	int _numVertsPerRow = vertexCount;

	for (int z = 0; z < _numVertsPerRow - 1; z++)
	{
		for (int x = 0; x < _numVertsPerRow - 1; x++)
		{

			indices[baseIndex + 0] = (z + 0) * _numVertsPerRow + x + 0;
			indices[baseIndex + 2] = (z + 0) * _numVertsPerRow + x + 1;
			indices[baseIndex + 1] = (z + 1) * _numVertsPerRow + x + 0;
			indices[baseIndex + 3] = (z + 1) * _numVertsPerRow + x + 0;
			indices[baseIndex + 5] = (z + 0) * _numVertsPerRow + x + 1;
			indices[baseIndex + 4] = (z + 1) * _numVertsPerRow + x + 1;

			baseIndex += 6;
		}
	}

	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.ByteWidth = sizeof(UINT) * indexSize;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA iInitData;
	ZeroMemory(&iInitData, sizeof(iInitData));
	iInitData.pSysMem = indices;

	g_pd3dDevice->CreateBuffer(&ibd, &iInitData, &g_pHeightMapIndexBuffer);
}

void CalcMatrixForHeightMap(float deltaTime)
{

	XMMATRIX mat = XMMatrixRotationY(0.0f);
	g_World = mat;

	XMMATRIX wvp = g_World * g_Camera.GetViewProjection();
	//XMMATRIX wvp = g_World * g_View * g_Projection;

	ConstantBuffer cb;
	cb.wvp = XMMatrixTranspose(wvp);
	cb.world = XMMatrixTranspose(g_World);
	cb.lightDir = lightDirection;
	cb.lightColor = lightColor;

	g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, 0, &cb, 0, 0); // update data
	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);// set constant buffer.
	g_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pConstantBuffer);



}

void LoadHeightMap()
{
	std::vector<BYTE> input(numVertices);
	std::ifstream loadFile;

	loadFile.open(L"Texture/heightMap.raw", std::ios_base::binary);

	if (loadFile)
	{
		loadFile.read((char*)&input[0], (std::streamsize)input.size());

		loadFile.close();
	}

	_heightMap.resize(numVertices);

	for (int i = 0; i < input.size(); i++)
	{
		_heightMap[i] = input[i];
	}
}

void Render(float deltaTime)
{

	g_Camera.UpdateViewMatrix();
	float clearColor[4] = { 0.125f,0.125f,0.125f,1.0f };
	g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, clearColor);
	g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	g_pImmediateContext->IASetInputLayout(g_pVertexLayout);
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	g_pImmediateContext->RSSetState(g_pSolidRS);

	UINT stride = sizeof(MyVertex);
	UINT offset = 0;

	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pHeightMapVertexBuffer, &stride, &offset);
	g_pImmediateContext->IASetIndexBuffer(g_pHeightMapIndexBuffer, DXGI_FORMAT_R32_UINT, 0);



	g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
	g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);

	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureRV);
	g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);


	CalcMatrixForHeightMap(deltaTime);

	g_pImmediateContext->DrawIndexed(indexSize, 0, 0);



	g_pSwapChain->Present(0, 0);
}