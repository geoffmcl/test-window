// ap4_ini.cxx

#include "testap4.hxx"

#ifdef UNICODE
#define m_atoi (int)_wtof
#define m_sscanf  swscanf
#else
#define m_atoi atoi
#define m_sscanf  sscanf
#endif

#define  it_None        0     // end of list
#define  it_Version     1
#define  it_String      2
#define  it_Int         3
#define  it_Bool        4
#define  it_WinSize     5     // special WINDOWPLACEMENT
#define  it_Rect        6
#define  it_Color       7
#ifdef   ADD_LINKED_LIST
#define  it_SList       8     // simple list
#endif // #ifdef   ADD_LINKED_LIST
#define  it_DWord       it_Int   // virtually the same in WIN32

#ifndef uint
typedef unsigned int uint;
#endif

typedef struct	tagINILST {	/* i */
	char *	i_Sect;
	char *	i_Item;
	uint	i_Type;
	char *	i_Deft;
	PBOOL   i_Chg;
	void *  i_Void;
	uint    i_Res1;
} INILST, * PINILST;

char m_szTmpBuf[1024];

// forward
void  GetModulePath( PTSTR lpb );

static	char	g_szDefIni[] = "testap4.ini";
char	g_szIni[264] = { "\0" };
char szBlk[] = "\0";
// Section
char szVer[] = "Version";
char szWin[] = "Window";
char szFil[] = "FileLists";
char szOpt[] = "Options";

// char szVer[] = L"Version";
char szDt[] = "Date";
char szCVer[] = VERS_DATE;
BOOL  bChgAll = FALSE;

// char szWin[] = L"Window";
char szShow[] = "ShowCmd";
char szMaxX[] = "MaxX";
char szMaxY[] = "MaxY";
char szMinX[] = "MinX";
char szMinY[] = "MinY";
char szLeft[] = "NormLeft";
char szTop[]  = "NormTop";
char szRite[] = "NormRight";
char szBot[]  = "NormBottom";

// section [Window]
char szZm[] = "Zoomed";
char szIc[] = "Iconic";
char szSz[] = "Size";

char szGOut[] = "OutSaved";
BOOL  gbGotWP = FALSE;
WINDOWPLACEMENT   g_sWP;
BOOL  bChgWP = FALSE;

BOOL  ChangedWP( WINDOWPLACEMENT * pw1, WINDOWPLACEMENT * pw2 )
{
   BOOL  bChg = FALSE;
   if( ( pw1->length != sizeof(WINDOWPLACEMENT) ) ||
       ( pw2->length != sizeof(WINDOWPLACEMENT) ) ||
       ( pw1->showCmd != pw2->showCmd ) ||
       ( pw1->ptMaxPosition.x != pw2->ptMaxPosition.x ) ||
       ( pw1->ptMaxPosition.y != pw2->ptMaxPosition.y ) ||
       ( !EqualRect( &pw1->rcNormalPosition, &pw2->rcNormalPosition ) ) )
   {
      bChg = TRUE;
   }
   return bChg;
}

VOID UpdateWP( HWND hwnd )
{
   WINDOWPLACEMENT wp;
   wp.length = sizeof(WINDOWPLACEMENT);
   if( GetWindowPlacement(hwnd,&wp) )
   {
      if( ChangedWP( &wp, &g_sWP ) ) {
         memcpy( &g_sWP, &wp, sizeof(WINDOWPLACEMENT) );
         g_sWP.length = sizeof(WINDOWPLACEMENT);
         bChgWP = TRUE;
      }
   }
}

INILST sIniLst[] = {
   { szVer, szDt,   it_Version,          szCVer,        &bChgAll, 0, 0 },  // CHANGE ALL!
   { szWin, szGOut, it_WinSize,   (PTSTR)&g_sWP,        &bChgWP, (PVOID)&gbGotWP, 0 },
   // last entry
   { 0,  0,  it_None, 0, 0, 0, 0 }
};

#define	GetStg( a, b )	\
	GetPrivateProfileString( a, b, &szBlk[0], lpb, 256, lpini )

