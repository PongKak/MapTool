#pragma once

#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "DirectXTex.h"
#include <vector>
#include <fstream>
#include "camera.h"
#include <windowsx.h>

#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3d11.lib")

using namespace DirectX;

HWND						g_hWnd = NULL;
IDXGISwapChain*				g_pSwapChain = NULL;
ID3D11Device*				g_pd3dDevice = NULL;
ID3D11DeviceContext*		g_pImmediateContext = NULL;
ID3D11RenderTargetView*		g_pRenderTargetView = NULL;

ID3D11VertexShader*			g_pVertexShader = NULL;
ID3D11InputLayout*			g_pVertexLayout = NULL;
ID3D11Buffer*				g_pVertexBuffer = NULL;

ID3D11PixelShader*			g_pPixelShader = NULL;

ID3D11Buffer*				g_pIndexBuffer = NULL;

D3D_FEATURE_LEVEL			g_featureLevel = D3D_FEATURE_LEVEL_11_0;

XMMATRIX					g_World;

ID3D11Buffer*				g_pConstantBuffer = NULL; // 상수 버퍼

ID3D11Texture2D*			g_pDepthStencil = NULL; // 스텐실 버퍼
ID3D11DepthStencilView*		g_pDepthStencilView = NULL;

ID3D11RasterizerState*		g_pSolidRS;
ID3D11RasterizerState*		g_pWireframeRS;

ID3D11ShaderResourceView*	g_pTextureRV = NULL; // 텍스쳐
ID3D11SamplerState*			g_pSamplerLinear = NULL;

ID3D11Buffer*				g_pHeightMapVertexBuffer = NULL;
ID3D11Buffer*				g_pHeightMapIndexBuffer = NULL;

Camera						g_Camera;
float						m_MoveSpeed = 3.0f;
float						m_RotateSpeed = 0.2f;

float						m_LastMouseX = 0.0f;
float						m_LastMouseY = 0.0f;
struct MyVertex
{
	XMFLOAT3     pos;
	XMFLOAT4     color;

	XMFLOAT3     normal;
	XMFLOAT2	 tex;
};


struct ConstantBuffer
{
	XMMATRIX     wvp;
	XMMATRIX     world;

	XMFLOAT4     lightDir;
	XMFLOAT4     lightColor;
};

int vertexCount = 257;
int numVertices = vertexCount * vertexCount;
int indexSize = 0;

std::vector<int> _heightMap;


HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("MapTool");

XMFLOAT4 lightDirection = 
{ 
	XMFLOAT4(1.0f,1.0f,1.0f,1.0f), 
};

XMFLOAT4 lightColor =
{
	XMFLOAT4(1.0f,1.0f,0.0f,1.0f),
};



LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);



void CreateDepthStencilTexture();


HRESULT InitDevice();

void CleanupDevice();


HRESULT CreateShader();

void CreateVertexBuffer();

void CreateIndexBuffer();

void InitMatrix();

void CreateConstantBuffer();

void CreateRenderState();

HRESULT LoadTexture();

void CreateHeightMapVB();

void CreateHeightMapIB();

void CalcMatrixForHeightMap(float deltaTime);

void LoadHeightMap();

void Render(float deltaTime);
