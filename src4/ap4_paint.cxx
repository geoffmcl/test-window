// ap4_paint.cxx

#include "testap4.hxx"
#include <windowsx.h>

#define DBL2X(a) border + (win_int(a * factor))
#define DBL2Y(a) border + (win_int(a * factor))
#define SCN2X(x) win_int( (double)(x - border) / factor )
#define SCN2Y(y) win_int( (double)(y - border) / factor )
#define ASCN2X(x) win_int( (double)x / factor )
#define ASCN2Y(y) win_int( (double)y / factor )

#define GETIX(a) DBL2X(a.GetX())
#define GETIY(a) DBL2Y(a.GetY())

static VLIMS vlimits = { 0 };
static double factor = 10.0;
static int border = 20;
RECT g_ParentRect;
RECT g_ClientRect;
RECT rcClip = { 0, 0, 0, 0 };
int half_clip = 10;
bool g_paint_mm_grid = false;    // paint grid per MIN/MAX values
bool g_paint_screen_grid = true; // paint whole screen grid
bool g_add_coords = false;
bool g_test_on_a_line = false;

SIZE g_szTime = { 10,15 };

HPEN g_hPenR2 = NULL, g_hPenB1 = NULL;
HPEN g_hPenG1 = NULL;
HPEN g_hPenGd1 = NULL;

HBRUSH g_hBrushB = NULL;
HFONT g_hFont = NULL;
COLORREF g_crGray = RGB(200,200,200);

Vector2dVector box;
Vector2dVector tris;

HFONT Get_Font()
{
    if (g_hFont)
        return g_hFont;
   LOGFONT lf;
   LOGFONT * plf = &lf;
   ZeroMemory( plf, sizeof(LOGFONT) );
   HDC hdc = GetDC(g_hWnd);
   plf->lfHeight = -MulDiv( 8, GetDeviceCaps(hdc, LOGPIXELSY), 72 );
   ReleaseDC(g_hWnd,hdc);
   //lstrcpy( &plf->lfFaceName[0], L"Courier New" );
   strcpy( plf->lfFaceName, "Courier New" );
   //plf->lfWeight = FW_BOLD;   // set BOLD font
   g_hFont = CreateFontIndirect( plf );
   return g_hFont;
}


void create_drawing_tools(void)
{
    g_hPenR2 = CreatePen( PS_SOLID,  // int fnPenStyle,    // pen style
          2, // int nWidth,        // pen width
          RGB(255,0,0) ); //  COLORREF crColor   // pen color
    g_hPenB1 = CreatePen( PS_SOLID,  // int fnPenStyle,    // pen style
          1, // int nWidth,        // pen width
          RGB(0,0,255) ); //  COLORREF crColor   // pen color
    g_hPenG1 = CreatePen( PS_SOLID,  // int fnPenStyle,    // pen style
          1, // int nWidth,        // pen width
          g_crGray ); //RGB(115,115,115) ); //  COLORREF crColor   // pen color
    g_hPenGd1 = CreatePen( PS_DASHDOT,  // int fnPenStyle,    // pen style
          1, // int nWidth,        // pen width
          g_crGray ); //RGB(115,115,115) ); //  COLORREF crColor   // pen color
    // Create a BLUEISH brush.
    g_hBrushB = CreateSolidBrush(RGB(128, 128, 255));

    Get_Font();
}

void delete_drawing_tools(void)
{
    if (g_hPenR2)
        DeleteObject(g_hPenR2);
    if (g_hPenB1)
        DeleteObject(g_hPenB1);
    if (g_hBrushB)
        DeleteObject(g_hBrushB);
    if (g_hPenG1)
        DeleteObject(g_hPenG1);
    if (g_hPenGd1)
        DeleteObject(g_hPenGd1);
    if (g_hFont)
        DeleteObject(g_hFont);
    g_hPenR2 = NULL;
    g_hPenB1 = NULL;
    g_hPenG1 = NULL;
    g_hPenGd1 = NULL;
    g_hBrushB = NULL;
    g_hFont = NULL;
}

HPEN Get_hPenR2(void) { return g_hPenR2; }
HPEN Get_hPenB1(void) { return g_hPenB1; }

