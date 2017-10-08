#include "Util.h"


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("MapTool");


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