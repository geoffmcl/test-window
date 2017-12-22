/*\
* winmain.c - part of test-window project
*
* Copyright (c) 2017 - Geoff R. McLane
* Licence: GNU GPL version 2
*
\*/
/* Based on the tutorial at http://www.winprog.org/tutorial/simple_window.html 
   Many thanks to Brook Miles for making it look simple... */

#include <windows.h>
#include <stdio.h> /* for sprintf, ... */
#include "resource.h"

#define DEF_WIDTH 800
#define DEF_HEIGHT 400
#define IDC_MAIN_EDIT	101

#define ID_FILE_EXIT 9001
#define ID_STUFF_GO 9002
#define ID_FILE_LOAD 9003
#define ID_HELP_ABOUT 9004

const char g_szClassName[] = "myWindowClass";
const char g_szTitle[] = "test-window";

LRESULT Do_WM_CREATE(HWND hwnd)
{
    HFONT hfDefault;
    HWND hEdit;
    HMENU hMenu, hSubMenu;

    // first create and add a menu
    hMenu = CreateMenu();

    hSubMenu = CreatePopupMenu();
    AppendMenu(hSubMenu, MF_STRING, ID_FILE_LOAD, "&Load...");
    AppendMenu(hSubMenu, MF_STRING, ID_FILE_EXIT, "E&xit");
    AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)hSubMenu, "&File");

    hSubMenu = CreatePopupMenu();
    AppendMenu(hSubMenu, MF_STRING, ID_STUFF_GO, "&Go");
    AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)hSubMenu, "&Stuff");

    hSubMenu = CreatePopupMenu();
    AppendMenu(hSubMenu, MF_STRING, ID_HELP_ABOUT, "&About...");
    AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)hSubMenu, "&Help");

    SetMenu(hwnd, hMenu);


    hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
        0, 0, 100, 100, hwnd, (HMENU)IDC_MAIN_EDIT, GetModuleHandle(NULL), NULL);
    if (hEdit == NULL) {
        MessageBox(hwnd, "Could not create edit box.", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    hfDefault = GetStockObject(DEFAULT_GUI_FONT);
    SendMessage(hEdit, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

    return 0;
}

LRESULT Do_WM_SIZE( HWND hwnd )
{
    HWND hEdit;
    RECT rcClient;

    GetClientRect(hwnd, &rcClient);

    hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
    if (hEdit) {
        SetWindowPos(hEdit, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);
    }
    return 0;
}

BOOL LoadTextFileToEdit(HWND hwnd, LPCTSTR pszFileName)
{
    HWND hEdit;
    HANDLE hFile;
    BOOL bSuccess = FALSE;

    hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
    if (!hEdit) {
        MessageBox(hwnd, "Failed to get Edit control handle!", "Error", MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    hFile = CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, 0, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        DWORD dwFileSize;
        dwFileSize = GetFileSize(hFile, NULL);
        if (dwFileSize != INVALID_FILE_SIZE)
        {
            LPSTR pszFileText;
            pszFileText = GlobalAlloc(GPTR, dwFileSize + 1);
            if (pszFileText != NULL)
            {
                DWORD dwRead;
                if (ReadFile(hFile, pszFileText, dwFileSize, &dwRead, NULL))
                {
                    pszFileText[dwFileSize] = 0; // Add null terminator
                    if (SetWindowText(hEdit, pszFileText)) {
                        bSuccess = TRUE; // It worked!
                    }
                    else {
                        MessageBox(hwnd, "Failed to set selected file text!", "Error", MB_OK | MB_ICONEXCLAMATION);
                    }
                }
                else {
                    MessageBox(hwnd, "Failed to read selected file!", "Error", MB_OK | MB_ICONEXCLAMATION);
                }
                GlobalFree(pszFileText);
            }
            else {
                MessageBox(hwnd, "Failed to allocate memory for the selected file!", "Error", MB_OK | MB_ICONEXCLAMATION);
            }
        }
        else {
            MessageBox(hwnd, "Failed to get selected file size!", "Error", MB_OK | MB_ICONEXCLAMATION);
        }
        CloseHandle(hFile);
    }
    else {
        MessageBox(hwnd, "Failed to open selected file!", "Error", MB_OK | MB_ICONEXCLAMATION);
    }
    return bSuccess;
}

VOID Do_ID_FILE_LOAD(HWND hwnd)
{
    OPENFILENAME ofn;
    char szFileName[MAX_PATH] = "";

    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = "txt";

    if (GetOpenFileName(&ofn))
    {
        if (LoadTextFileToEdit(hwnd, szFileName) == TRUE) { // Do something useful with the filename stored in szFileName 
            // change title to reflect file loaded
            char title[264];
            sprintf(title, "%s - %s", g_szTitle, szFileName);
            SetWindowText(hwnd, title);
        }
    }

}

VOID Do_WM_INITDIALOG(HWND hDlg)
{

}


BOOL CALLBACK AboutDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    switch (Message)
    {
    case WM_INITDIALOG:
        Do_WM_INITDIALOG(hwnd);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            EndDialog(hwnd, IDOK);
            break;
        case IDCANCEL:
            EndDialog(hwnd, IDCANCEL);
            break;
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

VOID Do_ID_HELP_ABOUT(HWND hwnd)
{
    INT_PTR ret = DialogBox(GetModuleHandle(NULL),
        MAKEINTRESOURCE(IDD_ABOUT), hwnd, (DLGPROC)AboutDlgProc);
    if (ret == IDOK) {
        // MessageBox(hwnd, "Dialog exited with IDOK.", "Notice", MB_OK | MB_ICONINFORMATION);
    }
    else if (ret == -1) {
        MessageBox(hwnd, "Dialog failed!", "Error",
            MB_OK | MB_ICONINFORMATION);
    }
}


// Step 4: the Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes = 0;
    switch(msg)
    {
    case WM_CREATE:
        lRes = Do_WM_CREATE(hwnd);
        break;
    case WM_SIZE:
        lRes = Do_WM_SIZE(hwnd);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_FILE_EXIT:
            PostMessage(hwnd, WM_CLOSE, 0, 0);
            break;
        case ID_STUFF_GO:

            break;
        case ID_FILE_LOAD:
            Do_ID_FILE_LOAD(hwnd);
            break;
        case ID_HELP_ABOUT:
            Do_ID_HELP_ABOUT(hwnd);
            break;
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        lRes = DefWindowProc(hwnd, msg, wParam, lParam);
        break;
    }
    return lRes;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    //Step 1: Registering the Window Class
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return -1;
    }

    // Step 2: Creating the Window
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        g_szTitle,  // "test-window"
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, DEF_WIDTH, DEF_HEIGHT,
        NULL, NULL, hInstance, NULL);

    if(hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return -1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Step 3: The Message Loop
    while(GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    return (int)Msg.wParam;
}

/* eof */