void Set_Factor( PVLIMS pvl )
{
    // ensure min,max fit on screen, and fill screen
    double width = (double)(g_ClientRect.right - (2 * border));
    double height = (double)(g_ClientRect.bottom - (2 * border));
    // miny - maxy
    double wrange = pvl->maxx - pvl->minx;
    double hrange = pvl->maxy - pvl->miny;
    if (width < 1.0)
        width = 1.0;
    if (height < 1.0)
        height = 1.0;
    if (wrange < 1.0)
        wrange = 1.0;
    if (hrange < 1.0)
        hrange = 1.0;

    double xfact = ( width / wrange );
    double yfact = ( height / hrange );
    if (xfact < yfact)
        factor = xfact;
    else
        factor = yfact;
}

void ResetSet_Factor(void)
{
    PVLIMS pvl = &vlimits;
    if (pvl->valid)
        Set_Factor(pvl);
}

int Change_Factor(int dir)
{
    if (dir == SB_LINEDOWN) {
        factor *= 0.9;
    } else {
        factor *= 1.1;
    }
    return 1;
}

void paint_line( HDC hdc, int x1, int y1, int x2, int y2 )
{
    MoveToEx( hdc, x1, y1, NULL ); // begin point
    LineTo( hdc, x2, y2 ); // line to, in currrent color
}

void paint_line( HDC hdc, Vector2d A, Vector2d B )
{
    int x1, x2, y1, y2;
    x1 = win_int(A.GetX());
    x2 = win_int(B.GetX());
    y1 = win_int(A.GetY());
    y2 = win_int(B.GetY());
    paint_line( hdc, x1, y1, x2, y2 );
}

void paint_tri_cent_lines( HDC hdc, int x1, int y1, int x2, int y2, int x3, int y3 )
{
    HPEN hp = g_hPenG1;
    HPEN hpOld = NULL;
    if(hp)
        hpOld = (HPEN)SelectObject(hdc, hp);
    paint_line( hdc, x1, y1, (x2 + x3) / 2, (y2 + y3) / 2);
    paint_line( hdc, x2, y2, (x1 + x3) / 2, (y1 + y3) / 2);
    paint_line( hdc, x3, y3, (x1 + x2) / 2, (y1 + y2) / 2);
    if(hpOld)
        SelectObject(hdc, hpOld);

}

void paint_number_at( HDC hdc, int x, int y, int num )
{
    HFONT hf = Get_Font();
    if (hf) {
        char * nb = getnxtbuf();
        HFONT hfOld = (HFONT)SelectObject(hdc,hf);
        int len = sprintf(nb,"%d",num);
        SIZE sz;
        GetTextExtentPoint32( hdc, nb, len, &sz );
        ExtTextOut( hdc, x - (sz.cx / 2), y - (sz.cy / 2), 0, NULL, nb, len, NULL );
        if(hfOld)
            SelectObject(hdc,hfOld);
    }
}

void paint_string_at( HDC hdc, int x, int y, char * cp )
{
    HFONT hf = Get_Font();
    if (hf) {
        HFONT hfOld = (HFONT)SelectObject(hdc,hf);
        int len = (int)strlen(cp);
        SIZE sz;
        GetTextExtentPoint32( hdc, cp, len, &sz );
        ExtTextOut( hdc, x - (sz.cx / 2), y - (sz.cy / 2), 0, NULL, cp, len, NULL );
        if(hfOld)
            SelectObject(hdc,hfOld);
    }
}


void paint_tri( HDC hdc, int x1, int y1, int x2, int y2, int x3, int y3, int num = 0 )
{
    //paint_tri_cent_lines( hdc, x1, y1, x2, y2, x3, y3 );
    paint_line( hdc, x1, y1, x2, y2 );
    paint_line( hdc, x2, y2, x3, y3 );
    paint_line( hdc, x3, y3, x1, y1 );
    if (num) {
        Vector2d v;
        if (get_tri_centroid( x1, y1, x2, y2, x3, y3, v ))
        {
            paint_number_at( hdc, win_int(v.GetX()), win_int(v.GetY()), num );
        }
    }
}


void draw_cross( HDC hdc, int xc, int yc, int sz, COLORREF cr )
{
    HPEN hp = CreatePen( PS_SOLID, 1, cr );
    HPEN hpOld = NULL;
    int x1, y1, x2, y2;
    int sz2 = sz / 2;
    if(hp)
        hpOld = (HPEN)SelectObject(hdc, hp);
    x1 = xc - sz2;
    y1 = yc - sz2;
    x2 = xc + sz2;
    y2 = yc + sz2;
    paint_line( hdc, x1, yc, x2, yc );
    paint_line( hdc, xc, y1, xc, y2 );
    if(hpOld)
        SelectObject(hdc, hpOld);
}