PTSTR GetINIFile( void )
{
   PTSTR lpini = g_szIni;
   if( *lpini == 0 ) {
      GetModulePath( lpini );    // does   GetModuleFileName( NULL, lpini, 256 );
      lstrcat(lpini, g_szDefIni);
   }
   return lpini;
}

///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : IsRectOk
// Return type: BOOL 
// Argument   : PRECT pr
// Description: A rough verification that a RECTANGLE 'looks' OK
//              NOTE: Thre must be SOME value, and they must all
// be positive (or zero, but not all).
///////////////////////////////////////////////////////////////////////////////
BOOL  IsRectOk( PRECT pr )
{
   BOOL  bRet = FALSE;
   if( pr->left || pr->top || pr->right || pr->bottom )
   {
      // good start - some value is NOT zero
      // here I am ONLY accepting POSITIVE values
      if( ( pr->left >= 0 ) &&
          ( pr->top >= 0 ) &&
          ( pr->right >= 0 ) &&
          ( pr->bottom >= 0 ) )
      {
         bRet = TRUE;
      }
      else if(( pr->right  > 0 ) &&
              ( pr->bottom > 0 ) )
      {
         // this is a POSSIBLE candidate,
         // but limit neg x,y to say 5 pixels
         if(( pr->left > -5 ) &&
            ( pr->top  > -5 ) )
         {
            bRet = TRUE;
         }
      }
   }
   return bRet;
}

BOOL  ValidShowCmd( UINT ui )
{
   BOOL  bRet = FALSE;
   if( ( ui == SW_HIDE ) ||   //Hides the window and activates another window. 
       ( ui == SW_MAXIMIZE ) ||  //Maximizes the specified window. 
       ( ui == SW_MINIMIZE ) ||  //Minimizes the specified window and activates the next top-level window in the Z order. 
       ( ui == SW_RESTORE ) ||   //Activates and displays the window. If the window is minimized or maximized, the system restores it to its original size and position. An application should specify this flag when restoring a minimized window. 
       ( ui == SW_SHOW ) || //Activates the window and displays it in its current size and position.  
       ( ui == SW_SHOWMAXIMIZED ) || //Activates the window and displays it as a maximized window. 
       ( ui == SW_SHOWMINIMIZED ) || //Activates the window and displays it as a minimized window. 
       ( ui == SW_SHOWMINNOACTIVE ) ||  //Displays the window as a minimized window. 
       ( ui == SW_SHOWNA ) || //Displays the window in its current size and position. 
       ( ui == SW_SHOWNOACTIVATE ) ||  //Displays a window in its most recent size and position. 
       ( ui == SW_SHOWNORMAL ) )
       bRet = TRUE;
   return bRet;
}

