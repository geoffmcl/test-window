// ap4_command.cxx

#include "testap4.hxx"

LRESULT Do_WM_COMMAND(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes = 0;
	int wmId, wmEvent;
	wmId    = LOWORD(wParam);
	wmEvent = HIWORD(wParam);
	// Parse the menu selections:
	switch (wmId)
	{
	case IDM_ABOUT:
		DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
		break;
	case IDM_EXIT:
		DestroyWindow(hWnd);
		break;
    case IDM_REFRESH:
        ResetSet_Factor();
        InvalidateRect(hWnd,NULL,TRUE);
        break;
	default:
		lRes = DefWindowProc(hWnd, message, wParam, lParam);
	}
    return lRes;

}

// eof - ap4_command.cxx