void set_gray_box_vectors(PVLIMS pvl)
{
    Vector2d v;
    box.clear();
    int x1, y1, x2, y2;
    // top and bottom
    // x min
    x1 = DBL2X(pvl->minx);
    // y min
    y1 = DBL2Y(pvl->miny);
    // x max
    x2 = DBL2Y(pvl->maxx);
    // y max
    y2 = DBL2Y(pvl->maxy);

    v.Set(x1,y1);   // minx, miny = TL
    box.push_back(v);   // pt 1

    v.Set(x2,y1);   // maxx, miny = TR
    box.push_back(v);   // pt 2

    v.Set(x2,y2);   // maxx, maxy = BR
    box.push_back(v);   // pt 3

    v.Set(x1,y2);   // minx, maxy = BL
    box.push_back(v);   // pt 4

}

typedef std::vector<POINT> VectorPt;


void paint_background_grid(HDC hdc, PVLIMS pvl)
{
    HPEN hpOld = NULL;
    COLORREF crOld;

    if (g_hPenGd1)
        hpOld = (HPEN)SelectObject(hdc, g_hPenGd1);
    crOld = SetTextColor(hdc,g_crGray);

    Vector2d v1, v2, v3, v4;
    // void get_range( double lat1, double lat2, int ticks );
    char * nb = getnxtbuf();
    double tick;
    PMRNG pmr;
    POINT pt;
    VectorPt vecpt1;
    VectorPt vecpt2;

    if (g_paint_mm_grid) {
        int i, x, y;
        // extract BOX screen vectors
        v1 = box[0];    // minx, miny = TL
        v2 = box[1];    // maxx, miny = TR
        v3 = box[2];    // maxx, maxy = BR
        v4 = box[3];    // minx, maxy = BL

        // top and bottom
        vecpt1.clear();
        vecpt2.clear();
        pmr = get_range( v1.GetX(), v2.GetX(), 10 );
        y = win_int(v1.GetY()) - (border / 2);
        for (i = 0; i <= pmr->_intervals; i++) {
            tick = pmr->_first + ((double)i * pmr->_d);
            x = win_int(tick);
            sprintf(nb, pmr->_format, tick);
            pt.x = x;
            pt.y = y;
            paint_string_at( hdc, x, y, nb );
            vecpt1.push_back(pt);
        }
        y = win_int(v3.GetY()) + (border / 2);
        for (i = 0; i <= pmr->_intervals; i++) {
            tick = pmr->_first + ((double)i * pmr->_d);
            x = win_int(tick);
            sprintf(nb, pmr->_format, tick);
            pt.x = x;
            pt.y = y;
            paint_string_at( hdc, x, y, nb );
            vecpt2.push_back(pt);
        }
        for (i = 0; i <= pmr->_intervals; i++) {
            paint_line(hdc, 
                vecpt1[i].x, vecpt1[i].y,
                vecpt2[i].x, vecpt2[i].y );
        }

        // left and right
        pmr = get_range( v1.GetY(), v3.GetY(), 10 );
        vecpt1.clear();
        vecpt2.clear();
        x = (border / 2);
        for (i = 0; i <= pmr->_intervals; i++) {
            tick = pmr->_first + ((double)i * pmr->_d);
            y = win_int(tick);
            sprintf(nb, pmr->_format, tick);
            pt.x = x;
            pt.y = y;
            paint_string_at( hdc, x, y, nb );
            vecpt1.push_back(pt);
        }
        x = win_int(v2.GetX()) + (border / 2);
        for (i = 0; i <= pmr->_intervals; i++) {
            tick = pmr->_first + ((double)i * pmr->_d);
            y = win_int(tick);
            sprintf(nb, pmr->_format, tick);
            pt.x = x;
            pt.y = y;
            paint_string_at( hdc, x, y, nb );
            vecpt2.push_back(pt);
        }
        for (i = 0; i <= pmr->_intervals; i++) {
            paint_line(hdc, 
                vecpt1[i].x, vecpt1[i].y,
                vecpt2[i].x, vecpt2[i].y );
        }

    } else if (g_paint_screen_grid) {
        int i, x1, x2, y1, y2;

        GetTicks * pgt = new GetTicks;
        vecpt1.clear();
        vecpt2.clear();

        //pgt->init_ticks( 0.0, (double)g_ClientRect.right, 10 );
        pgt->init_ticks( (double)g_ClientRect.left, (double)g_ClientRect.right, 10 );
        sprtf("Painting screen grid on %d,%d,%d,%d\n", 
            g_ClientRect.left, g_ClientRect.top, g_ClientRect.right, g_ClientRect.bottom);

        // set top and bottom, and do vertical columns - x
        y1 = g_ClientRect.top + (border / 2);
        y2 = g_ClientRect.bottom - (border / 2) - g_szTime.cy;
        for (i = 0; i <= pgt->_intervals; i++) {
            tick = pgt->_first + ((double)i * pgt->_d);
            x1 = win_int(tick) + g_ClientRect.left;
            pt.x = x1;
            pt.y = y1;
            if (x1 > g_ClientRect.right)
                break;
            sprtf("%d of %d: point %d,%d\n", (i + 1), pgt->_intervals, x1, y1);
            vecpt1.push_back(pt);
            pt.x = x1;
            pt.y = y2;
            vecpt2.push_back(pt);
        }

        // for (i = 0; i <= pgt->_intervals; i++) {
        for (i = 0; i < (int)vecpt1.size(); i++) {
            paint_line(hdc, 
                vecpt1[i].x, vecpt1[i].y,
                vecpt2[i].x, vecpt2[i].y );
        }

        y1 = g_ClientRect.top + (border / 2);
        y2 = g_ClientRect.bottom - (border / 2) - g_szTime.cy;
        for (i = 0; i < (int)vecpt1.size(); i++) {
            tick = pgt->_first + ((double)i * pgt->_d);
            x1 = win_int(tick) + g_ClientRect.left;
            sprintf(nb, pgt->_fmt, tick);
            paint_string_at( hdc, x1, y1, nb );
            paint_string_at( hdc, x1, y2, nb );
        }

        // right and left
        vecpt1.clear();
        vecpt2.clear();
        pgt->init_ticks((double)g_ClientRect.top, (double)g_ClientRect.bottom, 10 );

        x1 = g_ClientRect.left + (border / 2);
        x2 = g_ClientRect.right - (border / 2);
        for (i = 0; i <= pgt->_intervals; i++) {
            tick = pgt->_first + ((double)i * pgt->_d);
            y1 = win_int(tick) + g_ClientRect.top;
            pt.x = x1;
            pt.y = y1;
            if (y1 > g_ClientRect.bottom)
                break;
            vecpt1.push_back(pt);
            pt.x = x2;
            pt.y = y1;
            vecpt2.push_back(pt);
        }

        for (i = 0; i < (int) vecpt1.size(); i++) {
            paint_line(hdc, 
                vecpt1[i].x, vecpt1[i].y,
                vecpt2[i].x, vecpt2[i].y );
        }

        x1 = g_ClientRect.left + (border / 2);
        x2 = g_ClientRect.right - (border / 2);
        for (i = 0; i < (int)vecpt1.size(); i++) {
            tick = pgt->_first + ((double)i * pgt->_d);
            y1 = win_int(tick) + g_ClientRect.top;
            sprintf(nb, pgt->_fmt, tick);
            paint_string_at( hdc, x1, y1, nb );
            paint_string_at( hdc, x2, y1, nb );
        }
    }

    SetTextColor(hdc,crOld);
    if(hpOld)
        SelectObject(hdc, hpOld);
}