///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : GotWP
// Return type: BOOL 
// Arguments  : LPTSTR pSect
//            : LPTSTR pDef
//            : LPTSTR lpb
//            : LPTSTR lpini
// Description: Read in a special BLOCK of window palcement item from
//              the INI file. This is in its own [section].
///////////////////////////////////////////////////////////////////////////////
BOOL  GotWP( PTSTR pSect, PTSTR pDef, PTSTR lpb, PTSTR lpini )
{
   BOOL  bRet = FALSE;
   WINDOWPLACEMENT   wp;
   WINDOWPLACEMENT * pwp = (WINDOWPLACEMENT *)pDef;
   if( !pwp )
      return FALSE;

   *lpb = 0;
   GetStg( pSect, szShow ); // = "ShowCmd";
   if( *lpb == 0 )
      return FALSE;
   wp.showCmd = m_atoi(lpb);
   if( !ValidShowCmd( wp.showCmd ) )
      return FALSE;

   *lpb = 0;
   GetStg( pSect, szMaxX );
   if( *lpb == 0 )
      return FALSE;
   wp.ptMaxPosition.x = m_atoi(lpb);
   *lpb = 0;
   GetStg( pSect, szMaxY );
   if( *lpb == 0 )
      return FALSE;
   wp.ptMaxPosition.y = m_atoi(lpb);

   *lpb = 0;
   GetStg( pSect, szMinX );
   if( *lpb == 0 )
      return FALSE;
   wp.ptMinPosition.x = m_atoi(lpb);
   *lpb = 0;
   GetStg( pSect, szMinY );
   if( *lpb == 0 )
      return FALSE;
   wp.ptMinPosition.y = m_atoi(lpb);

   *lpb = 0;
   GetStg( pSect, szLeft );   // = "NormLeft";
   if( *lpb == 0 )
      return FALSE;
   wp.rcNormalPosition.left = m_atoi(lpb);
   *lpb = 0;
   GetStg( pSect, szTop ); // = "NormTop";
   if( *lpb == 0 )
      return FALSE;
   wp.rcNormalPosition.top = m_atoi(lpb);
   *lpb = 0;
   GetStg( pSect, szRite );   // = "NormRight";
   if( *lpb == 0 )
      return FALSE;
   wp.rcNormalPosition.right = m_atoi(lpb);
   *lpb = 0;
   GetStg( pSect, szBot ); //  = "NormBottom";
   if( *lpb == 0 )
      return FALSE;
   wp.rcNormalPosition.bottom = m_atoi(lpb);

   wp.flags = 0;
   wp.length = sizeof(WINDOWPLACEMENT);

   memcpy( pwp, &wp, sizeof(WINDOWPLACEMENT) );
   bRet = TRUE;
   return bRet;
}

///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : IsYes
// Return type: BOOL 
// Argument   : LPTSTR lpb
// Description: Return TRUE if the string given is "Yes" OR "On"
///////////////////////////////////////////////////////////////////////////////
BOOL  IsYes( PTSTR lpb )
{
   BOOL  bRet = FALSE;
   if( ( strcmpi(lpb, "YES") == 0 ) ||
       ( strcmpi(lpb, "ON" ) == 0 ) )
       bRet = TRUE;
   return bRet;
}

BOOL  IsNo( PTSTR lpb )
{
   BOOL  bRet = FALSE;
   if( ( strcmpi(lpb, "NO" ) == 0 ) ||
       ( strcmpi(lpb, "OFF") == 0 ) )
       bRet = TRUE;
   return bRet;
}

void Post_INI_Read( void )
{
   //if( IsFileListEmpty() )
   //   Add2FileListHead( L"C:\\GTools\\tools\\dv32\\Inst\\TEMPB208.BMP" );
}

