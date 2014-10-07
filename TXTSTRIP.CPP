/*-------------------------------------------------------------
           txtstrip.cpp   --   (c) Bruce Henry 1996
---------------------------------------------------------------*/


#define INCL_WINFRAMEMGR        // Needed for the WS_ equates
#define INCL_WINSYS
#define INCL_WINSTATICS          // Needed for SS_ equates
#define  INCL_GPIBITMAPS        // For RGB structure definition
#define  INCL_WINWINDOWMGR    // for WinSetWindowPos etc.

#include <os2.h>
#include <string.h>
#include "txtstrip.h"

    //-----------------------------------------------------------
    //   The constructor to set up and register the window
    //-----------------------------------------------------------
      textstrip::textstrip(HWND hwndframe, HWND hwndOwner, USHORT nBars)
      {
      // NOTE: The IDs for these two windows are hard-coded so that the handles can
      //         can be retrieved using the WinWindowFromID routine in the new
      //         Frame procedure below.  Since it is a static method (it would be shared
      //         among other objects of this class) it can't access variables particular to
      //         this object.  This means that this class can only be used once in any
      //         program.

         hwndFrame=hwndframe;

         hwndTextstrip1 = WinCreateWindow (
                            hwndFrame,       // Parent window handle
                            WC_STATIC,         // Window class
                            " yy ",              // Window text
                            WS_PARENTCLIP |  // Window style   (can be 'or'ed
                            WS_CLIPSIBLINGS |  //     with window class specific options)
                            WS_VISIBLE |
                            SS_TEXT | DT_LEFT | DT_VCENTER,
                            0,              // Initial position of window
                            0,
                            0,              // Initial size of window
                            0,
                            hwndOwner,         // Owner window handle
                            HWND_BOTTOM,     // Placement window handle
                            598,                  // window ID (hard-coded)
                            NULL,                 // Control data
                            NULL        );        // Presentation parameters

         if (nBars == 2)
         {
            hwndTextstrip2 = WinCreateWindow (
                            hwndFrame,       // Parent window handle
                            WC_STATIC,         // Window class
                            " xx ",              // Window text
                            WS_PARENTCLIP |  // Window style   (can be 'or'ed
                            WS_CLIPSIBLINGS |  //     with window class specific options)
                            WS_VISIBLE |
                            SS_TEXT | DT_LEFT | DT_VCENTER,
                            0,              // Initial position of window
                            0,
                            0,              // Initial size of window
                            0,
                            hwndOwner,         // Owner window handle
                            HWND_BOTTOM,     // Placement window handle
                            599,              // window ID (hard-coded)
                            NULL,                 // Control data
                            NULL        );        // Presentation parameters
         }

      //-------------------------------------------------------------
      // Set foreground colours and background colours and a font for our window(s).
      //-------------------------------------------------------------

      RGB rgb;
      rgb.bBlue=220;
      rgb.bGreen=220;
      rgb.bRed=220;

      WinSetPresParam (hwndTextstrip1, PP_BACKGROUNDCOLOR, (ULONG) sizeof(RGB),
                           (PVOID)&rgb);

      rgb.bBlue=68;
      rgb.bGreen=4;
      rgb.bRed=4;

      WinSetPresParam (hwndTextstrip1, PP_FOREGROUNDCOLOR, (ULONG) sizeof(RGB),
                           (PVOID)&rgb);

      pszfontnamesize = new CHAR [13];

      strcpy (pszfontnamesize, "5.System VIO");
      WinSetPresParam (hwndTextstrip1, PP_FONTNAMESIZE, (ULONG) strlen(pszfontnamesize) + 1,
                           (PSZ)pszfontnamesize);

      // If there are two windows called for, set the colours and fonts the same for the second.

      if (nBars == 2)
      {
         rgb.bBlue=220;
         rgb.bGreen=188;
         rgb.bRed=140;

         WinSetPresParam (hwndTextstrip2, PP_FONTNAMESIZE, (ULONG) strlen(pszfontnamesize) + 1,
                                         (PSZ)pszfontnamesize);

         WinSetPresParam (hwndTextstrip2, PP_BACKGROUNDCOLOR, (ULONG) sizeof(RGB),
                                         (PVOID)&rgb);

         rgb.bBlue=24;
         rgb.bGreen=4;
         rgb.bRed=4;

         WinSetPresParam (hwndTextstrip2, PP_FOREGROUNDCOLOR, (ULONG) sizeof(RGB),
                              (PVOID)&rgb);
      }

      // The Frame window must be subclassed to keep this window from being
      // overwritten by the client area.  Store the address of the old procedure in
      // STATICPARAMS structure defined in the header.  Also store the number
      // of status bars required there.

      staticparams.OldFrameProc = WinSubclassWindow (hwndFrame, NewFrameProc);
      staticparams.cWindows = nBars;

      PSTATICPARAMS pstaticparams = &staticparams;

      // Use the window words for the first window to pass the old procedure pointer.
      // It will be extracted in the new procedure.

      WinSetWindowULong (hwndTextstrip1, QWL_USER, (ULONG) pstaticparams);

      }

      //-------------------------------
      // A method to change the title bar text
      //-------------------------------
      void textstrip::newtitle1 (PSZ pNewtext)
      {
          WinSetWindowText (hwndTextstrip1, pNewtext);

      }

      //-------------------------------
      // A method to change the title bar text
      //-------------------------------
      void textstrip::newtitle2 (PSZ pNewtext)
      {
          WinSetWindowText (hwndTextstrip2, pNewtext);

      }

      //-------------------------------------------------------------
      // This whole procedure is written up in the article "Adding New Frame Control
      // Elements".  The biggest problem that could arise from this would be if another
      // subclassing of the Frame window was attempted that tried to grab these same
      // messages.  The result could be not as expected.
      //--------------------------------------------------------------
      static MRESULT EXPENTRY textstrip::NewFrameProc (HWND hwnd, ULONG msg,
                                                         MPARAM mp1, MPARAM mp2)
      {
         PFNWP OldFrameProc;
         HWND handle1, handle2;
         PSTATICPARAMS psparams;
         handle1 = WinWindowFromID (hwnd, 598);

         psparams = (PSTATICPARAMS) WinQueryWindowULong (handle1, QWL_USER);

         OldFrameProc = psparams->OldFrameProc;

         switch (msg)
         {
            case WM_QUERYFRAMECTLCOUNT:
            {
               USHORT itemcount;
               itemcount = SHORT1FROMMR (OldFrameProc (hwnd, msg, mp1, mp2));
               if (psparams->cWindows == 2) itemcount++;
               return ((MRESULT) ++itemcount);
            }

            case WM_FORMATFRAME:
            {
               USHORT usClient = 0;
               USHORT itemcount = SHORT1FROMMR (OldFrameProc (hwnd, msg, mp1, mp2));
               PSWP pswp = (PSWP) PVOIDFROMMP (mp1);

               while (pswp [usClient].hwnd != WinWindowFromID (hwnd, FID_CLIENT))
                        ++usClient;
               pswp [itemcount].hwnd = handle1;   // First textstrip object handle
               pswp [itemcount].fl = SWP_SIZE | SWP_MOVE | SWP_SHOW;
               pswp [itemcount].cy = 17;
               pswp [itemcount].cx = pswp [usClient].cx;
               pswp [itemcount].x = pswp [usClient].x;
               pswp [itemcount].y = pswp [usClient].y;
               pswp [itemcount].hwndInsertBehind = HWND_TOP;

               pswp [usClient].cy = pswp [usClient].cy - pswp [itemcount].cy;
               pswp [usClient].y = pswp [usClient].y + pswp [itemcount].cy;

               if (psparams->cWindows == 2)
               {
                  handle2 = WinWindowFromID (hwnd, 599);
                  itemcount++;

                  pswp [itemcount].hwnd = handle2;   // Second textstrip object handle
                  pswp [itemcount].fl = SWP_SIZE | SWP_MOVE | SWP_SHOW;
                  pswp [itemcount].cy = 17;
                  pswp [itemcount].cx = pswp [usClient].cx;
                  pswp [itemcount].x = pswp [usClient].x;
                  pswp [itemcount].y = pswp [usClient].y;
                  pswp [itemcount].hwndInsertBehind = HWND_TOP;

                  pswp [usClient].cy = pswp [usClient].cy - pswp [itemcount].cy;
                  pswp [usClient].y = pswp [usClient].y + pswp [itemcount].cy;

               }
               return (MPFROMSHORT (++itemcount));
            }
            default:
               return OldFrameProc (hwnd, msg, mp1, mp2);
         }
      }

      //------------------------------
      // The required destructor
      //------------------------------
     textstrip::~textstrip()
     {
         delete pszfontnamesize;
         DestroyWindow();
     }
      //--------------------------------------------------------------------
      // This method to destroy the window will be used by the destructor.
      //--------------------------------------------------------------------
      void textstrip::DestroyWindow ()
         {
            WinSubclassWindow (hwndFrame, staticparams.OldFrameProc);
            WinDestroyWindow (hwndTextstrip1);
            if (staticparams.cWindows == 2) WinDestroyWindow (hwndTextstrip2);
         }

