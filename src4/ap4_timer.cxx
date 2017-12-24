// ap4_timer.cxx

#include "testap4.hxx"

#ifdef	ADD_TIMER1
UINT_PTR uiTimer1 = NULL;

static int timer_count = 0;
static long timer_ms = 0;
static long timer_seconds = 0;
void Do_One_Second(HWND hWnd)
{
    timer_seconds++;
    HDC hdc = GetDC(hWnd);
    if(hdc) {
        paint_factor(hdc);
        ReleaseDC(hWnd,hdc);
    }
}

void Do_WM_TIMER( HWND hWnd )
{
    timer_ms += TIMER_INTERVAL1;
    while (timer_ms >= 1000) {
        timer_ms -= 1000;
        Do_One_Second(hWnd);
    }
}

#endif	/* ADD_TIMER1 */
// eof -ap4_timer.hxx