void ReadINI( void )
{
   PINILST  plst = &sIniLst[0];
   LPTSTR   lpb = &m_szTmpBuf[0];
   DWORD    dwt;
   LPTSTR   pSect, pItem, pDef;
   PBOOL    pb, pbd;
   PINT     pi;
   INT      i, icnt;
   PRECT    pr, pr1;
   PTSTR    lpini = GetINIFile();
   icnt = 0;
   while( ( dwt = plst->i_Type ) != it_None )
   {
      pSect = plst->i_Sect;   // pointer to [section] name
      pItem = plst->i_Item;   // pointer to itme in section
      pDef  = plst->i_Deft;   // pointer to destination
      pb    = plst->i_Chg;    // pointer to CHANGE flag
      *lpb = 0;
#ifdef   ADD_LINKED_LIST
      if( dwt == it_SList )
      {
         // must prepare multiple fetch items
         for( i = 0; i < (INT)plst->i_Res1; i++ )
         {
            wsprintf(lpb, pItem, (i+1));
            GetStg( pSect, lpb );
            Add2SimpleList( (PLE)pDef, lpb );
         }
         // put a value in lpb, so change will NOT be set
         wsprintf(lpb, pItem, (i+1));
      }
      else
#endif // #ifdef   ADD_LINKED_LIST
      {
         GetStg( pSect, pItem );
      }
      if( *lpb )
      {
         switch(dwt)
         {
         case it_Version:
            if( strcmp( pDef, lpb ) )
               *pb = TRUE;
            break;
         case it_String:
            if( strcmp( pDef, lpb ) )
               lstrcpy( pDef, lpb );
            break;
         case it_Int:   // also doubles as a DWORD in WIN32
            pi = (PINT)pDef;
            i = (int)m_atoi(lpb);
            *pi = i;
            break;
         case it_Bool:
            pbd = (PBOOL)pDef;
            if( IsYes(lpb) )
               *pbd = TRUE;
            else if( IsNo(lpb) )
               *pbd = FALSE;
            else
               *pb = TRUE;
            break;
         case it_WinSize:     // special WINDOWPLACEMENT
            if( ( IsYes(lpb) ) &&
                ( GotWP( pSect, pDef, lpb, lpini ) ) )
            {
               // only if SAVED is yes, AND then success
               pb = (PBOOL) plst->i_Void;
               *pb = TRUE; // set that we have a (valid!) WINDOWPLACEMENT
            }
            else
               *pb = TRUE;
            break;
         case it_Rect:
            pr = (PRECT) &lpb[ lstrlen(lpb) + 2 ];
            pr->left = 0;
            pr->top = 0;
            pr->right = 0;
            pr->bottom = 0;
            if( ( m_sscanf(lpb, "%d,%d,%d,%d", &pr->left, &pr->top, &pr->right, &pr->bottom ) == 4 ) &&
                ( IsRectOk( pr ) ) )
            {
               pr1 = (PRECT)pDef;
               pr1->left = pr->left;
               pr1->top  = pr->top;
               pr1->right = pr->right;
               pr1->bottom = pr->bottom;
               pb = (PBOOL) plst->i_Void;
               if(pb)
                  *pb = TRUE;
            }
            break;
         case it_Color:
            pr = (PRECT) &lpb[ lstrlen(lpb) + 2 ];
            pr->left = 0;
            pr->top = 0;
            pr->right = 0;
            pr->bottom = 0;
            if( m_sscanf(lpb, "%d,%d,%d", &pr->left, &pr->top, &pr->right ) == 3 )
            {
               COLORREF * pcr = (COLORREF *)pDef;
               *pcr = RGB( pr->left, pr->top, pr->right );
            }
            break;
#ifdef   ADD_LINKED_LIST
         case it_SList:
            // it has all been done
            break;
#endif // #ifdef   ADD_LINKED_LIST
         }
      }
      else
      {
         *pb = TRUE;    // can only SET change
         if( icnt == 0 )
            sprtf( "ADVICE: bChgAll has been set!\n" );

      }

      plst++;
      icnt++;
   }

   Post_INI_Read();
}

#define  WI( a, b )\
   {  sprintf(lpb, "%d", b );\
      WritePrivateProfileString(pSect, a, lpb, lpini ); }

