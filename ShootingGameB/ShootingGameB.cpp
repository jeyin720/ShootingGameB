// ShootingGameB.cpp: 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "ShootingGameB.h"
#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int,HWND&);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

LPDIRECT3D9 g_pD3D;//Direct의 다양한 기능을 담고있는 메모리 (본체 느낌~) 
LPDIRECT3DDEVICE9 g_pD3DDevice;//실질적인 화면 Device (보여지는 화면)
D3DCOLOR g_ClearColor = D3DCOLOR_XRGB(0, 0, 255);

void Render(LPD3DXSPRITE m_Sprite, LPDIRECT3DTEXTURE9 m_Texture);
bool InitDirect3D(HWND hwnd);
void ReleaseDirect3D();


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SHOOTINGGAMEB, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

	HWND hWnd = NULL;
    // 응용 프로그램 초기화를 수행합니다.
    if (!InitInstance (hInstance, nCmdShow, hWnd))
    {
        return FALSE;
    }

	InitDirect3D(hWnd);

	LPD3DXSPRITE m_Sprite;
	LPDIRECT3DTEXTURE9 m_Texture;
	D3DXCreateSprite(g_pD3DDevice, &m_Sprite);

	D3DXCreateTextureFromFileEx(g_pD3DDevice, _T("Airplane.bmp"), 0, 0, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, D3DX_FILTER_NONE, D3DX_DEFAULT, D3DCOLOR_XRGB(0, 0, 0), NULL, NULL, &m_Texture);

	

    MSG msg;
	ZeroMemory(&msg, sizeof(msg));

    // 기본 메시지 루프입니다.
	while (msg.message != WM_QUIT)
	{
		// getMessage는 message가 안들어오면 대기 peekMessage는 들어가면 끌어오고 안들어오면 넘어감
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			Render(m_Sprite,m_Texture);
		}
	}

	if (m_Texture != NULL)
	{
		m_Texture->Release();
		m_Texture = NULL;
	}
	if (m_Sprite != NULL)
	{
		m_Sprite->Release();
		m_Sprite = NULL;
	}
	ReleaseDirect3D();
    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SHOOTINGGAMEB));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow,HWND& hWnd)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      0, 0, 640, 480, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
void Render(LPD3DXSPRITE m_Sprite,LPDIRECT3DTEXTURE9 m_Texture)
{
	if (g_pD3DDevice == NULL)
		return;
	g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, g_ClearColor, 1.0f, 0);
	if (SUCCEEDED(g_pD3DDevice->BeginScene())){
		D3DXVECTOR3 cen = { 32.f,32.f,0.f };
		D3DXVECTOR3 pos = { 300.f,100.f,0.f };

		m_Sprite->Begin(D3DXSPRITE_ALPHABLEND);
		m_Sprite->Draw(m_Texture, NULL, &cen, &pos, D3DCOLOR_XRGB(255, 255, 255));
		m_Sprite->End();
		g_pD3DDevice->EndScene();
	}
	g_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}
bool InitDirect3D(HWND hwnd)
{
	//DX 오브젝트 생성
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if (g_pD3D == NULL)
		return false;

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));

	d3dpp.Windowed = TRUE;
	d3dpp.hDeviceWindow = hwnd;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferCount = 1; //보여지는 front와 바꿔주는 
	d3dpp.BackBufferWidth = 640;
	d3dpp.BackBufferHeight = 480;

	if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT
		, D3DDEVTYPE_HAL
		, hwnd
		, D3DCREATE_HARDWARE_VERTEXPROCESSING
		, &d3dpp
		, &g_pD3DDevice) == E_FAIL)
		return false;
}
void ReleaseDirect3D()
{
	if (g_pD3DDevice != NULL)
		g_pD3DDevice->Release();
	if (g_pD3D != NULL)
		g_pD3D->Release();

	g_pD3DDevice = NULL;
	g_pD3D = NULL;
}