void paint_gray_box(HDC hdc, PVLIMS pvl)
{
    HPEN hp = g_hPenG1;
    HPEN hpOld = NULL;
    Vector2d v1, v2, v3, v4;
    int xc, yc;
    if(hp)
        hpOld = (HPEN)SelectObject(hdc, hp);

    set_gray_box_vectors(pvl);

    // extract BOX screen vectors
    v1 = box[0];    // minx, miny = TL
    v2 = box[1];    // maxx, miny = TR
    v3 = box[2];    // maxx, maxy = BR
    v4 = box[3];    // minx, maxy = BL

    paint_line( hdc, v1, v2 );  // screen TL -> TR
    paint_line( hdc, v2, v3 );  //        TR -> BR
    paint_line( hdc, v3, v4 );  //        BR -> BL
    paint_line( hdc, v4, v1 );  //        BL -> TL

    paint_background_grid(hdc, pvl);

    xc = win_int((v1.GetX() + v2.GetX()) / 2.0);
    yc = win_int((v1.GetY() + v3.GetY()) / 2.0);
    draw_cross( hdc, xc, yc, 10, RGB(255,0,0) );

    //ExtFloodFill( hdc,          // handle to DC
    //         xc, // int nXStart,      // starting x-coordinate 
    //         yc, // int nYStart,      // starting y-coordinate 
    //         g_crGray, // RGB(115,115,115),  //RGB(255,255,0), //COLORREF crColor, // fill boundary color
    //         FLOODFILLBORDER );  // UINT fuFillType   // fill type

    if(hpOld)
        SelectObject(hdc, hpOld);
}

