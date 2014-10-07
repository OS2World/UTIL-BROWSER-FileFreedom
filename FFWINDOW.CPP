/*---------------------------------------------------------
   FFWINDOW -- An instance of a stdwindow class
                (c) Bruce Henry 1996
  ---------------------------------------------------------*/
#define INCL_WINFRAMEMGR
#define INCL_WINSTATICS          // Needed for SS_ equates


#include <os2.h>
//#include "ffmenu.h"
#include "stdwind.h"

      //---------------------------------------
      // This method called by the destructor
      //---------------------------------------
      void stdwindow::DestroyStdWin ()
      {
         WinDestroyWindow (hwndClient);
         WinDestroyWindow (hwndFrame);
      }

      //-------------------------------------------------------
      // The constructor to set up and register the window
      //-------------------------------------------------------
      stdwindow::stdwindow( HAB mainhab, ULONG flFrameFlags, PSZ szClassName,
                              PFNWP ClientWndProc, PSZ szWindowTitle, USHORT id)

      {
         hab = mainhab;
         WinRegisterClass (
            hab,                // Anchor block handle
            szClassName,      // Name of class being registered. 'sz' prefix is used
                                // to indicate a string variable terminated with a zero.
            ClientWndProc,    // Window procedure for class
            CS_CLIPSIBLINGS | CS_SIZEREDRAW | CS_CLIPCHILDREN , // Class style
            8) ;                // Extra bytes to reserve - so far storing the object window's
                                // hwnd here in the first position.  This is apparently the same
                                // index as QWL_USER since both would have an index value
                                // of zero, so QWL_USER would return this value
                                // but couldn't be used to store it's own distinct value.

         hwndFrame = WinCreateStdWindow (
            HWND_DESKTOP,       // Parent window handle
            NULL,         // Style of frame window - start off invisible
            &flFrameFlags,      // Pointer to control data
            szClassName,      // Client window class name
            szWindowTitle,         // Title bar text
            0L,                 // Style of client window
            0,                  // Module handle for resources
            id,              // ID must be reserved for this window
            &hwndClient) ;      // Pointer to client window handle

      }

       //----------------------------------------------------
       //A method to return the handle to the client window
       //----------------------------------------------------
      HWND stdwindow::getClientHWND()
      {
         return hwndClient;
      }
      //--------------------------------------------
      // A method to return the handle to the frame
      //--------------------------------------------
      HWND stdwindow::getFrameHWND()
      {
         return hwndFrame;
      }

      //---------------------------
      // The required destructor
      //---------------------------
     stdwindow::~stdwindow()
     {
        DestroyStdWin();
      }



