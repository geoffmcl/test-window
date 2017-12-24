// ap4_track.cxx

#include "testap4.hxx"

#define GetNxtBuf getnxtbuf

static RECT rcTrack;
static COLORREF pixel, last_pixel;

void DrawSelect2( HDC hDC, LPRECT lprcClip )
{
	// Don't have anything to do if the rectangle is empty.
	if( ( hDC == 0 ) || ( lprcClip == 0 ) || ( IsRectEmpty( lprcClip ) ) )
		return;

	// Draw rectangular clip region
	PatBlt( hDC, lprcClip->left, lprcClip->top,
		lprcClip->right - lprcClip->left, 1, DSTINVERT );
	PatBlt( hDC, lprcClip->left, lprcClip->bottom,
		1, -(lprcClip->bottom - lprcClip->top), DSTINVERT );
	PatBlt( hDC, lprcClip->right - 1, lprcClip->top,
		1, lprcClip->bottom - lprcClip->top, DSTINVERT );
	PatBlt( hDC, lprcClip->right, lprcClip->bottom - 1,
		-(lprcClip->right - lprcClip->left), 1, DSTINVERT );


}

void Remove_Clip2()
{
    HDC hdc = GetDC(NULL);
    if (hdc) {
        // remove any OLD (inverted) clip
        DrawSelect2( hdc, &rcTrack );
        ReleaseDC(NULL,hdc);
    }
    SetRectEmpty(&rcTrack);

}

void Paint_Pixel( HDC hdc, COLORREF cr )
{
    //static SIZE last_sz = {0,0};
    static int last_len = 0;
    HFONT hf = Get_Font();
    HFONT hfold = NULL;
    if(hf)
       hfold = (HFONT)SelectObject(hdc,hf);
    int red = GetRValue(cr);
    int green = GetGValue(cr);
    int blue = GetBValue(cr);

    char *tb = GetNxtBuf();
    int len = sprintf(tb,"RGB(%d,%d,%d) RGB(0x%X,0x%X,0x%X)", red,green, blue, red, green, blue);

    SIZE sz;
    GetTextExtentPoint32( hdc, tb, len, &sz );
    int x = 10;
    int y = g_ClientRect.bottom - sz.cy;
    if (len < last_len) {
        char *tb2 = &tb[len+2];
        *tb2 = 0;
        while ((int)strlen(tb2) < len)
            strcat(tb2," ");
        TextOut( hdc, x, y, tb2, len );
    }
    TextOut( hdc, x, y, tb, len );
    last_len = len;
}

void Show_Pixel()
{
    if (pixel == last_pixel)
        return;
    last_pixel = pixel;

    HDC hdc = GetDC(g_hWnd);
    if (!hdc)
        return;
    Paint_Pixel(hdc, pixel);
    ReleaseDC(g_hWnd,hdc);
}

void Draw_Clip2( int mx, int my )
{
    HDC hdc = GetDC(NULL);
    int half_clip = 10;

    if (hdc) {
        // remove any OLD (inverted) clip
        DrawSelect2( hdc, &rcTrack );
        // set NEW clip rectangle
        rcTrack.left   = mx - half_clip;
        rcTrack.right  = mx + half_clip;
        rcTrack.top    = my - half_clip;
        rcTrack.bottom = my + half_clip;
        // put it up
        DrawSelect2( hdc, &rcTrack );

        pixel = GetPixel(hdc, mx, my);

        ReleaseDC(NULL,hdc);

        Show_Pixel();
    }
}


VOID Do_WM_LBUTTONDOWN( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
    MSG   msg;
    POINT pt;

    SetCapture( hWnd );
    SetRectEmpty(&rcTrack);
    for( ;; )
    {
        // The WaitMessage function yields control to other threads
        // when a thread has no other messages in its message queue.
        // The WaitMessage function suspends the thread and does not
        // return until a new message is placed in the thread's
        // message queue. 
        WaitMessage();

        // PeekMessage Return Values
        // If a message is available, the return value is nonzero.
        if( PeekMessage( &msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE ) )
        {
            if( msg.message == WM_LBUTTONUP )
            {
                break;
            }
            pt.x = LOWORD(msg.lParam);
            pt.y = HIWORD(msg.lParam);
            ClientToScreen(hWnd, &pt);
            Draw_Clip2( pt.x, pt.y );

        }
    }

    Remove_Clip2();
    ReleaseCapture();

}

// eof - ap4_track.cxx