void WriteINI( void )
{
   LPTSTR   lpb   = &m_szTmpBuf[0];
   PINILST  plst  = &sIniLst[0];
   DWORD    dwt;
   LPTSTR   pSect, pItem, pDef;
   BOOL     ball = *(plst->i_Chg);
   BOOL     bchg;
   PINT     pint;
   PBOOL    pb;
   WINDOWPLACEMENT * pwp;
   PRECT    pr;
   PTSTR    lpini = GetINIFile();

   if( ball )
   {
      // clear down the current INI file
      while( ( dwt = plst->i_Type ) != it_None )
      {
         pSect = plst->i_Sect;
         WritePrivateProfileString( pSect,		// Section
            NULL,    // Res.Word
            NULL,    // String to write
            lpini );	// File Name
         plst++;
      }
   }
   plst = &sIniLst[0];  // start of LIST

   while( ( dwt = plst->i_Type ) != it_None )
   {
      pSect = plst->i_Sect;
      pItem = plst->i_Item;
      pDef  = plst->i_Deft;
      bchg  = *(plst->i_Chg);
      if( ball || bchg )
      {
         *lpb = 0;
         switch(dwt)
         {
         case it_Version:
            lstrcpy(lpb, pDef);
            break;
         case it_String:
            lstrcpy(lpb, pDef);
            break;
         case it_Int:
            pint = (PINT)pDef;
            sprintf(lpb, "%d", *pint );
            break;
         case it_Bool:
            pb = (PBOOL)pDef;
            if( *pb )
               strcpy(lpb, "Yes");
            else
               strcpy(lpb, "No");
            break;
         case it_WinSize:     // special WINDOWPLACEMENT
            pb = (PBOOL)plst->i_Void;
            pwp = (WINDOWPLACEMENT *)pDef;
            if( ( pwp->length == sizeof(WINDOWPLACEMENT) ) &&
                ( ValidShowCmd( pwp->showCmd ) ) )
            {
               WI( szShow, pwp->showCmd );
               WI( szMaxX, pwp->ptMaxPosition.x );
               WI( szMaxY, pwp->ptMaxPosition.y );
               WI( szMinX, pwp->ptMinPosition.x );
               WI( szMinY, pwp->ptMinPosition.y );
               WI( szLeft, pwp->rcNormalPosition.left );
               WI( szTop,  pwp->rcNormalPosition.top  );
               WI( szRite, pwp->rcNormalPosition.right);
               WI( szBot,  pwp->rcNormalPosition.bottom);
               strcpy(lpb, "Yes");
            }
            else
               strcpy(lpb, "No");
            break;
         case it_Rect:
            pr = (PRECT)pDef;
            sprintf(lpb, "%d,%d,%d,%d", pr->left, pr->top, pr->right, pr->bottom );
            break;
         case it_Color:
            {
               COLORREF * pcr = (COLORREF *)pDef;
               sprintf(lpb, "%d,%d,%d",
                  GetRValue(*pcr), GetGValue(*pcr), GetBValue(*pcr) );
            }
            break;
#ifdef   ADD_LINKED_LIST
         case it_SList:
            {
               PLE      plh = (PLE)pDef;
               PLE      pln;
               dwt = 0;
               Traverse_List( plh, pln )
               {
                  PFILLST pfl = (PFILLST)pln;
                  wsprintf(lpb, pItem, (dwt+1)); // build the reserved word
                  WritePrivateProfileString(
						   pSect,		// Section
						   lpb,		// Res.Word
                     &pfl->file[0],		// String to write
                     lpini );	// File Name
                  dwt++;      // bump to next LIST number

                  if( dwt >= plst->i_Res1 )  // do NOT exceed maximum
                     break;
               }
               *lpb = 0;   // ALL DONE - list is written
            }
            break;
#endif // #ifdef   ADD_LINKED_LIST
         }

         *(plst->i_Chg) = FALSE;

         if( *lpb )
         {
            WritePrivateProfileString(
						pSect,		// Section
						pItem,		// Res.Word
						lpb,		// String to write
						lpini );	// File Name
         }
      }
      plst++;
   }
}

BOOL  Chk4Debug( PTSTR lpd )
{
   BOOL     bret = FALSE;
   PTSTR ptmp = &m_szTmpBuf[0];
   PTSTR   p;
   DWORD  dwi;

   strcpy(ptmp, lpd);
   dwi = (DWORD)strlen(ptmp);
   if(dwi)
   {
      dwi--;
      if(ptmp[dwi] == '\\')
      {
         ptmp[dwi] = 0;
         p = strrchr(ptmp, '\\');
         if(p)
         {
            p++;
            if( strcmpi(p, "DEBUG") == 0 )
            {
               *p = 0;
               strcpy(lpd,ptmp);    // use this
               bret = TRUE;
            }
         }
      }
   }
   return bret;
}

void  GetModulePath( PTSTR lpb )
{
   LPTSTR   p;
   GetModuleFileName( NULL, lpb, 256 );
   p = strrchr( lpb, '\\' );
   if( p )
      p++;
   else
      p = lpb;
   *p = 0;
#ifndef  NDEBUG
   Chk4Debug( lpb );
#endif   // !NDEBUG

}

// eof - ap4_ini.cxx
