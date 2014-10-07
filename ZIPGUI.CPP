/*---------------------------------------------------------
   ZIPGUI -- An instance of a Unzip GUI interface
                (c) Bruce Henry 1996
  ---------------------------------------------------------*/
#define INCL_WINFRAMEMGR        // Needed for the WS_ equates
#define INCL_WINSTDCNR           // Needed for the CCS_ equates
#define INCL_WINSYS
#define INCL_WINSTATICS          // Needed for SS_ equates
#define  INCL_GPIBITMAPS        // For RGB structure definition
#define  INCL_WINWINDOWMGR    // for WinSetWindowPos etc.
#define INCL_WINERRORS
#define INCL_WINPROGRAMLIST
#define INCL_WINHELP
#define INCL_WINPOINTERS
#define INCL_DOSMISC
#define INCL_WININPUT

#include <os2.h>
#include <i86.h>
#include <string.h>
#include <stdio.h>
#include "message.h"
#include "zipcntr.h"
#include "zipgui.h"

   //-----------------------------------------------------------
   //   The constructor to set up and register the window
   //-----------------------------------------------------------
   zipgui::zipgui (PSZ pszTitle, PSZ pszFile, PSZ Destination, PSZ Tempdir, PSZ Viewer,
                                      PSZ Viewertype, USHORT viewercount, PSZ unzip, PSZ zip,
                                                        HWND help, HWND hostframe, ULONG inuse)
   {
      // Note: Can't add FCF_TASKLIST because this isn't a separate task - it will be
      // destroyed when File Freedom is destroyed.

      dm.frameflags = FCF_TITLEBAR|FCF_SYSMENU|FCF_SIZEBORDER|FCF_ACCELTABLE
                                            |FCF_MENU|FCF_ICON|FCF_MAXBUTTON;

      // Don't need a message queue here since one already exists.

      dm.hab = WinInitialize (0);
      dm.pszFilename = pszFile;
      dm.pszDestination = Destination;
      dm.pszHome = Tempdir;
      dm.pszViewer = Viewer;
      dm.pszViewertype = Viewertype;
      dm.Viewercount = viewercount;
      dm.pszUnzip = unzip;
      dm.pszZip = zip;
      dm.hwndHelp = help;
      dm.hwndHostframe = hostframe;
      dm.readyflag = 0;
      dm.inuseflag = inuse;
      dm.deleteflag = 0;
      dm.pRedirect = RedirectOutput;

      // Pick up the Command Processor from the "SET OS2_SHELL=?" statement in the
      // config.sys file.  This is needed in case the user has installed 4OS2 or some
      // other shell.  The command processor is used in various viewer and zip etc. routines.

      strcpy (dm.szcmdprocessor, "cmd.exe");
      APIRET rc;
      rc = DosScanEnv ("OS2_SHELL", &dm.pszCmdProcessor);
      if (rc) dm.pszCmdProcessor = dm.szcmdprocessor;

      WinRegisterClass (
         dm.hab,                // Anchor block handle
         "ZipShell",      // Name of class being registered. 'sz' prefix is used
                             // to indicate a string variable terminated with a zero.
         (PFNWP) MainProc,    // Window procedure for class
         CS_CLIPSIBLINGS | CS_SIZEREDRAW | CS_CLIPCHILDREN , // Class style
         0) ;                // Extra bytes to reserve

      dm.hwndframe = WinCreateStdWindow (
         HWND_DESKTOP,       // Parent window handle
         WS_VISIBLE,         // Style of frame window
         &dm.frameflags,      // Pointer to control data
         "ZipShell",      // Client window class name
         pszTitle,         // Title bar text
         0,                 // Style of client window
         0,                  // Module handle for resources
         ZIPWINDOWID,              // ID must be reserved for this window
         &dm.hwndclient) ;      // Pointer to client window handle

      // WinCreateStdWindow () does not create an owner for the window.
      // Set the owner here to be the FileFreedom window.  If this is not done, each time
      // a child of this window is closed, focus will go back to the main File Freedom window
      // instead of this window.

      WinSetOwner (dm.hwndframe, dm.hwndHostframe);


      // Put up the hourglass pointer.

      WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP,
                                                                                 SPTR_WAIT, FALSE));

      // Store the pointer for the data members in the QWL_USER slot for the frame window
      // since it is not allowed for a client window.  This can be retrieved in the main window
      // procedure by getting the frame handle with WinQueryWindow (hwnd, QW_PARENT) and
      // then PDATAMEMBERS = WinQueryWindowULong (hwndframe, QWL_USER).

      pdm = &dm;
      WinSetWindowULong (dm.hwndframe, QWL_USER, (ULONG) pdm);

      WinQueryWindowPos (dm.hwndclient, (PSWP)&dm.swp);

      dm.hwndChildTitle = WinCreateWindow (
                         dm.hwndframe,       // Parent window handle
                         WC_STATIC,         // Window class
                         pszFile,              // Window text
                         WS_PARENTCLIP |  // Window style   (can be 'or'ed
                         WS_CLIPSIBLINGS |  //     with window class specific options)
                         WS_VISIBLE | SS_TEXT | DT_LEFT | DT_VCENTER,
                         dm.swp.x, dm.swp.y + 20,  // Initial position of window - not important
                         dm.swp.cx, 20,         // Initial size of window - not important (invisible)
                         dm.hwndclient,         // Owner window handle
                         HWND_TOP,     // Placement window handle
                         ZIPWINDOWID +1,              // Child window ID +1 (keep unique)
                         NULL,                 // Control data
                         NULL        );        // Presentation parameters

      dm.hwndStatusbar = WinCreateWindow (
                         dm.hwndframe,       // Parent window handle
                         WC_STATIC,         // Window class
                         NULL,              // Window text
                         WS_PARENTCLIP |  // Window style   (can be 'or'ed
                         WS_CLIPSIBLINGS |  //     with window class specific options)
                         WS_VISIBLE | SS_TEXT | DT_LEFT | DT_VCENTER,
                         dm.swp.x, dm.swp.y,              // Initial position of window - not important
                         dm.swp.cx, 20,         // Initial size of window - not important (invisible)
                         dm.hwndclient,         // Owner window handle
                         HWND_TOP,     // Placement window handle
                         ZIPWINDOWID + 2,              // Child window ID +1 (keep unique)
                         NULL,                 // Control data
                         NULL        );        // Presentation parameters

     dm.hwndContainer = WinCreateWindow (
                         dm.hwndframe,       // Parent window handle
                         WC_CONTAINER,         // Window class
                         NULL,              // Window text
                         WS_PARENTCLIP |  // Window style   (can be 'or'ed
                         WS_CLIPSIBLINGS |  //     with window class specific options)
                         WS_VISIBLE | CCS_READONLY | CCS_SINGLESEL,
                         dm.swp.x, dm.swp.y + 40,      // Initial position of window - not important
                         dm.swp.cx, dm.swp.cy - 40,   // Initial size of window - not important (invisible)
                         dm.hwndclient,         // Owner window handle
                         HWND_TOP,     // Placement window handle
                         ZIPWINDOWID + 3,              // Child window ID +2 (keep unique)
                         NULL,                 // Control data
                         NULL        );        // Presentation parameters

      // Check first to see if the last window positions were stored.

      BOOL success = 0;
      success = WinRestoreWindowPos ("FileFreedom", "ZipShell", dm.hwndframe);
      WinRestoreWindowPos ("FileFreedom", "ZipShellSB", dm.hwndStatusbar);
      WinRestoreWindowPos ("FileFreedom", "ZipShellTitle", dm.hwndChildTitle);
      WinRestoreWindowPos ("FileFreedom", "ZipShellCont", dm.hwndContainer);

      // Give the window a default position if it was not saved (first usage).

      if (!success)
      {
         WinQueryWindowPos (hostframe, (PSWP)&dm.swp);
         WinSetWindowPos (dm.hwndframe, HWND_TOP, dm.swp.x + 30, dm.swp.y - 40,
                                               dm.swp.cx, dm.swp.cy - 30, SWP_SIZE | SWP_MOVE);
      }

   //----------------------------------------------
   // Set some background colours for the windows
   //----------------------------------------------
   RGB rgb;
   rgb.bBlue=140;
   rgb.bGreen=188;
   rgb.bRed=220;

   WinSetPresParam (dm.hwndChildTitle, PP_BACKGROUNDCOLOR, (ULONG) sizeof(RGB),
                        (PVOID)&rgb);
   rgb.bBlue=24;
   rgb.bGreen=4;
   rgb.bRed=4;

   WinSetPresParam (dm.hwndChildTitle, PP_FOREGROUNDCOLOR, (ULONG) sizeof(RGB),
                        (PVOID)&rgb);

   rgb.bBlue=220;
   rgb.bGreen=220;
   rgb.bRed=220;

   WinSetPresParam (dm.hwndStatusbar, PP_BACKGROUNDCOLOR, (ULONG) sizeof(RGB),
                        (PVOID)&rgb);
   rgb.bBlue=68;
   rgb.bGreen=4;
   rgb.bRed=4;

   WinSetPresParam (dm.hwndStatusbar, PP_FOREGROUNDCOLOR, (ULONG) sizeof(RGB),
                        (PVOID)&rgb);


   //--------------------------------
   // Set a font for the status bars.
   //--------------------------------
   dm.pszfontnamesize = new CHAR [25];
   strcpy (dm.pszfontnamesize, "5.System VIO");
   WinSetPresParam (dm.hwndChildTitle, PP_FONTNAMESIZE,
                                   ULONG (strlen(dm.pszfontnamesize) + 1), (PSZ)dm.pszfontnamesize);

   WinSetPresParam (dm.hwndStatusbar, PP_FONTNAMESIZE,
                                   ULONG (strlen(dm.pszfontnamesize) + 1), (PSZ)dm.pszfontnamesize);

   //-----------------------------------------------------------------------
   // Set a font for the files listing in the container.  Use a mono-space font because the
   // field data (crc, file size etc.) must line up with the headers whose alignment is based
   // on column (monofont) spacing.
   //-----------------------------------------------------------------------
   strcpy (dm.pszfontnamesize, "12.System VIO");
   WinSetPresParam (dm.hwndContainer, PP_FONTNAMESIZE,
                                   ULONG (strlen(dm.pszfontnamesize) + 1), (PSZ)dm.pszfontnamesize);

   //------------------------------------------------------------
   // Set up some PROGDETAILS parameters for later on with WinStartApp.
   // These values shouldn't change.
   //------------------------------------------------------------

         dm.Details.Length          = sizeof (PROGDETAILS);
         dm.Details.pszParameters   = NULL;
         dm.Details.pszStartupDir   = NULL;
         dm.Details.pszIcon         = NULL;
         dm.Details.pszEnvironment  = "\0\0";
         dm.Details.swpInitial.cy   = 0;    // width of window //
         dm.Details.swpInitial.cx   = 0;    // height of window //
         dm.Details.swpInitial.y    = 0;    // lower edge of window //
         dm.Details.swpInitial.x    = 0;    // left edge of window //
         dm.Details.swpInitial.hwndInsertBehind = HWND_TOP;
         dm.Details.swpInitial.ulReserved1      = 0;
         dm.Details.swpInitial.ulReserved2      = 0;

   //----------------------------------------------------------------------
   // Create the zipcntr object whose purpose it is to fill the container with the contents of
   // our zip file.
   //----------------------------------------------------------------------

   zipcntr zipshow (dm.hwndframe, dm.hwndContainer, dm.hwndStatusbar, dm.pszHome, dm.pszUnzip);
   dm.pZipcntr = &zipshow;

   // Start the first step of the unzipping process.  If there is an error, it means that
   // unzip.exe couldn't be activated.

   APIRET ret;
   ret = zipshow.Unziptofile (pszFile);
   if (ret)
   {
      WinMessageBox (HWND_DESKTOP, dm.hwndContainer, "Could not find UNZIP.EXE."
                                          "\nCheck ARCHIVER path in Settings Notebook.",
                                           "UnZip Error!", NULL, MB_OK | MB_ERROR);

      WinSendMsg (dm.hwndframe, WM_CLOSE, NULL, NULL);
   }

   // Put up the regular cursor.

   WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP, SPTR_ARROW, FALSE));

   while( WinGetMsg( dm.hab, &dm.qmsg, 0L, 0, 0 ) )
      WinDispatchMsg( dm.hab, &dm.qmsg );

   WinStoreWindowPos ("FileFreedom", "ZipShell", dm.hwndframe);
   WinStoreWindowPos ("FileFreedom", "ZipShellSB", dm.hwndStatusbar);
   WinStoreWindowPos ("FileFreedom", "ZipShellTitle", dm.hwndChildTitle);
   WinStoreWindowPos ("FileFreedom", "ZipShellCont", dm.hwndContainer);


   }

  //-------------------------------------------------------------
  //  A method to return the DirWindow Handle to the caller
  //-------------------------------------------------------------
   HWND zipgui::getContainerHWND()
   {
      return dm.hwndContainer;
   }

   //-------------------------------
   // A method to change the title bar text
   //-------------------------------
   void zipgui::newtitle (PSZ pNewtext)
   {
       WinSetWindowText (dm.hwndChildTitle, pNewtext);

   }
   //-------------------------------
   // A method to change the title bar font
   //-------------------------------
   void zipgui::titlefont (PSZ pszfont)
   {
      WinSetPresParam (dm.hwndChildTitle, PP_FONTNAMESIZE, ULONG (strlen (pszfont) + 1),
                        (PSZ) pszfont);
   }

   //--------------------------------
   // A method to change the title bar colors
   //--------------------------------
   void zipgui::titleparams (ULONG frgnd, ULONG bkgnd)
   {
      RGB rgb;
      ULONG temp = frgnd;
      temp = frgnd;
      rgb.bRed = BYTE (temp >> 16);
      temp = frgnd & 0xFF00;
      rgb.bGreen = BYTE (temp >> 8);
      temp = frgnd & 0xFF;
      rgb.bBlue = BYTE (temp);

      WinSetPresParam (dm.hwndChildTitle, PP_FOREGROUNDCOLOR, (ULONG) sizeof(RGB),
                           (PVOID)&rgb);

      temp = bkgnd;
      rgb.bRed = BYTE (temp >> 16);
      temp = bkgnd & 0xFF00;
      rgb.bGreen = BYTE (temp >> 8);
      temp = bkgnd & 0xFF;
      rgb.bBlue = BYTE (temp);

      WinSetPresParam (dm.hwndChildTitle, PP_BACKGROUNDCOLOR, (ULONG) sizeof(RGB),
                           (PVOID)&rgb);

   }
   //------------------------------
   // The required destructor
   //------------------------------
   zipgui::~zipgui()
   {
      delete dm.pszfontnamesize;
      DestroyWindow();
   }
   //--------------------------------------------------------------------
   // This method to destroy the window will be used by the destructor.
   //--------------------------------------------------------------------
   void zipgui::DestroyWindow ()
   {
      WinDestroyWindow (dm.hwndContainer);
      WinDestroyWindow (dm.hwndChildTitle);
      WinDestroyWindow (dm.hwndframe);
      WinTerminate (dm.hab );			

   }
   //------------------------
   // A main window procedure
   //------------------------
   static MRESULT EXPENTRY zipgui::MainProc (HWND hwnd, ULONG msg, MPARAM mp1,
                                                                                      MPARAM mp2)
   {
      HWND  hwndtemp;

      // Must retrieve the pointer (to all the data members) from the frame since a user pointer
      // can't be stored with a client window.

      hwndtemp = WinQueryWindow (hwnd, QW_PARENT);
      PDATAMEMBERS pdm2;
      if (hwndtemp) pdm2 = (PDATAMEMBERS) WinQueryWindowULong (hwndtemp, QWL_USER);

      switch (msg)
      {
         case WM_ACTIVATE:
            if (pdm2)
            {
               WinAssociateHelpInstance (pdm2->hwndHelp, hwnd);
            }
            break;

         case WM_CLOSE:

            // Test to see if a file has already been viewed and if so delete the
            // temporary file.  This couldn't be done in WM_APPTERMINATENOTIFY
            // because some programs (epm in particular) send a "terminated" response
            // even though they haven't even started to load the file.

            if (pdm2->deleteflag)
            {
               strcpy (pdm2->szpathbuffer, pdm2->pszHome);
               strcat (pdm2->szpathbuffer, "\\");
               strcat (pdm2->szpathbuffer, pdm2->szTempname);

               DosDelete (pdm2->szpathbuffer);
            }

            WinAssociateHelpInstance (pdm2->hwndHelp, pdm2->hwndHostframe);
            break;

         case WM_HELP:
            WinSendMsg (pdm2->hwndHelp, HM_DISPLAY_HELP, MPFROMSHORT (2110),
                                                                       MPFROMSHORT (HM_RESOURCEID));
            break;

            case WM_SIZE:

               if (pdm2)
               {
                  WinQueryWindowPos (pdm2->hwndclient, (PSWP)&pdm2->swp);
                  WinSetWindowPos (pdm2->hwndChildTitle, HWND_TOP, pdm2->swp.x,
                                     pdm2->swp.y + 20, pdm2->swp.cx, 20, SWP_SIZE | SWP_MOVE);

                  WinSetWindowPos (pdm2->hwndStatusbar, HWND_TOP, pdm2->swp.x, pdm2->swp.y,
                                                         pdm2->swp.cx, 20, SWP_SIZE | SWP_MOVE);

                  WinSetWindowPos (pdm2->hwndContainer, HWND_TOP, pdm2->swp.x,
                     pdm2->swp.y + 40, pdm2->swp.cx, pdm2->swp.cy - 40, SWP_SIZE | SWP_MOVE);
               }
               break;

            case WM_APPTERMINATENOTIFY:

               if (pdm2)
               {
                  // This first 'if' statement traps the return from the two FillContainer ()
                  // routines that use unzip to create a file with the information from the
                  // zip file that is used to build the records.
                  // The readyflag will equal zero after the first iteration of FillContainer ()
                  // and so will be called for the second time here to finish up the job.

                  if (pdm2->readyflag == 0)
                  {
                     // The FillContainer method must be called twice in succession here.
                     // If an error is returned, close down the zip viewer.

                     APIRET ret;
                     ret = pdm2->pZipcntr->FillContainer ();
                     if (ret)  WinSendMsg (hwnd, WM_CLOSE, NULL, NULL);
                     else
                     WinSetFocus (HWND_DESKTOP, pdm2->hwndContainer);

                     // Put up the regular pointer.

                     WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP,
                                                                                 SPTR_ARROW, FALSE));

                  }

                  // If the readyflag is set to one it means that a file has been extacted to our
                  // home directory and is ready to be read.
                  // This file is called 'pdm2->szTempname' ("fftemp.xxx").
                  // The 'read' command can be actived by either the enter key when it is over
                  // a record (or double mouse click) or by the menu item 'View File'.

                  else if (pdm2->readyflag == 1)
                  {
                     // Put up the hourglass pointer.

                     WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP,
                                                                                 SPTR_WAIT, FALSE));

                     USHORT Namelength;
                     Namelength = (USHORT) strlen (pdm2->szTempname);
                     int x, xx, xxx;
                     xx = 0;
                     PSZ ptemp;
                     USHORT internalzipviewer = 0;
                     USHORT internalrunprog = 0;

                     // Scan the filename string for a period.  If there is none, there is no
                     // file extension.  If there is, that is where the string comparison begins.
                     // If there are many periods in the name, this will mark the last one.

                     for (x = 0; x < Namelength; x++)
                        if (pdm2->szTempname [x] == 46) xx = x;

                     if (!xx ) strcpy (pdm2->szpathbuffer, "<none>");
                     else
                     {
                        ptemp = &pdm2->szTempname [xx + 1];
                        strcpy (pdm2->szpathbuffer, ptemp);
                     }

                     // If a match isn't found, the viewer will be set for the '0' index which
                     // is the viewer for "<none>" or the first item in the list.  This is
                     // the only ext/viewer pair protected from removal although the viewer
                     // can be edited.

                     xxx = pdm2->Viewercount;
                     for (x = 0; x < xxx; x++)
                     {
                        ptemp = &pdm2->pszViewertype [x * 7];
                        xx = stricmp (pdm2->szpathbuffer, ptemp);
                        if (!xx)
                        {
                           xxx = x;
                           xx = x;
                        }
                     }

                     // If there was no match, check to see if it is a zip file.  No match
                     // for the 'zip' extension means that the program will use the internal
                     // zip viewer if it is a zip file.
                     // Also test to see if it is an 'exe' program.  If it is it will be run
                     // as an independent program since an alternative viewer has not been
                     // specified (x==xxx).

                     if (x == xxx)
                     {
                        xxx = stricmp (pdm2->szpathbuffer, "zip");
                        if (!xxx) internalzipviewer = 1;
                        else
                        {
                           xxx = stricmp (pdm2->szpathbuffer, "exe");
                           if (!xxx) internalrunprog = 1;
                           else
                              xx = 0;
                        }
                     }

                     if (internalzipviewer)
                     {
                        strcpy (pdm2->szpathbuffer, pdm2->pszHome);
                        strcat (pdm2->szpathbuffer, "\\");
                        strcat (pdm2->szpathbuffer, pdm2->szTempname);

                        zipgui *ZipWin;
                        ZipWin = new zipgui ("Zip Viewer", pdm2->szpathbuffer,
                                               pdm2->pszDestination, pdm2->pszHome,
                                               pdm2->pszViewer, pdm2->pszViewertype,
                                               pdm2->Viewercount, pdm2->pszUnzip, pdm2->pszZip,
                                               pdm2->hwndHelp, pdm2->hwndHostframe, TRUE);

                        // Delete the temporary zip file here since this is a shell within
                        // a shell and this will be the last chance.

                        DosDelete (pdm2->szpathbuffer);
                        delete ZipWin;
                        WinSetFocus (HWND_DESKTOP, pdm2->hwndContainer);
                     }
                     else
                     {
                        ptemp = &pdm2->pszViewer [xx * 51];

                        pdm2->Details.pszExecutable = ptemp;
                        pdm2->Details.swpInitial.fl   = SWP_ACTIVATE | SWP_SHOW;
                        pdm2->szpathbuffer [0] = 0;

                        // Test to see if this is a CMD file and if it is, make special
                        // arrangements.

                        SHORT exelen;
                        exelen = SHORT (strlen (ptemp));
                        if ((ptemp [exelen -3] == 'c') || (ptemp [exelen - 3] == 'C'))
                        {
                           if ((ptemp [exelen -2] == 'm') || (ptemp [exelen - 2] == 'M'))
                           {
                              if ((ptemp [exelen -1] == 'd') || (ptemp [exelen - 1] == 'D'))
                              {
                                 // Make the cmd processor window invisible.
                                 pdm2->Details.swpInitial.fl   = SWP_HIDE;
                                 pdm2->Details.pszExecutable = (PSZ) pdm2->pszCmdProcessor;
                                 strcat (pdm2->szpathbuffer, "/C ");
                                 strcat (pdm2->szpathbuffer, ptemp);
                                 strcat (pdm2->szpathbuffer, " ");
                              }
                           }
                        }
                        // Check to see if epm in "browse mode" should be loaded.
                        // This is denoted by enclosing epm in arrowheads eg. "<epm>".
                        // This is set as the default viewer out of the box.

                        else if (ptemp [0] == '<')
                        {
                           if (ptemp [1] == 'e')
                           {
                              if (ptemp [2] == 'p')
                              {
                                 if (ptemp [3] == 'm')
                                 {
                                    if (ptemp [4] =='>')
                                    {
                                       pdm2->Details.swpInitial.fl = SWP_ACTIVATE;
                                       pdm2->Details.pszExecutable = "EPM.EXE";
                                       strcat (pdm2->szpathbuffer, "\'browse on\' ");
                                    }
                                 }
                              }
                           }
                        }
                        strcat (pdm2->szpathbuffer, pdm2->pszHome);
                        strcat (pdm2->szpathbuffer, "\\");
                        strcat (pdm2->szpathbuffer, pdm2->szTempname);

                        // Fill in the rest of the PROGDETAILS structure for the
                        // WinStartApp function.

                        pdm2->Details.progt.progc     = PROG_DEFAULT;
                        pdm2->Details.progt.fbVisible = SHE_VISIBLE;
                        pdm2->Details.pszTitle        = "Viewer";

                        if (internalrunprog)
                        {
                           pdm2->Details.pszExecutable = pdm2->szpathbuffer;
                        }

                        // Start the viewer.  If it has been determined that it is an exe prog
                        // and that no viewer has been set up for exe's, run it separately
                        // (it is set up as a viewer itself with no parameters) and not
                        // as a child app so that Windows programs will run.

                        if (!internalrunprog)

                        WinStartApp (pdm2->hwndframe, &pdm2->Details, pdm2->szpathbuffer,
                                                             NULL, SAF_STARTCHILDAPP);
                         else
                            WinStartApp (pdm2->hwndframe, &pdm2->Details, NULL,
                                                                   NULL, SAF_INSTALLEDCMDLINE);
                     }

                     // Reset the ready flag so that when the viewer is done,
                     // WM_APPTERMINATENOTIFY will trap the next 'else if' and delete the
                     // file used by the viewer.

                     pdm2->readyflag = 2;
                  }

                  // If the readyflag is set to two it means that there is a tempfile
                  // ("fftemp.xxx") that needs deleted.  A flag is set to accomplish this.
                  // This 'if' is used after the viewer has read a file.

                  else if (pdm2->readyflag == 2)
                  {
                     // Reset the readyflag to zero so that WM_APPTERMINATENOTIFY will
                     // process the proper code.

                     pdm2->readyflag = 0;

                     // Set a flag so that the temp file will be deleted if another file is viewed
                     // or when the gui closes.  Some programs return a terminated code
                     // before they begin (epm) so this can't be done here.

                     pdm2->deleteflag = 1;

                     // Put up the regular pointer.

                     WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP,
                                                                          SPTR_ARROW, FALSE));
                  }

                  // If readyflag is equal to 3, it has been dispatched by the EXTRACT_MID
                  // routine and needs only to reset the focus to clean up.

                  else if (pdm2->readyflag == 3)
                  {
                     WinSetFocus (HWND_DESKTOP, pdm2->hwndContainer);

                     // Reset the readyflag to zero so that WM_APPTERMINATENOTIFY will
                     // process the proper code.

                     pdm2->readyflag = 0;
                  }
               }
               break;

         case WM_COMMAND:

             // The regular switches will follow from here

            switch (SHORT1FROMMP (mp1))
            {
               // Display the zip file comment.

               case COMMENT_MID:

                  pdm2->pZipcntr->RedisplayMessage ();
                  break;

               case QUIT_MID:

                  WinSendMsg (hwnd, WM_CLOSE, NULL, NULL);
                  break;

               case HELP_MID:

                  WinSendMsg (pdm2->hwndHelp, HM_DISPLAY_HELP, MPFROMSHORT (1000),
                                                                    MPFROMSHORT (HM_RESOURCEID));
                  break;

               case VIEW_MID:

                  // This pdm2 test is really just a convienient way of enclosing this routine
                  // in brackets so that the variables will be local.

                  if (pdm2)
                  {
                     PCNRUNIT pRec;

                     // Test to see if a file has already been viewed and if so delete the
                     // temporary file.  This couldn't be done in WM_APPTERMINATENOTIFY
                     // because some programs (epm in particular) send a "terminated" response
                     // even though they haven't even started to load the file.

                     if (pdm2->deleteflag)
                     {
                        strcpy (pdm2->szpathbuffer, pdm2->pszHome);
                        strcat (pdm2->szpathbuffer, "\\");
                        strcat (pdm2->szpathbuffer, pdm2->szTempname);

                        DosDelete (pdm2->szpathbuffer);
                        pdm2->deleteflag = 0;
                     }

                     // A search will be made for the first selected record.  The search criteria for
                     // the first record is CMA_FIRST, but for the following records, the criteria is
                     // the record of the preceeding search.
                     // This file will be simply 'copied' to 'prn' so it won't work for other than
                     // ascii files.

                     pRec = (PCNRUNIT) CMA_FIRST;

                     pRec = (PCNRUNIT) WinSendMsg (pdm2->hwndContainer,
                                                    CM_QUERYRECORDEMPHASIS, MPFROMP( pRec ),
                                                                     MPFROMSHORT (CRA_SELECTED));
                     // If the record pointer is NULL, the pointer was not over a record when
                     // it was clicked so a break is called for.

                     if (!pRec) break;

                     // Set a pointer to the selected file name.

                     pdm2->pszSelectedFile = pRec->szName;

                     // Call the shared static procedure that redirects the output to a file.

                     pdm2->pRedirect ((PVOID) pdm2);
                  }
                  break;

               // Note that the hot keys chosen for these extractions seemed to affect the
               // way that the VIO window opened.  If a CONTROL or ALT key was part of the
               // hot key combination, the window would only open in the background.

               case EXTRACT_MID:
               case EXTRACTTOPATH_MID:

                  PCNRUNIT pExtract;

                  // A search will be made for the first selected record.  The search criteria for
                  // the first record is CMA_FIRST, but for the following records, the criteria is the
                  // record of the preceeding search.

                  pExtract = (PCNRUNIT) CMA_FIRST;

                  pExtract = (PCNRUNIT) WinSendMsg (pdm2->hwndContainer,
                                                 CM_QUERYRECORDEMPHASIS, MPFROMP (pExtract),
                                                                     MPFROMSHORT (CRA_SELECTED));
                  // If the record pointer is NULL, the pointer was not over a record when
                  // it was clicked so a break is called for.

                  if (!pExtract) break;

                  // Set the flag to let the WM_APPTERMINATENOTIFY case (above) know
                  // that the extraction routine is calling.

                  pdm2->readyflag = 3;

                  // Be sure to enclose the filename to be extracted in double quotes
                  // (and the name of the zip file in double quotes also)
                  // since filenames with embedded spaces would otherwise be interpreted
                  // as two filenames separated by spaces (a space acts as a delimiter in
                  // this instance).
                  // The entire destination path is enclosed in quotes because of the possibility
                  // of subdirectory names with embedded spaces.

                  // Test for which type of extraction to perform first, and copy the
                  // "-j" parameter to the string if the path is to be junked.

                  if (SHORT1FROMMP (mp1) == EXTRACT_MID)
                     strcpy (pdm2->szpathbuffer, "-j \"");
                  else
                     strcpy (pdm2->szpathbuffer, " \"");
                  strcat (pdm2->szpathbuffer, pdm2->pszFilename);
                  strcat (pdm2->szpathbuffer, "\" \"");
                  strcat (pdm2->szpathbuffer, pExtract->szName);
                  strcat (pdm2->szpathbuffer, "\" -d ");
                  strcat (pdm2->szpathbuffer, "\"");
                  strcat (pdm2->szpathbuffer, pdm2->pszDestination);
                  strcat (pdm2->szpathbuffer, "\"");

                  // Fill in the rest of the PROGDETAILS structure for the
                  // WinStartApp function.

                  pdm2->Details.progt.progc     = PROG_DEFAULT;
                  pdm2->Details.progt.fbVisible = SHE_VISIBLE;
                  pdm2->Details.pszTitle        = "Unzip";
                  pdm2->Details.pszExecutable   = pdm2->pszUnzip;
                  // OR SWP_SHOW with SWP_NOAUTOCLOSE for trouble shooting.
                  pdm2->Details.swpInitial.fl   = SWP_SHOW;// window positioning //

                  // Start the extraction.
                  // First, make sure the destination directory isn't stuck somewhere
                  // below the root.

                  DosSetCurrentDir ("\\");
                  WinStartApp (pdm2->hwndframe, &pdm2->Details, pdm2->szpathbuffer,
                                                          NULL, SAF_STARTCHILDAPP);
                  break;

               case DELETE_MID:

                  PCNRUNIT pDelete;

                  // A search will be made for the first selected record.  The search criteria for
                  // the first record is CMA_FIRST, but for the following records, the criteria is the
                  // record of the preceeding search.

                  pDelete = (PCNRUNIT) CMA_FIRST;

                  pDelete = (PCNRUNIT) WinSendMsg (pdm2->hwndContainer,
                                                 CM_QUERYRECORDEMPHASIS, MPFROMP( pDelete ),
                                                                     MPFROMSHORT (CRA_SELECTED));
                  // If the record pointer is NULL, the pointer was not over a record when
                  // it was clicked so a break is called for.

                  if (!pDelete) break;

                  // Set the flag to let the WM_APPTERMINATENOTIFY case (above) know
                  // that the viewer routine is calling.

                  pdm2->readyflag = 3;

                  // Be sure to enclose the filename to be deleted in double quotes
                  // (and the name of the zip file in double quotes also)
                  // since filenames with embedded spaces would otherwise be interpreted
                  // as two filenames separated by spaces (a space acts as a delimiter in
                  // this instance).

                  strcpy (pdm2->szpathbuffer, "-d \"");
                  strcat (pdm2->szpathbuffer, pdm2->pszFilename);
                  strcat (pdm2->szpathbuffer, "\" \"");
                  strcat (pdm2->szpathbuffer, pDelete->szName);
                  strcat (pdm2->szpathbuffer, "\"");

                  // Fill in the rest of the PROGDETAILS structure for the
                  // WinStartApp function.

                  pdm2->Details.progt.progc     = PROG_DEFAULT;
                  pdm2->Details.progt.fbVisible = SHE_VISIBLE;
                  pdm2->Details.pszTitle        = "Zip";
                  pdm2->Details.pszExecutable   = pdm2->pszZip;
                  pdm2->Details.swpInitial.fl   = SWP_SHOW; // window positioning //

                  // Do the deletion.

                  WinStartApp (pdm2->hwndframe, &pdm2->Details, pdm2->szpathbuffer,
                                                          NULL, SAF_STARTCHILDAPP);
                  // Now that the deletion is underway, remove the record from the container
                  // and update the display.

                  PCNRUNIT *pSingle;
                  pSingle = &pDelete;
                  WinSendMsg (pdm2->hwndContainer, CM_REMOVERECORD, MPFROMP (pSingle),
                                                                   MPFROM2SHORT( 1, CMA_FREE ));
                  WinSendMsg (pdm2->hwndContainer, CM_INVALIDATERECORD, NULL,
                                                            MPFROM2SHORT( 0,CMA_REPOSITION ));

                  break;

            }
            break;

         case WM_CONTROL:

            switch (SHORT2FROMMP (mp1))
            {
               // Help in a container must be trapped via CN_HELP

               case CN_HELP:

                  WinSendMsg (pdm2->hwndHelp, HM_DISPLAY_HELP, MPFROMSHORT (3200),
                                                                    MPFROMSHORT (HM_RESOURCEID));
                  break;

               case CN_ENTER:

                  PNOTIFYRECORDENTER pNRE;
                  pNRE = PNOTIFYRECORDENTER (PVOIDFROMMP (mp2));

                  // Test to see if a file has already been viewed and if so delete the
                  // temporary file.  This couldn't be done in WM_APPTERMINATENOTIFY
                  // because some programs (epm in particular) send a "terminated" response
                  // even though they haven't even started to load the file.

                  if (pdm2->deleteflag)
                  {
                     strcpy (pdm2->szpathbuffer, pdm2->pszHome);
                     strcat (pdm2->szpathbuffer, "\\");
                     strcat (pdm2->szpathbuffer, pdm2->szTempname);

                     DosDelete (pdm2->szpathbuffer);
                     pdm2->deleteflag = 0;
                  }

                  // This pdm2 test is really just a convienient way of enclosing this routine
                  // in brackets so that the variables will be local.

                  if (pdm2)
                  {
                     // Get the record pointer

                     PCNRUNIT pRec;
                     pRec = (PCNRUNIT) pNRE->pRecord;

                     // If the record pointer is NULL, the pointer was not over a record when
                     // it was clicked so a break is called for.

                     if (!pRec) break;

                     // Set a pointer to the selected file name.

                     pdm2->pszSelectedFile = pRec->szName;

                     // Call the shared static function that redirects the output to a file.

                     pdm2->pRedirect ((PVOID) pdm2);
                  }
                  break;
            }
            break;
      }
      return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
   }
