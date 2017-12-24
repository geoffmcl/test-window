// ap4_vers.hxx
#ifndef _ap4_vers_hxx_
#define _ap4_vers_hxx_

#define VERS_DATE   "31 August, 2012"
#define ADD_MOUSE_CAPTURE
// Add mouse capture and follow on left click and report pixel color on release

// VERS_DATE   "22 August, 2012"

#define  ADD_TIMER1
#define ADD_LISTVIEW

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#endif // #ifndef _ap4_vers_hxx_
// eof - ap4_vers.hxx
