// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "targetver.h"

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

// TODO: reference additional headers your program requires here

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <iostream>
#include <conio.h>

// Socket programming library support
#include <ws2tcpip.h>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

