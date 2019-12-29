// ShootingGameB.cpp: 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "ShootingGameB.h"
#include "CInput.h"
#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

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
D3DXVECTOR3 g_PcPos = { 320.f,400.f,0.f };
D3DXVECTOR3 g_InvaderPos = { 32.f,32.f,0.f };
D3DXVECTOR3 g_ShootPos = { -999.f,-999.f,0.f };

LPD3DXSPRITE m_Sprite;
LPD3DXSPRITE g_InvaderSprite;
LPD3DXSPRITE g_ShootSprite;
LPDIRECT3DTEXTURE9 m_Texture;
LPDIRECT3DTEXTURE9 g_Invader;
LPDIRECT3DTEXTURE9 g_Shoot;
boolean shootAlive = false;
CInput* g_Input;

LPD3DXFONT g_Font;
int g_Score = 0;

D3DXVECTOR3 g_InvaderDir = { 1.f,0.f,0.f };


void Render();
bool InitDirect3D(HWND hwnd);
void ReleaseDirect3D();
void DrawSprite(LPD3DXSPRITE m_Sprite, LPDIRECT3DTEXTURE9 m_Texture, D3DXVECTOR3 cen, D3DXVECTOR3 pos);
void ReleaseTexture(LPDIRECT3DTEXTURE9 Texture);
void ReleaseSprite(LPD3DXSPRITE Sprite);
LPD3DXFONT CreateFont(LPDIRECT3DDEVICE9 pD3DDevice, int height, int width);
void ReleaseFont(LPD3DXFONT font);

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

	g_Input = new CInput(hWnd, hInstance);

	
	D3DXCreateSprite(g_pD3DDevice, &m_Sprite);
	D3DXCreateSprite(g_pD3DDevice, &g_InvaderSprite);
	D3DXCreateSprite(g_pD3DDevice, &g_ShootSprite);

	D3DXCreateTextureFromFileEx(g_pD3DDevice, _T("Airplane.bmp"), 0, 0, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, D3DX_FILTER_NONE, D3DX_DEFAULT, D3DCOLOR_XRGB(0, 0, 0), NULL, NULL, &m_Texture);
	D3DXCreateTextureFromFileEx(g_pD3DDevice, _T("monster.bmp"), 0, 0, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, D3DX_FILTER_NONE, D3DX_DEFAULT, D3DCOLOR_XRGB(0, 0, 0), NULL, NULL, &g_Invader);
	D3DXCreateTextureFromFileEx(g_pD3DDevice, _T("shoot.bmp"), 0, 0, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, D3DX_FILTER_NONE, D3DX_DEFAULT, D3DCOLOR_XRGB(0, 0, 0), NULL, NULL, &g_Shoot);

	g_Font = CreateFont(g_pD3DDevice, 24, 12);
	

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
			g_Input->ReadKeyboard();
			if (g_Input->IsEscapePressed())
				msg.message = WM_QUIT;

			D3DXVECTOR3 keyDir = { 0.f,0.f,0.f };

			g_Input->GetInputDir(keyDir);

			g_PcPos += keyDir * 4.f;

			
			float speed = 3;
			//Invader 화면 가장자리 충돌 처리
			if (g_InvaderPos.x > 640.f-32.f) {
				g_InvaderDir = { -1.f,0.f,0.f };
				g_InvaderPos += {0.f, 10.f, 0.f};
			}
			else if (g_InvaderPos.x < 32.f) {
				g_InvaderDir = { 1.f,0.f,0.f };
				g_InvaderPos += {0.f, 10.f, 0.f};
			}

			g_InvaderPos = g_InvaderPos + g_InvaderDir * speed;
			//Shoot 화면 가장자리 충돌 처리
			if (g_ShootPos.y < 16.f) {
				g_ShootPos = { -999.f,-999.f,0.f };
				shootAlive = false;
			}
			//Shoot 발사
			if (g_Input->IsSpaceKeyPressed()) {
				if (!shootAlive) {
					g_ShootPos = g_PcPos + D3DXVECTOR3(0.f, -20.f, 0.f);
					shootAlive = true;
				}
			}
			//Shoot Invader 충돌 처리
			if (shootAlive) {
				if (g_ShootPos.x + 8 > g_InvaderPos.x - 32 && g_ShootPos.y - 16 < g_InvaderPos.y + 32 && g_ShootPos.x - 8 < g_InvaderPos.x + 32 && g_ShootPos.y + 16 > g_InvaderPos.y - 32) {
					g_Score += 10;
					g_InvaderPos = { 32.f,32.f,0.f };
					shootAlive = false;
					g_ShootPos = { -999.f,-999.f,0.f };
				}
			}

			if(shootAlive) g_ShootPos = g_ShootPos + D3DXVECTOR3(0.f, -1.f, 0.f)*speed;

			Render();
		}
	}

	//Release
	ReleaseFont(g_Font);
	ReleaseTexture(g_Shoot);
	ReleaseSprite(g_ShootSprite);
	ReleaseTexture(g_Invader);
	ReleaseSprite(g_InvaderSprite);
	ReleaseTexture(m_Texture);
	ReleaseSprite(m_Sprite);

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
void Render()
{
	if (g_pD3DDevice == NULL)
		return;
	g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, g_ClearColor, 1.0f, 0);
	if (SUCCEEDED(g_pD3DDevice->BeginScene())){
		D3DXVECTOR3 cen = { 32.f,32.f,0.f };
		D3DXVECTOR3 pos = g_PcPos;

		DrawSprite(m_Sprite,m_Texture,cen,pos);
		pos = g_InvaderPos;
		DrawSprite(g_InvaderSprite, g_Invader, cen, pos);

		cen = { 8.f,32.f,0.f };
		pos = g_ShootPos;
		DrawSprite(g_ShootSprite, g_Shoot, cen, pos);
		
		// Font 사용
		RECT rc;
		rc = { 0,0,640,480 };
		WCHAR szScore[100];
		swprintf_s(szScore, _T("점수: %d"), g_Score);
		g_Font->DrawTextW(NULL, szScore, -1, &rc, DT_LEFT | DT_TOP, D3DCOLOR_XRGB(255, 255, 255));

		g_pD3DDevice->EndScene();
	}
	g_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}
