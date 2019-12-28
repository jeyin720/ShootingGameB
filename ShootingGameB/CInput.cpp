#include "stdafx.h"

#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>

#include "CInput.h"


CInput::CInput(HWND hwnd, HINSTANCE hInstance) :m_lpDInput(NULL), m_lpKeyboard(NULL)
{
	InitInput(hwnd, hInstance);
}

CInput::~CInput()
{
	ShutdownInput();
}

int CInput::InitInput(HWND hwnd, HINSTANCE hInstance)
{
	auto result = DirectInput8Create(hInstance
		, DIRECTINPUT_VERSION
		, IID_IDirectInput8
		, (LPVOID*)&m_lpDInput
		, NULL);
	if (FAILED(result))
		return -1;

	// 키보드 디바이스 생성하기
	result = m_lpDInput->CreateDevice(GUID_SysKeyboard, &m_lpKeyboard, NULL);
	if (FAILED(result))
		return -2;
	//키보드 데이터 포맷 설정
	result = m_lpKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
		return -3;
	//키보드 독점 사용에 대해서 알리기
	result = m_lpKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
		return -4;
	//키보드 사용
	result = m_lpKeyboard->Acquire();
	if (FAILED(result))
		return -5;

	return 1;
}

void CInput::ShutdownInput()
{
	if (m_lpKeyboard != NULL) {
		m_lpKeyboard->Unacquire();
		m_lpKeyboard->Release();
		m_lpKeyboard = NULL;
	}
	if (m_lpDInput != NULL) {
		m_lpDInput->Release();
		m_lpDInput = NULL;
	}
}

bool CInput::ReadKeyboard()
{
	auto result = m_lpKeyboard->GetDeviceState(
		sizeof(m_keyboardState)
		, (LPVOID)&m_keyboardState);
	if (FAILED(result))
	{
		if (result == DIERR_INPUTLOST ||
			result == DIERR_NOTACQUIRED)
			m_lpKeyboard->Acquire();
		else
			return false;
	}
	return true;
}

void CInput::GetInputDir(D3DXVECTOR3 & vDir)
{
	vDir = { 0.f,0.f,0.f };
	vDir.x += IsKeyPressed(DIK_LEFTARROW) ? -1.f : 0.f;
	vDir.x += IsKeyPressed(DIK_RIGHTARROW) ? 1.f : 0.f;
}
bool CInput::IsSpaceKeyPressed() {
	static DWORD dwPrevTime = 0;
	unsigned long dwCurTime = GetTickCount();
	int dt = dwCurTime - dwPrevTime;

	if ((dt > 200) && (m_keyboardState[DIK_SPACE] & 0x80)) {
		m_keyboardState[DIK_SPACE] = 0;
		dwPrevTime = dwCurTime;
		return true;
	}
	return false;
}