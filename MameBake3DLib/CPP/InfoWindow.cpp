#include "stdafx.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <windows.h>

#include <InfoWindow.h>

#include <crtdbg.h>
#define DBGH
#include <dbg.h>


LRESULT CALLBACK InfoWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
int InitializeInfoWindow(CREATESTRUCT* createWindowArgs);

static CInfoWindow* s_contextwin = 0;

CInfoWindow::CInfoWindow()
{
	InitParams();
}
CInfoWindow::~CInfoWindow()
{
	DestroyObjs();
}
void CInfoWindow::InitParams()
{
	s_contextwin = 0;
	m_isfirstoutput = true;

	m_hWnd = NULL;
	m_hParentWnd = NULL;
	m_hdcM = 0;

	m_stroutput = 0;

	m_dataindex = 0;
	m_viewindex = 0;

}
void CInfoWindow::DestroyObjs()
{
	if (m_hdcM) {
		delete m_hdcM;
		m_hdcM = 0;
	}

	if (m_stroutput) {
		free(m_stroutput);
		m_stroutput = 0;
	}
}


int CInfoWindow::CreateInfoWindow(HWND srcparentwnd, int srcposx, int srcposy, int srcwidth, int srcheight)
{
	//�f�[�^���쐬
	m_stroutput = (WCHAR*)malloc(sizeof(WCHAR) * INFOWINDOWLINEW * INFOWINDOWLINEH);
	if (!m_stroutput) {
		return 1;
	}
	ZeroMemory(m_stroutput, sizeof(WCHAR) * INFOWINDOWLINEW * INFOWINDOWLINEH);


	//�E�B���h�E�N���X��o�^
	WNDCLASSEX wcex;
	ZeroMemory((LPVOID)&wcex, sizeof(WNDCLASSEX));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = 0;
	wcex.lpfnWndProc = InfoWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = (HINSTANCE)GetModuleHandle(NULL);
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"InfoWindow_";
	wcex.hIconSm = NULL;
	RegisterClassEx(&wcex);



	//�E�B���h�E�쐬
	int cxframe = GetSystemMetrics(SM_CXFRAME);
	int cyframe = GetSystemMetrics(SM_CYFRAME);
	//if (istopmost) {
	//	hWnd = CreateWindowEx(//WS_EX_TOOLWINDOW|WS_EX_TOPMOST,szclassName,title,WS_POPUP,
	//		WS_EX_LEFT | WS_EX_TOPMOST, szclassName, title, WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE,
	//		pos.x, pos.y,
	//		size.x, size.y,
	//		hWndParent, NULL, hInstance, NULL);
	//}
	//else {
	m_hWnd = CreateWindowEx(
		WS_EX_LEFT, L"InfoWindow_", L"InfoWindow", WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE | WS_THICKFRAME,
		srcposx, srcposy,
		srcwidth, srcheight - 2 * cyframe,
		srcparentwnd, NULL, (HINSTANCE)GetModuleHandle(NULL), NULL);
	//}
	//SetParent(s_infownd, s_mainhwnd);

	if (m_hWnd) {
		m_hdcM = new OrgWinGUI::HDCMaster();
		if (m_hdcM) {
			m_hdcM->setHWnd(m_hWnd);

			//�E�B���h�E�\��
			ShowWindow(m_hWnd, SW_SHOW);

			s_contextwin = this;

			return 0;
		}
		else {
			return 1;
		}
	}
	else {
		return 1;
	}
}

int CInfoWindow::OutputInfo(WCHAR* lpFormat, ...)
{
	if (!m_hWnd) {
		return 0;
	}
	if (!IsWindow(m_hWnd)) {
		return 0;
	}
	if (!m_stroutput) {
		return 1;
	}
	if (m_dataindex >= INFOWINDOWLINEH) {
		return 1;
	}

	int ret;
	va_list Marker;
	unsigned long wleng, writeleng;
	WCHAR outchar[INFOWINDOWLINEW];

	ZeroMemory(outchar, sizeof(WCHAR) * INFOWINDOWLINEW);

	va_start(Marker, lpFormat);
	ret = vswprintf_s(outchar, INFOWINDOWLINEW, lpFormat, Marker);
	va_end(Marker);

	if (ret < 0)
		return 1;

	if (!m_isfirstoutput) {
		m_dataindex++;
		if (m_dataindex >= INFOWINDOWLINEH) {
			//m_dataindex = 0;
			MessageBox(m_hWnd, L"InfoWindow�̏o�͍s���������ɒB���܂����B", L"�x��", MB_OK);
			return 1;
		}
	}


	//�ŐV�r���[�̏ꍇ�ɂ̂ݍŐV��ԂɍX�V����B
	//�z�C�[���Ńr���[�ʒu�𑀍삷�邽�߁A�ŏ��̏������݂̂Ƃ��ƍŐV�r���[�ȊO�̏ꍇ�͂�����Ȃ��B
	if (m_isfirstoutput || (m_viewindex == (m_dataindex - 1))) {
		m_viewindex = m_dataindex;
	}


	//wleng = (unsigned long)wcslen(outchar);
	wcscpy_s(m_stroutput + m_dataindex * INFOWINDOWLINEW, INFOWINDOWLINEW, outchar);

	m_isfirstoutput = false;

	//WriteFile(dbgfile, outchar, sizeof(WCHAR) * wleng, &writeleng, NULL);

	return 0;
}

void CInfoWindow::UpdateWindow()
{
	if (m_hWnd && IsWindow(m_hWnd)) {
		RECT clirect;
		GetClientRect(m_hWnd, &clirect);
		InvalidateRect(m_hWnd, &clirect, true);
		::UpdateWindow(m_hWnd);
	}
}