void paint_label( HDC hdc, int x, int y )
{
    char * nb = getnxtbuf();
    HFONT hf = Get_Font();
    //int x1 = (int)(((double)(x - border) / factor) + 0.5);
    //int y1 = (int)(((double)(y - border) / factor) + 0.5);
    int x1 = SCN2X(x);
    int y1 = SCN2Y(y);
    int len = sprintf(nb,"(%d,%d)", x1, y1);
    if (hf) {
        HFONT hfOld = (HFONT)SelectObject(hdc,hf);
        ExtTextOut( hdc, x, y, 0, NULL, nb, len, NULL );
        if (hfOld)
            SelectObject(hdc,hfOld);
    }
}

void paint_factor(HDC hdc)
{
    HFONT hf = Get_Font();
    HFONT hfOld = NULL;
    if(hf)
        hfOld = (HFONT)SelectObject(hdc,hf);
    char * nb = getnxtbuf();
    int len = sprintf(nb, "f=%g %s ", factor, get_time_hhmmss_stg());
    GetTextExtentPoint32( hdc, nb, len, &g_szTime );
    //int x = g_ClientRect.right - g_szTime.cx;
    //int y = g_ClientRect.bottom - g_szTime.cy;
    int x = g_ClientRect.right - g_szTime.cx;
    int y = g_ClientRect.bottom - g_szTime.cy;
    ExtTextOut( hdc, x, y, 0, NULL, nb, len, NULL );
    if(hfOld)
        SelectObject(hdc,hfOld);
}

int try_to_find_fit( Vector2d v1, Vector2d v2, Vector2d v3, int mx, int my, int num )
{
    bool bret2 = false;
    bool bret = false;
    double x,y;
    Vector2d p;
    int count = 0;
    //if (!bret2) {
        x = (double)(mx + 1);
        y = (double)(my + 1);
        p.Set(x,y);
        bret2 = PointInTriangle2( p, v1, v2, v3 );
        if (bret2) {
            if (!bret) {
                sprtf(",\n but found ");
                bret = true;
            }
            sprtf("(%s)+1,+1 ", get_vpoint_stg(p));
            count++;
        }
    //}
    //if (!bret2) {
        x = (double)(mx - 1);
        y = (double)(my - 1);
        p.Set(x,y);
        bret2 = PointInTriangle2( p, v1, v2, v3 );
        if (bret2) {
            if (!bret) {
                sprtf(",\n but found ");
                bret = true;
            }
            sprtf("(%s)-1,-1 ", get_vpoint_stg(p));
            count++;
        }
    //}
    //if (!bret2) {
        x = (double)(mx + 1);
        y = (double)my;
        p.Set(x,y);
        bret2 = PointInTriangle2( p, v1, v2, v3 );
        if (bret2) {
            if (!bret) {
                sprtf(",\n but found ");
                bret = true;
            }
            sprtf("(%s)+1,0 ", get_vpoint_stg(p));
            count++;
        }
    //}
    //if (!bret2) {
        x = (double)(mx - 1);
        y = (double)my;
        p.Set(x,y);
        bret2 = PointInTriangle2( p, v1, v2, v3 );
        if (bret2) {
            if (!bret) {
                sprtf(",\n but found ");
                bret = true;
            }
            sprtf("(%s)-1,0 ", get_vpoint_stg(p));
            count++;
        }
    //}
    //if (!bret2) {
        x = (double)mx;
        y = (double)(my + 1);
        p.Set(x,y);
        bret2 = PointInTriangle2( p, v1, v2, v3 );
        if (bret2) {
            if (!bret) {
                sprtf(",\n but found ");
                bret = true;
            }
            sprtf("(%s)0,+1 ", get_vpoint_stg(p));
            count++;
        }
    //}
    //if (!bret2) {
        x = (double)mx;
        y = (double)(my - 1);
        p.Set(x,y);
        bret2 = PointInTriangle2( p, v1, v2, v3 );
        if (bret2) {
            if (!bret) {
                sprtf(",\n but found ");
                bret = true;
            }
            sprtf("(%s)0,-1 ", get_vpoint_stg(p));
            count++;
        }
    //}
    return count;
}

