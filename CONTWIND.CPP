/*---------------------------------------------------------
   CONTWIND -- An instance of a CONTWINDOW class
                (c) Bruce Henry 1996
   This window contains a static titlebar and a static frame.  It uses
   the two id's following the main id for these add-ons so they must be
   reserved.
  ---------------------------------------------------------*/
#define INCL_WINFRAMEMGR        // Needed for the WS_ equates
#define INCL_WINSTDCNR           // Needed for the CCS_ equates
#define INCL_WINSYS
#define INCL_WINSTATICS          // Needed for SS_ equates
#define  INCL_GPIBITMAPS        // For RGB structure definition
#define  INCL_WINWINDOWMGR    // for WinSetWindowPos etc.

#include <os2.h>
#include <i86.h>
#include <string.h>
#include "contwind.h"

    //-----------------------------------------------------------
    //   The constructor to set up and register the window
    //-----------------------------------------------------------
      contwindow::contwindow(HWND hwndCLIENT, ULONG idCHILD, LONG xorg, LONG yorg,
                                 LONG xsize, LONG ysize, LONG cx, LONG cy, PSZ title)
      {
         xorigin=xorg;
         yorigin=yorg;
         width=xsize;
         height=ysize;
         startcx=cx;
         startcy=cy;
         hwndClient=hwndCLIENT;

      // NOTE:
      // In order to create a container with a static window frame around it, the container
      // must be created first and the static window second.  If they are created in the
      // reverse order, the static window will not allow input to the container (ie. mouse
      // clicks).

        hwndContainer = WinCreateWindow (
                            hwndCLIENT,       // Parent window handle
                            WC_CONTAINER,         // Window class
                            NULL,              // Window text
                            WS_PARENTCLIP |  // Window style   (can be 'or'ed
                            WS_CLIPSIBLINGS |  //     with window class specific options)
                            CCS_READONLY | CCS_MULTIPLESEL,
                            0, 0,              // Initial position of window - not important
                            100,100,         // Initial size of window - not important (invisible)
                            hwndCLIENT,         // Owner window handle
                            HWND_TOP,     // Placement window handle
                            idCHILD + 2,              // Child window ID +2 (keep unique)
                            NULL,                 // Control data
                            NULL        );        // Presentation parameters

        hwndChildTitle = WinCreateWindow (
                            hwndCLIENT,       // Parent window handle
                            WC_STATIC,         // Window class
                            title,              // Window text
                            WS_PARENTCLIP |  // Window style   (can be 'or'ed
                            WS_CLIPSIBLINGS |  //     with window class specific options)
                            SS_TEXT | DT_LEFT | DT_VCENTER,
                            0, 0,              // Initial position of window - not important
                            100,100,         // Initial size of window - not important (invisible)
                            hwndCLIENT,         // Owner window handle
                            HWND_BOTTOM,     // Placement window handle
                            idCHILD +1,              // Child window ID +1 (keep unique)
                            NULL,                 // Control data
                            NULL        );        // Presentation parameters

         hwndChildWindow = WinCreateWindow (
                            hwndCLIENT,       // Parent window handle
                            WC_STATIC,         // Window class
                            NULL,              // Window text
                            WS_PARENTCLIP | // Window style   (can be 'or'ed
                            WS_CLIPSIBLINGS | //     with window class specific options)
                            SS_FGNDFRAME,
                            0, 0,              // Initial position of window - not important
                            100,100,         // Initial size of window - not important (invisible)
                            hwndCLIENT,         // Owner window handle
                            HWND_BOTTOM,     // Placement window handle
                            idCHILD,              // Child window ID
                            NULL,                 // Control data
                            NULL        );        // Presentation parameters

      //----------------------------------------------
      // Set some background colours for the windows
      //----------------------------------------------
      RGB rgb;
      rgb.bBlue=220;
      rgb.bGreen=188;
      rgb.bRed=140;

      WinSetPresParam (hwndChildTitle, PP_BACKGROUNDCOLOR, (ULONG) sizeof(RGB),
                           (PVOID)&rgb);
      rgb.bBlue=24;
      rgb.bGreen=4;
      rgb.bRed=4;

      WinSetPresParam (hwndChildTitle, PP_FOREGROUNDCOLOR, (ULONG) sizeof(RGB),
                           (PVOID)&rgb);

      pszfontnamesize = new CHAR [13];
      strcpy (pszfontnamesize, "5.System VIO");
      WinSetPresParam (hwndChildTitle, PP_FONTNAMESIZE, ULONG (strlen(pszfontnamesize) + 1),
                           (PSZ)pszfontnamesize);

      }

     //-------------------------------------------------------------
     //  A method to return the Container Window Handle to the caller
     //-------------------------------------------------------------
      HWND contwindow::getContainerHWND()
      {
         return hwndContainer;
      }

     //-------------------------------------------------------------
     //  A method to return the Titlebar Window Handle to the caller
     //-------------------------------------------------------------
      HWND contwindow::getTitlebarHWND()
      {
         return hwndChildTitle;
      }
      //------------------------------------------------------------------
      // Reset the window dimensions in a static client.  This requires the original size
      // minus the space for the new new components.  The original size must be used since
      // it is used in ratio formulae which are used to derive the new container dimensions.
      //------------------------------------------------------------------
      VOID contwindow::rejig( LONG xsize, LONG ysize)
      {
         width = xsize;
         height = ysize;
      }

      //---------------------------------------------------------------------
      // A method to resize the various window parts to fit a resized client.
      // The size of static components (eg. toolbar) that must share the client space are needed.
      // An allowance is made here to leave room for a window with a static size component
      // in the height or width calculations. This static component is passed in the 'reservhorz'
      // and 'reservvert' parameters.
      // This is done by comparing a ratio of the original available space and the space
      // taken from the client space for this container, and the new available space and the
      // value 's' of the new size to be taken from the client space.
      //---------------------------------------------------------------------
      void contwindow::resizewin(INT reservhorz, INT reservvert)
      {
         INT orgAvailablecx=startcx-reservhorz;
         INT orgAvailablecy=startcy-reservvert;
         WinQueryWindowPos (hwndClient,(PSWP) &dirswp);

         INT newAvailablecx=dirswp.cx-reservhorz;
         INT tempwidth;
         tempwidth=(newAvailablecx * width) / orgAvailablecx;
         if (tempwidth*orgAvailablecx < newAvailablecx * width) tempwidth += 1;
                             // I did a little make-up here to keep the windows close to each other.
                             // The 'if' statement adds one pixel if there was a large remainder.

         INT newAvailablecy=dirswp.cy-reservvert;
         INT tempheight;
         tempheight=(newAvailablecy*height)/orgAvailablecy;
         if (tempheight*orgAvailablecy < newAvailablecy*height) tempheight +=1;// testing these lines Oct 8/96
                             // I did a little make-up here to keep the windows close to each other.
                             // The 'if' statement adds one pixel if there was a large remainder.

         INT tempxorigin, tempyorigin;
         if (!xorigin) tempxorigin=0;
         else
         {
            tempxorigin = dirswp.cx - reservhorz - tempwidth;
            tempwidth = dirswp.cx - reservhorz -tempxorigin;
         }
         if (!yorigin) tempyorigin = 0;
         else
         {
            tempyorigin = dirswp.cy - reservvert - tempheight;
            tempheight = dirswp.cy - reservvert - tempyorigin;
         }
         tempheight = dirswp.cy - reservvert - tempyorigin;

         WinSetWindowPos (hwndContainer, HWND_BOTTOM, tempxorigin + 1, tempyorigin + 1,
                              tempwidth - 2, tempheight -19, SWP_MOVE|SWP_SHOW|SWP_SIZE);
         WinSetWindowPos (hwndChildWindow, HWND_BOTTOM, tempxorigin, tempyorigin,
                              tempwidth, tempheight, SWP_MOVE|SWP_SHOW|SWP_SIZE);
         WinSetWindowPos (hwndChildTitle, HWND_BOTTOM, tempxorigin + 1, tempheight -18,
                              tempwidth - 2, 17, SWP_MOVE|SWP_SHOW|SWP_SIZE);
      }
      //-------------------------------
      // A method to change the title bar text
      //-------------------------------
      void contwindow::newtitle (PSZ pNewtext)
      {
          WinSetWindowText (hwndChildTitle, pNewtext);

      }
      //-------------------------------
      // A method to change the title bar font
      //-------------------------------
      void contwindow::titlefont (PSZ pszfont)
      {
      WinSetPresParam (hwndChildTitle, PP_FONTNAMESIZE, ULONG (strlen (pszfont) + 1),
                           (PSZ) pszfont);

      }

      //--------------------------------
      // A method to change the title bar colors
      //--------------------------------
      void contwindow::titleparams (ULONG frgnd, ULONG bkgnd)
      {
         RGB rgb;
         ULONG temp = frgnd;
         temp = frgnd;
         rgb.bRed = BYTE (temp >> 16);
         temp = frgnd & 0xFF00;
         rgb.bGreen = BYTE (temp >> 8);
         temp = frgnd & 0xFF;
         rgb.bBlue = BYTE (temp);

         WinSetPresParam (hwndChildTitle, PP_FOREGROUNDCOLOR, (ULONG) sizeof(RGB),
                              (PVOID)&rgb);

         temp = bkgnd;
         rgb.bRed = BYTE (temp >> 16);
         temp = bkgnd & 0xFF00;
         rgb.bGreen = BYTE (temp >> 8);
         temp = bkgnd & 0xFF;
         rgb.bBlue = BYTE (temp);

         WinSetPresParam (hwndChildTitle, PP_BACKGROUNDCOLOR, (ULONG) sizeof(RGB),
                              (PVOID)&rgb);

      }
      //------------------------------
      // The required destructor
      //------------------------------
     contwindow::~contwindow()
     {
         delete pszfontnamesize;
         DestroyWindow();
     }
      //--------------------------------------------------------------------
      // This method to destroy the window will be used by the destructor.
      //--------------------------------------------------------------------
      void contwindow::DestroyWindow ()
         {
            WinDestroyWindow (hwndContainer);
            WinDestroyWindow (hwndChildTitle);
            WinDestroyWindow (hwndChildWindow);
         }