void CInfoWindow::OnPaint()
{
	if (m_hWnd && IsWindow(m_hWnd)) {

		RECT clirect;
		GetClientRect(m_hWnd, &clirect);

		m_hdcM->beginPaint();
		m_hdcM->setPenAndBrush(RGB(70, 50, 70), RGB(70, 50, 70));
		Rectangle(m_hdcM->hDC, 0, 0, clirect.right, clirect.bottom);
		m_hdcM->setFont(12, _T("�l�r �S�V�b�N"));
		SetTextColor(m_hdcM->hDC, RGB(255, 255, 255));
		
		//TextOut(m_hdcM->hDC, 10, 2, outchar, (int)wcslen(outchar));

		//INFOWINDOWLINEH�s���A�Â����ɕ`�悷��
		int outputno;
		int curindex = m_viewindex - (INFOWINDOWLINEVIEW - 1);
		int dispno = 0;
		for (outputno = 0; outputno < INFOWINDOWLINEVIEW; outputno++) {
			if ((curindex >= 0) && (curindex <= m_dataindex)) {
				TextOut(m_hdcM->hDC, 5, 5 + 15 * dispno, m_stroutput + curindex * INFOWINDOWLINEW, (int)wcslen(m_stroutput + curindex * INFOWINDOWLINEW));
				dispno++;
			}
			curindex++;
		}
		m_hdcM->endPaint();
	}
}

void CInfoWindow::onMouseWheel(const OrgWinGUI::MouseEvent& e)
{
	//�z�C�[���ŕ`�悷��s���V�t�g����B

	if (e.wheeldelta < 0) {
		m_viewindex++;
		if (m_viewindex > m_dataindex) {
			m_viewindex = m_dataindex;
		}
		UpdateWindow();
	}
	else if (e.wheeldelta > 0) {
		m_viewindex--;
		if (m_viewindex < (INFOWINDOWLINEVIEW - 1)) {
			m_viewindex = (INFOWINDOWLINEVIEW - 1);
		}
		UpdateWindow();
	}
}

int CInfoWindow::GetStrNum()
{
	if (m_isfirstoutput) {
		return 0;
	}
	else {
		int retnum = m_dataindex + 1;
		if (retnum > INFOWINDOWLINEH) {
			retnum = INFOWINDOWLINEH;
		}
		return retnum;
	}
}
int CInfoWindow::GetStr(int srcindex, int srcoutleng, WCHAR* strout)
{
	if (!strout) {
		return 1;
	}
	if (srcoutleng < 0)
	{
		return 1;
	}

	if ((srcindex >= 0) && (srcindex < INFOWINDOWLINEH)) {
		wcscpy_s(strout, srcoutleng, m_stroutput + srcindex * INFOWINDOWLINEW);
		return 0;
	}
	else {
		return 1;
	}
}



LRESULT CALLBACK InfoWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	POINT tmpPoint;
	RECT tmpRect;
	OrgWinGUI::MouseEvent mouseEvent;

	switch (uMsg)
	{
		//case WM_TIMER:
		//	OnTimerFunc(wParam);
		//	break;
	case WM_DESTROY:
		//PostQuitMessage(0);
		return 0;
		break;
	case WM_CREATE:
		return InitializeInfoWindow((CREATESTRUCT*)lParam);
		break;
		//case WM_COMMAND:
		//{
		//	if ((menuid >= 59900) && (menuid <= (59900 + MAXMOTIONNUM))) {
		//		ActivatePanel(0);
		//		int selindex = menuid - 59900;
		//		OnAnimMenu(selindex);
		//		ActivatePanel(1);
		//		return 0;
		//	}
		//}
		//break;
	case WM_PAINT:				//�`��
		if (s_contextwin) {
			s_contextwin->OnPaint();
		}
		return 0;
	case WM_MOUSEWHEEL:
		if (s_contextwin) {
			GetCursorPos(&tmpPoint);
			GetWindowRect(s_contextwin->GetHWnd(), &tmpRect);

			mouseEvent.globalX = (int)tmpPoint.x;
			mouseEvent.globalY = (int)tmpPoint.y;
			mouseEvent.localX = mouseEvent.globalX - tmpRect.left;
			mouseEvent.localY = mouseEvent.globalY - tmpRect.top;
			mouseEvent.shiftKey = GetKeyState(VK_SHIFT) < 0;
			mouseEvent.ctrlKey = GetKeyState(VK_CONTROL) < 0;
			mouseEvent.altKey = GetKeyState(VK_MENU) < 0;
			mouseEvent.wheeldelta = GET_WHEEL_DELTA_WPARAM(wParam);

			s_contextwin->onMouseWheel(mouseEvent);

		}
		break;
	case WM_MOUSEHOVER:
		if (s_contextwin) {
			SetCapture(s_contextwin->GetHWnd());
		}
		break;
	case WM_MOUSELEAVE:
		if (s_contextwin) {
			ReleaseCapture();
		}
		break;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	return 0;
}

int InitializeInfoWindow(CREATESTRUCT* createWindowArgs)
{
	//TCHAR message[1024];
	//int messageResult;
	//wsprintf(message,
	//	TEXT("�E�B���h�E�N���X:%s\n�^�C�g��:%s\n�E�B���h�E�𐶐����܂����H"),
	//	createWindowArgs->lpszClass, createWindowArgs->lpszName
	//);

	//messageResult = MessageBox(NULL, message, TEXT("�m�F"), MB_YESNO | MB_ICONINFORMATION);

	//if (messageResult == IDNO)
	//	return -1;
	return 0;
}