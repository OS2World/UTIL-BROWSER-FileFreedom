/*---------------------------------------------------------
       zipgui.h -- a gui window interface for Info-Zips Unzip
                                (c) Bruce Henry 1996

 NOTE: The two sub-windows in this object will automatically use the next
         three numbers in the series of ID numbers assigned to this Frame
         window. eg. If this object is assigned ID#400, it will also use 401,
         402 and 403 as ID#s for the sub windows.  This may not be necessary
         since I haven't yet needed to use these IDs.
  -----------------------------------------------------------*/
#ifndef __zipguiwindow__
#define __zipguiwindow__

#define ZIPWINDOWID 260  // The next two numbers in this series are used automatically
                               // for sub-windows.
#define FILE_MID       263
#define VIEW_MID      264
#define EXTRACT_MID  265
#define EXTRACTTOPATH_MID  271
#define DELETE_MID    270
#define COMMENT_MID  266
#define QUIT_MID       267
#define HELPMENU_MID 268
#define HELP_MID       269

typedef struct _DATAMEMBERS
         {
            HWND hwndframe;      // This will hold the handle to the container parent
            HWND hwndHostframe;        // The handle of the calling window.
            HWND hwndChildTitle;     // Static window that holds title of child window
            HWND hwndStatusbar;     // Status bar
            HWND  hwndContainer;       // This will hold the handle to the container.
            HWND hwndclient ;               //Handle to Client window
            HWND hwndHelp;                // Help handle
            SWP  swp;               // Create SWP structure for window position info
            PSZ  pszfontnamesize;  // Pointer to string to hold PresParm font
            HAB hab;
            HMQ hmq;
            QMSG qmsg;
            ULONG frameflags;
            PSZ pszFilename;         // Complete zip filename including drive and path.
            PSZ pszSelectedFile;      // File to operated on within the zip
            CHAR szpathbuffer [(CCHMAXPATH * 2) + 1];
            CHAR szTempname [CCHMAXPATH +1];
            PSZ pszDestination;       // Complete destination path including drive.
            PSZ pszHome;             // Complete path including drive to File Freedom's home dir.
            PSZ pszViewer;            // Pointer to an array of supported viewer names.
            PSZ pszViewertype;        // Pointer to an array of supported file extensions for viewers.
            PSZ pszUnzip;             // Pointer to Unzip.exe path/name.
            PSZ pszZip;                // Pointer to the Zip.exe pathname.
            USHORT Viewercount;      // Number of viewers registered.
            ULONG readyflag;
            ULONG inuseflag;
            ULONG deleteflag;
            PROGDETAILS Details;
            zipcntr *pZipcntr;
            CHAR szcmdprocessor [8];// = "cmd.exe";
            char const *pszCmdProcessor; // This points to the name of the command processor (from OS2_SHELL)
            void (*pRedirect) (PVOID);
            HFILE hFile, hTemp, hStdout;
         } DATAMEMBERS, *PDATAMEMBERS;

class zipgui
{
   protected:

      DATAMEMBERS dm;
      PDATAMEMBERS pdm;

      /****Protected Methods***************************/

      //--------------------------------------------------------------------
      // This method to destroy the window will be used by the destructor.
      //--------------------------------------------------------------------
      void DestroyWindow () ;

      //------------------------------------------------------------------
      // A main window procedure
      //------------------------------------------------------------------
      static MRESULT EXPENTRY MainProc (HWND hwnd, ULONG msg, MPARAM mp1,
                                                                                        MPARAM mp2);

      //-----------------------------------------------------------------
      // A method that will be called from within the window procedure to set up stdout
      // redirection and start the unzip prodedure.
      //-----------------------------------------------------------------
      static void RedirectOutput (PVOID pvDm);

   public:
      /******Public Methods ***************************/

      /* First, a constructor to build the window */
      //----------------------------------------------------------------
      //-The first param holds the title for this window.
      //-The second param passes the filename of the zip to be listed.
      //-The third param holds the destination path for any unzipping.
      //-The fourth param holds the path to the temp directory for temporary files.
      //-The fifth param holds a pointer to the string that contains the viewer names.
      //-The sixth param holds a pointer to the string of viewer-supported extensions.
      //-The seventh param holds a count of the available viewers.
      //-The eighth param holds the unzip.exe pathname.
      //-The ninth param holds the zip.exe pathname.
      //-The tenth param holds a handle to a help instance.
      //-The eleventh param holds the handle of the host frame which is used for
      //  relative positioning of this frame.
      //-The twelvth param holds a flag which is either TRUE or FALSE to indicate
      // that the shell has already been to a second level.
      //----------------------------------------------------------------
      zipgui (PSZ pszTitle, PSZ pszFile, PSZ Destination, PSZ Tempdir, PSZ Viewer,
                                      PSZ Viewertype, USHORT viewercount, PSZ unzip, PSZ zip,
                                               HWND help, HWND hostframe, ULONG inuse);

      //-----------------------------------
      // A method to get the handle of the Frame
      //-----------------------------------
      HWND getContainerHWND();

      //--------------------------------
      // A method to change the title bar font
      //--------------------------------
      void titlefont (PSZ pszfont);

      //-------------------------------------------------------------------------
      // A method that will allow sticking in new text into the title bar of the container window
      //-------------------------------------------------------------------------
      void newtitle (PSZ pNewtext);

      //---------------------------------------
      // A method to change the colors of the title bar.
      //---------------------------------------
      void titleparams (ULONG frgnd, ULONG bkgnd);

      //------------------------
      // The required destructor
      //------------------------
      ~zipgui();
     };
#endif