//--------------------------------------------------------------------
// This is a static function which is called from the static Window Procedure
//--------------------------------------------------------------------
static void  zipgui::RedirectOutput (PVOID pvDm)
{
      APIRET rc;
      PDATAMEMBERS pDm;
      pDm = (PDATAMEMBERS) pvDm;

      // Set the flag to let the WM_APPTERMINATENOTIFY case (above) know
      // that the viewer routine is calling.

      pDm->readyflag = 1;

      // Put up the hourglass pointer.

      WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP,
                                                                  SPTR_WAIT, FALSE));

      // Build a command line with parameters to redirect stdout
      // to a file named "fftemp.xxx" where ".xxx" is the actual extension
      // of the original file (it could be nothing).

      USHORT Namelength;
      Namelength = (USHORT) strlen (pDm->pszSelectedFile);
      int x, xx;
      xx = 0;
      PSZ ptemp;

      // Start the file name string.

      strcpy (pDm->szTempname, "fftemp");

      // Scan the filename string for a period.  If there is none, there is no
      // file extension.  If there is, that will become the extension of the
      // temporary file.
      // If there are many periods in the name, this will mark the last one.

      for (x = 0; x < Namelength; x++)
         if (pDm->pszSelectedFile [x] == 46) xx = x;

      USHORT skip;
      skip = 0;

      // Copy the extension over (including the period) if there was one.

      if (xx )
      {
         ptemp = &pDm->pszSelectedFile [xx];
         strcat (pDm->szTempname, ptemp);

         ptemp = &pDm->pszSelectedFile [xx + 1];

         // Check to see if it is a zip file.  If it is a zip file and the zip viewer
         // is already two levels deep, don't extract the file or the current file
         // (fftemp.zip) will be destroyed.  Put up a message to notify the user.

         xx = stricmp (ptemp, "zip");
         if (xx == 0 && pDm->inuseflag == 1)
         {
            WinMessageBox (HWND_DESKTOP, pDm->hwndContainer, "Can't go deeper\n",
                                           "SORRY!", NULL, MB_CANCEL | MB_WARNING);
            skip = 1;
         }
      }
      if (!skip)
      {
         ULONG Action;  // This parameter is ignored in this case.

         strcpy (pDm->szpathbuffer, pDm->pszHome);
         strcat (pDm->szpathbuffer, "\\");
         strcat (pDm->szpathbuffer, pDm->szTempname);

         rc = DosOpen (pDm->szpathbuffer, &pDm->hFile, &Action, NULL, NULL,
                         OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
                         OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE, NULL);

         // If error opening "X:\homepath\fftemp.xxx" file, put up message.

         if (rc)
         {
            WinMessageBox (HWND_DESKTOP, pDm->hwndframe, "Can't open temporary file\n",
                                              "ERROR!", NULL, MB_OK | MB_ERROR);
         }
         else
         {
            // Be sure to enclose the filename to be extracted in double quotes
            // (and the name of the zip file in double quotes also)
            // since filenames with embedded spaces would otherwise be interpreted
            // as two filenames separated by spaces (a space acts as a delimiter in
            // this instance).
            // When the parameter string is complete, it will look like
            //        "-vq "zipname""
            // and the program name is "unzip.exe".


            strcpy (pDm->szpathbuffer, "-p \"");
            strcat (pDm->szpathbuffer, pDm->pszFilename);
            strcat (pDm->szpathbuffer, "\" \"");
            strcat (pDm->szpathbuffer, pDm->pszSelectedFile);
            strcat (pDm->szpathbuffer, "\"");

            // Fill in the rest of the PROGDETAILS structure for the
            // WinStartApp function.

            pDm->Details.progt.progc     = PROG_DEFAULT;
            pDm->Details.progt.fbVisible = SHE_VISIBLE;
            pDm->Details.pszTitle        = "UnZip";
            pDm->Details.pszExecutable   = pDm->pszUnzip;
            pDm->Details.swpInitial.fl   = SWP_HIDE; // window positioning //

            // Set up to redirect stdout to the temporary file "X:\homepath\fftemp.xxx".
            // First the file must be opened (as above) so that a handle is available.
            // Then DosDupHandle is used to:
            // 1) Duplicate the handle to stdout and save the new handle
            // 2) Substitute the old handle of stdout for the handle for "ziptemp"
            // Next the original file handle is closed and this will force stdout to the file.
            // After the child app is started (it inherits the environment and outputs from
            // it's parent) DosDupHandle is again used to exchange the saved substituted
            // handle for stdout with the original value (1).
            // When the child app finishes outputting to the redirected stdout (ziptemp),
            // it will close (flush) stdout before it returns.

            pDm->hStdout = 1;
            pDm->hTemp = -1;
            DosDupHandle (pDm->hStdout, &pDm->hTemp);
            pDm->hStdout = 1;
            DosDupHandle (pDm->hFile, &pDm->hStdout);
            DosClose (pDm->hFile);

            // Start the extraction.

            WinStartApp (pDm->hwndframe, &pDm->Details, pDm->szpathbuffer,
                                                             NULL, SAF_STARTCHILDAPP);
            pDm->hStdout = 1;
            DosDupHandle (pDm->hTemp, &pDm->hStdout);

            WinSetFocus (HWND_DESKTOP, pDm->hwndContainer);
         }
      }
}