bool point_in_tri( Vector2d v1, Vector2d v2, Vector2d v3, int mx, int my, int num )
{
    Vector2d p((double)mx, (double)my);
    bool bret = PointInTriangle( p, v1, v2, v3 );
    bool bret2 = PointInTriangle2( p, v1, v2, v3 );
    if (bret != bret2) {
        int count = 0;
        sprtf("For pt %d,%d in Tri %d(%s) ", mx, my, num, get_vtri_stg( v1, v2, v3 ));
        if(bret) {
            sprtf("Equation 1 TRUE, 2 FALSE");
            count = try_to_find_fit( v1, v2, v3, mx, my, num );
        } else {
            sprtf("Equation 1 FALSE, 2 TRUE");
        }
        if(count)
            sprtf(" - %d in +/-1 pixel.\n", count);
        else
            sprtf(" \nand found none in +/-1 pixel range.\n");
    }
    return bret;
}

void DrawSelect( HDC hDC, LPRECT lprcClip )
{
#if 0 // ================
	TCHAR	szStr[80];
	//DWORD	dwExt;
	int		x, y, nLen, dx, dy;
	HDC		hDCBits;
	HBITMAP	hBitmap, hOldmap;
    SIZE     sz;
#endif // 0

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

#if 0 // ==================================
	hDCBits = CreateCompatibleDC( hDC );
   if(hDCBits) { // Output the text to a compatible DC
	   // Format the dimensions string
	   nLen = wsprintf( szStr,
		   L"%dx%d",
		   (lprcClip->right - lprcClip->left),
		   (lprcClip->bottom - lprcClip->top) );
	   // and center it in the rectangle
      GetTextExtentPoint32( hDC, szStr, nLen, &sz );
      dx = sz.cx;
      dy = sz.cy;
	   x  = (lprcClip->right  + lprcClip->left - dx) / 2;
	   y  = (lprcClip->bottom + lprcClip->top  - dy) / 2;
	   SetTextColor( hDCBits, RGB(255, 255, 255) );
	   SetBkColor(   hDCBits, RGB(  0,   0,   0) );
	   hBitmap = CreateBitmap( dx, dy, 1, 1, NULL );
	   if( hBitmap ) {
		   hOldmap = (HBITMAP)SelectObject( hDCBits, hBitmap );
		   ExtTextOut( hDCBits, 0, 0, 0, NULL, szStr, nLen, NULL );
		   BitBlt( hDC, x, y, dx, dy, hDCBits, 0, 0, SRCINVERT );
		   hOldmap = (HBITMAP)SelectObject( hDCBits, hOldmap );
		   DeleteObject( hBitmap );
	   }
	   DeleteDC( hDCBits );
   }

	rcPrevClip.left   = lprcClip->left;
	rcPrevClip.top    = lprcClip->top;
	rcPrevClip.right  = lprcClip->right;
	rcPrevClip.bottom = lprcClip->bottom;
#endif // 0

}

void Clear_Clip(void)
{
    rcClip.bottom = 0;
    rcClip.left   = 0;
    rcClip.right  = 0;
    rcClip.top    = 0;
}

void Draw_Clip( HDC hdc, int mx, int my )
{
    // remove any OLD (inverted) clip
    DrawSelect( hdc, &rcClip );
    // set NEW clip rectangle
    rcClip.left   = mx - half_clip;
    rcClip.right  = mx + half_clip;
    rcClip.top    = my - half_clip;
    rcClip.bottom = my + half_clip;
    // put it up
    DrawSelect( hdc, &rcClip );
}

typedef struct tagSNEAR {
    double dist;
    Vector2d v;
    Vector2dLine line;
}SNEAR, * PSNEAR;
typedef std::vector<SNEAR> VNEAR;

#define V2SCRN(v) v.Set(SCN2X(v.GetX()),SCN2Y(v.GetY()))

