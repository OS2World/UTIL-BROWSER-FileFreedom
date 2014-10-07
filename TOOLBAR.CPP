/*---------------------------------------------------------
   TOOLBAR.CPP -- An instance of a TOOLBAR class
                (c) Bruce Henry 1996
  ---------------------------------------------------------*/
#define INCL_WINFRAMEMGR        // Needed for the WS_ equates
#define INCL_WINSTDCNR           // Needed for the CCS_ equates
#define INCL_WINBUTTONS          // Needed for PushButton equates
#define INCL_WINSYS
#define  INCL_GPIBITMAPS        // For RGB structure definition
#define  INCL_WINWINDOWMGR    // for WinSetWindowPos etc.
#define INCL_WINPOINTERS         // for WinLoadPointer
#define INCL_WINSTATICS

#define FRAME_FLAGS         (FCF_BORDER | FCF_NOBYTEALIGN)

#include <os2.h>
#include <i86.h>
#include <string.h>
#include "toolbar.h"

    //-----------------------------------------------------------
    //   The constructor to set up and register the window
    //-----------------------------------------------------------
      toolbar::toolbar (HWND hwndCLIENT, ULONG idCHILD, PBTNINFO pbinfo)
      {
         buttoncx = pbinfo->btncx;
         buttoncy = pbinfo->btncy;
         buttonrows = pbinfo->brows;
         buttoncols = pbinfo->bcolumns;
         totalbuttons = pbinfo->totalbuttons;//brows*pbinfo->bcolumns;
         width=pbinfo->bcolumns*buttoncx;
         height=pbinfo->brows*buttoncy;

         hwndClient = hwndCLIENT;
         pInfo = pbinfo;

         //----------------------------------------------
         //Create a static window to hold the pushbuttons
         //----------------------------------------------
         hwndChildWindow = WinCreateWindow (
                            hwndCLIENT,       // Parent window handle
                            WC_STATIC,         // Window class
                            NULL,              // Window text
                            WS_PARENTCLIP | // Window style   (can be 'or'ed
                            WS_CLIPSIBLINGS | //     with window class specific options)
                            SS_TEXT,
                            0, 0,              // Initial position of window - not important
                            45,45,         // Initial size of window - not important (invisible)
                            hwndCLIENT,         // Owner window handle
                            HWND_BOTTOM,     // Placement window handle
                            idCHILD,              // Child window ID
                            NULL,                 // Control data
                            NULL        );        // Presentation parameters

         //--------------------------------------------------------------------
         //The bitmap information is set up here so that a different bitmap can be referenced
         //each time a new pushbutton is created below.
         //--------------------------------------------------------------------
         memset ( &bdata, 0, sizeof (MYBTNCDATA));
         bdata.cb = sizeof (MYBTNCDATA);
         bdata.fsCheckState = NULL;
         bdata.fsHiliteState = 0;
         //------------------------------------------------
         //Create an array of Pushbuttons complete with icons or text
         //------------------------------------------------
         int loadicon = 0;
         CHAR *label = " ";
         PSZ pszfontnamesize;

         // This line will determine whether the pushbuttons are going to be icons or text
         // by examining the frame creation flags passed in the BTNINFO structure.

         if (pbinfo->flBstyle & BS_ICON) loadicon = 1;
         if (!loadicon)
         {
            pszfontnamesize = new CHAR [25];
            strcpy (pszfontnamesize, "9.Tms Rmn Bold");// "5.System VIO");//9.Helvetica");//9.Helv");
         }

         for (int i=0; i<totalbuttons; i++)
         {
            if (loadicon) hicon = WinLoadPointer (HWND_DESKTOP, 0, idCHILD + 50 + i);
            else label = pbinfo->label + (i * pbinfo->labelsize);
            bdata.hImage = hicon;

            hwndPushB [i] = WinCreateWindow (
                            hwndChildWindow,       // Parent window handle
                            WC_BUTTON,         // Window class
                            label,                  // Window text
                            WS_PARENTCLIP |  // Window style   (can be 'or'ed
                            WS_CLIPSIBLINGS |  //     with window class specific options)
                            pbinfo->flBstyle,
                            0, 0,              // Initial position of window - not important
                            32,32,         // Initial size of window - not important (invisible)
                            hwndCLIENT,         // Owner window handle
                            HWND_BOTTOM,     // Placement window handle
                            idCHILD +1+i,              // Child window ID +1 etc.
                            &bdata,                 // Control data
                            NULL        );        // Presentation parameters
            if (!loadicon)
            {
               WinSetPresParam (hwndPushB [i], PP_FONTNAMESIZE,
                                        ULONG (strlen (pszfontnamesize) + 1), (PSZ) pszfontnamesize);
            }
         }
         if (!loadicon) delete pszfontnamesize;
         //----------------------------------------------
         // Set some background colours for our windows
         //----------------------------------------------
         RGB rgb;
         rgb.bBlue = 200;
         rgb.bGreen = 200;
         rgb.bRed = 200;

         WinSetPresParam (hwndChildWindow, PP_BACKGROUNDCOLOR, (ULONG) sizeof(RGB),
                           (PVOID)&rgb);
      }
     //-------------------------------------------------------------
     //  A method to return the DirWindow Handle to the caller
     //-------------------------------------------------------------
      HWND toolbar::getToolbarHWND ()
      {
         return hwndChildWindow;
      }

      //---------------------------------------------------------------------
      //A method to rejig the toolbar parameters so that it will reorient itself to horizontal or
      //vertical the next time it is resized.
      //---------------------------------------------------------------------
      PSTATICDIM toolbar::rejig (SHORT brows, SHORT bcolumns)
      {
         // Make an allowance here for the margins around the buttons if there is
         // another bar created that has to have space reserved for.

         pInfo->reservecx > 0 ? holdcx = pInfo->reservecx + 2: holdcx = 0;
         if (holdcx) holdcx *= pInfo->reservecolumns;
         pInfo->reservecy > 0 ? holdcy = pInfo->reservecy + 2: holdcy = 0;
         if (holdcy) holdcy *= pInfo->reserverows;

         buttoncols = bcolumns;
         buttonrows = brows;
         width = bcolumns*buttoncx;
         height = brows*buttoncy;
         INT  tempwidth = (buttoncols * (buttoncx + 2));
         INT tempheight = (buttonrows * (buttoncy + 2));

         // Do a fixup here in case the toolbar has been created, but is made effectivly
         // invisible by making the buttons of zero width and zero height.

         if (pInfo->btncy == 0) tempheight = 0;
         if (pInfo->btncx == 0) tempwidth = 0;

         if (tempwidth>tempheight)
         {
            sdim.vert = tempheight;
            sdim.horz = 0;
         } else
         {
            sdim.vert = 0;
            sdim.horz = tempwidth;
         }
         return &sdim;
      }

      //---------------------------------------------------------------------
      //A method to resize the toolbar window parts to fit a resized client.
      // There will be a static dimension either vertically or horizontally.  If the toolbar
      // is a vertical toolbar, it will only stretch vertically. If it is a horizontal toolbar,
      // it will only stretch horizontally.
      // The basic size of the toolbar is determined by multiplying the number of rows of
      // buttons by the number of columns of buttons by the button size.
      //---------------------------------------------------------------------
      PSTATICDIM toolbar::resizewin ()
      {
         // Reinitialize these values here, in case the calling program has changed certain
         // these PSTATICDIM values.

         buttoncx = pInfo->btncx;
         buttoncy = pInfo->btncy;

         // Set the position and size of the toolbar frame

         WinQueryWindowPos (hwndClient,(PSWP) &dirswp);
         INT  tempwidth=(buttoncols * (buttoncx + 2));
         INT tempheight=(buttonrows * (buttoncy + 2));

         // Do a fixup here in case the toolbar has been created, but is made effectivly
         // invisible by making the buttons of zero width and zero height.

         if (buttoncy == 0)tempheight = 0;
         if (buttoncx == 0)tempwidth = 0;

         INT tempxorigin, tempyorigin;
         if (tempwidth>tempheight)
         {
            tempxorigin=0;
            tempwidth=dirswp.cx;
            tempyorigin=dirswp.cy - tempheight - holdcy;
            sdim.vert=tempheight;
            sdim.horz=0;
         }
         else
         {
            tempyorigin=0;
            tempheight=dirswp.cy;
            tempxorigin=dirswp.cx - tempwidth - holdcx;
            sdim.vert=0;
            sdim.horz=tempwidth;
         }

         WinSetWindowPos (hwndChildWindow, HWND_BOTTOM, tempxorigin, tempyorigin,
                              tempwidth, tempheight, SWP_MOVE|SWP_SIZE|SWP_SHOW);

         // Set the size and position of the toolbar buttons.

         WinQueryWindowPos (hwndChildWindow, (PSWP)&dirswp);
         int ii;     //need some extra indices
         int iii=0;
         if (tempheight>tempwidth)
         {
            tempxorigin=tempwidth - (buttoncx + 2);
            tempyorigin=tempheight - (buttoncy + 2);
            INT tempyo=tempyorigin;
            tempwidth=buttoncx;
            tempheight=buttoncy;
            for (int i=0; i<buttoncols;i++)
            {
               for (ii=0; ii<buttonrows; ii++)
               {
                  WinSetWindowPos (hwndPushB[iii], HWND_BOTTOM, tempxorigin, tempyorigin,
                                 tempwidth, tempheight, SWP_MOVE|SWP_SHOW
                                 |SWP_SIZE);
                  iii++;
                  tempyorigin-=(tempheight+2);
                }
                tempxorigin-=(tempwidth+2);
                tempyorigin=tempyo;
            }
            return &sdim;
         }
         else
         {
            tempxorigin=2;
            tempyorigin=1;
            tempwidth=buttoncx;
            tempheight=buttoncy;
            for (int i=0; i<buttoncols;i++)
            {
               for (ii=0; ii<buttonrows; ii++)
               {
                  WinSetWindowPos (hwndPushB[iii], HWND_BOTTOM, tempxorigin, tempyorigin,
                                 tempwidth, tempheight, SWP_MOVE|SWP_SHOW
                                 |SWP_SIZE);
                  iii++;
                  tempyorigin+=(tempheight+2);
                }
                tempxorigin+=(tempwidth+2);
                tempyorigin=1;
            }
            return &sdim;
         }
      }

      //------------------------------
      // The required destructor
      //------------------------------
     toolbar::~toolbar()
     {
         DestroyWindow ();
     }
     void toolbar::DestroyWindow ()
     {
         for (int i = 0; i < totalbuttons; i++) WinDestroyWindow (hwndPushB[i]);
         WinDestroyWindow (hwndChildWindow);
     }