void DrawSprite(LPD3DXSPRITE m_Sprite, LPDIRECT3DTEXTURE9 m_Texture, D3DXVECTOR3 cen, D3DXVECTOR3 pos) {
	m_Sprite->Begin(D3DXSPRITE_ALPHABLEND);
	m_Sprite->Draw(m_Texture, NULL, &cen, &pos, D3DCOLOR_XRGB(255, 255, 255));
	m_Sprite->End();
}
void ReleaseSprite(LPD3DXSPRITE Sprite) {
	if (Sprite != NULL)
	{
		Sprite->Release();
		Sprite = NULL;
	}
}
void ReleaseTexture(LPDIRECT3DTEXTURE9 Texture) {
	if (Texture != NULL)
	{
		Texture->Release();
		Texture = NULL;
	}
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
LPD3DXFONT CreateFont(LPDIRECT3DDEVICE9 pD3DDevice, int height, int width) {
	LPD3DXFONT font;
	D3DXFONT_DESC desc;
	ZeroMemory(&desc, sizeof(D3DXFONT_DESC));

	desc.CharSet = HANGUL_CHARSET;
	desc.Height = height;
	desc.Width = width;
	desc.Weight = FW_NORMAL;
	desc.Quality = DEFAULT_QUALITY;
	desc.MipLevels = 1;

	auto hr = D3DXCreateFontIndirect(pD3DDevice, &desc, &font);
	if (FAILED(hr))
		return NULL;
	return font;
}

void ReleaseFont(LPD3DXFONT font) {
	if (font != NULL) {
		font->Release();
	}
}

