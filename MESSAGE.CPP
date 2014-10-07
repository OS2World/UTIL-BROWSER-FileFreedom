/******************************************************************/
/*           Message -   A class to display a large, scrollable message            */
/*                                                                                      */
/*                       This message box will self-destruct if it loses focus.     */
/******************************************************************/

#define INCL_WIN
#define INCL_GPI
#define INCL_DOSFILEMGR

#include <os2.h>			
#include <string.h>
#include <stdlib.h>
#include <dos.h>
#include "message.h"


message::message (PSZ pszMessage, PSZ pszTitle, HWND owner)
{
   md.hab = WinInitialize (0);

   WinRegisterClass (
         md.hab,                // Anchor block handle
         "messagewin",      // Name of class being registered. 'sz' prefix is used
                             // to indicate a string variable terminated with a zero.
         (PFNWP) MainProc,    // Window procedure for class
         CS_CLIPSIBLINGS | CS_SIZEREDRAW | CS_CLIPCHILDREN , // Class style
         0) ;                // Extra bytes to reserve

   md.flCreate = FCF_TITLEBAR | FCF_SYSMENU | FCF_SIZEBORDER | FCF_MINMAX;


   md.hwndFrame = WinCreateStdWindow (
	       HWND_DESKTOP,		
          NULL,
	       &md.flCreate,		
	       "messagewin",
	       pszTitle,			
	       0,			
	       0,	
	       ID_WINDOW,		
	       &md.hwndClient		
	       );

   WinQueryWindowPos (owner, (PSWP)&md.swp);
   WinSetWindowPos (md.hwndFrame, HWND_TOP, md.swp.x + 20, md.swp.y + 65, md.swp.cx - 60,
                                                md.swp.cy/3, SWP_SIZE | SWP_SHOW | SWP_MOVE);

   // Save a pointer to the MESSAGEDATA structure in the window words.
   // This is needed for the MainWindowProc which is a static method and
   // otherwise wouldn't have access to the private data members.

   pmd = &md;
   WinSetWindowULong (md.hwndFrame, QWL_USER, (ULONG) pmd);

   // Set up a MLE control window in the client window.

   MLECTLDATA  mledata;

   // --------------------------------------------------------------------------
   // Populate an MLE control data structure.
   // --------------------------------------------------------------------------
   mledata.cbCtlData     =  sizeof (MLECTLDATA);      // size of structure
   mledata.cchText       =  -1L;                     // no text limit boundary
   mledata.iptAnchor     =  0;                       // begin selection pointer
   mledata.iptCursor     =  0;                       // end selection pointer
   mledata.cxFormat      =  0;                       // formatrect width
   mledata.cyFormat      =  0;                       // formatrect height
   mledata.afFormatFlags =  MLFFMTRECT_MATCHWINDOW;  // formatrect flags
   mledata.afIEFormat    =  MLFIE_CFTEXT;            // import/export format

   // --------------------------------------------------------------------------
   // Get the rectangle coordinates for the Client Window.
   // --------------------------------------------------------------------------
   WinQueryWindowRect(md.hwndClient, &md.rcl);

   // --------------------------------------------------------------------------
   // Create our editor window by drawing an MLE window within the client
   // window.  We leave room at the top of the client window for our button
   // bar windows, and room at the bottom of the client window for a status
   // window.
   // --------------------------------------------------------------------------
   md.hwndMLE = WinCreateWindow (
                             md.hwndFrame,   // Parent Window Handle
                             WC_MLE,         // Window Class
                             NULL,            // Window Text
                             MLS_HSCROLL | MLS_VSCROLL |
                             MLS_BORDER | WS_VISIBLE, // Window Styles
                             md.rcl.xLeft + 4,    // Starting x coordinate
                             md.rcl.yBottom + 4, // Leave room at bottom
                             md.rcl.xRight + 1,       // Horizontal Length
                             md.rcl.yTop + 1,         // Leave room at top
                             md.hwndClient,  // Client is Owner Window
                             HWND_TOP,     // Window Placement
                             ID_WINDOW + 1,        // MLE Window Identifier
                             (PVOID) &mledata,      // Control Data
                             NULL);                  // Presentation Parameters


   WinSendMsg (md.hwndMLE, MLM_SETIMPORTEXPORT, MPFROMP (pszMessage),
                                                            MPFROMLONG (strlen (pszMessage) - 1));
   ULONG marker;
   marker = 0;
   WinSendMsg (md.hwndMLE, MLM_IMPORT, MPFROMP (&marker),
                                                              MPFROMLONG (strlen (pszMessage) - 1));

   WinSetFocus (HWND_DESKTOP, md.hwndFrame);

   while( WinGetMsg( md.hab, &md.qmsg, 0L, 0, 0 ) )
      WinDispatchMsg( md.hab, &md.qmsg );

}

message::~message ()
{
   WinDestroyWindow (md.hwndFrame);
   WinTerminate (md.hab );			

}
   //------------------------
   // A main window procedure
   //------------------------
   static MRESULT EXPENTRY message::MainProc (HWND hwnd, ULONG msg, MPARAM mp1,
                                                                                      MPARAM mp2)
   {
      HWND  hwndtemp;

      // Must retrieve the pointer (to all the data members) from the frame since a user pointer
      // can't be stored with a client window.

      hwndtemp = WinQueryWindow (hwnd, QW_PARENT);
      PMESSAGEDATA pmd2;
      if (hwndtemp) pmd2 = (PMESSAGEDATA) WinQueryWindowULong (hwndtemp, QWL_USER);

      switch (msg)
      {
         case WM_SIZE:

            // Keep the MLE sized to fit the client.

            if (pmd2)
            {
               WinQueryWindowPos (pmd2->hwndClient, (PSWP)&pmd2->swp);
               WinSetWindowPos (pmd2->hwndMLE, HWND_TOP, pmd2->swp.x,
                            pmd2->swp.y, pmd2->swp.cx, pmd2->swp.cy, SWP_SIZE | SWP_MOVE);
            }
            break;

         case WM_SETFOCUS:

            // If the message window loses focus, shut it down.
            // This will ensure that the function that called it will get a return and
            // will have a chance to delete new memory etc.

            if (pmd2)
            {
               if (SHORT1FROMMP (mp2) == 0)
                                 WinSendMsg (hwnd, WM_CLOSE, NULL, NULL);
            }
            break;

//-         case WM_CLOSE:
//-            if (pmd2)
//-            {
//-
//-            }
//-            break;
      }
      return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
   }

   //--------------------------------------------------
   // A Method to return the HWND of the window frame.
   //--------------------------------------------------
   HWND message::GetHwnd ()
   {
      return md.hwndFrame;
   }