static char _s_blank[256] = { " " };
static int _s_blen = 1;
void SetMousePos(HWND hWnd, int mx, int my )
{
    static SIZE max_size = { 0, 0 };
    HDC hdc = GetDC(hWnd);
    bool found = false;
    Vector2d v1,v2,v3,v4,p;
    if (hdc) {
        HFONT hf = Get_Font();
        HFONT hfOld = NULL;
        if(hf)
            hfOld = (HFONT)SelectObject(hdc,hf);
        size_t max = tris.size() / 3;
        size_t i;
        char * nb1 = getnxtbuf();
        *nb1 = 0;

        Draw_Clip( hdc, mx, my );
        p.Set((double)mx,(double)my);
        //if (g_bShiftDown) {
            for (i = 0; i < max; i++) {
                v1 = tris[i*3 + 0];
                v2 = tris[i*3 + 1];
                v3 = tris[i*3 + 2];
                if ( point_in_tri( v1, v2, v3, mx, my, (int)(i+1) ) ) {
                    sprintf(nb1,"T%d", (int)(i + 1));
                    found = true;
                    break;
                }
            }
            if (!found) {
                v1 = box[0];
                v2 = box[1];
                v3 = box[2];
                v4 = box[3];
                if ( point_in_tri( v1, v2, v3, mx, my, 0 ) ||
                     point_in_tri( v1, v3, v4, mx, my, 0 ) )
                {
                    strcpy(nb1,"Box");
                    found = true;
                }
            }
            if (g_test_on_a_line) {
                Vector2dLine line;
                found = false;
                for (i = 0; i < max; i++) {
                    v1 = tris[i*3 + 0];
                    v2 = tris[i*3 + 1];
                    v3 = tris[i*3 + 2];
                    if ( point_on_line1( p, v1, v2 ) ) {
                        line.Set(v1,v2);
                        found = true;
                        break;
                    } else if ( point_on_line1( p, v2, v3 ) ) {
                        line.Set(v2,v3);
                        found = true;
                        break;
                    } else if ( point_on_line1( p, v3, v1 ) ) {
                        line.Set(v3,v1);
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    SNEAR sn;
                    //VNEAR vn;
                    Vector2d N;
                    double dist;
                    double min_dist = 1.0;
                    sn.dist = 999999999.9;
                    //vn.clear();
                    for (i = 0; i < max; i++) {
                        v1 = tris[i*3 + 0];
                        v2 = tris[i*3 + 1];
                        v3 = tris[i*3 + 2];
                        dist = DistToSegment( p, v1, v2, N );
                        if ((dist < min_dist)&&(dist < sn.dist)) {
                            sn.dist = dist;
                            sn.v.Set(N.GetX(),N.GetY());
                            sn.line.Set(v1,v2);
                            found = true;
                        }
                        dist = DistToSegment( p, v2, v3, N );
                        if ((dist < min_dist)&&(dist < sn.dist)) {
                            sn.dist = dist;
                            sn.v.Set(N.GetX(),N.GetY());
                            sn.line.Set(v2,v3);
                            found = true;
                        }
                        dist = DistToSegment( p, v3, v1, N );
                        if ((dist < min_dist)&&(dist < sn.dist)) {
                            sn.dist = dist;
                            sn.v.Set(N.GetX(),N.GetY());
                            sn.line.Set(v3,v1);
                            found = true;
                        }
                    }
                    if (found) {
                        v1 = sn.line.GetA();
                        v2 = sn.line.GetB();
                        //v1.Set(SCN2X(v1.GetX()),SCN2Y(v1.GetY()));
                        //v2.Set(SCN2X(v2.GetX()),SCN2Y(v2.GetY()));
                        V2SCRN(v1);
                        V2SCRN(v2);
                        line.Set(v1,v2);
                        sprintf(EndBuf(nb1)," N(%s) %s u.",
                            get_vline_stg(line),
                            get_formatted_double(sn.dist,3));
                    }
                    found = false;
                }
                if (found) {
                    v1 = line.GetA();
                    v2 = line.GetB();
                    v1.Set(SCN2X(v1.GetX()),SCN2Y(v1.GetY()));
                    v2.Set(SCN2X(v2.GetX()),SCN2Y(v2.GetY()));
                    line.Set(v1,v2);
                    sprintf(EndBuf(nb1)," On (%s)", get_vline_stg(line) );
                }
            }
        //}
        char * nb = getnxtbuf();
        int len = sprintf(nb, "%d,%d %s", mx, my, nb1);
        if (g_add_coords) {
            sprintf(EndBuf(nb)," (%d,%d)", SCN2X(mx), SCN2Y(my));
            len = (int)strlen(nb);
        }
        SIZE sz;
        GetTextExtentPoint32( hdc, nb, len, &sz );
        int x = 10;
        int y = g_ClientRect.bottom - sz.cy;
        if (sz.cx > max_size.cx) {
            sprtf("Increase max size from %d, to %d px with [%s]\n",
                max_size.cx, sz.cx, nb );
            max_size.cx = sz.cx;
        } else {
            char * bb = _s_blank;
            int blen = (int)strlen(bb);
            GetTextExtentPoint32( hdc, bb, blen, &sz );
            while (sz.cx < max_size.cx) {
                strcat(bb," ");
                blen = (int)strlen(bb);
                GetTextExtentPoint32( hdc, bb, blen, &sz );
            }
            ExtTextOut( hdc, x, y, 0, NULL, bb, blen, NULL );
            if (blen > _s_blen) {
                sprtf("Output blank [%s]%d, for max size %d px [%s]\n",
                    bb, blen, max_size.cx, nb );
            }
            _s_blen = blen;
        }
        ExtTextOut( hdc, x, y, 0, NULL, nb, len, NULL );
        if(hfOld)
            SelectObject(hdc,hfOld);
        ReleaseDC(hWnd,hdc);
    }
}


Vector2dVector contour;
Vector2dVector result;  //  Invoke the triangulator to triangulate this polygon.
void Init_Vector(void)
{
    PVLIMS pvl = &vlimits;
    contour.clear();
    result.clear();

    Get_Polygon_Vectors(contour);
    Get_Limits_of_Vector(contour, pvl );
    // Set_Factor( pvl );
    Triangulate::Process(contour,result);  // get triange results.
}

void paint_vector( HDC hdc )
{
    PVLIMS pvl = &vlimits;
    Vector2d v;
    HPEN hp1 = Get_hPenR2();
    HPEN hp2 = Get_hPenB1();
    HPEN hpOld = NULL;
    HFONT hf = Get_Font();
    paint_gray_box(hdc, pvl);
    if(hp1)
        hpOld = (HPEN)SelectObject(hdc, hp1);

    Vector2d v1, v2, v3;
    size_t max = contour.size();
    size_t i, i2;
    int x1, y1, x2, y2, x3, y3;

    for (i = 0; i < max; i++) {
        i2 = i + 1;
        if (i2 >= max)
            i2 = 0;
        v1 = contour[i];
        v2 = contour[i2];
        x1 = GETIX(v1);
        y1 = GETIY(v1);
        x2 = GETIX(v2);
        y2 = GETIY(v2);
        paint_line( hdc, x1, y1, x2, y2 );
        paint_label( hdc, x1, y1 );
    }

    if(hp2)
        SelectObject(hdc, hp2);

    max = result.size() / 3;
    tris.clear();
    for (i = 0; i < max; i++) {
        v1 = result[i*3 + 0];
        v2 = result[i*3 + 1];
        v3 = result[i*3 + 2];
        x1 = GETIX(v1);
        y1 = GETIY(v1);
        x2 = GETIX(v2);
        y2 = GETIY(v2);
        x3 = GETIX(v3);
        y3 = GETIY(v3);
        //paint_tri( hdc, x1, y1, x2, y2, x3, y3 );
        paint_tri( hdc, x1, y1, x2, y2, x3, y3, (int)(i + 1) );
        v.Set(x1,y1);
        tris.push_back(v);
        v.Set(x2,y2);
        tris.push_back(v);
        v.Set(x3,y3);
        tris.push_back(v);
    }

    paint_factor(hdc);

    if(hpOld)
        SelectObject(hdc, hpOld);
}

//	case WM_PAINT:
VOID Do_WM_PAINT( HWND hWnd )
{
	PAINTSTRUCT ps;

    HDC hdc = BeginPaint(hWnd, &ps);

    paint_vector( hdc );

    EndPaint(hWnd, &ps);

    Clear_Clip();
}

void Do_WM_MOUSEMOVE( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
   INT   xPos = GET_X_LPARAM(lParam);
   INT   yPos = GET_Y_LPARAM(lParam);
   SetMousePos( hWnd, xPos, yPos );
}

// eof - ap4_paint.cxx
