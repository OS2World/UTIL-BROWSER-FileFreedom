/*----------------------------------------------------------
   FILE FREEDOM -- A Program that manages files and directories
                (c) Bruce Henry 1996
FILE FREEDOM IS PROVIDED AS IS AND COMES WITH NO WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED. IN  NO  EVENT  WILL
BRUCE HENRY BE LIABLE FOR ANY DAMAGES RESULTING
FROM THE USE OF THIS SOFTWARE OR FROM THE USE OF THIS FREELY
AVAILABLE SOURCE CODE ASSOCIATED WITH SAME.
THIS SOURCE CODE HAS BEEN RELEASED INTO THE PUBLIC DOMAIN
ON OCTOBER 7, 1999.
  ---------------------------------------------------------*/
#define INCL_WINDIALOGS
#define INCL_WINWINDOWMGR
#define INCL_DOSPROCESS
#define INCL_WINSTDCNR
#define INCL_WINBUTTONS
#define INCL_WININPUT
#define INCL_WINFRAMEMGR
#define INCL_WINERRORS
#define INCL_WINENTRYFIELDS
#define INCL_DOSFILEMGR
#define INCL_DOSERRORS
#define INCL_DOSMISC
#define INCL_WINLISTBOXES
#define INCL_WINSTDSPIN
#define INCL_WINMENUS
#define INCL_WINPROGRAMLIST
#define INCL_WINSHELLDATA
#define INCL_WINSTDBOOK
#define INCL_DOSSESMGR
#define INCL_WINPOINTERS
#define INCL_WINSYS
#define INCL_GPIBITMAPS
#define INCL_WINHELP
#define INCL_WINACCELERATORS
#define INCL_WINSTDDRAG
#define INCL_DOSSEMAPHORES
#define INCL_GPILCIDS
#define INCL_GPIPRIMITIVES
#define INCL_DOSDEVICES
#define INCL_WINTIMER
#define INCL_DOSNLS

#include <os2.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <fcntl.h>
#include <share.h>
#include <stdio.h>
#include <process.h>
#include "stdwind.h"
#include "ffIDs.h"
#include "contwind.h"
#include "toolbar.h"
#include "roots.h"
#include "dirfiles.h"
#include "txtstrip.h"
#include "message.h"
#include "zipcntr.h"
#include "zipgui.h"

//------------------------------------------------------------
// First, the basic external message processing procedure for any window
// defined below in this 'main' section
//------------------------------------------------------------
MRESULT EXPENTRY MainWindowProc (HWND, ULONG, MPARAM, MPARAM) ;
MRESULT EXPENTRY SubFilesContProc (HWND, ULONG, MPARAM, MPARAM) ;
MRESULT EXPENTRY CopyDlgProc (HWND, ULONG, MPARAM, MPARAM) ;
MRESULT EXPENTRY DeleteDlgProc (HWND, ULONG, MPARAM, MPARAM) ;
MRESULT EXPENTRY CreateDirDlgProc (HWND, ULONG, MPARAM, MPARAM) ;
MRESULT EXPENTRY RenameDlgProc (HWND, ULONG, MPARAM, MPARAM) ;
MRESULT EXPENTRY MoveDlgProc (HWND, ULONG, MPARAM, MPARAM) ;
MRESULT EXPENTRY AttribDlgProc (HWND, ULONG, MPARAM, MPARAM) ;
MRESULT EXPENTRY ViewerDlgProc (HWND, ULONG, MPARAM, MPARAM) ;
MRESULT EXPENTRY PathsDlgProc (HWND, ULONG, MPARAM, MPARAM) ;
MRESULT EXPENTRY EditorDlgProc (HWND, ULONG, MPARAM, MPARAM) ;
MRESULT EXPENTRY SettingsDlgProc (HWND, ULONG, MPARAM, MPARAM) ;
MRESULT EXPENTRY ZipDlgProc (HWND, ULONG, MPARAM, MPARAM) ;
MRESULT EXPENTRY UserDlgProc (HWND, ULONG, MPARAM, MPARAM) ;
MRESULT EXPENTRY ZipnameDlgProc (HWND, ULONG, MPARAM, MPARAM) ;
MRESULT EXPENTRY AboutDlgProc (HWND, ULONG, MPARAM, MPARAM) ;
MRESULT EXPENTRY DragDropDlgProc (HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY ButtonDropProc (HWND, ULONG, MPARAM, MPARAM) ;
MRESULT EXPENTRY ObjectWindowProc (HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY DirBookmarkDlgProc (HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY VolumeLabelDlgProc (HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY TimedRefreshDlgProc (HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY SearchToolDlgProc (HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY AppendDlgProc (HWND, ULONG, MPARAM, MPARAM) ;

//-------------------------------------------------------------------
//Initialize these objects here so that they can be accessed everywhere.
//-------------------------------------------------------------------
contwindow *FirstChild;
contwindow *SecondChild;
toolbar *FFtoolbar;
roottree *DTree;
dirfiles *DFiles;
stdwindow *FFmainwin;
toolbar  *FFdrivebar;
textstrip *Statusbar;
zipgui *Zipshell;
roottree *ddtree;

//------------------------------------------
// Some functions to consolidate some repeated work
//------------------------------------------
void resize ();
void treethread ();
void filesthread();
void getFSInfo (LONG newdrive);
void fillStatusbar1 (ULONG, PSZ);
void fillStatusbar2 (USHORT, ULONG);
void MessageBox (HWND hwnd, APIRET error);
PCNRITEM SearchForOpen ();
void InsertCommas (ULONG, PSZ);
void ChangeDrive (SHORT);
void SetViewerDefaults ();
void SetupViewers ();
void OrientToolbar ();
void ToggleToolbar ();
void DeleteTree (PSZ);
void returnto (PSZ, HWND);
BOOL DoDrag (PRECORDCORE, HWND);
void DrawEmphasis (HWND);
void DoTree (HWND, PCNRITEM, roottree *);
void ExpandTreeFix (PCNRITEM, HWND);
void CollapseTreeFix (PCNRITEM, HWND, roottree *);
void DoCopy (PSZ, PSZ, USHORT, USHORT, USHORT, HWND);
BOOL DoCopyOp (PSZ, PSZ, USHORT, USHORT, USHORT, HWND);
BOOL DoUnzip (SHORT, HWND);
BOOL CompareandRefresh ();
void SetView (ULONG, HWND);
BOOL CopyTree (PSZ, PSZ, PSZ);
BOOL MakeDirs (PCOPYTREEROOTS, PFILEFINDBUF3);
BOOL MakeCopies (PSZ, PSZ, PFILEFINDBUF3);
void NoCanDo ();
void SortFiles (int, HWND);
void CountBytes (PSZ);
ULONG DoTally (PSZ, PFILEFINDBUF3);
ULONG drillfortally (PSZ, PHDIR, INT *, PFILEFINDBUF3, INT, INT);
BOOL IsDriveReady (ULONG);
void WriteMenuItem (PSZ, PSZ, const ULONG, ULONG);
void RefreshVolLabels (ULONG, BOOL);
void SetTimers (HWND, HWND, ULONG, PULONG);
void GetAvailableDrives (BOOL);
void RefillTree (ULONG, ULONG, HWND, roottree *, PSZ);
//----------------------------------------------------------
// Thread creation function - they must have a __syscall format in Watcom
// if using DosCreateThread().
//----------------------------------------------------------
void threadall (PVOID);
//------------------------------------------
// A pointer to a structure for file system information
//------------------------------------------
PFSALLOCATE pFSalloc;
//--------------------------------------------
//Some handles and structures etc. that need file scope
//--------------------------------------------
PFNWP FilesContainerProc;      // Pointer to main window proc
PFNWP *ButtonProc;  // Pointer to array of pointers to the Drive Bar Button procs (again only used by the system)
HWND hwndframe;      // handle for frame window
HWND hwndclient;      // Need file scope so that the Dialog Procedure can access it.
HWND hwndmenu;       // Handle for the main menu.
HWND hwndpopup;      // Handle for the popup menu
HWND hwndtoolbar;     // Handle for the toolbar window frame
HWND hwnddircntr;     // Handle for directory window container
HWND hwndfilescntr;   // Handle for the files window container
HWND hwnddrivebar;   // Handle for the drive bar window frame
HWND hwndRestore;    // Handle used to store the last window to lose focus
HWND hwndStatusbar1; // Handles for the status bar object which can hold two status bars.
HWND hwndStatusbar2;
HWND hwndFilesTitlebar;
HWND hwndTreeTitlebar;
HWND hwndvcombobox, hwndentryfield, hwndentryadd;  // Used in 'ViewerDlgProc'
HWND hwndsettings;
HWND hwndnotebook;
HWND hwndviewers;
HWND hwndfocus;
HWND hwnddropcont;
HWND hwndtemp;
HWND hwndhelp;
HWND hwndobject;
HAB hab1, hab2, hab3, hab4, hab5, hab6;              //anchor block handles
QMSG qmsg;            // Main message queue
HMQ hmq1, hmq2, hmq3, hmq4, hmq5, hmq6;      // Message Queue handles (to QMSG structure)
TID maintid, tid2, tid3, tid4, tid5, tid6, tid7;           // The second, etc. thread ids
HINI hini;                  // Initialization file handle (for FileFrdm.ini).
USHORT flag;           // A general purpose flag
//---------------------------------------------------------------------------
//Variable 'flag' reference:bit 0 =Set= Resize firstchild window
//                          bit 1 =Set= Resize secondchild window
//                          bit 2 =Set= Resize toolbar
//                          bit 3 =Set= Resize drive bar
//                          bit 4 =Set= Block CN_EMPHASIS routine for returnto () function.
//                          bit 5 =Set= Re-Paint 'Source' statusbar
//                          bit 6 =Set= Thread two is finished
//                          bit 7 =Set= Thread three is finished
//                          bit 8 =Set= ...Don't use anymore - too many dependencies on 8-bit values
//---------------------------------------------------------------------------
ULONG toolflag;
ULONG expandflag;
//---------------------------------------------------------------------------
// Used with the toolbar identification message routine in 'WM_CONTROLPOINTER'
//Variable 'toolflag' reference:bit 0  =Set=  not used
//                              bit 1  =Set= The message for PushButton one is showing.
//                              .  .  .  .  .  .
//                              bit 31 =Set= The message for PushButton 31 is showing.
//---------------------------------------------------------------------------
ULONG drivebarflag;
//---------------------------------------------------------------------------
// Used with the drivebar identification message routine in 'WM_CONTROLPOINTER'
//Variable 'drivebarflag' reference:bit 0  =Set=  not used
//                                  bit 1  =Set= The message for DriveButton one is showing.
//                                   .  .  .  .  .  .
//                                  bit 26 =Set= The message for DriveButton 26 is showing.
//                                  bit 27 =Set= The message for the Directory Bookmarks is showing.
//---------------------------------------------------------------------------
ULONG Viewersetflag;

PSTATICDIM ptooldim;            // Pointer to structure that holds the static dims. of the toolbar
PSTATICDIM pdrivedim;           // Same for the drive bar static dimensions.
PBTNINFO ptbarinfo;                 // Button info for the toolbar
PBTNINFO pdbarinfo;                // Button info for the drivebar
PCNRITEM pcnrTemp;               // Used for temporary storage.
PCNRITEM pWhatRec;
PPROGDETAILS pDetails;           // Used in WinStartApp
PROGDETAILS Details;               // Used for above pointer
PDRAGINFO pDinfo;
HELPINIT helpinit;                  // Help initializing structure
PHELPINIT phelpinit = &helpinit;   // Pointer to same
SHORT buttoncolumns = 18;        // The button counts for the toolbar.
SHORT buttonrows = 1;
SHORT totalbuttons = 18;
USHORT TBwidth = 29, TBheight = 29;    // Toolbar button sizes.
USHORT DBwidth = 24, DBheight = 20;    // Drive bar button sizes.
SHORT drivecolumns = 0;          // Variable for drive columns (assigned later)
SHORT driverows = 1;             // Variable for drive rows (default)
SHORT totaldrives;                 // Number to be determined
INT FCheight, FCwidth;     // Store the original dimensions of the First Child window.
INT  SCheight, SCwidth;     // Store the original dimensions of the Second Child window.
INT  orgswpcx, orgswpcy;          // Store the original client window dimensions.
SWP  swp;                         // Structure used for getting and setting window positions.
SWP frameswp;
POINTL popptl;               // Used with loading setting up the pop up menu
RGB rgb;
ULONG CurrDrive, DrvMap;         // used with DosQueryCurrentDisk ()
ULONG DropDrive;                  // used in the DragDrop dialog procedure
ULONG DestDrive;                 // Storage for the destination drive number.
INT drivearray [26];               // An array to hold logical disks (possible 26)
CHAR *pLabelarray;               // Labels for the drive pushbuttons
CHAR szFilesTitle [CCHMAXPATH + 1];
CHAR szSource [81];
CHAR szDestination [81];
CHAR szDestPath [CCHMAXPATH + 1];
CHAR sznamebuffer [(CCHMAXPATH +1) * 2];
CHAR szpathbuffer [(CCHMAXPATH + 1) * 2];  // See notes in 'case COPYOKID:'
CHAR szHomepath [CCHMAXPATH + 1];       // Storage for starting directory (FileFrdm.ini)
CHAR szHomedrivepath [CCHMAXPATH + 1];  // Storage for the home path including drive designator
CHAR szDefaultSource [CCHMAXPATH + 1];   // Storage for Paths dialog.
CHAR szDefaultDest [CCHMAXPATH + 1];     //    "
CHAR szDefaultEditor [CCHMAXPATH + 1];    // Storage for Editor dialog.
CHAR szDefaultEdParms [CCHMAXPATH + 1]; //    "
CHAR szUser1prog [CCHMAXPATH + 1];      //  Storage for associated dialogs
CHAR szUser1dir [CCHMAXPATH + 1];       //            "
CHAR szUser1parms [CCHMAXPATH + 1];    //            "
CHAR szUser1name [14];                     //            "
CHAR szUser2prog [CCHMAXPATH + 1];      //            "
CHAR szUser2parms [CCHMAXPATH + 1];    //             "
CHAR szUser2dir [CCHMAXPATH + 1];       //            "
CHAR szUser2name [14];                     //            "
CHAR szUser3prog [CCHMAXPATH + 1];      //            "
CHAR szUser3parms [CCHMAXPATH + 1];    //             "
CHAR szUser3dir [CCHMAXPATH + 1];       //            "
CHAR szUser3name [14];                     //            "
CHAR szUser4prog [CCHMAXPATH + 1];      //            "
CHAR szUser4parms [CCHMAXPATH + 1];    //             "
CHAR szUser4dir [CCHMAXPATH + 1];       //            "
CHAR szUser4name [14];                     //            "
CHAR szUser5prog [CCHMAXPATH + 1];      //           "
CHAR szUser5dir [CCHMAXPATH + 1];       //            "
CHAR szUser5parms [CCHMAXPATH + 1];    //            "
CHAR szUser5name [14];                     //            "
CHAR szUser6prog [CCHMAXPATH + 1];      //            "
CHAR szUser6parms [CCHMAXPATH + 1];    //             "
CHAR szUser6dir [CCHMAXPATH + 1];       //            "
CHAR szUser6name [14];                     //            "
CHAR szUser7prog [CCHMAXPATH + 1];      //            "
CHAR szUser7parms [CCHMAXPATH + 1];    //             "
CHAR szUser7dir [CCHMAXPATH + 1];       //            "
CHAR szUser7name [14];                     //            "
CHAR szUser8prog [CCHMAXPATH + 1];      //            "
CHAR szUser8parms [CCHMAXPATH + 1];    //             "
CHAR szUser8dir [CCHMAXPATH + 1];       //            "
CHAR szUser8name [14];                     //            "
CHAR szZipprog [CCHMAXPATH + 1];         //             "
CHAR szZipparms [CCHMAXPATH + 1];       //             "
CHAR szUnzipprog [CCHMAXPATH + 1];       //             "
CHAR szUnzipparms [CCHMAXPATH + 1];     //              "
CHAR szDefaultSearchTool [CCHMAXPATH + 1];    //      "
CHAR szDefaultSTParms [CCHMAXPATH + 1]; //           "
CHAR szDirHist [10] [CCHMAXPATH + 1];     // Storage for recently used directory path/names.
CHAR szTreetitlefont [50];
CHAR szFilestitlefont [50];
CHAR szcmdprocessor [] = "cmd.exe";
CHAR szdemostamp [] = "rpffd16";
CHAR szversionstamp [] = "ds20";   //**** This is the string to change each version
CHAR szFormats [] = "<DRM_OS2FILE, DRF_UNKNOWN>";
CHAR szSwapbookmark [CCHMAXPATH + 1];
CHAR szSwappathmark [CCHMAXPATH + 1];
char const *pszCmdProcessor; // This points to the name of the command processor (from OS2_SHELL)
CHAR sztemp [5];
ULONG Homedrive;       //  Storage for the drive the program starts from.
PSZ pszViewerTypes;
PSZ pszViewers;
PSZ pszTbuffer, pszNbuffer;    // Used in the Viewer Dialog
PSZ *pszVT, *pszVN;          // Arrays of pointers used in the Viewer Dialog
ULONG ViewerTypeslen;        // length of the 'ViewerTypes' string.
USHORT Namelength;          // Used in the CN_ENTER (viewer) routine.
SHORT comboitem;           // The current selection in the Viewer combo box.
USHORT addflag;             // Used in ViewerDlgProc
ULONG settingsPage1, settingsPage2, settingsPage3;
ULONG settingsPage4, settingsPage5, settingsPage6;
ULONG settingsPage7, settingsPage8, settingsPage9;
ULONG settingsPage10, settingsPage11, settingsPage12;
ULONG settingsPage13, settingsPage14;
USHORT cValue;             // Needed! - Using local variables in main prog causes artifacts
                              // on files container screen whenever a subdir is opened!
ULONG focusbkgnd, focusfrgnd;
ULONG passivebkgnd, passivefrgnd;
ULONG filefilter;            // Storage for file filter storage
ULONG basicview;          // Storage for container view.
ULONG sortmethod;         // Storage for the sort method
ULONG toolbarorient = 0;    // Storage for toolbar orientation
ULONG xposdivider;          // Storage for the divider x position
USHORT test, ccName, ccPath;
SHORT tempd, skiprefresh;
SHORT whatRec;
USHORT cFiles;
ULONG cbFiles;
ULONG ulApptype;
USHORT regpassed;
CHAR szregnumber [51];    // Stores registration name for Product Information dialog.
ULONG ccRegnumber;
CHAR sznumber [6];
STARTDATA StartData;    /* Start session data structure */
ULONG SessID;            /* Session ID (returned) */
PID pid;                   /* Process ID (returned) */
CHAR ObjBuf [100];       /* Object buffer */
UCHAR ucVoldat [26][17];  // Storage for Volume labels and IDs.
UCHAR ucParms [2];       // Storage needed for DosDevIOCtl
UCHAR ucDataArea [40];   // Storage needed for DosDevIOCtl
ULONG ulParmlen;
ULONG ulDatalen;
APIRET rc;                /* Return code */
BOOL success;
BOOL internalzipviewer;
BOOL internalrunprog;
ULONG Strlength;
SHORT exelen;
SHORT len;
PSZ pszpath;
HMTX hmtx, hmtx2, hmtxFF;
ULONG ulAttr;
BOOL32 ulMutexState, ulMutexState2, ulMutexFFrunning;
ULONG inhibitrefreshflag;
ULONG interval1, interval2;
ULONG ltemp, ltemp1, fEmphasis;
int i, ii;

PFILESANDBYTES pFB;     // Defined in 'cnrstuff.h'
DROPDEST DropDestOp;     // Defined in 'cnrstuff.h'

// "threadparms" and "CopyParms" structures.
// These two structures are used in the threadall () function and must be the identical
// for the first three items in the structrure.
// A pointer to a structure is passed to the function and is used to pull the job, hab
// and hmq parameters for each thread.

struct threadparms
{
   ULONG job;    // Job to be started via a 'switch' statement
   HAB Hab;      // Anchor block handle
   HMQ Hmq;     // Message Queue handle
   CHAR szSavename [CCHMAXPATH +1];  // Name of file (used in returnto ()).
   HWND hwnd;     // Name of window (used in returnto ()).
} tparms2, tparms3, tparms4, tparms6, tparms7;
threadparms *tp, *tp1, *tp2, *tp3, *tp6, *tp7;

struct CopyParms
{
   ULONG job;    // Job to be started via a 'switch' statement
   HAB Hab;      // Anchor block handle
   HMQ Hmq;     // Message Queue handle
   PSZ pszName;    // Name pointer for COPY and MOVE procedures
   PSZ pszPath;     // Path for same.
   USHORT cPath;  // Count of the characters in the path.
   USHORT usReplace;  // Replace state for copy operations.
   USHORT usOp;       // COPY or MOVE operation - defined below as copy = 1, move = 2.
   HWND hwndfocus;  // The window (files or tree) that called this operation.
} cparms;
CopyParms *cp;

const ULONG FILES = 1;
const ULONG TREE = 2;
const ULONG INI = 3;
const ULONG DIVERGE = 4;
const ULONG SCROLLDOWN = 5;
const ULONG SCROLLUP = 6;
const ULONG RETURNTO = 7;
const ULONG DETAILVIEW = 1;
const ULONG COLUMNHEADINGS = 2;
const ULONG FLOWVIEW = 4;
const ULONG ICONVIEW = 8;
const ULONG TEXTVIEW = 16;
const ULONG CUSTOM_NO_FILES_SELECTED = 70000;
const USHORT COPY = 1;
const USHORT MOVE = 2;
const int SORTBYNAME = 1;
const int SORTBYDATE = 2;
const int SORTBYSIZE = 3;
const int SORTBYEXT = 4;
const int REVSORTBYNAME = 5;
const int REVSORTBYDATE = 6;
const int REVSORTBYSIZE = 7;
const int REVSORTBYEXT = 8;
const ULONG DIRTIMER = 22222;
const ULONG DRIVETIMER = 22223;
const ULONG VIEWERSTRLEN = 51;
const ULONG USEPATH = 1;

int main (int argc, char *argv [])
   {
      pFB = new FILESANDBYTES;
      DropDestOp.cbDD = sizeof (DROPDEST);

      static CHAR  szClassName [] = "FileFrdm" ;
      static CHAR  szWindowTitle [] ="File Freedom";

      static ULONG flFrameFlags = FCF_TITLEBAR  | FCF_SYSMENU |  FCF_ICON |
          FCF_SIZEBORDER  | FCF_MINMAX  |  FCF_MENU | FCF_ACCELTABLE |
          FCF_NOBYTEALIGN  |  FCF_AUTOICON | FCF_TASKLIST | FCF_SHELLPOSITION;

      szSwapbookmark [0] = 0;
      szSwappathmark [0] = 0;
      toolflag =0;
      expandflag = 0;
//----------------------------------------------------------------
// Must start off by initializing an Anchor Block Handle for this standard window
//----------------------------------------------------------------
      hab1 = WinInitialize (0) ;
//----------------------------------------------------------------------
// This will fill the QMSG structure with whatever input was available
//----------------------------------------------------------------------
      hmq1 = WinCreateMsgQueue (hab1, 0) ;
//---------------------------------------------------------

//---------------------------------------------------------------------
// All we're after here is the current drive.  The drivemap is refilled later in
// GetAvailableDrives.
//---------------------------------------------------------------------
      DosQueryCurrentDisk (&CurrDrive, &DrvMap);

//------------------------------------------------------------------
//Get the current path so that the FileFrdm.ini file can be written to the same path
//as the executable file is found in (this is done in the INI thread and in WM_CLOSE).
//------------------------------------------------------------------

      ULONG dirpathlen;
      dirpathlen = CCHMAXPATH + 1;
      DosQueryCurrentDir (NULL, szHomepath, &dirpathlen);
      Homedrive = CurrDrive;

      // Now build a complete drive + path string (used in zip viewer for writing
      // temporary files).

      szHomedrivepath [0] = CHAR (Homedrive + 64);
      szHomedrivepath [1] = 0;
      strcat (szHomedrivepath, ":\\");
      strcat (szHomedrivepath, szHomepath);
//-------------------------------------------------------------------
// Load up the required source and destination paths from the ini file
// as well as other immediately required user settings.
// They may not exist but that will set the storage strings to zero length.
// If they do exist, make the necessary changes to ensure that the proper
// DTree and DFiles paths and drives are set.
//-----------------------------------------------------------
      ULONG ulStringlenS, ulStringlenD;
      PSZ ptemp;
      ptemp = szDefaultSource;
      szDefaultSource [0] = 0;

      // Open our initialization file here, and close it later after the window title fonts and
      // colors are loaded.  It must be closed before the main INI thread is called since
      // it will be reopened and closed in that thread.

      hini = PrfOpenProfile (hab1, "FileFrdm.ini");
      ulStringlenS = PrfQueryProfileString (hini, "FileFrdm.exe", "DSource", NULL,
                                                          (PSZ) szDefaultSource, CCHMAXPATH + 1);
      ulStringlenD = PrfQueryProfileString (hini, "FileFrdm.exe", "DDestination", NULL,
                                                            (PSZ) szDefaultDest, CCHMAXPATH + 1);

      // For the file filters, there is a safeguard to insure that there is a value in 'filefilter'
      // regardless of the state of the ini file.

      ltemp = sizeof (ULONG);
      success = PrfQueryProfileData (hini, "FileFrdm.exe", "Filters", &filefilter, &ltemp);
      if (!success) filefilter =  FILE_ARCHIVED | FILE_DIRECTORY | FILE_SYSTEM | FILE_HIDDEN
                                                                                   | FILE_READONLY;
      else filefilter |= FILE_ARCHIVED | FILE_DIRECTORY | FILE_READONLY;

      // Basic views are read here.  The default is the detail view with column headings.

      ltemp = sizeof (ULONG);
      success = PrfQueryProfileData (hini, "FileFrdm.exe", "View", &basicview, &ltemp);
      if (!success) basicview = COLUMNHEADINGS;

      // Sort views are read here.  The default is to sort by name.

      ltemp = sizeof (ULONG);
      success = PrfQueryProfileData (hini, "FileFrdm.exe", "Sort", &sortmethod, &ltemp);
      if (!success) sortmethod = SORTBYNAME;

      // Get the saved toolbar orientation.  bit 0 not set = Horizontal,
      //                                      bit 0 set = Vertical,
      //                                      bit 1 set = Hidden.

      ltemp = sizeof (ULONG);
      success = PrfQueryProfileData (hini, "FileFrdm.exe", "ToolBarPos", &toolbarorient, &ltemp);
      if (!success) toolbarorient = NULL;

      ltemp = sizeof (ULONG);
      success = PrfQueryProfileData (hini, "FileFrdm.exe", "DividerPos", &xposdivider, &ltemp);
      if (!success) xposdivider = NULL;

      // Set some drive defaults;
      // If the INI file has a default source drive stored, use it as the default source.
      // ptemp has been initialized to point to the szDefaultSource string which has also been
      // initialized to a zero length.  If a source string was saved and loaded into
      // szDefaultSource, ptemp will strip the drive name and colon and save the path.
      // If the path is just a drive root designator ("d:\"), ptemp is adjusted so that no backslash
      // is saved as a path (this won't work).  If the default saved is just "d:", a fix-up is
      // done so that this is also acceptable.
      // In other words, the user may specify a root directory as either "d:" or "d:\".

      CurrDrive = Homedrive;
      DestDrive = Homedrive;

      // ulStringlenS and ulStringlenD return the length of the string *including* the
      // the null terminator.

      if (ulStringlenS >1)
      {
         if (szDefaultSource [1] == ':')
         {
            cValue = szDefaultSource [0];
            ptemp = &szDefaultSource [2];

            if (ulStringlenS == 4) ptemp =& szDefaultSource [3];

            if (ulStringlenS > 4)
            {
               if (szDefaultSource [ulStringlenS - 2] == 92) szDefaultSource [ulStringlenS -2] = 0;
            }
            if (cValue > 96) cValue -= 96;
            else cValue -= 64;
            CurrDrive = cValue;
         }
      }
         // If the paths are not fully qualified (including drive designator), ignore them.

      else ulStringlenS = 0;

      // ulStringlenS and ulStringlenD return the length of the string *including* the
      // the null terminator.

      if (ulStringlenD >1)
      {
         if (szDefaultDest [1] == ':')
         {
               cValue = szDefaultDest [0];

               if (ulStringlenD > 4)
               {
                  if (szDefaultDest [ulStringlenD - 2] == 92) szDefaultDest [ulStringlenD -2] = 0;
               }
               if (cValue > 96) cValue -= 96;
               else cValue -= 64;
               DestDrive = cValue;
         }
      }
      else ulStringlenD = 0;
//----------------------------------------------------------------------
// Create a Mutex semaphore that will never be used.
// It's purpose is to indicate to other instances of File Freedom that another instance
// is running.  This allows for the proccess to allow the system to place the window
// so that new instances don't cover over older instances.  The ulMutexFFrunning variable
// is used as a flag to signal the WinRestoreWindowPos function below.
//----------------------------------------------------------------------
      ulMutexFFrunning = 0;
      rc = DosCreateMutexSem ("\\SEM32\\FILEFREEDOM", &hmtxFF, NULL, ulMutexFFrunning);
      if (rc == ERROR_DUPLICATE_NAME) ulMutexFFrunning = 1;
//----------------------------------------------------------------------
// Create a main window and a Message Queue.  It will be self-sizing and positioning
// and initially invisible.
//----------------------------------------------------------------------
      FFmainwin = new stdwindow (hab1, flFrameFlags,szClassName, MainWindowProc,
                                       szWindowTitle, STDWINDOWID);
      hwndclient = FFmainwin->getClientHWND();
      hwndframe = FFmainwin->getFrameHWND();
      hwndmenu = WinWindowFromID (hwndframe, FID_MENU);

// This is a good place to determine the screen resolution so that toolbar button sizes can
// be adjusted.  This is necessary because in 1024 x 768 mode and higher, icons are loaded
// as 40 x 40 regardless if they have been saved at that resolution.
// The solution is to save the icons in both Independant Color Form - VGA (32 x 32) and
// 8514 -16 color (40 x 40) formats.  It makes a bigger .ico file but the operating system will load
// the right resolution icon depending on the screen resolution.
// What I'm doing here is adjusting the actual size of the button that will hold the icon.
// The buttons are originally set at 29 x 29.

      ULONG cx;
      cx = WinQuerySysValue (HWND_DESKTOP, SV_CXSCREEN);
      if (cx > 800)
      {
         TBwidth = 37;
         TBheight = 37;    // Toolbar button sizes.
      }
      // Is there a command line parameter? So far only using "-d" to restore default window

      skiprefresh = 0;
   	if (argc>1)
   	{
   		if (argv[1][0]==45)
   		{
            if (strlen (argv [1]) > 1)
            {
      			if ((argv [1][1] == 68) || (argv [1][1] == 100)) skiprefresh = 1;
            }
         }
      }

      // Pick up the Command Processor from the "SET OS2_SHELL=?" statement in the
      // config.sys file.  This is needed in case the user has installed 4OS2 or some
      // other shell.  The command processor is used in various viewer and zip etc. routines.

      rc = DosScanEnv ("OS2_SHELL", &pszCmdProcessor);
      if (rc) pszCmdProcessor = szcmdprocessor;

      // Load the Help system here so that it is available right away.

      helpinit.cb = sizeof (HELPINIT);               /* Count of bytes of the initialization    */
      helpinit.ulReturnCode = 0;              /* The IPF return code Manager */
      helpinit.pszTutorialName = NULL;           /* Pointer to tutorial name       */
      helpinit.phtHelpTable = (PHELPTABLE) MAKEULONG (HELPTABLEID, 0xffff);
                                                              /* Pointer to help table          */
      helpinit.hmodHelpTableModule = NULLHANDLE;       /* Resource file identity         */
      helpinit.hmodAccelActionBarModule = NULLHANDLE;  /* DLL identity                   */
      helpinit.idAccelTable = 0;              /* Identity of the accelerator table */
      helpinit.idActionBar = 0;               /* Identity of the menu (action) bar template */
      helpinit.pszHelpWindowTitle = "File Freedom Help";
                                               /* Window title for the main help window */
      helpinit.fShowPanelId = CMIC_HIDE_PANEL_ID;   /* Show window identity indicator */
      helpinit.pszHelpLibraryName = "FileFrdm.hlp";       /* Help library names of .HLP files */

      hwndhelp = WinCreateHelpInstance (hab1, &helpinit);
      if (!hwndhelp)
      {
         WinMessageBox (HWND_DESKTOP, hwndframe, "Can't find FileFrdm.hlp\nHelp not available",
                                        "ERROR!", MESSAGEBOXID, MB_OK | MB_ERROR);
      }
      else
      {
         WinAssociateHelpInstance (hwndhelp, hwndclient);
      }

//-----------------------------------------------------------------
// Add a Status Bar to the main window.  When handles are required for this object
// (there can be two handles used) they can be retrieved using the hard-coded IDs
// of 598 and 599.  These handles will be needed to update the status message.
//-----------------------------------------------------------------
      Statusbar = new textstrip (hwndframe, hwndclient, 2);
      hwndStatusbar1 = WinWindowFromID (hwndframe, 598);
      hwndStatusbar2 = WinWindowFromID (hwndframe, 599);

//----------------------------------------------------------------
// The window frame must be resized in order for the status bars to be activated.
// This is a very small move and is done with the window hidden, and then it is restored to
// it's original size so that a cummulative shrinking (or growing) of the window
// doesn't take place.
//----------------------------------------------------------------

// NOTE: This only seems needed in 1024 x 768.  Something I've done has fixed this problem
//           in the other modes, but I must do it for this one.


      WinSetWindowPos (hwndframe, HWND_TOP, 0, 0, 0, 0, SWP_HIDE);

      WinQueryWindowPos (hwndframe, &frameswp);
      WinSetWindowPos (hwndframe, HWND_TOP, frameswp.x, frameswp.y,
                                              frameswp.cx, frameswp.cy - 1, SWP_SIZE);
      WinSetWindowPos (hwndframe, HWND_TOP, frameswp.x, frameswp.y,
                                              frameswp.cx, frameswp.cy, SWP_SIZE);

      // Now that the frame has a handle, we can retrieve its last position from the OS2.INI
      // file.  If this is the first start (skiprefresh = 1), the system will place the window.
      // If another instance is running, we can retrieve the alternate frame position which
      // will keep the frames of the two instances in the same position relative to each other.
      // If more than two frames are opened, the others will be hidden beneath the second one.
      // This will also make the window visible and is the first view of the complete window.

      if ((!skiprefresh) && (!ulMutexFFrunning))
             WinRestoreWindowPos ("FileFreedom", "Position", hwndframe);
      else if ((!skiprefresh) && (ulMutexFFrunning))
             WinRestoreWindowPos ("FileFreedom", "AltPosition", hwndframe);

      // Also be sure to use the SWP_RESTORE  param in case the window
      // had been minimized before being closed.  This causes problems with the math used
      // to find window areas.
      // It is also necessary to have a SWP_SHOW if a '-d' parameter is used, and on first start.
      // The program hangs without it in these instances.
      // If the window is already visible, the SWP_SHOW won't hurt anything.

      WinSetWindowPos (hwndframe, HWND_TOP, 0, 0, 0, 0, SWP_SHOW | SWP_RESTORE);

//-------------------------------------------------
// Create a toolbar on the main window.  First get the size of the client area and determine
// from the width if one row of buttons will fit or if it will have to be split into
// two rows of buttons.
// Process this even if the toolbar is hidden ((toolbarorient & 2) == 1) since the toolbar must
// be created even if it is hidden.
// Also process the DriveBar.
//-------------------------------------------------
      WinQueryWindowPos (hwndclient, (PSWP)&swp);

      // For a horizontal bar do this

      if ((toolbarorient & 1) == 0)
      {
         if (swp.cx < (totalbuttons * (TBwidth + 2)))
         {
            buttonrows = 2;
            buttoncolumns = SHORT (totalbuttons / 2);
            if ((buttoncolumns * 2) < totalbuttons) buttoncolumns++;
         }
         else
         {
            buttonrows = 1;
            buttoncolumns = totalbuttons;
         }
      }
      // For a vertical bar do this.  Also switch the drive buttonbar rows and columns
      // here since it must move with the toolbar.
      // Note that the drive buttonbar will only be a single column if vertical or a
      // single row if horizontal.

      else
      {
         if (swp.cy < (totalbuttons * (TBheight + 2)))
         {
            buttonrows = SHORT (totalbuttons / 2);
            if ((buttonrows * 2) < totalbuttons) buttonrows++;
            buttoncolumns = 2;
         }
         else
         {
            buttonrows = totalbuttons;
            buttoncolumns = 1;
         }
      }

      // Fill in the information used to create the toolbar.  This is also used
      // later here for repositioning information storage.

      ptbarinfo = new BTNINFO;
      ptbarinfo->btncx = TBwidth;
      ptbarinfo->btncy = TBheight;
      ptbarinfo->flBstyle = BS_PUSHBUTTON | BS_NOPOINTERFOCUS | BS_ICON;
      ptbarinfo->brows = buttonrows;
      ptbarinfo->bcolumns = buttoncolumns;
      ptbarinfo->totalbuttons = totalbuttons;
      ptbarinfo->reservecx = 0;
      ptbarinfo->reservecy = 0;
      ptbarinfo->reservecolumns =0;
      ptbarinfo->reserverows = 0;
      ptbarinfo->label = NULL;
      ptbarinfo->labelsize = NULL;

      FFtoolbar = new toolbar(hwndclient, TOOLBARID, ptbarinfo);
      ptooldim = FFtoolbar->rejig( buttonrows, buttoncolumns);

//-//-------------------------------------------------
//-//    Get the drive table and create a drive bar on the main window
//-//-------------------------------------------------

      GetAvailableDrives (FALSE);
//-----------------------------------------------------------
// Create two container windows.
// Be sure to send size parameters that leave room for toolbars, etc.
//-----------------------------------------------------------
      WinQueryWindowPos (hwndclient, (PSWP)&swp) ;
      orgswpcx=swp.cx;
      orgswpcy=swp.cy;

      // If there is not a saved width, make first window approx. one third of client
      // width minus the width of the toolbar and drive bar if it is so configured.

      if (!xposdivider) FCwidth = (orgswpcx-ptooldim->horz-pdrivedim->horz)/3;
      else FCwidth = xposdivider;

      // Full height of client minus toolbar and drive bar height if toolbar is a horizontal toolbar.

      FCheight=orgswpcy-ptooldim->vert - pdrivedim->vert;

      SCwidth=orgswpcx-FCwidth - ptooldim->horz - pdrivedim->horz;
      SCheight = FCheight;

      FirstChild = new contwindow(hwndclient, DIRWINDOWID, 0, 0, FCwidth, FCheight,
                                    orgswpcx, orgswpcy,  " ");

//-----------------------------------------------------------------------
// Test the current drive for media (it could be an INI file setting).
// If for some reason that drive is not ready, a value of 1 will be returned and the Current
// drive will be reset to the Home drive (which is known to be ready).
//-----------------------------------------------------------------------
      rc = IsDriveReady (CurrDrive);
      if (!rc)
      {
         CurrDrive = Homedrive;
         ulStringlenS = 0;
       }

      // Create a title banner for the files container title strip with the directory path in it.
      // If there was a default Source path written to the ini file, use it.

      if (ulStringlenS) strcpy (szFilesTitle, szDefaultSource);
      else
      {
         szFilesTitle [0] = CHAR (CurrDrive + 64);
         szFilesTitle [1] = 58;
         szFilesTitle [2] = 92;
         szFilesTitle [3] = 0;
      }

      SecondChild = new contwindow(hwndclient, FILESWINDOWID, FCwidth, 0, SCwidth,
                                       SCheight, orgswpcx, orgswpcy, szFilesTitle);

      // Copy this path into the default start-up destination path on the status bar
      // unless there is a default destination path set in the ini file.

      if (!ulStringlenD) fillStatusbar1 (CurrDrive, (PSZ) szFilesTitle);
      else fillStatusbar1 (DestDrive, (PSZ) szDefaultDest);
//--------------------------------------------------------------------------
// Note:  The toolbar and the other modeless windows are not set to be visible. The first
// repositioning sets the (SWP_SHOW) flag making them visible and is similarily set each time
// the client window is resized with the same routine in the 'case WM_SIZE' procedure.
// Also get some window handles and read in the window position for the main window from
// the OS2.INI file.  WinRestoreWindowPos also restores the window presentation parameters,
// which also are wanted here to restore colours and fonts that may have been dropped on the
// the windows by the user.
//------------------------------------------------------------------
      hwnddircntr = FirstChild->getContainerHWND();
      hwndfilescntr = SecondChild->getContainerHWND();
      hwndTreeTitlebar = FirstChild->getTitlebarHWND ();
      hwndFilesTitlebar = SecondChild->getTitlebarHWND ();
      hwndtoolbar = FFtoolbar->getToolbarHWND();
      hwnddrivebar = FFdrivebar->getToolbarHWND();

      // Set a known value into the QWL_USER window word of the Files container so that
      // it can be recognized by concurrent FF processes.  This is used in the OBJM_DROP
      // message handler in the Object window.

      WinSetWindowULong (hwndfilescntr, QWL_USER, 123456);

      // Sub-class the DriveBar Buttons here for use with the Drag and Drop implementation

      ButtonProc = new PFNWP [totaldrives];

      for (i = 0; i < totaldrives; i++)
      {
         hwndtemp = WinWindowFromID (hwnddrivebar, pbA + i);
         ButtonProc [i] = WinSubclassWindow (hwndtemp, (PFNWP) ButtonDropProc);

         // Save the index to this window procedure pointer in the QWL_USER window word
         // so that the sub-classed procedure will be able to get the pointer to the originating
         // Button window procedure.

         WinSetWindowULong (hwndtemp, QWL_USER, (ULONG) i);
      }

      // Was there a command line parameter? So far only using "-d" to restore default window
      // colours and fonts.  This was resolved after the main window was created above
      // (skiprefresh == 0).

      success = 0;
      if (!skiprefresh)
      {
         WinRestoreWindowPos ("FileFreedom", "Directories", hwnddircntr);
         WinRestoreWindowPos ("FileFreedom", "Files", hwndfilescntr);
         WinRestoreWindowPos ("FileFreedom", "StatusBar1", hwndStatusbar1);
         success = (BOOL) WinRestoreWindowPos ("FileFreedom", "StatusBar2", hwndStatusbar2);
         WinRestoreWindowPos ("FileFreedom", "Toolbar", hwndtoolbar);
         WinRestoreWindowPos ("FileFreedom", "Drivebar", hwnddrivebar);
      }
      // Now is a good time to reset the filescontainer selection-bar background in case
      // it has been changed and the WM_PRESPARAMCHANGED message routine was not
      // called (system crashed etc.?).  This is used in the WM_DRAWITEM for the
      // container CA_OWNERDRAW feature.
      // See the notes there for more info.

      ltemp = SYSCLR_HILITEBACKGROUND;
      WinSetPresParam (hwndfilescntr, PP_HILITEBACKGROUNDCOLORINDEX, sizeof (ULONG),
                           (PVOID) &ltemp);

      // Set up the default colors for the window titlebars.  The Titlebars are created with
      // colors set the same, so we must adjust them now.

      focusfrgnd = 0x040424;
      focusbkgnd = 0x8CBCDC;
      passivebkgnd = 0xDCDCDC;
      passivefrgnd =  0x040424;

      // If success was set in the WinRestoreWindowPos above, it means that this is not the
      // first run for the program and therefore we must check for new defined colors and fonts
      // saved to the INI file.

      if (success)
      {
         PrfQueryProfileString (hini, "FileFrdm.exe", "Treetitlefont", NULL,
                                                          (PSZ) szTreetitlefont, 50);
         FirstChild->titlefont ((PSZ) szTreetitlefont);
         PrfQueryProfileString (hini, "FileFrdm.exe", "Filestitlefont", NULL,
                                                          (PSZ) szFilestitlefont, 50);
         SecondChild->titlefont ((PSZ) szFilestitlefont);

         Strlength = sizeof (ULONG);
         PrfQueryProfileData (hini, "FileFrdm.exe", "Activetitlecolor", (PVOID) &focusbkgnd,
                                                                                &Strlength);
         Strlength = sizeof (ULONG);
         PrfQueryProfileData (hini, "FileFrdm.exe", "Passivetitlecolor", (PVOID) &passivebkgnd,
                                                                                &Strlength);
      }

      // Close the INI file now.  It must be closed before the INI thread is called since it
      // will be reopened and closed there.

      PrfCloseProfile (hini);

      // Set the window title colors, whatever they have been determined to be (default or
      // loaded from the INI file).

      FirstChild->titleparams (passivefrgnd, passivebkgnd);
      SecondChild->titleparams (focusfrgnd, focusbkgnd);

      flag = 0;
      flag|=1;             // Enable resizeing of firstchild window
      flag|=2;             // Enable resizing of secondchild window
      flag|=4;             // Enable re-sizing of toolbar.
      flag|=8;             // Enable re-sizing of the drive bar.
      resize();           // Draw the modeless windows to fit and make visible.

//-------------------------------------------------------------------------
// This looks like a good place to sub-class the files window procedures (used for DM_RENDER)
//-------------------------------------------------------------------------
      FilesContainerProc = WinSubclassWindow (hwndfilescntr, (PFNWP) SubFilesContProc);
//-----------------------------------------------------------------------
// Get the Current drive and put the File System info for that drive on the container title.
//-----------------------------------------------------------------------
      getFSInfo (CurrDrive);
//-----------------------------------------
// Get the directory tree and stuff it in the container
//-----------------------------------------
      DTree = new roottree (hwnddircntr, (SHORT) CurrDrive);

      // Send the refilltree function a flag that indicates that the path should be
      // taken from the szFilesTitle string. (USEPATH)

      RefillTree (CurrDrive, USEPATH, hwnddircntr, DTree, NULL);
//-------------------------------------------------------
// Fill the other container with the files from the required directory
//-------------------------------------------------------
      DFiles = new dirfiles (hwndfilescntr, (SHORT) CurrDrive);

      // A thread isn't used for this first fill of the files container because
      // nothing can be done until this container is filled.  The root container is
      // on another thread so they are not interfering with each other.
      // Ptemp has been set with the default source path found in the ini file
      // and CurrDrive has been changed if necessary.

      if (basicview & DETAILVIEW)
      {
         DFiles->SetDetailView ();
         SetView (DETAILVIEW, hwndmenu);
      }
      else if (basicview & COLUMNHEADINGS)
      {
         // No menu settings have to be changed here since this is the default.

         DFiles->SetColumnHeadings ();
      }
      else if (basicview & FLOWVIEW)
      {
         DFiles->SetFlowView ();
         SetView (FLOWVIEW, hwndmenu);
      }
      else if (basicview & ICONVIEW)
      {
         DFiles->SetNameView ();
         SetView (ICONVIEW, hwndmenu);
      }
      else if (basicview & TEXTVIEW)
      {
         DFiles->SetTextView ();
         SetView (TEXTVIEW, hwndmenu);
      }
      // Set the sort method here for the files container.  Allthough the files haven't been
      // loaded yet, the container will accept an order.
      // Nothing is required if the sort method is sortbyname since it is the default.

      if (sortmethod != SORTBYNAME)
      {
         SortFiles (sortmethod, hwndmenu);
      }
      if (ulStringlenS) DFiles->PathToOrder (ptemp, (SHORT) CurrDrive);
      DFiles->FillContainer (filefilter);

      // Must set this next flag to enable drive changes.  It is disabled when a thread
      // is running that manipulates the current drive.

      flag |= 192;

      fillStatusbar2 (NULL, NULL);
//-----------------------------------------------------------------
// This is a good place to start a thread to load the info from the FileFrdm.ini file.
// Be sure not to use this tparms4 structure again since this thread will remain
// running to handle an object window that runs concurrent to the main window.
//-----------------------------------------------------------------
      tparms4.Hab = hab4;
      tparms4.Hmq = hmq4;
      tparms4.job = INI;
      tid4 = _beginthread (threadall, NULL, 8192, &tparms4);

      if (toolbarorient & 2) ToggleToolbar ();

      // Also set the focus to the files window.

      WinSetFocus (HWND_DESKTOP, hwndfilescntr);
      hwndfocus = hwndfilescntr;

//----------------------------------------------------------------------
// The code to test for demo stamp or registration number used to be here.
// This isn't needed anymore but is still a good place to install a Desktop object
// if it already hasn't been done.  If an object already exists, it won't be created.
// (CO_FAILIFEXISTS)
//----------------------------------------------------------------------

      // Get rid of any demo stamps - this is now Freeware.

      PrfWriteProfileData (HINI_USERPROFILE, "rpffd16", NULL, NULL, NULL);

      CHAR szSetupString [300];
      strcpy (szSetupString, "PROGTYPE=PM;EXENAME=");
      strcat (szSetupString, szHomedrivepath);
      strcat (szSetupString, "\\filefrdm.exe;OBJECTID=<FF_FILEMANAGER>");
      HOBJECT hObject;
      hObject = WinCreateObject ("WPProgram", "File Freedom", szSetupString,
                          "<WP_DESKTOP>", CO_FAILIFEXISTS);

   //-------------------------------------------------------------
   // This is a good place to create an mutex semaphore that will be used in
   // Copy and Move routines to signal when they have completed.
   // This semephore may only actually be used by the copy/move thread
   // but queried by other operations.
   // This is also a good place to create a mutex for the dynamic scrolling
   // while swiping routine threads.
   //-------------------------------------------------------------

      ulAttr = 0;
      ulMutexState = FALSE;
      rc = DosCreateMutexSem (NULL, &hmtx, ulAttr, ulMutexState);
      ulMutexState2 = FALSE;
      rc = DosCreateMutexSem (NULL, &hmtx2, ulAttr, ulMutexState2);

      //-------------------------------------------
      // This is the basic 'Wait for input loop'
      //-------------------------------------------

      while (WinGetMsg (hab1, &qmsg, NULLHANDLE, 0, 0))
         {
            WinDispatchMsg (hab1, &qmsg) ;
         }
//--------------------------------------------------------------------
// On exiting, the appropriate user settings will be written to the "FileFrdm.ini" file
// through a routine in the WM_CLOSE portion of the main window procedure below.
// When I had the procedure located here, it produced a box-like artifact when a sub-
// directory was opened from the files container.
//--------------------------------------------------------------------

      WinDestroyMsgQueue (hmq1);
      WinTerminate (hab1);

      return 0 ;
     }
//----------------------------------------------------------------------
// This is a sub-classed procedure for the Files Container window.  This is needed to
// implement dynamic scrolling during swiping.  The mouse is captured during
// a swipe and this is the only way to intervene.
// A new use has been found for this sub-classed procedure - handling the DM_RENDER
// portion of inter-FF Drag/Drop operations from concurrent processes.
//----------------------------------------------------------------------
MRESULT EXPENTRY SubFilesContProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
      switch (msg)
      {
         // Note:
         // WM_BEGINSELECT and  WM_BUTTON1DOWN don't seem to arrive here.

         // Here is where we handle the Drag/Drop packages from a concurrent FF process.
         // This allows two or more instances of FF to be running concurrently and Drag/Drop
         // operations to be performed between instances.
         // This is done by requesting that the Source of the operation RENDER the package.
         // The message is intercepted here and is eventually processed as if it were a normal
         // DragDrop request.  Normally, with an internal Drag/Drop operation in a single instance,
         // the target window handles the rendering.


         case DM_RENDER:

            // Get the pointer to the DRAGTRANSFER structure.

            PDRAGTRANSFER pDxfer;
            pDxfer = (PDRAGTRANSFER) PVOIDFROMMP (mp1);

            // Get the target name which includes the drive and path.  All we want
            // are the drive letter and sub-dir path here.  The filename was added in case
            // a foreign process was requesting to drop.

            DrgQueryStrName (pDxfer->hstrRenderToName, CCHMAXPATH +1, szpathbuffer);

            PSZ pPath;
            pPath = &szpathbuffer [2];
            DropDrive = szpathbuffer [0] - 64;
            DropDestOp.szDropName [0] = 0;

            // Since this is a message from an Object window in another FF process,
            // retrieve and save the Object window handle
            // so that a "refresh Files display" message can be sent back to the caller
            // when all threads have finished rendering (OBJM_REFRESH).

            // The first handle will be the handle of the Client window since it is the
            // Parent of the Files Container window.  The Parent of the Client window
            // will be the Frame window which is where the handle to that instance's
            // Object window is stored (at zero index into the user-requested window
            // word storage area).

            hwndtemp = WinQueryWindow (pDxfer->hwndClient, QW_PARENT);
            hwndtemp = WinQueryWindow (hwndtemp, QW_PARENT);
            DropDestOp.hwndCaller = (HWND) WinQueryWindowULong (hwndtemp, 0);

            // Find where the target directory name begins (foreshorten the path)
            // and use this name as the new directory name (pDirname).

            int pathlen, stub, findbackslash;
            pathlen = strlen (pPath);
            stub = 0;
            for (findbackslash = 0; findbackslash < pathlen; findbackslash ++)
            {
               if (pPath [findbackslash] == 92) stub = findbackslash;
            }
            pPath [stub] = 0;

            strcpy (DropDestOp.szDropPath, pPath);
            DropDestOp.ccPathname = (USHORT) strlen (pPath);

            // Change the operation to a form my DragDrop Dlg can understand.

            if (pDxfer->usOperation == DO_COPY) DropDestOp.Operation = 2;
            if (pDxfer->usOperation == DO_MOVE) DropDestOp.Operation = 4;

            // Call up the DragDrop Dialog and get the necessary information about
            // destination and operation.
            // Note: The default operation going into this dialog is MOVE.  This can be
            // changed to COPY via the Control key.  Other key combinations could be
            // implemented to invoke ZIP and UNZIP operations, but presently these must
            // be changed manually in the dialog.

            WinDlgBox (HWND_DESKTOP, hwnd, DragDropDlgProc, NULLHANDLE, DDDEST_ID,
                                            (PVOID) &DropDestOp);

            // Set the flag in DropDestOp.Operation so that the DoCopyOp thread can send a
            // message back to the Files window in the calling FF to refresh the display.
            // This must be done in DoCopyOp because it is running as a separate thread.

            DropDestOp.Operation |= 64;

            // Read the replace files flag set in the DragDropDlgProc.

            USHORT usReplace;
            usReplace = USHORT (DropDestOp.Operation & 1);

            // Pick out the operation to perform on the files selected
            // NOTE:
            //       For all of the following operations, the destination drive has already
            //       been tested and found to be ready.  This was done in CN_DROP which
            //       in turn calls OBJM_DROP which in turn sends any DM_RENDER calls.

            if (DropDestOp.Operation & 2)
            {
               // Now, change the destination drive/directory to the Drop Path.  This will
               // make it easy for Drag/Droppers to check the destination after doing a drop.
               // It is also required to get the proper destination set when dropping between
               // instances.

               fillStatusbar1 (DropDrive, (PSZ) DropDestOp.szDropPath);

               // Call the copy routine that is shared with the standard CopyDlgProc.

               DoCopy (DropDestOp.szDropName, DropDestOp.szDropPath,
                                     usReplace,  DropDestOp.ccPathname, COPY, hwndfilescntr);
            }
            else if (DropDestOp.Operation & 4)
            {
               fillStatusbar1 (DropDrive, (PSZ) DropDestOp.szDropPath);

               // Call the MOVE routine that is shared with the MoveDlgProc

               DoCopy (DropDestOp.szDropName, DropDestOp.szDropPath,
                                     usReplace,  DropDestOp.ccPathname, MOVE, hwndfilescntr);
            }
            else if (DropDestOp.Operation & 8)
            {
               // First, change the destination to the dragdrop choice.

               strcpy (szDestPath, DropDestOp.szDropPath);

               // Call the ZipName dialog

               WinDlgBox (HWND_DESKTOP, hwnd, ZipnameDlgProc, NULLHANDLE,
                                                                INPUTZIPNAMEID, NULL);
               fillStatusbar1 (DropDrive, (PSZ) DropDestOp.szDropPath);

               // Reset the default disk since it somehow gets lost in the dragdrop shuffle.

               DosSetDefaultDisk (CurrDrive);
            }
            else if (DropDestOp.Operation & 16)
            {
               // First, change the destination to the dragdrop choice.

               fillStatusbar1 (DropDrive, (PSZ) DropDestOp.szDropPath);

               // Call the Unzip routine

               DoUnzip ((SHORT) MID_UNZIP, hwnd);

               // Reset the default disk since it somehow gets lost in the dragdrop shuffle.

               DosSetDefaultDisk (CurrDrive);
            }

            else if (DropDestOp.Operation & 128)
            {

               // First, change the destination to the dragdrop choice.

               strcpy (szDestPath, DropDestOp.szDropPath);

               // Call the Append dialog

               WinDlgBox (HWND_DESKTOP, hwnd, AppendDlgProc, NULLHANDLE,
                                                                APPENDID, NULL);
               fillStatusbar1 (DropDrive, (PSZ) DropDestOp.szDropPath);

               // Reset the default disk since it somehow gets lost in the dragdrop shuffle.

               DosSetDefaultDisk (CurrDrive);
            }

            else if (DropDestOp.Operation & 32)
            {
               // The user has pressed the cancel button, so Cancel this call.
               // Reset the default disk since it somehow gets lost in the dragdrop shuffle.

               DosSetDefaultDisk (CurrDrive);
            }

            return (*FilesContainerProc) (hwnd, msg, mp1, mp2);

         case WM_MOUSEMOVE:

           // NOTES:
           // This is used for the Auto-Scrolling routine.
           // It seems that the mouse is captured by the files container window only during
           // a swipe.

            SWP filesswp;
            POINTL ptl;
            HWND temp;
            temp = WinQueryCapture (HWND_DESKTOP);

            // First check to make sure the files container has the capture.

            if (temp == hwndfilescntr)
            {
               // Find the position of the mouse relative to the container window.

               WinQueryWindowPos (hwnd, &filesswp);

               rc = WinQueryPointerPos (HWND_DESKTOP, &ptl);
               WinMapWindowPoints (HWND_DESKTOP, hwnd, &ptl, 1);

               // If the mouse is below the container window (allow 12 pels for the scrollbar)
               // then it is time to scroll dynamically for one line.  This is done in a thread
               // and is controlled by a mutex semephore so that threads don't overlap.
               // Strict control of the mutex is the key to having this work right.  If mutex
               // requests have piled up they are released until there are none left.  Then
               // and only then is the mutex requested for the Scroll thread.  Otherwise, a
               // condition can arise where there some requests on hand and newer requests
               // only are removed, leaving a balance of requests that can't be removed
               // and therefore blocking the scroll.
               // The mutex query conditional statement assures that eventually all
               // requests will be removed.

               if (ptl.y < (filesswp.y + 12))
               {
                  PID pid;
                  TID tid;
                  rc = DosQueryMutexSem (hmtx2, &pid, &tid, &ulMutexState2);
                  if (ulMutexState2 == 0)
                  {
                     rc = DosRequestMutexSem (hmtx2, SEM_IMMEDIATE_RETURN);
                     if (!rc)
                     {
                        inhibitrefreshflag = 1;
                        tparms6.Hab = hab6;
                        tparms6.Hmq = hmq6;
                        tparms6.job = SCROLLDOWN;
                        tid6 = _beginthread (threadall, NULL, 8192, &tparms6);
                     }
                  }
                  else DosReleaseMutexSem (hmtx2);
               }

               // If the mouse pointer is above the container window (allow 12 pels for the
               // title bar) it is time to scroll up dynamically for one line.

               else if (ptl.y > (filesswp.cy - 12))
               {
                  PID pid;
                  TID tid;
                  rc = DosQueryMutexSem (hmtx2, &pid, &tid, &ulMutexState2);
                  if (ulMutexState2 == 0)
                  {
                     rc = DosRequestMutexSem (hmtx2, SEM_IMMEDIATE_RETURN);
                     if (!rc)
                     {
                        inhibitrefreshflag = 1;
                        tparms6.Hab = hab6;
                        tparms6.Hmq = hmq6;
                        tparms6.job = SCROLLUP;
                        tid6 = _beginthread (threadall, NULL, 8192, &tparms6);
                     }
                  }
                  else DosReleaseMutexSem (hmtx2);
               }
            }
            return (*FilesContainerProc) (hwnd, msg, mp1, mp2);

         default:
            return (*FilesContainerProc) (hwnd, msg, mp1, mp2);
      }
}
//----------------------------------------------------------------------
// This procedure processes the messages found in the QMSG structure "qmsg"
// The name for this procedure is sent to the 'WinRegisterClass' procedure (in stdwind.h)
//----------------------------------------------------------------------
MRESULT EXPENTRY MainWindowProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
      switch (msg)
      {
         case WM_TIMER:

            if (SHORT1FROMMP (mp1) == DIRTIMER)
            {
               // Check for a blocking flag.

               if (inhibitrefreshflag) return 0;

               pWhatRec = (PCNRITEM) WinSendMsg (hwndfilescntr,
                                     CM_QUERYRECORDEMPHASIS, MPFROMP (CMA_FIRST),
                                                            MPFROMSHORT (CRA_SELECTED));
               if (pWhatRec) return 0;

               // Save the filename so that we can returnto () it below.

               pWhatRec = (PCNRITEM) WinSendMsg (hwndfilescntr,
                                     CM_QUERYRECORDEMPHASIS, MPFROMP (CMA_FIRST),
                                                           MPFROMSHORT (CRA_CURSORED));
               // Protection is needed here in case the drive or subdir has disappeared.
               // In this case no file will be found with the cursor on it or otherwise.

               if (pWhatRec)
                  strcpy (tparms7.szSavename, pWhatRec->szFileName);

               // Test for DoCopy () thread mutex

               if (ulMutexState)
               {
                  NoCanDo ();
                  break;
               }
               // Refresh the source and destination containers, info, etc.

               // Copy the path into the destination status bar spec and update info.

               fillStatusbar1 (DestDrive, (PSZ) szDestPath);

               // Update the root window titlebar byte count.

               getFSInfo (CurrDrive);

               // Reload the root tree container with the source drive tree
               // BUT first hide the files window to get rid of the flicker.

               WinSetWindowPos (hwndfilescntr, HWND_TOP, 0, 0, 0, 0, SWP_HIDE);

               // This routine reloads the Tree window and also reopens any open folders
               // and places the cursor on them.

               RefillTree (CurrDrive, USEPATH, hwnddircntr, DTree, NULL);

               // Refill the files container to the original path.

               DFiles->EmptyContainer ();
               DFiles->FillContainer (filefilter);
               fillStatusbar2 (NULL, NULL);

               // Use the returnto thread to move the cursor back on to the last file it was on.

               tparms7.Hab = NULL;
               tparms7.Hmq = NULL;
               tparms7.hwnd = hwndfilescntr;
               tparms7.job = RETURNTO;
               tid7 = _beginthread (threadall, NULL, 8192, &tparms7);
            }
            if (SHORT1FROMMP (mp1) == DRIVETIMER)
            {
               GetAvailableDrives (TRUE);
               OrientToolbar ();

               // Refresh the volume labels also, but skip the two possible floppys
               // since they are local anyway and the process is too time consuming.

               RefreshVolLabels (3, FALSE);

            }
            return 0;

         case WM_PRESPARAMCHANGED:

            if (hwnd != hwndfilescntr) return FALSE;

            // If the highlight colour in the files container has been changed, change it back
            // to the standard.  This could happen if someone drops a scheme palatte on this
            // window etc. Otherwise, an index colour will have to be set up that can be
            // used in the WinDrawText routine below (in WM_DRAWITEM).

            ULONG colour;
            colour = SYSCLR_HILITEBACKGROUND;
            WinSetPresParam (hwndfilescntr, PP_HILITEBACKGROUNDCOLORINDEX, sizeof (ULONG),
                                 (PVOID) &colour);
            return FALSE;
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
         // So far this is only used with the files container when sub-directories are
         // owner drawn.

         case WM_DRAWITEM:

            // Make a buffer and fill it with spaces.  This is required because there is no way
            // to tell exactly how long a string is needed to fill the column in a details view.
            // This column will hold the sub-directory name in this case but also holds
            // file names which can be quite long.
            // The column will be the length of the longest name entered into it and this is
            // not known without doing a dry run.  This is a much simpler solution - the string
            // is made much longer than could possibly occur (padded with spaces) and then the
            // container control truncates it wherever it wants.

            CHAR szbuffer [CCHMAXPATH +1];
            memset (szbuffer, ' ', CCHMAXPATH);
            szbuffer [CCHMAXPATH] = 0;

            POWNERITEM pOwneritem;
            PCNRDRAWITEMINFO pInfo;
            PCNRITEM pOwnerRec;
            LONG lena, lenb, written;

            lenb = strlen (szbuffer);
            pOwneritem = (POWNERITEM) PVOIDFROMMP (mp2);

            // If the item id is that of the icon in a Text-Icon pair, let the container draw
            // it.  All we want to draw is the name.

            if (pOwneritem->idItem == CMA_ICON) return MPFROMSHORT (FALSE);
            pInfo = (PCNRDRAWITEMINFO) pOwneritem->hItem;
            pOwnerRec = (PCNRITEM) pInfo->pRecord;

            // If the record pointer is null, this means in this scenario that it isn't a record
            // name being drawn, but rather the column title as in the case of the Details view
            // with Column Headings.  I didn't find this documented anywhere, but it seems to
            // be true.
            // In this case we want the container to draw the column titles.

            if (pOwnerRec == NULL)   return MPFROMSHORT (FALSE);

            // Check to see if it's a sub-directory

            if (pOwnerRec->attrFile & 16)
            {
               // If it is a sub-directory name, transfer the name string into the buffer filled with
               // spaces.  Remember to leave a single space at the beginning in order to match the
               // container's style.]

               lena = strlen (pOwnerRec->szFileName);
               ii = lena + 1;
               for (i = 1; i < ii; i++)
               {
                  szbuffer [i] = pOwnerRec->szFileName [i - 1];
               }

               // The sub-directory record may be selected or unselected.
               // Different colors are used for each possibility.
               // In the case of the record being selected, the background must also be painted
               // a different colour to match the highlite bar.  This requires manipulating the
               // GpiSetBackMix function before and after painting.

               if ((pOwneritem->fsAttribute & CRA_SELECTED) == 1)
               {
// WM_DRAWITEM  is supposed to be called only once in the Details view of
// Container controls, but this is not the case.  If Multiple Selection is used
// (CCS_MULTIPLESEL) and multiple records are selected with a mouse swipe,
// after the first record has been drawn, the following records in the multiple selection
// call WM_DRAWITEM twice - first when the cursor is not on the record, and again
// after the cursor is on the record.  As far as I can tell, this is the only time this
// happens.
// Because of this, I started to save the record pointer and compare it in the routine below
// that paints the record when the cursor is on it.

//         if ((pOwneritem->fsAttribute & CRA_CURSORED) == 0)
//         {
//            rc = GpiSetBackMix (pOwneritem->hps, BM_OVERPAINT);
//            pcnrTemp = pOwnerRec;
//         }

// If the selected record now has the cursor on it, this part will test to see
// if the record has been painted before.  If it hasn't, it will be painted, but if
// it has, it will return with the message that it has been painted to prevent the
// container from overpainting.

//         else
//         {
//            if (pcnrTemp == pOwnerRec) return MRFROMSHORT (TRUE);
//            rc = GpiSetBackMix (pOwneritem->hps, BM_OVERPAINT);
//         }

// In the end, all of this was replaced with the single line below.  Even though those
// particular records are painted twice, since they are overpainted, it doesn't matter.
// If I wanted to use BM_XOR for instance, I would have to have this control since
// the record would turn a different color each time.

                  rc = GpiSetBackMix (pOwneritem->hps, BM_OVERPAINT);

                  written = WinDrawText (pOwneritem->hps, -1,
                                                           szbuffer,
                                                           &pOwneritem->rclItem, CLR_CYAN,
                                                           SYSCLR_HILITEBACKGROUND,
                                                           NULL);
                  rc = GpiSetBackMix (pOwneritem->hps, BM_LEAVEALONE);
               }
               else
               {
                  written = WinDrawText (pOwneritem->hps, -1,
                                                        szbuffer,
                                                        &pOwneritem->rclItem, CLR_BLUE,
                                                        0,
                                                        NULL);
               }
               // Return a TRUE tells the container that the item has been painted.

               return MRFROMSHORT (TRUE);
            }
            else

            // Returning a FALSE tells the container that it must paint this item itself.

            return MRFROMSHORT (FALSE);

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
         case WM_APPTERMINATENOTIFY:

            // So far this is used only with the UnZip routine.
            // All the other commands that use WinStartApp have the hwndfilescntr window
            // set as the window to be notified, and since there is no procedure for
            // that window, it is ignored.

            PCNRITEM pOrgRec;

            // Save the record name of the selected file for the returnto () function below.

            pOrgRec = (PCNRITEM) WinSendMsg (hwndfilescntr,
                               CM_QUERYRECORDEMPHASIS, MPFROMP (CMA_FIRST),
                                                      MPFROMSHORT (CRA_SELECTED));

            if (pOrgRec == NULL)
            {
               // This could be the result of a DragDrop record that wasn't selected
               // Test to see if a single file under the mouse is being used.
               // If so the selected files are ignored while this file is used,
               // Be sure to clear this variable now.

               if (DropDestOp.recundermouse) pOrgRec = (PCNRITEM) DropDestOp.pRec;
               DropDestOp.recundermouse = 0;
             }

            strcpy (tparms7.szSavename, pOrgRec->szFileName);

            // Now that the name is saved, deselect the file.

            WinSendMsg (hwndfilescntr, CM_SETRECORDEMPHASIS,
                   MPFROMLONG (pOrgRec), MPFROM2SHORT (FALSE, CRA_SELECTED));

            // WinStartApp can be told to call this procedure on completion of it's program.
            // When the unarchiver terminates, this dialog is notified. This allows the
            // filescontainer to refresh itself if the destination directory was the same as
            // the source directory (a zip was unzipped into the source directory).
            // (See the code for MID_UNZIP).

            // Update the Destination status bar info with new bytes free etc.

            fillStatusbar1 (DestDrive, (PSZ) szDestPath);

            // Update the root window titlebar byte count.
            // This (as well as reloading the tree display) is done because a new
            // directory may have been created on the same drive with the UNZIPTODIR
            // command.

            getFSInfo (CurrDrive);

            // Reload the root tree container with the source drive tree.

            if (CurrDrive == DestDrive)
            {
               // This section searches for the last open folder in the tree
               // so that it can be redisplayed as open if the tree display is
               // rebuilt.
               // Rebuilding the files container would not be necessary since it is kept
               // current, but it is already wired into the 'CM_EXPANDTREE'
               // routine.

               PCNRITEM pOpenRec;
               pOpenRec = SearchForOpen ();

               if (pOpenRec)
               {
                  PSZ pszTestPath;
                  pszTestPath = &szFilesTitle [2];
                  SHORT mismatch;
                  mismatch = (SHORT) strcmp (pszTestPath, pOpenRec->szDirPath);

                  // This will disqualify the next checkpoint if the source path isn't the same
                  // as the destination.

                  if (mismatch != 0) pOpenRec = 0;
               }

               // If there aren't any open folders, it is the root directory and must
               // be totally rebuilt since I haven't determined if the directory being
               // unzipped to is in the root or not.  (The only thing that is determined
               // is if there is a matching open folder.)
               // Otherwise, collapse the folder and then refill it so that it's current.

               if (pOpenRec)
               {
                  WinSendMsg (hwnddircntr, CM_COLLAPSETREE, MPFROMP (pOpenRec), NULL);

                  // Fill the directory container here without using a thread since the
                  // expansion of the folder can't take place until the container is filled.
                  // Going thru the expand procedure in either case will close up the peer
                  // folders and repaint the container.

                  DTree->TreetoOrder (pOpenRec);
                  DTree->FillContainer();

                  WinSendMsg (hwnddircntr, CM_EXPANDTREE, MPFROMP (pOpenRec), NULL);
               }
               else
               {
                  // This routine reloads the Tree window and also reopens any open folders
                  // and places the cursor on them.

                  RefillTree (CurrDrive, USEPATH, hwnddircntr, DTree, NULL);
               }
               // Compare the source and the destination.  If they are the same, the
               // files container will have to be updated to show the new files.

               success = CompareandRefresh ();

               if (success)
               {
                  // Get the cursor back on the original file.

                  tparms7.Hab = NULL;
                  tparms7.Hmq = NULL;
                  tparms7.hwnd = hwndfilescntr;
                  tparms7.job = RETURNTO;
                  tid7 = _beginthread (threadall, NULL, 8192, &tparms7);
               }
            }
            WinSetFocus (HWND_DESKTOP, hwndfilescntr);

            // Restore the arrow pointer.
            WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP,
                                                                              SPTR_ARROW, FALSE));
            // If this is bit is set, the origin of this process was another instance of FF.
            // This means that the caller's object window handle is stored and a message
            // must be sent to it telling it to refresh the displays due to a change in contents
            // and byte counts.

            if ((DropDestOp.Operation & 64) == 64)
            {
               WinPostMsg (DropDestOp.hwndCaller, OBJM_REFRESH, NULL, NULL);
            }

            break;
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
         case WM_CHAR:

            hwndfocus = WinQueryFocus (HWND_DESKTOP);
            test = SHORT2FROMMP (mp2);
            cValue = SHORT1FROMMP (mp1);

            if (cValue & KC_VIRTUALKEY)
            {
               // Throw away all up-stroke keys.

               if  (cValue & KC_KEYUP) break;

               switch (test)
               {
                  case VK_BACKTAB:
                  case VK_TAB:

                     // Change the focus of the two main windows according to the action
                     // of the tab key.  This will not change the colour of the titlebars
                     // however so that job is done when the WM_FOCUSCHANGE message
                     // is intercepted below.

                     if (hwndfocus == hwnddircntr)
                     {
                        WinSetFocus (HWND_DESKTOP, hwndfilescntr);
                     }
                     else if (hwndfocus == hwndfilescntr)
                     {
                        WinSetFocus (HWND_DESKTOP, hwnddircntr);
                     }

                     break;
               }
               break;
            }
            else if (hwndfocus == hwndfilescntr)
            {
               // This section overcomes some of the Files Containers built-in features.
               // The first one allows the user to press a key and the cursor moves to the
               // first file that begins with that char and marks the file.  I found this really
               // inconvenient insofaras files continually have to be unmarked if they are not
               // the desired file (which is likely on a first letter search). The last routine
               // unmarks the file.  If the user WANTS to mark the files this way, it can still
               // be done by holding down the shift key or by just holding down the key.
               // The second feature overcome is the containers way of marking/unmarking files
               // with the grey minus key.  It does this a file ahead of the cursor and is not
               // intuitive.  I have added a routine for trapping the grey plus key for
               // marking/unmarking which works as one would expect, on the key below the file.

               // First, mask out the shift, alt and control keys and then locate the file that
               // has the cursor on it.  (The virtual keys are trapped above)

               if (! (SHORT1FROMMP (mp1) & (KC_SHIFT | KC_ALT | KC_CTRL)) )
               {
                  whatRec = CMA_FIRST;
                  pWhatRec = (PCNRITEM) TRUE;
                  while (pWhatRec)
                  {
                     pWhatRec = (PCNRITEM) WinSendMsg (hwndfilescntr, CM_QUERYRECORD,
                                                                MPFROMP (pWhatRec ),
                                                MPFROM2SHORT (whatRec, CMA_ITEMORDER));
                     if (pWhatRec->rc.flRecordAttr & CRA_CURSORED)
                     break;
                     whatRec = CMA_NEXT;
                  }

                  // Trap the grey plus key on the downstroke.

                  if (SHORT1FROMMP (mp2) == 43)
                  {
                     if (pWhatRec->rc.flRecordAttr & CRA_SELECTED)
                        WinSendMsg (hwndfilescntr, CM_SETRECORDEMPHASIS,
                              MPFROMLONG (pWhatRec), MPFROM2SHORT (FALSE, CRA_SELECTED));
                     else
                        WinSendMsg (hwndfilescntr, CM_SETRECORDEMPHASIS,
                              MPFROMLONG (pWhatRec), MPFROM2SHORT (TRUE, CRA_SELECTED));

                     WinSendMsg (hwndfilescntr, WM_CHAR, MPFROMSH2CH
                                       (KC_VIRTUALKEY, 0, 0), MPFROM2SHORT (0, VK_DOWN));
                  }

                  // If it is not the grey plus key on the upstroke, or the grey minus key
                  // on the up or down stroke, it must be a search key since all of the virtual
                  // keys, shift keys, alt keys, and control keys have been masked out.

                  else if (SHORT1FROMMP (mp2) != 20011)
                  {
                     if (SHORT1FROMMP (mp2) != 18989)
                     {
                        if (SHORT1FROMMP (mp2) != 45)
                           WinSendMsg (hwndfilescntr, CM_SETRECORDEMPHASIS,
                              MPFROMLONG (pWhatRec), MPFROM2SHORT (FALSE, CRA_SELECTED));
                     }
                  }
               }
            }
            break;
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
         case WM_COMMAND:

            // Check on the state of the mutex semaphore here so that it can be used as
            // flag to disallow any other command below.
            // The number of processes requesting the mutex are returned in ulMutexState.
            // If the mutex is currently unowned, the value is zero.

            PID pid;
            TID tid;
            DosQueryMutexSem (hmtx, &pid, &tid, &ulMutexState);

            // This first filter will separate the pushbuttons for drives (401 to 426)
            // and dispatch the drive change orders.

            SHORT tempmsg;
            tempmsg = SHORT1FROMMP (mp1);
            if ((tempmsg > 400) && (tempmsg < 427))
            {
               // Test for DoCopy () thread mutex

               if (ulMutexState)
               {
                  NoCanDo ();
                  break;
               }
               // Tempmsg is reduced to hold the drive designation.
               // Create a new directory tree and fill up the directory container.

               // The drive designation is derived from the label since there may be
               // gaps in the order.  The labels are mapped to the actual drive array.

               tempmsg -=400;
               tempmsg--;
               tempmsg = SHORT (pLabelarray [tempmsg * 4]);

               // Is it the Directory History button?
               // If so, load the History dialog.

               if (tempmsg == 42)
                  WinDlgBox (HWND_DESKTOP, hwnd, DirBookmarkDlgProc, NULLHANDLE,
                                                                                  DIRHISTORYID, NULL);
               // Otherwise, change drives as indicated.
               else
               {
                  tempmsg -=64;

                  // Test if the drive is ready (necessary because of removable media drives)

                  rc = IsDriveReady (tempmsg);
                  if (rc)
                  {
                     ChangeDrive (tempmsg);
                  }
               }
               break;
            }

             // The regular switches will follow from here

            switch (tempmsg)
            {
               case PBone:

                  // Toggle the orientation of the toolbar by switching rows for colums.
                  // Toggle the orientation storage value.

                  toolbarorient ^= 1;

                  SHORT tempcount;

                  tempcount=buttonrows;
                  buttonrows=buttoncolumns;
                  buttoncolumns=tempcount;

                  tempcount = driverows;
                  driverows = drivecolumns;
                  drivecolumns = tempcount;

                  OrientToolbar ();
                  break;

               case PBtwo:
               case MID_FILEDELETE:

                  // Test for DoCopy () thread mutex

                  if (ulMutexState)
                  {
                     NoCanDo ();
                     break;
                  }
                  // Call up the file delete dialog

                  WinDlgBox (HWND_DESKTOP, hwnd, DeleteDlgProc, NULLHANDLE, FILEDELETEID,
                                                                                          NULL);
                  // Update the status bar with no files now selected.

                  fillStatusbar2 (NULL, NULL);
                  break;

               case PBthree:
               case MID_FILECOPY:

                  // Test for DoCopy () thread mutex

                  if (ulMutexState)
                  {
                     NoCanDo ();
                     break;
                  }

                  // Call up the Copy File dialog

                  WinDlgBox (HWND_DESKTOP, hwnd, CopyDlgProc, NULLHANDLE, FILECOPYID,
                                                                                          NULL);

                  break;

               case PBfour:
               case MID_FILEMOVE:

                  // Test for DoCopy () thread mutex

                  if (ulMutexState)
                  {
                     NoCanDo ();
                     break;
                  }

                  // Call up the Move File dialog

                  WinDlgBox (HWND_DESKTOP, hwnd, MoveDlgProc, NULLHANDLE, MOVEFILESID,
                                                                                          NULL);
                  fillStatusbar2 (NULL, NULL);
                  break;

               case PBfive:
               case MID_FILERENAME:

                  // Test for DoCopy () thread mutex

                  if (ulMutexState)
                  {
                     NoCanDo ();
                     break;
                  }

                  // Call up the file delete dialog

                  WinDlgBox (HWND_DESKTOP, hwnd, RenameDlgProc, NULLHANDLE, RENAMEITEMID,
                                                                                          NULL);
                  break;

               case PBsix:
               case MID_FILEATTR:

                  // Test for DoCopy () thread mutex

                  if (ulMutexState)
                  {
                     NoCanDo ();
                     break;
                  }

                  // Call up the file attribute dialog

                  WinDlgBox (HWND_DESKTOP, hwnd, AttribDlgProc, NULLHANDLE, ATTRIBID,
                                                                                          NULL);
                  break;
               case PBseven:
               case MID_MARKALL:

                  // Test for DoCopy () thread mutex

                  if (ulMutexState)
                  {
                     NoCanDo ();
                     break;
                  }

                  // Select all the files in the files container.

                  cFiles = 0;
                  cbFiles = 0;

                  pcnrTemp = (PCNRITEM) WinSendMsg (hwndfilescntr, CM_QUERYRECORD,
                                                                   MPFROMP (pcnrTemp),
                                                   MPFROM2SHORT (CMA_FIRST, CMA_ITEMORDER ));
                  while (pcnrTemp)
                  {
                     // Don't select any subdirectory entrys.

                     if  (!(pcnrTemp->attrFile & 16))
                     {
                        pcnrTemp->rc.flRecordAttr |= CRA_SELECTED;
                        cFiles++;
                        cbFiles += pcnrTemp->cbFile;
                     }

                     pcnrTemp = (PCNRITEM) WinSendMsg (hwndfilescntr, CM_QUERYRECORD,
                                                                   MPFROMP (pcnrTemp),
                                                   MPFROM2SHORT (CMA_NEXT, CMA_ITEMORDER));
                  }

                  // Re-display the files with proper emphasis.

                  WinSendMsg (hwndfilescntr, CM_INVALIDATERECORD, NULL,
                                                         MPFROM2SHORT (NULL,CMA_NOREPOSITION));
                  fillStatusbar2 (cFiles, cbFiles);

                  break;

               case PBeight:
               case MID_UNMARKALL:

                  // Test for DoCopy () thread mutex

                  if (ulMutexState)
                  {
                     NoCanDo ();
                     break;
                  }

                  // Unselect all the records in the files container.

                  pcnrTemp = (PCNRITEM) WinSendMsg (hwndfilescntr, CM_QUERYRECORD,
                                                                   MPFROMP (pcnrTemp),
                                                   MPFROM2SHORT (CMA_FIRST, CMA_ITEMORDER));
                  while (pcnrTemp)
                  {
                     pcnrTemp->rc.flRecordAttr &= 65534; // CRA_SELECTED = 1
                     pcnrTemp = (PCNRITEM) WinSendMsg (hwndfilescntr, CM_QUERYRECORD,
                                                                   MPFROMP (pcnrTemp ),
                                                   MPFROM2SHORT (CMA_NEXT, CMA_ITEMORDER));

                  }
                  // Redisplay the records with no emphasis.

                  WinSendMsg (hwndfilescntr, CM_INVALIDATERECORD, NULL,
                                                         MPFROM2SHORT (NULL,CMA_NOREPOSITION));
                  fillStatusbar2 (NULL, NULL);

                  break;
               case PBnine:
               case MID_ZIP:

                  // Test for DoCopy () thread mutex

                  if (ulMutexState)
                  {
                     NoCanDo ();
                     break;
                  }

                  // Test for source drive ready (media inserted etc.).

                  rc = IsDriveReady (CurrDrive);
                  if (!rc) break;

                  // Test to see if the destination drive is ready (removable media drives).

                  rc = IsDriveReady (DestDrive);
                  if (!rc) break;

                  // Call up the zip name input dialog which also takes care of the rest of
                  // the work for zipping.

                  WinDlgBox (HWND_DESKTOP, hwnd, ZipnameDlgProc, NULLHANDLE,
                                                                              INPUTZIPNAMEID, NULL);
                  break;

               case PBten:
               case MID_UNZIP:
               case MID_ZIPTEST:
               case MID_UNZIPTODIR:

                  // Test for DoCopy () thread mutex

                  if (ulMutexState)
                  {
                     NoCanDo ();
                     break;
                  }
                  // Test for source drive ready (media inserted etc.).

                  rc = IsDriveReady (CurrDrive);
                  if (!rc) break;

                  // Test to see if the destination drive is ready (removable media drives).

                  rc = IsDriveReady (DestDrive);
                  if (!rc) break;

                  DoUnzip (tempmsg, hwnd);

                  break;

               case PBeleven:
               case MID_DESTINATION:

                  // Test for DoCopy () thread mutex

                  if (ulMutexState)
                  {
                     NoCanDo ();
                     break;
                  }

                  // Save the filename with the cursor on it so that it can be returned to later.
                  // It is saved as the szSwapbookmark string.

                  pWhatRec = (PCNRITEM) WinSendMsg (hwndfilescntr,
                                           CM_QUERYRECORDEMPHASIS, MPFROMP (CMA_FIRST),
                                                                  MPFROMSHORT (CRA_CURSORED));

                  // Protection is needed here in case the drive (or Subdir) has disappeared.
                  // In this case, no record will be found.

                  if (pWhatRec)
                  {
                     strcpy (szSwapbookmark, pWhatRec->szFileName);
                  }

                  // Copy a new path into the destination status bar spec

                  fillStatusbar1 (CurrDrive, (PSZ) szFilesTitle);

                  break;

               case PBtwelve:
               case MID_SWAPSandD:

                  // Test for DoCopy () thread mutex

                  if (ulMutexState)
                  {
                     NoCanDo ();
                     break;
                  }
                  // Test for destination drive ready (media inserted etc.).

                  rc = IsDriveReady (DestDrive);
                  if (!rc) break;

                  // Test for source drive ready (media inserted etc.).

                  rc = IsDriveReady (CurrDrive);
                  if (!rc) break;

                  // Put up the hourglass pointer.

                  WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP,
                                                                                SPTR_WAIT, FALSE));
                  // Switch the source and destination paths.

                  // Save the filename with the cursor on it so that we can returnto () it below.
                  // It is saved as the szSwapbookmark string.
                  // Use the szSavename string as temporary storage for the current bookmark
                  // (it will be used right away).

                  pWhatRec = (PCNRITEM) WinSendMsg (hwndfilescntr,
                                           CM_QUERYRECORDEMPHASIS, MPFROMP (CMA_FIRST),
                                                                  MPFROMSHORT (CRA_CURSORED));
                  strcpy (tparms7.szSavename, szSwapbookmark);

                  // Again, protection is needed in case the subdir or drive has disappeared.

                  if (pWhatRec)
                  {
                     strcpy (szSwapbookmark, pWhatRec->szFileName);
                  }

                  // Save the destination path in a buffer for use later because it will get
                  // updated by getFSInfo () below.

                  // Use a new string here because other dependant routines may be using
                  // the szpathbuffer and sznamebuffer strings.

                  PSZ psztmpbuffer, psztmppath;
                  psztmpbuffer = new CHAR [CCHMAXPATH * 2];
                  strcpy (psztmpbuffer, szDestPath);
                  len = (SHORT) strlen (psztmpbuffer);

                  // Strip the drive designator from the path pointer.

                  if (len == 3) psztmppath = NULL;
                  else psztmppath = psztmpbuffer + 2;

                  // Hide the files window now.  It will be made visible in the
                  // the RETURNTO thread (below).  This eliminates the herky-jerky
                  // window refresh.

                  WinSetWindowPos (hwndfilescntr, HWND_TOP, 0,0,0,0, SWP_HIDE);

                  // Reload the root tree container with the destination drive tree.

                  tempd = (SHORT) DestDrive;

                  // This routine reloads the Tree window and also reopens any open folders
                  // and places the cursor on them.

                  RefillTree (tempd, NULL, hwnddircntr, DTree, NULL);

                  // Write the new destination string to the bottom statusbar.

                  tempd = (SHORT) DestDrive;
                  fillStatusbar1 (CurrDrive, (PSZ) szFilesTitle);

                  // Get the file system info and format a title for the container with
                  // the drive designation and the bytes free.

                  getFSInfo (tempd);

                  // Reload the source (files container) with the destination path.

                  DFiles->PathToOrder (psztmppath, tempd);
                  DFiles->FillContainer (filefilter);

                 // This statusbar information can't be gathered until the container is filled so it
                 // must follow it.

                  fillStatusbar2 (NULL, NULL);

                  // Update the title of the Files container with the new path

                  szFilesTitle [0] = CHAR (CurrDrive + 64);
                  szFilesTitle [1] = 58;
                  szFilesTitle [2] = 0;
                  if (!psztmppath)
                  {
                     szFilesTitle [2] = 92;
                     szFilesTitle [3] = 0;
                  }
                  else strcat (szFilesTitle, psztmppath);
                  SecondChild->newtitle ((PSZ) szFilesTitle);

                  // Return to the last file that had the cursor on it in what was the destination
                  // directory.

                  if (strlen (tparms7.szSavename))

                  tparms7.Hab = NULL;
                  tparms7.Hmq = NULL;
                  tparms7.hwnd = hwndfilescntr;
                  tparms7.job = RETURNTO;
                  tid7 = _beginthread (threadall, NULL, 8192, &tparms7);

                  // Restore the arrow pointer.

                  WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP,
                                                                              SPTR_ARROW, FALSE));
                  delete psztmpbuffer;
                  break;

               case PBthirteen:
               case MID_EDIT:

                  // Test for DoCopy () thread mutex

                  if (ulMutexState)
                  {
                     NoCanDo ();
                     break;
                  }

                  // A search will be made for the first selected record.  The search criteria for
                  // the first record is CMA_FIRST, but for the following records, the criteria is the
                  // record of the preceeding search.


                  pWhatRec = (PCNRITEM) CMA_FIRST;

                  pWhatRec = (PCNRITEM) WinSendMsg (hwndfilescntr,
                                           CM_QUERYRECORDEMPHASIS, MPFROMP (pWhatRec),
                                                                  MPFROMSHORT (CRA_SELECTED));

                  // Fill in the PROGDETAILS structure for the WinStartApp function.

                  pDetails = &Details;
                  pDetails->progt.progc     = PROG_DEFAULT;
                  pDetails->progt.fbVisible = SHE_VISIBLE;
                  pDetails->pszTitle        = "Editor";
                  pDetails->pszStartupDir   = NULL;
                  pDetails->pszExecutable   = (PSZ) szDefaultEditor;
                  pDetails->pszParameters = szpathbuffer;

                  // Test for application type and if it's a windows app, make it a seamless
                  // windowed presentation.

                  DosQueryAppType (pDetails->pszExecutable, &ulApptype);
                  if ((ulApptype == 4096) || (ulApptype == 1024))
                  {
                     pDetails->progt.progc = PROG_31_ENHSEAMLESSVDM;
                  }

                  // Test for a Dos app and if it is, make it a windowed app also.

                  if ((ulApptype == 32))
                  {
                     pDetails->progt.progc = PROG_WINDOWEDVDM;
                  }

                  // Copy the parameters to the parameter buffer.

                  strcpy (szpathbuffer, szDefaultEdParms);

                  // If a file has been selected, do the following:

                  if (pWhatRec)
                  {
                     // Get the path and filename of the file selected.  Enclose the path/filename
                     // in quotation marks.  This is required for filenames with embedded spaces.
                     // Do this only if the program is neither a Dos nor a Windows program.

                     sztemp [0] = 32;
                     if ((ulApptype & 5152) == 0) sztemp [1] = 34;
                     else sztemp [1] = 32;
                     sztemp [2] = CHAR (CurrDrive + 64);
                     sztemp [3] = 58;
                     sztemp [4] = 0;
                     strcat (szpathbuffer, sztemp);
                     strcat (szpathbuffer, pWhatRec->szDirPath);
                     if ((ulApptype & 5152) == 0)
                         strcat (szpathbuffer, "\"");  // Add closing quote mark

                     // When the work is done, deselect the file.

                     WinSendMsg (hwndfilescntr, CM_SETRECORDEMPHASIS,
                               MPFROMLONG (pWhatRec), MPFROM2SHORT (FALSE, CRA_SELECTED));
                  }

                  // Put up the hourglass pointer.

                  WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP,
                                                                                SPTR_WAIT, FALSE));

                  // Start the editor.
                  // SAF_INSTALLCMDLINE param must be used if Win-OS/2 programs are to
                  // be run.  (I don't know why this is)
                  // Don't use the SAF_STARTCHILDAPP modifier so that the editor will
                  // continue to run even if File Freedom is closded.

                  // Change the current dir to the home dir so that the desired editor can
                  // found if it is there.

                  DosSetDefaultDisk (Homedrive);
                  DosSetCurrentDir (szHomepath);

                  WinStartApp (hwndfilescntr, pDetails, NULL, NULL,
                                                    SAF_INSTALLEDCMDLINE);

                  // Change the drive back to where we were.

                  DosSetDefaultDisk (CurrDrive);

                  break;

               case PBfourteen:
               case MID_APPEND:

                  // Make sure the Drag/Drop record won't be triggered since the DoCopyOp dialog
                  // checks for it and the dialog is used both by this routine and the DragDrop
                  // routine.

                  DropDestOp.recundermouse = 0;

                  WinDlgBox (HWND_DESKTOP, hwnd, AppendDlgProc, NULLHANDLE, APPENDID, NULL);
                  break;

               case PBfifteen:
               case MID_SEEK:

                  // Test for DoCopy () thread mutex

                  if (ulMutexState)
                  {
                     NoCanDo ();
                     break;
                  }

                  // A link to PMSeek - this is probably installed on every system.

                  // A search will be made for the first selected record.  The search criteria for
                  // the first record is CMA_FIRST, but for the following records, the criteria is the
                  // record of the preceeding search.

                  pWhatRec = (PCNRITEM) CMA_FIRST;

                  pWhatRec = (PCNRITEM) WinSendMsg (hwndfilescntr,
                                           CM_QUERYRECORDEMPHASIS, MPFROMP (pWhatRec),
                                                                  MPFROMSHORT (CRA_SELECTED));
                  szpathbuffer [0] = 0;

                  // Fill in the PROGDETAILS structure for the WinStartApp function.

                  pDetails = &Details;
                  pDetails->progt.progc     = PROG_DEFAULT;
                  pDetails->progt.fbVisible = SHE_VISIBLE;
                  pDetails->pszTitle        = "SEARCH Tool";
                  pDetails->pszStartupDir   = NULL;
                  pDetails->pszExecutable   = (PSZ) szDefaultSearchTool;
                  pDetails->pszParameters = szpathbuffer;

                  // Test for application type and if it's a windows app, make it a seamless
                  // windowed presentation.

                  DosQueryAppType (pDetails->pszExecutable, &ulApptype);
                  if ((ulApptype == 4096) || (ulApptype == 1024))
                  {
                     pDetails->progt.progc = PROG_31_ENHSEAMLESSVDM;
                  }

                  // Test for a Dos app and if it is, make it a windowed app also.

                  if ((ulApptype == 32))
                  {
                     pDetails->progt.progc = PROG_WINDOWEDVDM;
                  }

                  // Copy the parameters to the parameter buffer.

                  strcpy (szpathbuffer, szDefaultSTParms);

                  // If a file has been selected, do the following:

                  if (pWhatRec)
                  {
                     // Only supply the filename of the selected file.
                     // This allows searching for duplicate filenames whereas a path attached
                     // would interfere.
                     // Enclose the path/filename in quotation marks.  This is required for
                     // filenames with embedded spaces.
                     // Do this only if the program is neither a Dos nor a Windows program.

                     sztemp [0] = 32;
                     if ((ulApptype & 5152) == 0) sztemp [1] = 34;
                     else sztemp [1] = 32;
                     sztemp [2] = 0;
                     strcpy (szpathbuffer, sztemp);
                     strcat (szpathbuffer, pWhatRec->szFileName);
                     if ((ulApptype & 5152) == 0)
                         strcat (szpathbuffer, "\"");  // Add closing quote mark

                     // When the work is done, deselect the file.

                     WinSendMsg (hwndfilescntr, CM_SETRECORDEMPHASIS,
                               MPFROMLONG (pWhatRec), MPFROM2SHORT (FALSE, CRA_SELECTED));
                  }

                  // Put up the hourglass pointer.

                  WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP,
                                                                                SPTR_WAIT, FALSE));

                  // Start the Search tool.
                  // SAF_INSTALLCMDLINE param must be used if Win-OS/2 programs are to
                  // be run.  (I don't know why this is)
                  // Don't use the SAF_STARTCHILDAPP modifier so that the editor will
                  // continue to run even if File Freedom is closded.

                  WinStartApp (hwndfilescntr, pDetails, NULL, NULL,
                                                    SAF_INSTALLEDCMDLINE);

                  // Change the drive back to where we were.

                  DosSetDefaultDisk (CurrDrive);
                  break;

               case PBsixteen:
               case MID_REFRESH:

                  // Test for DoCopy () thread mutex

                  if (ulMutexState)
                  {
                     NoCanDo ();
                     break;
                  }
               // Save the filename so that we can returnto () it below.

               pWhatRec = (PCNRITEM) WinSendMsg (hwndfilescntr,
                                     CM_QUERYRECORDEMPHASIS, MPFROMP (CMA_FIRST),
                                                           MPFROMSHORT (CRA_CURSORED));
               // Protection is needed here in case the drive or subdir has disappeared.
               // In this case no file will be found with the cursor on it or otherwise.

               if (pWhatRec)
                  strcpy (tparms7.szSavename, pWhatRec->szFileName);

                  // Refresh the source and destination containers, info, etc.
                  // Do not use threads here since it is uncertain when the user will
                  // preform a refresh and the usual designated threads could still be running.

                  // Put up the hourglass pointer.

                  WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP,
                                                                                SPTR_WAIT, FALSE));
                  // Copy the path into the destination status bar spec and update info.

                  fillStatusbar1 (DestDrive, (PSZ) szDestPath);

                  // Update the root window titlebar byte count.

                  getFSInfo (CurrDrive);

                  // Reload the root tree container with the source drive tree.

                  // This routine reloads the Tree window and also reopens any open folders
                  // and places the cursor on them.

                  RefillTree (CurrDrive, USEPATH, hwnddircntr, DTree, NULL);

                  // Refill the files container to the original path.

                  DFiles->EmptyContainer ();
                  DFiles->FillContainer (filefilter);
                  fillStatusbar2 (NULL, NULL);

                  // Use the returnto thread to move the cursor back on to the last file it was on.

                  tparms7.Hab = NULL;
                  tparms7.Hmq = NULL;
                  tparms7.hwnd = hwndfilescntr;
                  tparms7.job = RETURNTO;
                  tid7 = _beginthread (threadall, NULL, 8192, &tparms7);

                  // Also, refresh the Drives Button Bar and refresh the volume labels.

                  GetAvailableDrives (TRUE);
                  OrientToolbar ();

                  // Refresh all of the volume labels except the floppys.

                  RefreshVolLabels (3, FALSE);

                  // Restore the arrow pointer.

                  WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP,
                                                                              SPTR_ARROW, FALSE));
                  break;

               case PBseventeen:
               case MID_DIVIDERRIGHT:

                  // Move the divider to the right

                  if (SCwidth > 15)
                  {
                     SCwidth -= 15;
                     FCwidth += 15;
                  }
                  FirstChild->rejig ( FCwidth, FCheight);
                  SecondChild->rejig ( SCwidth, SCheight);
                  resize();

                  break;

               case PBeighteen:
               case MID_DIVIDERLEFT:

                  // Move the divider to the left

                  if (FCwidth > 15)
                  {
                     FCwidth -= 15;
                     SCwidth += 15;
                  }
                  FirstChild->rejig ( FCwidth, FCheight);
                  SecondChild->rejig ( SCwidth, SCheight);
                  resize();

                  break;

               case MID_PRINT:

                  // Test for DoCopy () thread mutex

                  if (ulMutexState)
                  {
                     NoCanDo ();
                     break;
                  }

                  // A search will be made for the first selected record.  The search criteria for
                  // the first record is CMA_FIRST, but for the following records, the criteria is the
                  // record of the preceeding search.
                  // This file will be simply 'copied' to 'prn' so it won't work for other than
                  // ascii files.

                  pWhatRec = (PCNRITEM) CMA_FIRST;

                     pWhatRec = (PCNRITEM) WinSendMsg (hwndfilescntr,
                                           CM_QUERYRECORDEMPHASIS, MPFROMP (pWhatRec),
                                                                  MPFROMSHORT (CRA_SELECTED));
                  if (pWhatRec)
                  {
                     // Copy the system command and file name to a buffer.
                     // The '/C' is a parameter for DosStartSession that tells it to
                     // exit the session when the program is finished.  It is used when
                     // a secondary command processor has been loaded as here.

                     strcpy (szpathbuffer, "/C copy ");

                     // Get the path and filename of the file selected and attach "prn" to the string.

                     sztemp [0] = CHAR (CurrDrive + 64);
                     sztemp [1] = 58;
                     sztemp [2] = 0;
                     strcat (szpathbuffer, sztemp);
                     strcat (szpathbuffer, pWhatRec->szDirPath);
                     strcat (szpathbuffer, " prn");

                     // Send the command string to DosStartSession.


                     //  Specify the various session start parameters yet to be filled in //

                     StartData.Related = 0;   // Not a Child session //
                     StartData.FgBg = SSF_FGBG_BACK; // Start session in background //
                     StartData.PgmName = (PSZ) pszCmdProcessor;  // Program path-name string //
                     StartData.PgmInputs = szpathbuffer; // input arguments passed to the program//
                     StartData.SessionType = SSF_TYPE_DEFAULT; // Allow the Shell to establish //
                                                                        //   the session type           //
                     StartData.PgmControl = SSF_CONTROL_INVISIBLE; // Start as invisible //

                     // Start this mini session.  If there is an error, beep.

                     rc = DosStartSession (&StartData, &SessID, &pid);
                     if (rc != 0) DosBeep (1000,100);

                     // Deselect the file.

                     WinSendMsg (hwndfilescntr, CM_SETRECORDEMPHASIS,
                              MPFROMLONG (pWhatRec), MPFROM2SHORT (FALSE, CRA_SELECTED));
                  }
                  break;

               case MID_ZIPVIEWER:

                  // Test for DoCopy () thread mutex

                  if (ulMutexState)
                  {
                     NoCanDo ();
                     break;
                  }

                  // This routine will force the zip viewer (either internal or user
                  // installed) to view a file that does not have a ".zip" extension.
                  // This is good for looking at self-extracting executables etc.

                  // A search will be made for the first selected record.  The search criteria for
                  // the first record is CMA_FIRST, but for the following records, the criteria is the
                  // record of the preceeding search.

                  pWhatRec = (PCNRITEM) CMA_FIRST;

                  pWhatRec = (PCNRITEM) WinSendMsg (hwndfilescntr,
                                           CM_QUERYRECORDEMPHASIS, MPFROMP (pWhatRec),
                                                                  MPFROMSHORT (CRA_SELECTED));
                  if (pWhatRec)
                  {
                     internalzipviewer = 0;
                     Namelength = (USHORT) strlen (pWhatRec->szFileName);
                     int x, xx, xxx;
                     xx = 0;
                     PSZ ptemp;

                     // Search for a 'zip' viewer in the list of installed viewers.
                     // If a match isn't found, the viewer will be set for using the built-in
                     // zip viewer. Otherwise, the user-installed viewer will be called.

                     xxx = ViewerTypeslen / 7;
                     for (x = 0; x < xxx; x++)
                     {
                        ptemp = &pszViewerTypes [x * 7];
                        xx = stricmp ("zip", ptemp);
                        if (!xx)
                        {
                           xxx = x;
                           xx = x;
                        }
                     }

                     // If there was a match, x will get incremented one more time as the loop
                     // finishes and so will not match xxx.

                     // If there was no match for the 'zip' extension means that the
                     // program will use the internal zip viewer for this file.

                     if (x == xxx) internalzipviewer = 1;

                     if (internalzipviewer)
                     {
                        sztemp [0] = CHAR (CurrDrive + 64);
                        sztemp [1] = 58;
                        sztemp [2] = 0;
                        strcpy (szpathbuffer, sztemp);
                        strcat (szpathbuffer, pWhatRec->szDirPath);

                        // Change the current dir to the home dir so that the Info-zip utilities can be
                        // found if they are there.

                        DosSetDefaultDisk (Homedrive);
                        DosSetCurrentDir (szHomepath);

                        // Do a fix-up here so that the passed path can't end in a backslash
                        // as it would with just a drive designator eg. "H:\".
                        // Unzip can't handle this.

                        int shortlen;
                        strcpy (sznamebuffer, szDestPath);
                        shortlen = strlen (sznamebuffer);
                        if (shortlen == 3)
                        {
                           if (sznamebuffer [2] == 92)
                              sznamebuffer [2] = 0;
                        }

                        Zipshell = new zipgui ("Zip Viewer", szpathbuffer, sznamebuffer,
                                                     szHomedrivepath, pszViewers, pszViewerTypes,
                                                  USHORT (ViewerTypeslen/7), szUnzipprog, szZipprog,
                                                                            hwndhelp, hwndframe, NULL);

                        // Change the drive back to where we were.

                        DosSetDefaultDisk (CurrDrive);

                        delete Zipshell;

                        // The files container will have to be updated to show any filesize changes
                        // may may have resulted from files being extracted or deleted from the zip.

                        DFiles->EmptyContainer ();

                        // Fill the files container and update the source status bar.

                        DFiles->FillContainer (filefilter);

                        // This statusbar information can't be gathered until the container is filled.

                        fillStatusbar2 (NULL, NULL);

                        // Update the root window titlebar byte count.

                        getFSInfo (CurrDrive);

                        // Update the Destination status bar info with new bytes free etc.

                        fillStatusbar1 (DestDrive, (PSZ) szDestPath);

                        WinSetFocus (HWND_DESKTOP, hwndfilescntr);
                     }
                     else
                     {
                        ptemp = &pszViewers [xx * VIEWERSTRLEN];

                        // Get the path and filename of the file selected.

                        sznamebuffer [0] = 34;
                        sznamebuffer [1] = CHAR (CurrDrive + 64);
                        sznamebuffer [2] = 58;
                        sznamebuffer [3] = 0;
                        strcat (sznamebuffer, pWhatRec->szDirPath);
                        strcat (sznamebuffer, "\"");

                        szpathbuffer [0] = 0;
                        pDetails->pszStartupDir   = NULL;
                        pDetails->pszExecutable = ptemp;

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
                                 //Make the cmd processor window invisible.

                                 pDetails->swpInitial.fl = SWP_HIDE;
                                 pDetails->pszExecutable = (PSZ) pszCmdProcessor;
                                 strcat (szpathbuffer, "/C ");
                                 strcat (szpathbuffer, ptemp);
                                 strcat (szpathbuffer, " ");
                              }
                           }
                        }
                        strcat (szpathbuffer, sznamebuffer);

                        // Fill in the rest of the PROGDETAILS structure for the
                        // WinStartApp function.

                        pDetails = &Details;
                        pDetails->progt.progc     = PROG_DEFAULT;
                        pDetails->progt.fbVisible = SHE_VISIBLE;
                        pDetails->pszTitle        = "Viewer";

                        // Put up the hourglass pointer.

                        WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP,
                                                                                SPTR_WAIT, FALSE));
                        // Change the current dir to the home dir so that the Info-zip utilities can be
                        // found if they are there.

                        DosSetDefaultDisk (Homedrive);
                        DosSetCurrentDir (szHomepath);

                        // Start the viewer.

                        WinStartApp( hwndfilescntr, pDetails, szpathbuffer,
                                                                NULL, SAF_STARTCHILDAPP);
                        // Change the drive back to where we were.

                        DosSetDefaultDisk (CurrDrive);

                        // Reset this parameter so next app will be visible.

                        pDetails->swpInitial.fl = SWP_ACTIVATE;
                     }

                     // When the show is over, deselect the file.

                     WinSendMsg (hwndfilescntr, CM_SETRECORDEMPHASIS,
                          MPFROMLONG (pWhatRec), MPFROM2SHORT (FALSE, CRA_SELECTED));

                     strcpy (tparms7.szSavename, pWhatRec->szFileName);

                     tparms7.Hab = NULL;
                     tparms7.Hmq = NULL;
                     tparms7.hwnd = hwndfilescntr;
                     tparms7.job = RETURNTO;
                     tid7 = _beginthread (threadall, NULL, 8192, &tparms7);
                  }
                  break;

               case MID_ABOUT:

                  WinDlgBox (HWND_DESKTOP, hwnd, AboutDlgProc, NULLHANDLE, ABOUTID, NULL);
                  break;

               case MID_CREATEDIR:

                  // Test for DoCopy () thread mutex

                  if (ulMutexState)
                  {
                     NoCanDo ();
                     break;
                  }

                  // Call up the Create Dir dialog

                  WinDlgBox (HWND_DESKTOP, hwnd, CreateDirDlgProc, NULLHANDLE, CREATEDIRID,
                                                                                          NULL);
                  break;

               case MID_TREESIZE:
                  // Test for DoCopy () thread mutex

                  if (ulMutexState)
                  {
                     NoCanDo ();
                     break;
                  }

                  // This routine will return the size of a directory hierarchy in a
                  // message box.

                  hwndfocus = WinQueryFocus (HWND_DESKTOP);

                  // A search will be made for the first selected record.

                  pWhatRec = (PCNRITEM) CMA_FIRST;

                  if (hwndfocus == hwnddircntr)
                  {
                     pWhatRec = (PCNRITEM) WinSendMsg (hwnddircntr,
                                           CM_QUERYRECORDEMPHASIS, MPFROMP (pWhatRec),
                                                                  MPFROMSHORT (CRA_SELECTED));
                  }
                  // If the directory tree window doesn't have the focus, default to the files
                  // window.  This is ok since no other window has records.
                  else
                  {
                     pWhatRec = (PCNRITEM) WinSendMsg (hwndfilescntr,
                                           CM_QUERYRECORDEMPHASIS, MPFROMP (pWhatRec),
                                                                  MPFROMSHORT (CRA_SELECTED));
                  }
                  if (pWhatRec)
                  {
                     // Filter out any record that isn't a directory.

                     if (pWhatRec->attrFile  & 16)
                     {
                        CountBytes (pWhatRec->szDirPath);

                        // Now that the size has been given, deselect the file.

                        WinSendMsg (hwndfilescntr, CM_SETRECORDEMPHASIS,
                               MPFROMLONG (pWhatRec), MPFROM2SHORT (FALSE, CRA_SELECTED));
                     }
                  }
                  break;

               case MID_BOOKMARKS:

                  // Load the bookmarks window.

                  WinDlgBox (HWND_DESKTOP, hwnd, DirBookmarkDlgProc, NULLHANDLE,
                                                                                  DIRHISTORYID, NULL);
                  break;

               case MID_CREATEOBJ:

                  // Test for DoCopy () thread mutex

                  if (ulMutexState)
                  {
                     NoCanDo ();
                     break;
                  }

                  // A routine to create a Desktop Object from the highlighted file

                  // Find the record in the files container that has been selected.

                  pcnrTemp = (PCNRITEM) WinSendMsg (hwndfilescntr,
                                        CM_QUERYRECORDEMPHASIS, MPFROMP (CMA_FIRST),
                                                               MPFROMSHORT (CRA_SELECTED));
                  // Check for a valid record.

                  if (pcnrTemp)
                  {
                     // If it is a subdirectory, exit.

                     if (pcnrTemp->attrFile & 16) break;

                     szpathbuffer [0] = CHAR (CurrDrive + 64);
                     szpathbuffer [1] = 58;
                     szpathbuffer [2] = 0;
                     strcat (szpathbuffer, pcnrTemp->szDirPath);

                     // Don't use "PROGTYPE=PM;" in the set-up string so that the default
                     // type of program will be configured.

                     CHAR szSetupString [300];
                     strcpy (szSetupString, "EXENAME=");
                     strcat (szSetupString, szpathbuffer);
                     strcat (szSetupString, ";STARTUPDIR=");
                     strcat (szSetupString, szFilesTitle);
                     strcat (szSetupString, ";");
                     HOBJECT hObject;
                     hObject = WinCreateObject ("WPProgram", pcnrTemp->szFileName, szSetupString,
                                      "<WP_DESKTOP>", CO_FAILIFEXISTS);
                     if (!hObject)
                     {
                        WinMessageBox (HWND_DESKTOP, hwndclient, "Could Not Create Desktop"
                              " Object.", "Error", MESSAGEBOXID, MB_OK | MB_ICONEXCLAMATION);
                     }
                  }
                  break;

               case MID_DRIVENEXT:

                  // Test for DoCopy () thread mutex

                  if (ulMutexState)
                  {
                     NoCanDo ();
                     break;
                  }

                  // Rotate ahead to the next drive in a circular queue.
                  // Note that the drive CurrDrive is '1' based while the drivearray is 'zero'
                  // based.  This works ok in the loop since it must start one past the
                  // current drive.  Also note that the drive designation finally is derived
                  // from the drive label since it is mapped to the actual drive array.
                  // If there are gaps in the order (eg. CDROM mapped to a particular letter)
                  // this will maintain the correct mapping.

                  ii = 0;
                  for (i = CurrDrive; i < 26; i++)
                  {
                     if (drivearray [i])
                     {
                        ii = i;
                        i = 26;
                     }
                     if ((!ii) && (i == 25)) i = -1;
                  }

                  // Derive actual drive designation from label. The labels are four bytes
                  // long and the first char in the label is the drive letter.  These were
                  // previously mapped to the actual drive array.  Subtract 64 to get the drive.
                  // eg. drive 'A' is 65-64= 1.  Drives start at 1 and go to possibly 26.
                  // That is to say that the drive referenced by drivearray [0] will have a
                  // label of "A:" and will be drive 1.

                  ii = pLabelarray [ii * 4] - 64;
                  ChangeDrive ((SHORT) ii);

                  break;

               case MID_DRIVEPREV:

                  // Test for DoCopy () thread mutex

                  if (ulMutexState)
                  {
                     NoCanDo ();
                     break;
                  }

                  // Rotate back to the previous drive in a circular queue.  See above.

                  ii = 0;
                  tempd = SHORT (CurrDrive - 2);
                  if (tempd < 0) tempd = 25;
                  for (i = tempd; i > -1; i--)
                  {
                     if (drivearray [i])
                     {
                        ii = i;
                        i = -1;
                     }
                     if ((!ii) && (i == 0)) i = 26;
                  }

                  // Derive actual drive designation from label.  See above.

                  ii = pLabelarray [ii * 4] - 64;

                  ChangeDrive ((SHORT) ii);
                  break;

               case MID_REFRESHLABELS:

                  // Refresh all of the volume labels including the floppies.

                  RefreshVolLabels (1, FALSE);

                  // Test to see if a Volume label is currently being displayed.
                  // If it is, it must be refreshed with the new information.
                  // If a label is being displayed, the drivebarflag will still be non-zero.
                  // The drive can be derived from the flag since the flag is a bit indicator
                  // for the drive.  By shifting the bit right, it can be determined what bit
                  // was turned on.  The number of positions it had to be shifted indicates
                  // the drive number.

                  if (drivebarflag)
                  {
                     ULONG proxy;
                     proxy = drivebarflag;

                     for (i = 0; i < 26; i++)
                     {
                        proxy >>= 1;
                        if (!proxy)
                        {
                           ii = i - 1;
                           i = 26;
                        }
                     }
                        Statusbar->newtitle2 ((CHAR*)&ucVoldat [ii][5]);
                  }
                  break;

               case MID_VOLUMELABEL:

                  WinDlgBox (HWND_DESKTOP, hwnd, VolumeLabelDlgProc, NULLHANDLE,
                                                                VOLUMELABELID, NULL);
                  break;

               case MID_FILEDETAIL:

                  // Test for DoCopy () thread mutex

                  if (ulMutexState)
                  {
                     NoCanDo ();
                     break;
                  }
                  // Save the filename so that we can returnto () it below.

                  pWhatRec = (PCNRITEM) WinSendMsg (hwndfilescntr,
                                           CM_QUERYRECORDEMPHASIS, MPFROMP (CMA_FIRST),
                                                                  MPFROMSHORT (CRA_CURSORED));
                  strcpy (tparms7.szSavename, pWhatRec->szFileName);

                  // A method to set the details view of the files container.  It is also
                  // responsible for enabling or disabling the 'Column Headings' menu item
                  // which of course can't be enabled if the details view is disabled.
                  // If the details view is disabled, the 'text view' is enabled which
                  // serves as the default view.

                  DFiles->SetDetailView ();
                  basicview = DETAILVIEW;

                  SetView (DETAILVIEW, hwndmenu);

                  tparms7.Hab = NULL;
                  tparms7.Hmq = NULL;
                  tparms7.hwnd = hwndfilescntr;
                  tparms7.job = RETURNTO;
                  tid7 = _beginthread (threadall, NULL, 8192, &tparms7);

                  break;

               case MID_FILEHEADINGS:

                  // Test for DoCopy () thread mutex

                  if (ulMutexState)
                  {
                     NoCanDo ();
                     break;
                  }
                  // Save the filename so that we can returnto () it below.

                  pWhatRec = (PCNRITEM) WinSendMsg (hwndfilescntr,
                                           CM_QUERYRECORDEMPHASIS, MPFROMP (CMA_FIRST),
                                                                  MPFROMSHORT (CRA_CURSORED));
                  strcpy (tparms7.szSavename, pWhatRec->szFileName);

                  // The Column Headings are enabled or disabled.  The detail view must be
                  // enabled before the Column Headings view is enabled.  Therefore, clicking
                  // on this feature will only have effect if it is enabled.

                  test = (USHORT) WinSendMsg (hwndmenu, MM_QUERYITEMATTR, MPFROM2SHORT
                                       (MID_FILEHEADINGS, TRUE), MPFROMSHORT (MIA_CHECKED));
                  if (!test)
                  {
                     DFiles->SetColumnHeadings ();
                     basicview = COLUMNHEADINGS;
                     WinSendMsg (hwndmenu, MM_SETITEMATTR, MPFROM2SHORT
                                                      (MID_FILEHEADINGS, TRUE), MPFROM2SHORT
                                                                   (MIA_CHECKED, MIA_CHECKED));
                  }
                  else
                  {
                     DFiles->SetDetailView ();
                     basicview = DETAILVIEW;
                     WinSendMsg (hwndmenu, MM_SETITEMATTR, MPFROM2SHORT
                                                      (MID_FILEHEADINGS, TRUE), MPFROM2SHORT
                                                                   (MIA_CHECKED, NULL));
                  }
                  tparms7.Hab = NULL;
                  tparms7.Hmq = NULL;
                  tparms7.hwnd = hwndfilescntr;
                  tparms7.job = RETURNTO;
                  tid7 = _beginthread (threadall, NULL, 8192, &tparms7);

                  break;

               case MID_FILEFLOWED:

                  // Test for DoCopy () thread mutex

                  if (ulMutexState)
                  {
                     NoCanDo ();
                     break;
                  }
                  // Save the filename so that we can returnto () it below.

                  pWhatRec = (PCNRITEM) WinSendMsg (hwndfilescntr,
                                           CM_QUERYRECORDEMPHASIS, MPFROMP (CMA_FIRST),
                                                                  MPFROMSHORT (CRA_CURSORED));
                  strcpy (tparms7.szSavename, pWhatRec->szFileName);

                  // The Text Flowed view is enabled.

                  DFiles->SetFlowView ();
                  basicview = FLOWVIEW;

                  SetView (FLOWVIEW, hwndmenu);

                  tparms7.Hab = NULL;
                  tparms7.Hmq = NULL;
                  tparms7.hwnd = hwndfilescntr;
                  tparms7.job = RETURNTO;
                  tid7 = _beginthread (threadall, NULL, 8192, &tparms7);

                  break;

               case MID_FILEICON:

                  // Test for DoCopy () thread mutex

                  if (ulMutexState)
                  {
                     NoCanDo ();
                     break;
                  }
                  // Get the file name that currently has the cursor on it and save it for
                  // the returnto () function.

                  pWhatRec = (PCNRITEM) WinSendMsg (hwndfilescntr,
                                           CM_QUERYRECORDEMPHASIS, MPFROMP (CMA_FIRST),
                                                                  MPFROMSHORT (CRA_CURSORED));
                  strcpy (tparms7.szSavename, pWhatRec->szFileName);

                  // The Name Flowed view is enabled.
                  // The Files container must also be reloaded if this view is being enabled.
                  // The records are stored with icon pointers only if the 'CV_NAME' attribute
                  // is set.  This is to speed up loading if the icons are not required.

                  DFiles->SetNameView ();
                  basicview = ICONVIEW;

                  SetView (ICONVIEW, hwndmenu);

                  // Now that the 'CV_NAME' attribute has been set, refill the files container.

                  DFiles->EmptyContainer ();

                  // Fill the files container and update the source status bar.

                  DFiles->FillContainer (filefilter);

                  fillStatusbar2 (NULL, NULL);

                  tparms7.Hab = NULL;
                  tparms7.Hmq = NULL;
                  tparms7.hwnd = hwndfilescntr;
                  tparms7.job = RETURNTO;
                  tid7 = _beginthread (threadall, NULL, 8192, &tparms7);

                  break;

               case MID_DEFAULTVIEW:

                  // Test for DoCopy () thread mutex

                  if (ulMutexState)
                  {
                     NoCanDo ();
                     break;
                  }
                  // Save the filename so that we can returnto () it below.

                  pWhatRec = (PCNRITEM) WinSendMsg (hwndfilescntr,
                                           CM_QUERYRECORDEMPHASIS, MPFROMP (CMA_FIRST),
                                                                  MPFROMSHORT (CRA_CURSORED));
                  strcpy (tparms7.szSavename, pWhatRec->szFileName);

                  // The Text view is enabled.

                  DFiles->SetTextView ();
                  basicview = TEXTVIEW;

                  SetView (TEXTVIEW, hwndmenu);

                  tparms7.Hab = NULL;
                  tparms7.Hmq = NULL;
                  tparms7.hwnd = hwndfilescntr;
                  tparms7.job = RETURNTO;
                  tid7 = _beginthread (threadall, NULL, 8192, &tparms7);

                  break;

            case MID_SETTINGS:

               // Call up the settings notebook.

               WinDlgBox (HWND_DESKTOP, hwnd, SettingsDlgProc, NULLHANDLE,
                                                                           SETTINGSID, NULL);
               break;

               case MID_SHOWHIDDEN:

                  // Test for DoCopy () thread mutex

                  if (ulMutexState)
                  {
                     NoCanDo ();
                     break;
                  }

                  // Show Hidden Files is enabled or disabled.
                  // This will toggle the feature and refill the files container to show
                  // the files as requested.

                  test = (USHORT) WinSendMsg (hwndmenu, MM_QUERYITEMATTR, MPFROM2SHORT
                                       (MID_SHOWHIDDEN, TRUE), MPFROMSHORT (MIA_CHECKED));
                  if (!test)
                  {
                     filefilter |= FILE_HIDDEN;

                     // Now that the filter attributes have been set, refill the files container
                     // rewrite the statusbar information.

                     DFiles->EmptyContainer ();
                     DFiles->FillContainer (filefilter);
                     fillStatusbar2 (NULL, NULL);

                     WinSendMsg (hwndmenu, MM_SETITEMATTR, MPFROM2SHORT
                                                      (MID_SHOWHIDDEN, TRUE), MPFROM2SHORT
                                                                   (MIA_CHECKED, MIA_CHECKED));
                  }
                  else
                  {
                     filefilter &= FILE_ARCHIVED | FILE_DIRECTORY | FILE_SYSTEM
                                                                                   | FILE_READONLY;

                     // Now that the filter attributes have been set, refill the files container.

                     DFiles->EmptyContainer ();
                     DFiles->FillContainer (filefilter);
                     fillStatusbar2 (NULL, NULL);

                     WinSendMsg (hwndmenu, MM_SETITEMATTR, MPFROM2SHORT
                                                      (MID_SHOWHIDDEN, TRUE), MPFROM2SHORT
                                                                   (MIA_CHECKED, NULL));
                  }
                  break;

               case MID_SHOWSYSTEM:

                  // Test for DoCopy () thread mutex

                  if (ulMutexState)
                  {
                     NoCanDo ();
                     break;
                  }

                  // Show Hidden Files is enabled or disabled.
                  // This will toggle the feature and refill the files container to show
                  // the files as requested.

                  test = (USHORT) WinSendMsg (hwndmenu, MM_QUERYITEMATTR, MPFROM2SHORT
                                       (MID_SHOWSYSTEM, TRUE), MPFROMSHORT (MIA_CHECKED));
                  if (!test)
                  {
                     filefilter |= FILE_SYSTEM;

                     // Now that the filter attributes have been set, refill the files container
                     // rewrite the statusbar information.

                     DFiles->EmptyContainer ();
                     DFiles->FillContainer (filefilter);
                     fillStatusbar2 (NULL, NULL);

                     WinSendMsg (hwndmenu, MM_SETITEMATTR, MPFROM2SHORT
                                                      (MID_SHOWSYSTEM, TRUE), MPFROM2SHORT
                                                                   (MIA_CHECKED, MIA_CHECKED));
                  }
                  else
                  {
                     filefilter &= FILE_ARCHIVED | FILE_DIRECTORY | FILE_HIDDEN
                                                                                   | FILE_READONLY;

                     // Now that the filter attributes have been set, refill the files container
                     // rewrite the statusbar information.

                     DFiles->EmptyContainer ();
                     DFiles->FillContainer (filefilter);
                     fillStatusbar2 (NULL, NULL);

                     WinSendMsg (hwndmenu, MM_SETITEMATTR, MPFROM2SHORT
                                                      (MID_SHOWSYSTEM, TRUE), MPFROM2SHORT
                                                                   (MIA_CHECKED, NULL));
                  }
                  break;

               // All the sort functions are here and will be broken down by using 'tempmsg'.

               case MID_SORTBYNAME:
               case MID_SORTBYDATE:
               case MID_SORTBYSIZE:
               case MID_SORTBYEXT:
               case MID_REVSORTBYNAME:
               case MID_REVSORTBYDATE:
               case MID_REVSORTBYSIZE:
               case MID_REVSORTBYEXT:

                  // Test for DoCopy () thread mutex

                  if (ulMutexState)
                  {
                     NoCanDo ();
                     break;
                  }

                  pWhatRec = (PCNRITEM) WinSendMsg (hwndfilescntr,
                                           CM_QUERYRECORDEMPHASIS, MPFROMP (CMA_FIRST),
                                                                  MPFROMSHORT (CRA_CURSORED));
                  if (tempmsg == MID_SORTBYNAME)
                     SortFiles (SORTBYNAME, hwndmenu);
                  else if (tempmsg == MID_SORTBYDATE)
                     SortFiles (SORTBYDATE, hwndmenu);
                  else if (tempmsg == MID_SORTBYSIZE)
                     SortFiles (SORTBYSIZE, hwndmenu);
                  else if (tempmsg == MID_SORTBYEXT)
                     SortFiles (SORTBYEXT, hwndmenu);
                  else if (tempmsg == MID_REVSORTBYNAME)
                     SortFiles (REVSORTBYNAME, hwndmenu);
                  else if (tempmsg == MID_REVSORTBYDATE)
                     SortFiles (REVSORTBYDATE, hwndmenu);
                  else if (tempmsg == MID_REVSORTBYSIZE)
                     SortFiles (REVSORTBYSIZE, hwndmenu);
                  else if (tempmsg == MID_REVSORTBYEXT)
                     SortFiles (REVSORTBYEXT, hwndmenu);

                  strcpy (tparms7.szSavename, pWhatRec->szFileName);

                  tparms7.Hab = NULL;
                  tparms7.Hmq = NULL;
                  tparms7.hwnd = hwndfilescntr;
                  tparms7.job = RETURNTO;
                  tid7 = _beginthread (threadall, NULL, 8192, &tparms7);

                  break;

               case MID_TOOLBAR:

                  // This toggles the toolbar visibility.

                  ToggleToolbar ();
                  break;

               case MID_USER1:
               case MID_USER2:
               case MID_USER3:
               case MID_USER4:
               case MID_USER5:
               case MID_USER6:
               case MID_USER7:
               case MID_USER8:

                  // Test for DoCopy () thread mutex

                  if (ulMutexState)
                  {
                     NoCanDo ();
                     break;
                  }

                  // A search will be made for the first selected record.  The search criteria for
                  // the first record is CMA_FIRST, but for the following records, the criteria is the
                  // record of the preceeding search.
                  // Unlike the editor routine, if there is not a file selected, this program will
                  // still load and pass the parameters alone.
                  // If there is a file selected, it will be passed as a second parameter.
                  // This routine will also accept a Rexx 'cmd' file as a User Program name
                  // and then pass the same parameters in the same order to the cmd file.

                  pWhatRec = (PCNRITEM) CMA_FIRST;

                  pWhatRec = (PCNRITEM) WinSendMsg (hwndfilescntr,
                                           CM_QUERYRECORDEMPHASIS, MPFROMP (pWhatRec),
                                                                  MPFROMSHORT (CRA_SELECTED));

                  // Set up the buffer so that we can concatenate.

                  szpathbuffer [0] = 0;

                  // Fill in the PROGDETAILS structure for the WinStartApp function.
                  // If the application is a Win-OS/2 application, make it a windowed app.

                  pDetails = &Details;
                  pDetails->progt.progc     = PROG_DEFAULT;
                  pDetails->swpInitial.fl = SWP_ACTIVATE;

                  pDetails->progt.fbVisible = SHE_VISIBLE;

                  if (tempmsg == MID_USER1)
                  {
                     strcpy (sznamebuffer, szUser1parms);
                     pDetails->pszTitle        = szUser1name;//"User Program 1";
                     pDetails->pszExecutable   = (PSZ) szUser1prog;
                     pDetails->pszStartupDir   = (PSZ) szUser1dir;
                  }
                  else if (tempmsg == MID_USER2)
                  {
                     strcpy (sznamebuffer, szUser2parms);
                     pDetails->pszTitle        = szUser2name;//"User Program 2";
                     pDetails->pszExecutable   = (PSZ) szUser2prog;
                     pDetails->pszStartupDir   = (PSZ) szUser2dir;
                  }
                  else if (tempmsg == MID_USER3)
                  {
                     strcpy (sznamebuffer, szUser3parms);
                     pDetails->pszTitle        = szUser3name;//"User Program 3";
                     pDetails->pszExecutable   = (PSZ) szUser3prog;
                     pDetails->pszStartupDir   = (PSZ) szUser3dir;
                  }
                  else if (tempmsg == MID_USER4)
                  {
                     strcpy (sznamebuffer, szUser4parms);
                     pDetails->pszTitle        = szUser4name;//"User Program 4";
                     pDetails->pszExecutable   = (PSZ) szUser4prog;
                     pDetails->pszStartupDir   = (PSZ) szUser4dir;
                  }
                  else if (tempmsg == MID_USER5)
                  {
                     strcpy (sznamebuffer, szUser5parms);
                     pDetails->pszTitle        = szUser5name;//"User Program 5";
                     pDetails->pszExecutable   = (PSZ) szUser5prog;
                     pDetails->pszStartupDir   = (PSZ) szUser5dir;
                  }
                  else if (tempmsg == MID_USER6)
                  {
                     strcpy (sznamebuffer, szUser6parms);
                     pDetails->pszTitle        = szUser6name;//"User Program 6";
                     pDetails->pszExecutable   = (PSZ) szUser6prog;
                     pDetails->pszStartupDir   = (PSZ) szUser6dir;
                  }
                  else if (tempmsg == MID_USER7)
                  {
                     strcpy (sznamebuffer, szUser7parms);
                     pDetails->pszTitle        = szUser7name;//"User Program 7";
                     pDetails->pszExecutable   = (PSZ) szUser7prog;
                     pDetails->pszStartupDir   = (PSZ) szUser7dir;
                  }
                  else if (tempmsg == MID_USER8)
                  {
                     strcpy (sznamebuffer, szUser8parms);
                     pDetails->pszTitle        = szUser8name;//"User Program 8";
                     pDetails->pszExecutable   = (PSZ) szUser8prog;
                     pDetails->pszStartupDir   = (PSZ) szUser8dir;
                  }
                  pDetails->pszParameters = szpathbuffer;

                  // Test for application type and if it's a windows app, make it a seamless
                  // windowed presentation.

                  DosQueryAppType (pDetails->pszExecutable, &ulApptype);
                  if ((ulApptype == 4096) || (ulApptype == 1024))
                  {
                     pDetails->progt.progc = PROG_31_ENHSEAMLESSVDM;
                  }

                  // Test for a Dos app and if it is, make it a windowed app also.

                  if ((ulApptype == 32))
                  {
                     pDetails->progt.progc = PROG_WINDOWEDVDM;
                  }

                  if (pWhatRec)
                  {
                     // Get the path and filename of the file selected.  Surround the name in
                     // quotes to allow for filenames with embedded spaces only if the program
                     // is neither a Dos nor a Windows program.

                     sztemp [0] = 32;
                     if ((ulApptype & 5152) == 0) sztemp [1] = 34;
                     else sztemp [1] = 32;
                     sztemp [2] = CHAR (CurrDrive + 64);
                     sztemp [3] = 58;
                     sztemp [4] = 0;
                     strcat (sznamebuffer, sztemp);
                     strcat (sznamebuffer, pWhatRec->szDirPath);
                     if ((ulApptype & 5152) == 0)
                         strcat (sznamebuffer, "\"");  // Add closing quote mark

                     // Deselect the file so it won't still be hilited on return.

                     WinSendMsg (hwndfilescntr, CM_SETRECORDEMPHASIS,
                               MPFROMLONG (pWhatRec), MPFROM2SHORT (FALSE, CRA_SELECTED));
                  }

                  // Test to see if this is a CMD file and if it is, make special
                  // arrangements.

                  exelen = SHORT (strlen (pDetails->pszExecutable));
                  if ((pDetails->pszExecutable [exelen -3] == 'c') ||
                                                          (pDetails->pszExecutable [exelen - 3] == 'C'))
                  {
                     if ((pDetails->pszExecutable [exelen -2] == 'm') ||
                                                          (pDetails->pszExecutable [exelen - 2] == 'M'))
                     {
                        if ((pDetails->pszExecutable [exelen -1] == 'd') ||
                                                          (pDetails->pszExecutable [exelen - 1] == 'D'))
                        {
                           //Make the cmd processor window invisible.

                           strcpy (szpathbuffer, "/C ");
                           strcat (szpathbuffer, pDetails->pszExecutable);
                           strcat (szpathbuffer, " ");
                           pDetails->swpInitial.fl = SWP_HIDE;
                           pDetails->pszExecutable = (PSZ) pszCmdProcessor;
                        }
                     }
                  }
                  strcat (szpathbuffer, sznamebuffer);

                  // Put up the hourglass pointer.

                  WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP,
                                                                                SPTR_WAIT, FALSE));

                  // Start the User program.
                  // SAF_INSTALLCMDLINE param must be used if Win-OS/2 programs are to
                  // be run.  (I don't know why this is)
                  // Allow the user program to continue even if File Freedom is shut down
                  // (don't use SAF_STARTCHILDAPP).

                  WinStartApp (hwndfilescntr, pDetails, NULL, NULL,
                                                    SAF_INSTALLEDCMDLINE);
                  // Reset this flag so next app will be visible.

                  pDetails->swpInitial.fl = SWP_ACTIVATE;

                  break;

               case MID_QUIT:

                  WinSendMsg (hwnd, WM_CLOSE, NULL, NULL);
                  break;

               case MID_REGISTER:

                  WinMessageBox (HWND_DESKTOP, hwndclient, "Source code has been released\n"
                                   "into the public domain.\n"
                                   "\nContact:\n"
                                   "\nBruce Henry\n"
                                   "703 Hillview Rd.\nCambridge, ON,\n"
                                   "Canada N3H 5C2\n\n"
                                   "or by email to:\n"
                                   "brhenry@kw.igs.net\n"
                                   "Web Page:\n"
                                   "www.kw.igs.net\\~brhenry\n",
                                   "File Freedom", MESSAGEBOXID, MB_OK | MB_INFORMATION);
                  break;

               case MID_USINGHELP:

                  WinSendMsg (hwndhelp, HM_DISPLAY_HELP, NULL, NULL);
                  break;

               case MID_HELP:

                  WinSendMsg (hwndhelp, HM_GENERAL_HELP, NULL, NULL);
                  break;

               case MID_KEYSHELP:

                  // See HM_QUERY_KEYS_HELP for setup (below)

                  WinSendMsg (hwndhelp, HM_KEYS_HELP, NULL, NULL);
                  break;

            }
            break;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      case HM_QUERY_KEYS_HELP:

         return (MRESULT) 1200;
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

      case WM_CONTROL:

         // Some variables are required for the drag/drop code.

         PDRAGINFO pDragInfo;
         PDRAGITEM pDragItem;
         USHORT usOp;
         USHORT usIndicator, cItems, i;

         // If this a Drag/Drop Notification code, set up the variables.

         if ((SHORT2FROMMP (mp1) == CN_DRAGOVER)
             || (SHORT2FROMMP (mp1) == CN_DRAGLEAVE)
             || (SHORT2FROMMP (mp1) == CN_DROP))
         {

            // Set the Destination drive variable here since it is needed in the DragDrop dialog
            // in order to fill the container window.  It will always be One more than the index.

            DropDrive = CurrDrive;

            // Initialize the pointer for the DRAGINFO structure

            PCNRDRAGINFO pcnrd;
            pcnrd = (PCNRDRAGINFO) mp2;
            pDragInfo = pcnrd->pDragInfo;
         }

         switch (SHORT2FROMMP (mp1))
         {
            // Help in a container must be trapped via CN_HELP

            case CN_HELP:

                  WinSendMsg (hwndhelp, HM_GENERAL_HELP, NULL, NULL);
                  return 0;

            case CN_EXPANDTREE:

               // The id of the container is always 2 more than the window frame

               if (SHORT1FROMMP (mp1) == DIRWINDOWID + 2)
                  {
                     pWhatRec = (PCNRITEM) PVOIDFROMMP (mp2);

                     // Check for the flag that indicates this message is being sent
                     // only to rebuild the Tree window.

                     if (!expandflag)
                     {
                        // This first section fills the Files container with the files in this
                        // new folder that is being opened in the Directory container.
                        // Don't use a thread here because of latency problems in other
                        // areas that depend on a timely completion here.

                        DFiles->PathToOrder (pWhatRec->szDirPath, (SHORT) CurrDrive);

                        // Fill the files container and update the source status bar.

                        DFiles->FillContainer (filefilter);

                        // This statusbar information can't be gathered until the container is filled.

                        fillStatusbar2 (NULL, NULL);

                        // Update the title of the Files container with the new path

                        szFilesTitle [0] = CHAR (CurrDrive + 64);
                        szFilesTitle [1] = 58;
                        szFilesTitle [2] = 0;
                        if (!strlen (pWhatRec->szDirPath))
                        {
                           szFilesTitle [2] = 92;
                           szFilesTitle [3] = 0;
                        }
                        else strcat (szFilesTitle, pWhatRec->szDirPath);
                        CHAR *pTitle = szFilesTitle;
                        SecondChild->newtitle (pTitle);
                     }
                     // For Records Without Children Only:
                     // This section substitutes the open folder icon for the collapsed folder
                     // icon.  This is required because the folder must appear to be
                     // open since the Files container will display the files inside regardless
                     // of whether there are subdirectories or not.
                     // This routine also closes all of the peer folders in the Directory container that
                     // is being opened.  This keeps the display as small as possible.
                     // This routine is also used with the DragDrop dialog.

                     ExpandTreeFix (pWhatRec, hwnddircntr);
                  }
                  expandflag = 0;
                  return 0;

               case CN_COLLAPSETREE:

                  // The id of the container is always 2 more than the window frame

                  if (SHORT1FROMMP (mp1) == DIRWINDOWID + 2)
                  {
                     // This section removes child records from the container being closed.
                     // This will ensure that the next time this folder is opened, the contents
                     // will be up-to-date.

                     CollapseTreeFix ( (PCNRITEM) PVOIDFROMMP (mp2), hwnddircntr, DTree);
                  }
                  return 0;

               case CN_ENTER:

                  PNOTIFYRECORDENTER pNRE;
                  pNRE = PNOTIFYRECORDENTER (PVOIDFROMMP (mp2));

                  PCNRITEM pEnterRec;
                  pEnterRec = (PCNRITEM) pNRE->pRecord;
                  pDetails = &Details;

                  // If the record pointer is NULL, the pointer was not over a record when
                  // it was clicked so a break is called for.

                  if (!pEnterRec) return 0;

                  // If the pointer was over a record, it is determined if it was in the files
                  // container or the root container.  If it was in the root container, the folder
                  // is opened.  If it was in the files container, it is determined if the record
                  // was a subdirectory or a file.  If it was a subdirectory, the directory is
                  // opened.  If it was a file, it is implied that a viewer is requested so the
                  // proper viewer is loaded according to the extension.  If there is no viewer
                  // setup for that extension, a default viewer will be used.

                  if (pNRE->hwndCnr == hwnddircntr)
                  {
                     // The routine used to open a record is done in a sub-routine since it
                     // is also used in the DragDrop dialog container.
                     // They are distinguished by the last parameter.  The drag/drop routine
                     // sends a pointer to it's tree object.

                     DoTree (hwnddircntr, pEnterRec, NULL);
                  }

                  if (pNRE->hwndCnr == hwndfilescntr)
                  {
                     // Save the filename for possible returnto () use.

                     strcpy (tparms7.szSavename, pEnterRec->szFileName);

                     // If this is a sub-directory

                     if (pEnterRec->attrFile & 16)
                     {
                        // Update the title of the Files container with the new path
                        // before the file is erased in the PathToOrder routine.
                        // This would happen because this time files in THIS container
                        // are being used as reference material instead of files from the
                        // root tree container.

                        szFilesTitle [0] = CHAR (CurrDrive + 64);
                        szFilesTitle [1] = 58;
                        szFilesTitle [2] = 0;
                        if (!strlen (pEnterRec->szDirPath))
                        {
                           szFilesTitle [2] = 92;
                           szFilesTitle [3] = 0;
                        }

                        else strcat (szFilesTitle, pEnterRec->szDirPath);
                        CHAR *pTitle = szFilesTitle;
                        SecondChild->newtitle (pTitle);

                        // This section fills the Files container with the files in this
                        // new folder that is being opened in the Directory container.

                        DFiles->PathToOrder (pEnterRec->szDirPath, (SHORT) CurrDrive);
                        tparms3.Hab = hab3;
                        tparms3.Hmq = hmq3;
                        tparms3.job = FILES;
                        tid3 = _beginthread (threadall, NULL, 8192, &tparms3);
                     }
                     else
                     {
                        // This is not a subdirectory so it will be classified a file that the user
                        // wants to view.

                        internalzipviewer = 0;
                        internalrunprog = 0;
                        Namelength = (USHORT) strlen (pEnterRec->szFileName);
                        int x, xx, xxx;
                        xx = 0;
                        PSZ ptemp;

                        // Scan the filename string for a period.  If there is none, there is no
                        // file extension.  If there is, that is where the string comparison begins.
                        // If there are many periods in the name, this will mark the last one.

                        for (x = 0; x < Namelength; x++)
                           if (pEnterRec->szFileName [x] == 46) xx = x;

                        if (!xx ) strcpy (sznamebuffer, "<none>");
                        else
                        {
                           ptemp = &pEnterRec->szFileName [xx + 1];
                           strcpy (sznamebuffer, ptemp);
                        }

                        // If a match isn't found, the viewer will be set for the '0' index which
                        // is the viewer for "<none>" or the first item in the list.  This is
                        // the only ext/viewer pair protected from removal although the viewer
                        // can be edited.

                        xxx = ViewerTypeslen / 7;
                        for (x = 0; x < xxx; x++)
                        {
                           ptemp = &pszViewerTypes [x * 7];
                           xx = stricmp (sznamebuffer, ptemp);
                           if (!xx)
                           {
                              xxx = x;
                              xx = x;
                           }
                        }

                        // If there was a match, x will get incremented one more time as the loop
                        // finishes and so will not match xxx.

                        // If there was no match, check to see if it is a zip file.  No match
                        // for the 'zip' extension means that the program will use the internal
                        // zip viewer if it is a zip file.
                        // Also test to see if it is an 'exe' program.  If it is it will be run
                        // as an independent program since an alternative viewer has not been
                        // specified (x==xxx).
                        // Also test for a .bat or .cmd file since they will also be handled as
                        // necessary.

                        if (x == xxx)
                        {
                           xxx = stricmp (sznamebuffer, "zip");
                           if (!xxx) internalzipviewer = 1;
                           if (xxx)
                           {
                              xxx = stricmp (sznamebuffer, "exe");
                              if (!xxx) internalrunprog = 1;
                           }
                           if (xxx)
                           {
                              xxx = stricmp (sznamebuffer, "cmd");
                              if (!xxx) internalrunprog = 2;
                           }
                           if (xxx)
                           {
                              xxx = stricmp (sznamebuffer, "bat");
                              if (!xxx) internalrunprog = 2;
                              else
                                 xx = 0;  // Default viewer.
                           }
                        }

                        if (internalzipviewer)
                        {
                           sztemp [0] = CHAR (CurrDrive + 64);
                           sztemp [1] = 58;
                           sztemp [2] = 0;
                           strcpy (szpathbuffer, sztemp);
                           strcat (szpathbuffer, pEnterRec->szDirPath);

                           // Change the current dir to the home dir so that the Info-zip utilities
                           // can be found if they are there.

                           DosSetDefaultDisk (Homedrive);
                           DosSetCurrentDir (szHomepath);

                           // Do a fix-up here so that the passed path can't end in a backslash
                           // as it would with just a drive designator eg. "H:\".
                           // Unzip can't handle this.

                           int shortlen;
                           strcpy (sznamebuffer, szDestPath);
                           shortlen = strlen (sznamebuffer);
                           if (shortlen == 3)
                           {
                              if (sznamebuffer [2] == 92)
                                 sznamebuffer [2] = 0;
                           }

                           Zipshell = new zipgui ("Zip Viewer", szpathbuffer, sznamebuffer,
                                          szHomedrivepath, pszViewers, pszViewerTypes,
                                           USHORT (ViewerTypeslen/7), szUnzipprog, szZipprog,
                                                                        hwndhelp, hwndframe, NULL);

                           // Change the drive back to where we were.

                           DosSetDefaultDisk (CurrDrive);

                           delete Zipshell;

                          // The files container will have to be updated to show any filesize changes
                          // may may have resulted from files being extracted or deleted from the zip.
                          // Don't use a thread here since returnto () is being used below.

                          DFiles->EmptyContainer ();

                          // Fill the files container and update the source status bar.

                          DFiles->FillContainer (filefilter);

                          // This statusbar information can't be gathered until the container is filled.

                          fillStatusbar2 (NULL, NULL);

                          // Update the root window titlebar byte count.

                          getFSInfo (CurrDrive);

                           // Update the Destination status bar info with new bytes free etc.

                           fillStatusbar1 (DestDrive, (PSZ) szDestPath);

                           WinSetFocus (HWND_DESKTOP, hwndfilescntr);

                           // Return to the last cursored file.

                           tparms7.Hab = NULL;
                           tparms7.Hmq = NULL;
                           tparms7.hwnd = hwndfilescntr;
                           tparms7.job = RETURNTO;
                           tid7 = _beginthread (threadall, NULL, 8192, &tparms7);
                        }
                        else
                        {
                           ptemp = &pszViewers [xx * VIEWERSTRLEN];

                           // Get the path and filename of the file selected and enclose it in quotes.

                           sznamebuffer [0] = 34;
                           sznamebuffer [1] = CHAR (CurrDrive + 64);
                           sznamebuffer [2] = 58;
                           sznamebuffer [3] = 0;
                           strcat (sznamebuffer, pEnterRec->szDirPath);
                           strcat (sznamebuffer, "\"");

                           szpathbuffer [0] = 0;
                           pDetails->pszStartupDir   = NULL;
                           pDetails->pszExecutable = ptemp;

                           // Test to see if the viewer is a CMD file and if it is, make special
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

                                    pDetails->swpInitial.fl = SWP_HIDE;
                                    pDetails->pszExecutable = (PSZ) pszCmdProcessor;
                                    strcat (szpathbuffer, "/C ");
                                    strcat (szpathbuffer, ptemp);
                                    strcat (szpathbuffer, " ");
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
                                          pDetails->swpInitial.fl = SWP_ACTIVATE;
                                          pDetails->pszExecutable = "EPM.EXE";
                                          strcat (szpathbuffer, "\'browse on\' ");
                                       }
                                    }
                                 }
                              }
                           }
                           // If the file name selected was determined to be a .cmd or .bat file
                           // (rather than the viewer) it also gets special treatment.
                           // Make sure the window is visible and persistent.

                           if (internalrunprog ==2)
                           {
                              pDetails->pszExecutable = (PSZ) pszCmdProcessor;
                              pDetails->pszParameters = szpathbuffer;
                              strcat (szpathbuffer, "/K ");
                              sznamebuffer [0] = CHAR (CurrDrive + 64);
                              sznamebuffer [1] = 58;
                              sznamebuffer [2] = 0;
                              strcat (sznamebuffer, pEnterRec->szDirPath);
                              pDetails->pszStartupDir = szFilesTitle;
                           }
                           strcat (szpathbuffer, sznamebuffer);

                           // Test if it is an exe program rather than a .bat or .cmd.

                           if (internalrunprog == 1)
                           {
                              sznamebuffer [0] = CHAR (CurrDrive + 64);
                              sznamebuffer [1] = 58;
                              sznamebuffer [2] = 0;
                              strcat (sznamebuffer, pEnterRec->szDirPath);
                              pDetails->pszExecutable = sznamebuffer;
                              pDetails->pszStartupDir = szFilesTitle;
                              pDetails->pszParameters = NULL;
                           }

                           // Fill in the rest of the PROGDETAILS structure for the
                           // WinStartApp function.

                           pDetails->progt.progc     = PROG_DEFAULT;
                           pDetails->progt.fbVisible = SHE_VISIBLE;
                           pDetails->pszTitle        = "Viewer";

                           // Put up the hourglass pointer.

                           WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP,
                                                                                   SPTR_WAIT, FALSE));
                           // Start the viewer.  If it has been determined that it is an exe prog
                           // and that no viewer has been set up for exe's, run it separately
                           // (it is set up as a viewer itself with no parameters) and not
                           // as a child app so that Windows programs will run.

                           // Change the current dir to the home dir so that the Info-zip utilities or
                           // viewers can be found if they are there.

                           DosSetDefaultDisk (Homedrive);
                           DosSetCurrentDir (szHomepath);

                           if (!internalrunprog)
                              WinStartApp( hwndfilescntr, pDetails, szpathbuffer,
                                                                   NULL, SAF_STARTCHILDAPP);
                           else
                              WinStartApp( hwndfilescntr, pDetails, NULL,
                                                                   NULL, SAF_INSTALLEDCMDLINE);

                           // Unmark the file that was selected.

                           WinSendMsg (hwndfilescntr, CM_SETRECORDEMPHASIS,
                                            MPFROMLONG (pEnterRec),
                                            MPFROM2SHORT (FALSE, CRA_SELECTED));

                           // Change the drive back to where we were.

                           DosSetDefaultDisk (CurrDrive);

                           // Reset this parameter so that the next app window is visible.

                           pDetails->swpInitial.fl = SWP_ACTIVATE;
                        }
                     }
                  }
                  return 0;

               // This notification is used to determine if a record has been selected.
               // If it has, it is determined if it is a file. If it is, the size of the file is added
               // to the status bar and the selected file count is incremented.

               case CN_EMPHASIS:
                  // If the flag bit 4 is set (this means the 'returnto ()' function is active),
                  // do not change the selected file count.

                  if (!(flag & 16))
                  {
                     PNOTIFYRECORDEMPHASIS pNREM;
                     pNREM = PNOTIFYRECORDEMPHASIS (PVOIDFROMMP (mp2));

                     if (pNREM->fEmphasisMask & CRA_SELECTED)
                     {
                        USHORT cFiles = 0;
                        ULONG cbFiles = 0;

                        if (pNREM->hwndCnr == hwndfilescntr)
                        {
                           // For the first search, use CMA_FIRST.  The following searches use
                           // the previous record as the criteria.

                           PCNRITEM pSelRec;
                           pSelRec = (PCNRITEM) CMA_FIRST;

                           while (pSelRec)
                           {
                              pSelRec = (PCNRITEM) WinSendMsg (hwndfilescntr,
                                                  CM_QUERYRECORDEMPHASIS, MPFROMP (pSelRec),
                                                                        MPFROMSHORT (CRA_SELECTED));
                              if (pSelRec)
                              {
                                 cFiles++;
                                 cbFiles += pSelRec->cbFile;
                              }
                           }
                           fillStatusbar2 (cFiles, cbFiles);
                        }
                     }
                     return 0;
                  }
                  return 0;

               case CN_INITDRAG:

                  // The container has sent a drag message.  Make sure it is from the
                  // files container before processing it.

                  DoDrag (PCNRDRAGINIT (PVOIDFROMMP (mp2))->pRecord,
                                  PCNRDRAGINIT (PVOIDFROMMP (mp2))->hwndCnr);
                  return 0;

               case CN_DRAGOVER:

                  // Determine if this message if from the Tree or Files Windows.  If not, ignore it.

                  if ((SHORT1FROMMP (mp1) == DIRWINDOWID + 2)
                     || (SHORT1FROMMP (mp1) == FILESWINDOWID + 2))
                  {
                     /* Get access to the DRAGINFO data structure */

                     DrgAccessDraginfo (pDragInfo);

                     // Determine if the source is either the Files or Tree container for this process
                     // or the Files or Tree container for a concurrent FF process.

                     if (pDragInfo->hwndSource != hwndfilescntr
                                                          && pDragInfo->hwndSource != hwnddircntr)
                     {
                        ULONG ulFFcalling;
                        ulFFcalling = WinQueryWindowULong (pDragInfo->hwndSource, QWL_USER);
                        if (ulFFcalling != 123456)
                        {
                           DrgFreeDraginfo (pDragInfo);
                           return (MRFROM2SHORT (DOR_NODROPOP, 0));
                        }
                     }

                     /* Can we accept this drop? */
                     switch (pDragInfo->usOperation)
                     {
                      /* Return DOR_NODROPOP if current operation */
                      /* is link or unknown                       */

                        case DO_UNKNOWN:
                           DrgFreeDraginfo (pDragInfo);
                           return (MRFROM2SHORT (DOR_NODROPOP, 0));

                        /* Our default operation is Move */
                        case DO_DEFAULT:
                             usOp =  DO_MOVE;
                           break;

                        /* Do the requested specific operation */
                        case DO_MOVE:
                        case DO_COPY:
                           usOp = pDragInfo->usOperation;
                           break;

                        default:
                           DrgFreeDraginfo (pDragInfo);
                           return (MRFROM2SHORT (DOR_NODROPOP, 0));
                     }

                     usIndicator = DOR_DROP;
                     cItems = (USHORT) DrgQueryDragitemCount (pDragInfo);

                     /* Now, we need to look at each item in turn */

                     for (i = 0; i < cItems; i++)
                     {
                        pDragItem = DrgQueryDragitemPtr (pDragInfo, i);

                        /* Make sure we can move for a Move request */
                        /* or copy for a Copy                       */

                        if (((pDragItem->fsSupportedOps & DO_COPYABLE)   &&
                             (usOp == (USHORT) DO_COPY))                 ||
                            ((pDragItem->fsSupportedOps & DO_MOVEABLE)   &&
                             (usOp == (USHORT) DO_MOVE)))

                        {
                           /* Check the rendering format */
                           if (DrgVerifyRMF (pDragItem, "DRM_OS2FILE", "DRF_UNKNOWN"))
                              usIndicator = DOR_DROP;
                           else
                              usIndicator = DOR_NEVERDROP;
                        }
                        else
                           usIndicator = DOR_NODROPOP;
                     }

                     /* Release the draginfo data structure */

                     DrgFreeDraginfo (pDragInfo);

                     return (MRFROM2SHORT (usIndicator, usOp));
                  }
                  else return 0;

               case CN_DRAGLEAVE:

                  // Emphasis is handled by the container internally.

                  return (MRESULT) FALSE;

               case CN_DROP:

                  // All of the CN_DROP code is handled in an Object window which is running
                  // in the INI thread.  This keeps all of the conversations from interfering with
                  // the main thread message queue.

                  // Determine if this message is for the Tree or Files Window.  If not, ignore it.

                  if ((SHORT1FROMMP (mp1) == DIRWINDOWID + 2)
                     || (SHORT1FROMMP (mp1) == FILESWINDOWID + 2))
                  {
                     // Find out if the current source drive (something is being dropped on
                     // one of the container windows which are the source drive) is ready
                     // and has media present.

                     rc = IsDriveReady (CurrDrive);
                     if (!rc)
                     {
                        return (MRFROM2SHORT (DOR_NODROPOP, 0));
                     }
                     WinPostMsg (hwndobject, OBJM_DROP, mp1, mp2);

                  }
                  else return (MRFROM2SHORT (DOR_NODROPOP, 0));
                  break;

               case CN_CONTEXTMENU:
                  // Get the last position of the pointer so that the menu can be
                  // positioned close to it.

                  WinQueryPointerPos (HWND_DESKTOP, &popptl);
                  WinMapWindowPoints (HWND_DESKTOP, HWND_DESKTOP, &popptl, 1);

                  WinPopupMenu (HWND_DESKTOP, hwnd, hwndpopup, popptl.x, popptl.y - 60, 0,
                                 PU_HCONSTRAIN | PU_VCONSTRAIN | PU_MOUSEBUTTON1
                                 | PU_MOUSEBUTTON2 | PU_KEYBOARD);

                  // This next part takes care of updating all of the Menu item attributes.
                  // Although the popup menu is exactly the same as the Main menu, it
                  // leads a different life and doesn't receive the update notifications.
                  // This part looks after that work.

                  SetView (basicview, hwndpopup);
                  SortFiles (sortmethod, hwndpopup);
                  //..
                  ltemp = MIA_CHECKED;
                  if (!(filefilter & FILE_HIDDEN)) ltemp = 0;
                  WinSendMsg (hwndpopup, MM_SETITEMATTR, MPFROM2SHORT
                                (MID_SHOWHIDDEN, TRUE), MPFROM2SHORT (MIA_CHECKED, ltemp));
                  ltemp = MIA_CHECKED;
                  if (!(filefilter & FILE_SYSTEM)) ltemp = 0;
                  WinSendMsg (hwndpopup, MM_SETITEMATTR, MPFROM2SHORT
                               (MID_SHOWSYSTEM, TRUE), MPFROM2SHORT (MIA_CHECKED, ltemp));

                  ltemp = MIA_CHECKED;
                  if (toolbarorient & 2) ltemp = 0;
                  WinSendMsg (hwndpopup, MM_SETITEMATTR, MPFROM2SHORT
                                    (MID_TOOLBAR, TRUE), MPFROM2SHORT (MIA_CHECKED, ltemp));

                  return 0;
            }
            return 0;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
         case WM_CREATE:

            hwndpopup = WinLoadMenu (hwnd, 0, STDWINDOWID);
            return 0;
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
       case WM_CLOSE:

           //--------------------------------------------------------------------
           // On exiting, the appropriate user settings will be written to the "FileFrdm.ini" file.
           //--------------------------------------------------------------------

            // Set the current drive and directory to the starting path (where the executable
            // file was located).

            DosSetDefaultDisk (Homedrive);
            DosSetCurrentDir (szHomepath);

            USHORT cChars;
            cChars = USHORT (ViewerTypeslen / 7);
            hini = PrfOpenProfile (hab1, "FileFrdm.ini");

            PrfWriteProfileData (hini, "FileFrdm.exe", "Viewers", pszViewers,
                                                                        cChars * VIEWERSTRLEN + 1);
            PrfWriteProfileData (hini, "FileFrdm.exe", "ViewerType", pszViewerTypes,
                                                                                         cChars * 7 + 1);
            PrfWriteProfileString (hini, "FileFrdm.exe", "DSource", (PSZ) szDefaultSource);
            PrfWriteProfileString (hini, "FileFrdm.exe", "DDestination", (PSZ) szDefaultDest);

            PrfWriteProfileString (hini, "FileFrdm.exe", "EdName", (PSZ) szDefaultEditor);
            PrfWriteProfileString (hini, "FileFrdm.exe", "EdParms", (PSZ) szDefaultEdParms);

            PrfWriteProfileString (hini, "FileFrdm.exe", "Archiver", (PSZ) szZipprog);
            PrfWriteProfileString (hini, "FileFrdm.exe", "ArchiverParms", (PSZ) szZipparms);

            PrfWriteProfileString (hini, "FileFrdm.exe", "UnArchiver", (PSZ) szUnzipprog);
            PrfWriteProfileString (hini, "FileFrdm.exe", "UnArchiverParms", (PSZ) szUnzipparms);

            PrfWriteProfileString (hini, "FileFrdm.exe", "SearchToolName", (PSZ) szDefaultSearchTool);
            PrfWriteProfileString (hini, "FileFrdm.exe", "STParms", (PSZ) szDefaultSTParms);

            PrfWriteProfileString (hini, "FileFrdm.exe", "User1Prog", (PSZ) szUser1prog);
            PrfWriteProfileString (hini, "FileFrdm.exe", "User1Parms", (PSZ) szUser1parms);
            PrfWriteProfileString (hini, "FileFrdm.exe", "User1Dir", (PSZ) szUser1dir);
            PrfWriteProfileString (hini, "FileFrdm.exe", "User1Name", (PSZ) szUser1name);

            PrfWriteProfileString (hini, "FileFrdm.exe", "User2Prog", (PSZ) szUser2prog);
            PrfWriteProfileString (hini, "FileFrdm.exe", "User2Parms", (PSZ) szUser2parms);
            PrfWriteProfileString (hini, "FileFrdm.exe", "User2Dir", (PSZ) szUser2dir);
            PrfWriteProfileString (hini, "FileFrdm.exe", "User2Name", (PSZ) szUser2name);

            PrfWriteProfileString (hini, "FileFrdm.exe", "User3Prog", (PSZ) szUser3prog);
            PrfWriteProfileString (hini, "FileFrdm.exe", "User3Parms", (PSZ) szUser3parms);
            PrfWriteProfileString (hini, "FileFrdm.exe", "User3Dir", (PSZ) szUser3dir);
            PrfWriteProfileString (hini, "FileFrdm.exe", "User3Name", (PSZ) szUser3name);

            PrfWriteProfileString (hini, "FileFrdm.exe", "User4Prog", (PSZ) szUser4prog);
            PrfWriteProfileString (hini, "FileFrdm.exe", "User4Parms", (PSZ) szUser4parms);
            PrfWriteProfileString (hini, "FileFrdm.exe", "User4Dir", (PSZ) szUser4dir);
            PrfWriteProfileString (hini, "FileFrdm.exe", "User4Name", (PSZ) szUser4name);

            PrfWriteProfileString (hini, "FileFrdm.exe", "User5Prog", (PSZ) szUser5prog);
            PrfWriteProfileString (hini, "FileFrdm.exe", "User5Parms", (PSZ) szUser5parms);
            PrfWriteProfileString (hini, "FileFrdm.exe", "User5Dir", (PSZ) szUser5dir);
            PrfWriteProfileString (hini, "FileFrdm.exe", "User5Name", (PSZ) szUser5name);

            PrfWriteProfileString (hini, "FileFrdm.exe", "User6Prog", (PSZ) szUser6prog);
            PrfWriteProfileString (hini, "FileFrdm.exe", "User6Parms", (PSZ) szUser6parms);
            PrfWriteProfileString (hini, "FileFrdm.exe", "User6Dir", (PSZ) szUser6dir);
            PrfWriteProfileString (hini, "FileFrdm.exe", "User6Name", (PSZ) szUser6name);

            PrfWriteProfileString (hini, "FileFrdm.exe", "User7Prog", (PSZ) szUser7prog);
            PrfWriteProfileString (hini, "FileFrdm.exe", "User7Parms", (PSZ) szUser7parms);
            PrfWriteProfileString (hini, "FileFrdm.exe", "User7Dir", (PSZ) szUser7dir);
            PrfWriteProfileString (hini, "FileFrdm.exe", "User7Name", (PSZ) szUser7name);

            PrfWriteProfileString (hini, "FileFrdm.exe", "User8Prog", (PSZ) szUser8prog);
            PrfWriteProfileString (hini, "FileFrdm.exe", "User8Parms", (PSZ) szUser8parms);
            PrfWriteProfileString (hini, "FileFrdm.exe", "User8Dir", (PSZ) szUser8dir);
            PrfWriteProfileString (hini, "FileFrdm.exe", "User8Name", (PSZ) szUser8name);

            PrfWriteProfileString (hini, "FileFrdm.exe", "Dir1", (PSZ) szDirHist [0]);
            PrfWriteProfileString (hini, "FileFrdm.exe", "Dir2", (PSZ) szDirHist [1]);
            PrfWriteProfileString (hini, "FileFrdm.exe", "Dir3", (PSZ) szDirHist [2]);
            PrfWriteProfileString (hini, "FileFrdm.exe", "Dir4", (PSZ) szDirHist [3]);
            PrfWriteProfileString (hini, "FileFrdm.exe", "Dir5", (PSZ) szDirHist [4]);
            PrfWriteProfileString (hini, "FileFrdm.exe", "Dir6", (PSZ) szDirHist [5]);
            PrfWriteProfileString (hini, "FileFrdm.exe", "Dir7", (PSZ) szDirHist [6]);
            PrfWriteProfileString (hini, "FileFrdm.exe", "Dir8", (PSZ) szDirHist [7]);
            PrfWriteProfileString (hini, "FileFrdm.exe", "Dir9", (PSZ) szDirHist [8]);
            PrfWriteProfileString (hini, "FileFrdm.exe", "Dir10", (PSZ) szDirHist [9]);

            PrfWriteProfileData (hini, "FileFrdm.exe", "Filters", &filefilter, sizeof (ULONG));

            PrfWriteProfileData (hini, "FileFrdm.exe", "View", &basicview, sizeof (ULONG));

            PrfWriteProfileData (hini, "FileFrdm.exe", "Sort", &sortmethod, sizeof (ULONG));

            PrfWriteProfileData (hini, "FileFrdm.exe", "ToolBarPos", &toolbarorient, sizeof (ULONG));

            PrfWriteProfileData (hini, "FileFrdm.exe", "DividerPos", &FCwidth, sizeof (ULONG));

            // The size, position and presentation parameters will be stored in the OS2.INI file.
            // Note:
            // The main window frame position will be saved with the "Position" key
            // if it is the first instance opened.  If it is an additional instance which has
            // been opened, the position will be saved to the "AltPosition" key.
            // This will prevent other instances from overwriting the original
            // window position and also enable the other instance to save its position also.

            if (!ulMutexFFrunning)
               WinStoreWindowPos ("FileFreedom", "Position", hwndframe);
            else
               WinStoreWindowPos ("FileFreedom", "AltPosition", hwndframe);

            WinStoreWindowPos ("FileFreedom", "Directories", hwnddircntr);
            WinStoreWindowPos ("FileFreedom", "Files", hwndfilescntr);
            WinStoreWindowPos ("FileFreedom", "StatusBar1", hwndStatusbar1);
            WinStoreWindowPos ("FileFreedom", "StatusBar2", hwndStatusbar2);
            WinStoreWindowPos ("FileFreedom", "Toolbar", hwndtoolbar);
            WinStoreWindowPos ("FileFreedom", "Drivebar", hwnddrivebar);

            // Store the fonts of the titlebar in the focus window and in the inactive window.

            PrfWriteProfileString (hini, "FileFrdm.exe", "Treetitlefont", (PSZ) szTreetitlefont);
            PrfWriteProfileString (hini, "FileFrdm.exe", "Filestitlefont", (PSZ) szFilestitlefont);

            // Store the colours for the window titlebars (inactive and focused).

            PrfWriteProfileData (hini, "FileFrdm.exe", "Passivetitlecolor", &passivebkgnd,
                                                                          sizeof (ULONG));
            PrfWriteProfileData (hini, "FileFrdm.exe", "Activetitlecolor", &focusbkgnd,
                                                                          sizeof (ULONG));
            // Write a value for the timed refresh of the Directories and the Drives.
            // A value of 0 means no refresh.

            PrfWriteProfileData (hini, "FileFrdm.exe", "DirRefresh", &interval1, sizeof (ULONG));
            PrfWriteProfileData (hini, "FileFrdm.exe", "DriveRefresh", &interval2, sizeof (ULONG));

            PrfCloseProfile (hini);

            break ;
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
         case WM_FOCUSCHANGE:

            hwndtemp = HWNDFROMMP (mp1);

            // Change the colours of the titlebar losing the focus to passive and change the
            // colours of the window gaining the focus to a the focus colors.
            // These colors are saved each time the program closes and reloaded each time
            // the program starts.  The code below will trap a new color if it has been dropped
            // on the titlebars.
            // Also save the fonts from the two window titlebars whenever a window has lost
            // focus (that is not the Tree or Files window) - this could be expected when a new
            // font is dropped on File Freedom and focus is returned to the program.
            // This is in lieu of Drag & Drop coding.

            // Save the window handle of the last window to have focus but only if it is
            // one of the container windows.  This is used to reset the focus if a toolbar
            // pushbutton or any other static or non-responsive window area
            // such as the window that holds the pushbuttons has been clicked on.

            if (SHORT1FROMMP (mp2))
            {
               if (hwndtemp != hwnddircntr && hwndtemp != hwndfilescntr)
               {
                  WinQueryPresParam (hwndTreeTitlebar, PP_FONTNAMESIZE, NULL, NULL,
                                                            50, (PSZ) szTreetitlefont, QPF_NOINHERIT);
                  WinQueryPresParam (hwndFilesTitlebar, PP_FONTNAMESIZE, NULL, NULL,
                                                            50, (PSZ) szFilestitlefont, QPF_NOINHERIT);
               }

               HWND temp;
               temp = WinQueryFocus (HWND_DESKTOP);

               if (temp == hwnddircntr)
               {
                  if (hwndtemp == hwndfilescntr || hwndfocus == hwndfilescntr)
                  {
                     WinQueryPresParam (hwndFilesTitlebar, PP_BACKGROUNDCOLOR, NULL, NULL,
                                                                 sizeof (RGB), &rgb, QPF_NOINHERIT);
                     focusbkgnd = (rgb.bRed * 65536) + (rgb.bGreen * 256) + rgb.bBlue;

                     WinQueryPresParam (hwndTreeTitlebar, PP_BACKGROUNDCOLOR, NULL, NULL,
                                                                 sizeof (RGB), &rgb, QPF_NOINHERIT);
                     passivebkgnd = (rgb.bRed * 65536) + (rgb.bGreen * 256) + rgb.bBlue;
                  }
                  else if (hwndfocus == hwnddircntr)
                  {
                     WinQueryPresParam (hwndTreeTitlebar, PP_BACKGROUNDCOLOR, NULL, NULL,
                                                                 sizeof (RGB), &rgb, QPF_NOINHERIT);
                     focusbkgnd = (rgb.bRed * 65536) + (rgb.bGreen * 256) + rgb.bBlue;

                     WinQueryPresParam (hwndFilesTitlebar, PP_BACKGROUNDCOLOR, NULL, NULL,
                                                                 sizeof (RGB), &rgb, QPF_NOINHERIT);
                     passivebkgnd = (rgb.bRed * 65536) + (rgb.bGreen * 256) + rgb.bBlue;
                  }
                  FirstChild->titleparams (focusfrgnd, focusbkgnd);
                  SecondChild->titleparams (passivefrgnd, passivebkgnd);
                  hwndfocus = hwnddircntr;
               }
               else if (temp == hwndfilescntr)
               {
                  if (hwndtemp == hwnddircntr || hwndfocus == hwnddircntr)
                  {
                     WinQueryPresParam (hwndTreeTitlebar, PP_BACKGROUNDCOLOR, NULL, NULL,
                                                                 sizeof (RGB), &rgb, QPF_NOINHERIT);
                     focusbkgnd = (rgb.bRed * 65536) + (rgb.bGreen * 256) + rgb.bBlue;

                     WinQueryPresParam (hwndFilesTitlebar, PP_BACKGROUNDCOLOR, NULL, NULL,
                                                                 sizeof (RGB), &rgb, QPF_NOINHERIT);
                     passivebkgnd = (rgb.bRed * 65536) + (rgb.bGreen * 256) + rgb.bBlue;
                  }
                  else if (hwndfocus == hwndfilescntr)
                  {
                     WinQueryPresParam (hwndFilesTitlebar, PP_BACKGROUNDCOLOR, NULL, NULL,
                                                                 sizeof (RGB), &rgb, QPF_NOINHERIT);
                     focusbkgnd = (rgb.bRed * 65536) + (rgb.bGreen * 256) + rgb.bBlue;

                     WinQueryPresParam (hwndTreeTitlebar, PP_BACKGROUNDCOLOR, NULL, NULL,
                                                                 sizeof (RGB), &rgb, QPF_NOINHERIT);
                     passivebkgnd = (rgb.bRed * 65536) + (rgb.bGreen * 256) + rgb.bBlue;
                  }
                  SecondChild->titleparams (focusfrgnd, focusbkgnd);
                  FirstChild->titleparams (passivefrgnd, passivebkgnd);
                  hwndfocus = hwndfilescntr;
               }
            }
            break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
         case WM_CONTROLPOINTER:

            // First, only discriminating for the Directory  Bookmarks button.

            if (SHORT1FROMMP (mp1) == (400 + pdbarinfo->totalbuttons))
            {
               if (!(drivebarflag & 134217728))
               {
                  drivebarflag = 134217728;
                  flag |= 32;
                  Statusbar->newtitle2 (" Directory Bookmarks");
               }
               break;
            }

            switch (SHORT1FROMMP (mp1))
            {
               // Since it's never clear how many drive buttons there will be, we will
               // include all the possible constants here and sort them out below.
               // This is used for displaying drive labels also.

               case pbA:
               case pbB:
               case pbC:
               case pbD:
               case pbE:
               case pbF:
               case pbG:
               case pbH:
               case pbI:
               case pbJ:
               case pbK:
               case pbL:
               case pbM:
               case pbN:
               case pbO:
               case pbP:
               case pbQ:
               case pbR:
               case pbS:
               case pbT:
               case pbU:
               case pbV:
               case pbW:
               case pbX:
               case pbY:
               case pbZ:

                  // This routine handles all of the flags and strings for the 26 possible drives.

                  USHORT test;
                  test = SHORT1FROMMP (mp1);
                  test -= 400;
                  ULONG power;

                  // The drives are bit mapped so turn on the bit corresponding to the drive.

                  power = 1;
                  power <<= test;
                  test --;

                  if (!(drivebarflag & power))
                  {
                     drivebarflag = power;
                     flag |= 32;

                     // Display the drive volume label in the status bar.

                     ii = pLabelarray [test * 4] - 65;
                     Statusbar->newtitle2 ((CHAR*)&ucVoldat [ii][5]);
                  }
                  break;

               case PBone:

                  if (!(toolflag & 2))
                  {
                     toolflag = 2;
                     flag |= 32;
                     Statusbar->newtitle2 (" Flip ToolBar Vertical/Horizontal");
                  }
                  break;

               case PBtwo:

                  if (!(toolflag & 4))
                  {
                     toolflag = 4;
                     flag |= 32;
                     Statusbar->newtitle2 (" Delete File(s)");
                  }
                  break;

               case PBthree:

                  if (!(toolflag & 8))
                  {
                     toolflag = 8;
                     flag |= 32;
                     Statusbar->newtitle2 (" Copy File(s)");
                  }
                  break;

               case PBfour:

                  if (!(toolflag & 16))
                  {
                     toolflag = 16;
                     flag |= 32;
                     Statusbar->newtitle2 (" Move File(s)");
                  }
                  break;

               case PBfive:

                  if (!(toolflag & 32))
                  {
                     toolflag = 32;
                     flag |= 32;
                     Statusbar->newtitle2 (" Rename File or Directory");
                  }
                  break;

               case PBsix:

                  if (!(toolflag & 64))
                  {
                     toolflag = 64;
                     flag |= 32;
                     Statusbar->newtitle2 (" Set or Change File Attributes");
                  }
                  break;

               case PBseven:

                  if (!(toolflag & 128))
                  {
                     toolflag = 128;
                     flag |= 32;
                     Statusbar->newtitle2 (" Mark All Files");
                  }
                  break;

               case PBeight:

                  if (!(toolflag & 256))
                  {
                     toolflag = 256;
                     flag |= 32;
                     Statusbar->newtitle2 (" UnMark All Files");
                  }
                  break;

               case PBnine:

                  if (!(toolflag & 512))
                  {
                     toolflag = 512;
                     flag |= 32;
                     Statusbar->newtitle2 (" Compress File(s)");
                  }
                  break;

               case PBten:

                  if (!(toolflag & 1024))
                  {
                     toolflag = 1024;
                     flag |= 32;
                     Statusbar->newtitle2 (" UnCompress File(s)");
                  }
                  break;

               case PBeleven:

                  if (!(toolflag & 2048))
                  {
                     toolflag = 2048;
                     flag |= 32;
                     Statusbar->newtitle2 (" Set Current Directory as Destination");
                  }
                  break;

               case PBtwelve:

                  if (!(toolflag & 4096))
                  {
                     toolflag = 4096;
                     flag |= 32;
                     Statusbar->newtitle2 (" Swap Source and Destination Directories");
                  }
                  break;

               case PBthirteen:

                  if (!(toolflag & 8192))
                  {
                     toolflag = 8192;
                     flag |= 32;
                     Statusbar->newtitle2 (" Edit File");
                  }
                  break;

               case PBfourteen:

                  if (!(toolflag & 16384))
                  {
                     toolflag = 16384;
                     flag |= 32;
                     Statusbar->newtitle2 (" Append Files");
                  }
                  break;

               case PBfifteen:

                  if (!(toolflag & 32768))
                  {
                     toolflag = 32768;
                     flag |= 32;
                     Statusbar->newtitle2 (" Search for files or text using Search Tool");
                  }
                  break;

               case PBsixteen:

                  if (!(toolflag & 65536))
                  {
                     toolflag = 65536;
                     flag |= 32;
                     Statusbar->newtitle2 (" Refresh Tree and File Displays");
                  }
                  break;

               case PBseventeen:

                  if (!(toolflag & 131072))
                  {
                     toolflag = 131072;
                     flag |= 32;
                     Statusbar->newtitle2 (" Move Divider to the Right");
                  }
                  break;

               case PBeighteen:

                  if (!(toolflag & 262144))
                  {
                     toolflag = 262144;
                     flag |= 32;
                     Statusbar->newtitle2 (" Move Divider to the Left");
                  }
                  break;

               default:

                  if (flag & 32)
                  {
                     Statusbar->newtitle2 ((PSZ) szSource);
                     toolflag = 0;
                     drivebarflag = 0;
                     flag &= 223;
                  }
            }
            break;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

      case WM_HELP:
         DosBeep(2000,50);
         break;
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      case WM_PAINT:
           //------------------------------------------------------
              // If WinBeginPaint and WinEndPaint are not processed here,
              // make sure this routine breaks to the default 'WinDefWindowProc'
              // otherwise parts of the window will not be functional.  The default
              // procedure calls WinBeginPaint and WinEndPaint itself.
              //------------------------------------------------------
            break;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
         case WM_SIZE:

            // This code will dynamically re-configure the toolbar (not including the drivebar)
            // to be either one or two rows (if it is a horizontal toolbar), or one or two columns
            // (if it is a vertical toolbar), depending on the space available.  This will happen
            // every time a WM_SIZE message is sent.
            // It must wait until all the windows have been created however.

            if (flag&15)
            {
               OrientToolbar ();
            }
            break;
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//         case WM_SYSCOMMAND:
//            DosBeep (1000,50);
//            break;
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      }
   return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
}

//----------------------------------------------------------------------
// This procedure processes the messages found in the QMSG structure "qmsg"
// The name for this procedure is sent to the 'WinRegisterClass' procedure (in stdwind.h)
//----------------------------------------------------------------------
MRESULT EXPENTRY ObjectWindowProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
      switch (msg)
      {
         case WM_CREATE:
            break;

         case WM_DESTROY:
            break;

         case OBJM_REFRESH:

                  // Update the root window titlebar byte count.

                  getFSInfo (CurrDrive);

                  // Reload the root tree container with the source drive tree.

                  // This routine reloads the Tree window and also reopens any open folders
                  // and places the cursor on them.

                  RefillTree (CurrDrive, USEPATH, hwnddircntr, DTree, NULL);

                  // Refill the files container to the original path.

                  DFiles->EmptyContainer ();
                  DFiles->FillContainer (filefilter);
                  fillStatusbar2 (NULL, NULL);

               break;

         // These are User-Defined messages.  They are handled here mainly because this window
         // runs in a separate thread and can do the job without blocking the message queue.

         case OBJM_DROP:

            // Change to the clock pointer.

            WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP,
                                                                              SPTR_WAIT, FALSE));
            // This message handles the CN_DROP code for the Tree Window container.
            // Some variables are required for the drag/drop code.

            char szDir [CCHMAXPATH];
            PDRAGINFO pDragInfo;
            PDRAGITEM pDragItem;
            USHORT usOp;
            USHORT usIndicator, cItems, i;
            ULONG ulBytes;

            PCNRITEM prc;
            PCNRDRAGINFO pdi;
            pdi = (PCNRDRAGINFO) mp2;

            prc = (PCNRITEM) pdi->pRecord;
            pDragInfo = pdi->pDragInfo;

            /* Get access to the DRAGINFO data structure */

            DrgAccessDraginfo (pDragInfo);

            /* Can we accept this drop? */

            // Set up the DropDestOp structure.

            switch (pDragInfo->usOperation)
            {
               // Return DOR_NODROPOP if current operation is link or unknown

               case DO_UNKNOWN:
                  DrgFreeDraginfo (pDragInfo);
                  return (MRFROM2SHORT (DOR_NODROPOP, 0));

               // Our default operation is Move

               case DO_DEFAULT:

                  pDragItem = DrgQueryDragitemPtr (pDragInfo, 0);
                  ulBytes  = DrgQueryStrName (pDragItem->hstrContainerName,
                                                                          sizeof (szDir), szDir);
                  if (!ulBytes)
                    return (MRFROM2SHORT (DOR_NODROPOP, 0));

                  usOp = (USHORT) DO_MOVE;
                  DropDestOp.Operation |= 4;
                  break;

               /* Do the requested specific operation */

               case DO_MOVE:
                  usOp = pDragInfo->usOperation;
                  DropDestOp.Operation |= 4;
                  break;

               case DO_COPY:
                  usOp = pDragInfo->usOperation;
                  DropDestOp.Operation |= 2;
                  break;

               default:
                  DrgFreeDraginfo (pDragInfo);
                  return (MRFROM2SHORT (DOR_NODROPOP, 0));
            }
            usIndicator = DOR_DROP;
            cItems = (USHORT) DrgQueryDragitemCount (pDragInfo);

            // In the ButtonDropProc, all members of the DropDestOp struct are NULL,
            // (the files are being dropped on a Drive letter icon)
            // and can be since all the information they need is gathered in the
            // DragDropDlgProc.  Since the container routine is responding to files
            // being dropped on a subdirectory record in either the files window or the
            // tree window however, we must fill in the
            // Destination path so that it can be displayed (and saved) in the
            // DragDropDlgProc.  The fact that it is no longer NULL will allow the
            // DragDropDlgProc to differentiate between the calling sources.

            // If it is dropped on a blank space on the window, the record will be NULL.

            if (prc)
            {
               strcpy (DropDestOp.szDropName, prc->szFileName);
               strcpy (DropDestOp.szDropPath, prc->szDirPath);

               // Files and Tree containers from any FF process have the same IDs.
               // If either container is being dropped on, it could be dropped
               // on a directory or a file or a blank area in the window.  This bit and the
               // routine below (in the "else") will fix up the destination path in any case.
               // Files are ignored, blank areas must become paths.

               if (SHORT1FROMMP (mp1) == FILESWINDOWID + 2)
               {
                  // If the source is the Files container, chop off the trailing backslash that
                  // was added to the name for appearance sake.
                  // The Tree container doesn't have this backslash added.

                  if ((prc->attrFile & 16) != 16)
                  DropDestOp.szDropPath
                                    [(strlen (prc->szDirPath)) - (strlen (prc->szFileName)) - 1] = 0;
               }
            }
            else
            // It was dropped on a blank space on the window, therefore no record was selected.
            // If it was dropped on the Tree window, treat it as if it were a drive button.
            // If it was dropped on a Files window, steal the path from the title.
            {
               if (SHORT1FROMMP (mp1) == DIRWINDOWID + 2)
                  DropDestOp.szDropPath [0] = 0;
               else
               {
                  PSZ pS;
                  pS = &szFilesTitle [2];
                  if (strlen (pS) < 2)
                     DropDestOp.szDropPath [0] = 0;
                  else
                     strcpy (DropDestOp.szDropPath, pS);
               }
               DropDestOp.szDropName [0] = 0;
            }
            // The following section allows FF to recognize if the package being dropped
            // originates from the current process or from another FF process.  A concurrent
            // process's package must be handled differently since PM must set up some shared
            // memory that both processes can access to exchange information about the package.
            // This allows two or more instances of FF to be running concurrently and Drag/Drop
            // operations to be performed between instances.
            // This is done by requesting that the Source of the operation RENDER the package.
            // The message is intercepted there and is eventually processed as if it were a normal
            // DragDrop request.  Normally, with an internal Drag/Drop operation in a single
            // instance, the target window handles the rendering.
            // Since it is a concurrent FF process sending the package, some of the rules can
            // be ignored, but the code that handles this exchange is found in the
            // SubFilesContProc () under the DM_RENDER case.

            // First, determine if it is an external window handle sending this message.
            // This is done by comparing it to this process's File and Tree Window handles.

            if (pDragInfo->hwndSource != hwndfilescntr && pDragInfo->hwndSource != hwnddircntr)
            {
            // Next, get the QWL_USER window word from this external window.
            // If it is another FF process calling, the value stored there will be "123456".

               ULONG ulFFcalling;
               ulFFcalling = WinQueryWindowULong (pDragInfo->hwndSource, QWL_USER);
               if (ulFFcalling == 123456)
               {
                  PDRAGTRANSFER pDragtransfer;
                  pDragtransfer = DrgAllocDragtransfer (1);

                  pDragtransfer->cb = sizeof (DRAGTRANSFER);
                  pDragtransfer->hwndClient = pDragInfo->hwndSource;//hwndfilescntr;
                  pDragtransfer->usOperation = pDragInfo->usOperation;
                  pDragtransfer->fsReply = NULL;

                  // Now, we should look at each item in turn, but since this is an internal
                  // process, we won't since FF will scan for selected files anyway, and ignores
                  // the list.

                  i = 0;

// Should I ever want to expand the drag/drop functionality, this part would be necessary,
// so leave it here for now.
//            for (i = 0; i < cItems; i++)
//            {
                  pDragItem = DrgQueryDragitemPtr (pDragInfo, i);
                  pDragtransfer->pditem = pDragItem;
                  pDragtransfer->hstrSelectedRMF = pDragItem->hstrRMF;

                  sznamebuffer [0] = CHAR (CurrDrive + 64);
                  sznamebuffer [1] = 58;
                  sznamebuffer [2] = 0;

                  if (DropDestOp.szDropPath [1] != '.')
                  {
                     strcat (sznamebuffer, DropDestOp.szDropPath);
                  }
                  DrgQueryStrName (pDragItem->hstrTargetName, sizeof (szpathbuffer),
                                                                              szpathbuffer);
                  strcat (sznamebuffer, "\\");
                  strcat (sznamebuffer, szpathbuffer);

                  pDragtransfer->hstrRenderToName = DrgAddStrHandle (sznamebuffer);

                  // Note:
                  // To send DM_ENDCONVERSATION, DM_RENDER, or DM_RENDERPREPARE
                  // messages, WinSendMsg cannot be used.  Instead, use
                  // DrgSendTransferMsg.

                  rc = 1;

// These commented messages (DC_PREPARE, DC_ENDCONVERSATION) are only used with
// external processes that are not FF concurrent processes and therefore are not required here
// since all external processes other than FF are blocked.
// Keep them for possible upgrading later.

//+                  if (pDragItem->fsControl == DC_PREPARE)
//+                     rc = (BOOL) DrgSendTransferMsg (pDragInfo->hwndSource,
//+                                          DM_RENDERPREPARE,
//+                                          MPFROMP (pDragtransfer), NULL);

                  if (rc)
                  {
                     rc = (BOOL) DrgSendTransferMsg (pDragInfo->hwndSource, DM_RENDER,
                                          MPFROMP (pDragtransfer), NULL);

//+                     // Our involvement with the source can now be terminated regardless
//+                     // of the outcome of the rendering (if it failed, tough luck! - this
//+                     // applies to outside processes).  Other instances of File Freedom
//+                     // will not return errors.
//+
//+                     if (rc)
//+                        DrgSendTransferMsg (pDragInfo->hwndSource, DM_ENDCONVERSATION,
//+                                          MPFROMLONG (pDragItem->ulItemID),
//+                                          MPFROMLONG (DMFL_TARGETSUCCESSFUL));
//+                     else
//+                        DrgSendTransferMsg (pDragInfo->hwndSource, DM_ENDCONVERSATION,
//+                                          MPFROMLONG (pDragItem->ulItemID),
//+                                          MPFROMLONG (DMFL_TARGETFAIL));
                  }
//            }
                  DrgFreeDragtransfer (pDragtransfer);
               }
            }
            // We can determine here that this is an internal Drag/Drop process

            if (pDragInfo->hwndSource == hwndfilescntr || pDragInfo->hwndSource == hwnddircntr)
            {
               // Save the source window handle since it gets abused in the DragDropDlgProc.

               HWND htemp;
               htemp = pDragInfo->hwndSource;

               // Call up the DragDrop Dialog and get the necessary information about
               // destination and operation.
               // Note: The default operation going into this dialog is MOVE.  This can be
               // changed to COPY via the Control key.  Other key combinations could be
               // implemented to invoke ZIP and UNZIP operations, but presently these must
               // be changed manually in the dialog.

               WinDlgBox (HWND_DESKTOP, hwnd, DragDropDlgProc, NULLHANDLE,
                                                       DDDEST_ID, (PVOID) &DropDestOp);

               // Read the replace files flag set in the DragDropDlgProc.

               USHORT usReplace;
               usReplace = USHORT (DropDestOp.Operation & 1);

               // Now, change the destination drive/directory to the Drop Path.  This will
               // make it easy for Drag/Droppers to check the destination after doing a drop.
               // It is also required to get the proper destination set when dropping between
               // instances.

               // If the operation isn't a CANCEL, change the destination to reflect a new dir.

               if ((DropDestOp.Operation & 32) == 0)
               {
                  fillStatusbar1 (DropDrive, (PSZ) DropDestOp.szDropPath);
               }

               // Pick out the operation to perform on the files selected
               // NOTE:
               //       For all of the following operations, the destination drive has already
               //       been tested and found to be ready.  This was done in CN_DROP which
               //       in turn calls OBJM_DROP which in turn sends any DM_RENDER calls.

               if (DropDestOp.Operation & 2)
               {
                  // Call the copy routine that is shared with the standard CopyDlgProc.

                  DoCopy (DropDestOp.szDropName, DropDestOp.szDropPath,
                              usReplace,  DropDestOp.ccPathname, COPY, htemp);
               }
               else if (DropDestOp.Operation & 4)
               {
                  // Call the MOVE routine that is shared with the MoveDlgProc

                  DoCopy (DropDestOp.szDropName, DropDestOp.szDropPath,
                             usReplace,  DropDestOp.ccPathname, MOVE, htemp);
               }
               else if (DropDestOp.Operation & 8)
               {
                  // Call the ZipName dialog

                  WinDlgBox (HWND_DESKTOP, hwnd, ZipnameDlgProc, NULLHANDLE,
                                                                         INPUTZIPNAMEID, NULL);
                   return (MRFROM2SHORT (usIndicator, usOp));
               }
               else if (DropDestOp.Operation & 16)
               {
                  // Call the Unzip routine

                  DoUnzip ((SHORT) MID_UNZIP, hwnd);

                  return (MRFROM2SHORT (usIndicator, usOp));
               }

            else if (DropDestOp.Operation & 128)
            {
               // Call the Append dialog

               WinDlgBox (HWND_DESKTOP, hwnd, AppendDlgProc, NULLHANDLE,
                                                                APPENDID, NULL);
               return (MRFROM2SHORT (usIndicator, usOp));
            }

               else if (DropDestOp.Operation & 32)
               {
                  // The user has pressed the cancel button, so Cancel this call.

                  return (MRFROM2SHORT (usIndicator, usOp));
               }

               return (MRFROM2SHORT (usIndicator, usOp));
            }
            // Deallocate the memory that was set up to hold the string handles for both
            // Draginfo and Dragitem structures associated with a specific DragInfo and
            // invalidate the handles.

            DrgDeleteDraginfoStrHandles (pDragInfo);

            /* Release the draginfo data structure */

            DrgFreeDraginfo (pDragInfo);
            return FALSE;
      }
   return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
}

//--------------------------------------------------------
// This is the dialog procedure for the file copy dialog.
//--------------------------------------------------------
MRESULT EXPENTRY CopyDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   switch (msg)
   {
      case WM_ACTIVATE:
         WinAssociateHelpInstance (hwndhelp, hwnd);
         break;

      case WM_CLOSE:
         WinAssociateHelpInstance (hwndhelp, hwndframe);
         break;

      case WM_HELP:
         WinSendMsg (hwndhelp, HM_DISPLAY_HELP, MPFROMSHORT (2110),
                                                                    MPFROMSHORT (HM_RESOURCEID));
         return 0;

      case WM_INITDLG:

         // Make sure the Drag/Drop record won't be triggered since the DoCopyOp dialog checks
         // for it and the dialog is used both by this routine and the DragDrop routine.

         DropDestOp.recundermouse = 0;

         HWND hEntryfield;

         // Set the possible length of the strings in the entry boxes.
         // Load the required text into the entry fields.
         // Pass a 'END' char to the path entry field so that the end of the path is visible.

         sznamebuffer [0] = 0;
         strcpy (sznamebuffer, szDestPath);

         hEntryfield = WinWindowFromID (hwnd, COPYTOPATHID);
         WinSendDlgItemMsg (hwnd, COPYTOPATHID, EM_SETTEXTLIMIT,
                                                             MPFROMSHORT (CCHMAXPATH + 1), 0);
         WinSetWindowText (hEntryfield, sznamebuffer);
         WinSendMsg (hEntryfield, WM_CHAR, MPFROMSH2CH
                                       (KC_VIRTUALKEY, 0, 0), MPFROM2SHORT (0, VK_END));

         hEntryfield = WinWindowFromID (hwnd, COPYTOFILENAMEID);
         WinSendDlgItemMsg (hwnd, COPYTOFILENAMEID, EM_SETTEXTLIMIT,
                                                              MPFROMSHORT (CCHMAXPATH + 1), 0);
         WinSetWindowText (hEntryfield, "*");

         // Change the focus to the entry field so as to keep the default button from changing.
         // Must return TRUE for the change to take effect.

         hEntryfield = WinWindowFromID (hwnd, COPYTOFILENAMEID);
         WinSetFocus (HWND_DESKTOP, hEntryfield);
         return (PVOID) TRUE;

      case WM_COMMAND:

         switch (SHORT1FROMMP (mp1))
         {
            case COPYOKID:

               // Get the text from the entry fields which are limited to 'CCHMAXPATH + 1' each.
               // 'szpathbuffer' is double length so that oversize path and filename concatenations
               // can be made without overwriting the reserved memory.  The "ERROR_FILENAME_
               // EXCED_RANGE" error will then trap the mistake when a copy is attempted.

               ccName = (USHORT) WinQueryDlgItemText (hwnd, COPYTOFILENAMEID,
                                               CCHMAXPATH + 1, sznamebuffer);

               ccPath = (USHORT) WinQueryDlgItemText (hwnd, COPYTOPATHID,
                                               CCHMAXPATH + 1, szpathbuffer);

              // Examine the 'Replace Existing Files' check box in the dialog to find out
              // if the files are to be overwritten if they already exist.

               hwndtemp = WinWindowFromID (hwnd, COPYREPLACEID);
               static USHORT replacestate;
               replacestate = (USHORT) WinSendMsg (hwndtemp, BM_QUERYCHECK, NULL, NULL);

               // Dismiss the dialog now because in the thread, error messages will be sent.
               // to the frame window.

               WinDismissDlg (hwnd, TRUE);

               DoCopy (sznamebuffer, szpathbuffer, replacestate, ccPath, COPY, hwndfocus);

               break;

            case COPYCANCELID:

               WinDismissDlg (hwnd, TRUE);
               break;

            // Don't break after any command that will require the dialog box to persist.
            // If the WinDefDlgProc receives any WM_COMMAND messages it will automatically
            // dismiss the dialog box.

            case COPYHELPID:
               WinSendMsg (hwndhelp, HM_DISPLAY_HELP, MPFROMSHORT (2110),
                                                                    MPFROMSHORT (HM_RESOURCEID));

               return 0;

            case COPYDEFAULTID:

               // The default pushbutton will pass the focus to the OK pushbutton when pressed.
               // It is set as the default so as to give the user a chance to pause before
               // pressing OK.  This happens automatically when ENTER is pressed in an
               // entry field.

               hEntryfield = WinWindowFromID (hwnd, COPYOKID);
               WinSetFocus (HWND_DESKTOP, hEntryfield);
               return 0;
         }
         break;

   }
   return WinDefDlgProc (hwnd, msg, mp1, mp2);
}
//----------------------------------------------------------------------
// This is the dialog procedure for the Delete Files Dialog.
//----------------------------------------------------------------------
MRESULT EXPENTRY DeleteDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   switch (msg)
   {
      // The next three messages are used mainly to set up the help system for the dialog.

      case WM_ACTIVATE:
         WinAssociateHelpInstance (hwndhelp, hwnd);
         break;

      case WM_CLOSE:
         WinAssociateHelpInstance (hwndhelp, hwndframe);
         break;

      case WM_HELP:
         WinSendMsg (hwndhelp, HM_DISPLAY_HELP, MPFROMSHORT (2130),
                                                                    MPFROMSHORT (HM_RESOURCEID));
         break;

      case WM_INITDLG:

         HWND hwndlistbox;
         hwndlistbox = WinWindowFromID (hwnd, DELETELISTID);

         SHORT Rindex;
         pWhatRec = (PCNRITEM) CMA_FIRST;

         // Scan for the selected records and then insert them into the listbox.  Attach the
         // Record pointer to each item as a handle so that it can be recovered later.

         while (pWhatRec)
         {
            pWhatRec = (PCNRITEM) WinSendMsg (hwndfocus,
                                  CM_QUERYRECORDEMPHASIS, MPFROMP (pWhatRec),
                                                         MPFROMSHORT (CRA_SELECTED));
            if (pWhatRec)
            {
               Rindex = (SHORT) WinSendMsg (hwndlistbox, LM_INSERTITEM,
                      MPFROMSHORT (LIT_SORTASCENDING), MPFROMP (pWhatRec->pszFileName));
               WinSendMsg (hwndlistbox, LM_SETITEMHANDLE, MPFROMSHORT (Rindex),
                                                                       MPFROMLONG (pWhatRec));
            }
         }
      return 0;

      case WM_COMMAND:

         switch (SHORT1FROMMP (mp1))
         {
            case CANCELDELETEID:

               WinDismissDlg (hwnd, TRUE);
               break;

            case DELETEID:

               // Check to be sure there are items in the listbox.  If there are not, there is
               // nothing to delete so exit with a beep!
               // In the case of subdirectories, they can be deleted from either the Files
               // Window or the Tree Window.

               SHORT cItems;

               hwndlistbox = WinWindowFromID (hwnd, DELETELISTID);
               cItems = (SHORT) WinSendMsg (hwndlistbox, LM_QUERYITEMCOUNT, NULL, NULL);
               if (!cItems)
               {
                  WinDismissDlg (hwnd, TRUE);
                  DosBeep (100,50);
                  return 0;
               }

               PSZ pszFile;

               // This array will have to be deleted if the routine bombs out.

               ULONG *pRecArray;
               pRecArray = new ULONG [cItems];

               // This flag (fRebuild) will indicate if a subdirectory has been deleted.  This will
               // necessitate a rebuild of the folder before exiting the function in order
               // to update the display.  'CM_REMOVERECORD' and 'CM_INVALIDATERECORD'
               // alone did not prove reliable as far as keeping the container integrity current.
               // The folder to be rebuilt will be held in 'pOpenRec'.
               // The fFilebuild flag is used to indicate if the files window needs to be rebuilt.
               // This happen if a subdirectory is deleted from the Tree window that also
               // appears in the files window.

               USHORT fRebuild, fFilesbuild;
               fRebuild = 0;
               fFilesbuild =0;

               // Put up the hourglass cursor.

               WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP,
                                                                                 SPTR_WAIT, FALSE));

               // Retrieve the selected record's Record pointers from the listbox
               // and then delete them from the directory and from the container.

               int i;
               int ii;
               ii = 0;
               for (i =0; i < cItems; i++)
               {

                  pWhatRec = (PCNRITEM) WinSendMsg (hwndlistbox, LM_QUERYITEMHANDLE,
                                                                        MPFROMSHORT (i), NULL);
                  if (pWhatRec)
                  {
                     pRecArray [ii] = (ULONG) pWhatRec;

                     // If the record is a subdirectory remove the subdirectory, otherwise
                     // remove the file.

                     pszFile = pWhatRec->szDirPath;
                     if (pWhatRec->attrFile  & 16)
                     {
                        // Set a flag to rebuild the Tree window since it is a directory
                        // being deleted.  This may or may not be a visible directory, but
                        // it's easier to just rebuild it anyway.

                        fRebuild = 1;
// NOTE:
// DosDeteteDir and DosDelete do not need the filename or subdir name surrounded in quotes
// if they contain embedded spaces.  They will handle any name.

                        rc = DosDeleteDir (pszFile);

                        // If the subdirectory is not empty, this error will trap it and a message
                        // box is presented that will query the user if they want all files etc.
                        // in the directory removed.

                        if (rc == ERROR_ACCESS_DENIED)
                        {
                           CHAR *pszrec;
                           pszrec = new CHAR [CCHMAXPATH * 2 +2];

                           // Copy the filename into a buffer but trim off the trailing backslash
                           // and enclose it in quotes.

                           strcpy (pszrec, "\"");
                           strcat (pszrec, pWhatRec->szFileName);

                           // If it's the files container calling, remove the extra backslash
                           // from the directory name.

                           if (hwndfocus == hwndfilescntr)
                              pszrec [strlen (pWhatRec->szFileName)] = 0;
                           strcat (pszrec, "\"");
                           strcat (pszrec, "\nThis Directory contains Files\n and/or other Directories\n"
                                       "which may also contain Files.\n\n"
                                       "Do you wish to delete this directory and all of it's contents?");
                           USHORT usResponse;
                           usResponse = (USHORT) WinMessageBox (HWND_DESKTOP, hwnd,
                                           pszrec, "WARNING!", MESSAGEBOXID,
                                           MB_OKCANCEL | MB_WARNING | MB_DEFBUTTON2);
                           delete pszrec;
                           if (usResponse == MBID_OK)
                           {
                              // Put up the hourglass cursor.

                              WinSetPointer (HWND_DESKTOP, WinQuerySysPointer
                                                  (HWND_DESKTOP, SPTR_WAIT, FALSE));

                              DeleteTree (pWhatRec->szDirPath);
                              rc = DosDeleteDir (pszFile);
                           }
                           else
                           {
                              // Break out of the loop for this item only.
                              break;
                           }
                        }
                        // If a subdirectory has been deleted, it is more complicated.
                        // Check to find if the new pathname matches the destination pathname,
                        // or, if the originating window was the Tree container, check also
                        // to see if the pathname matches the Files container title.
                        // If so, change these pathnames to a viable path and refill the
                        // necessary windows and status information.

                        if (!rc)
                        {
                           // Check to find if the deleted tree pathname matches
                           // in part the destination pathname.
                           // If so, change the destination pathname to the remaining
                           // viable stub.

                           sznamebuffer [0] = CHAR (CurrDrive + 64);
                           sznamebuffer [1] = 58;
                           sznamebuffer [2] = 0;
                           strcat (sznamebuffer, pWhatRec->szDirPath);

                           int pathlen;
                           pathlen = strlen (sznamebuffer);
                           SHORT match;
                           match = (SHORT) strnicmp (sznamebuffer, szDestPath, pathlen);
                           if (match == 0)
                           {
                              // Forshorten the destination path to the length of the
                              // matching directory name.

                              szDestPath [pathlen] = 0;

                              // Find where the viable directory name begins (foreshorten
                              // the path one more level)
                              // and use this name as the new directory name (pDirname).

                              int stub, findbackslash;
                              stub = 0;
                              for (findbackslash = 0; findbackslash < pathlen; findbackslash ++)
                              {
                                 if (szDestPath [findbackslash] == 92) stub = findbackslash;
                              }
                              szDestPath [stub] = 0;

                              fillStatusbar1 (DestDrive, (PSZ) szDestPath);
                           }
                           // Check to find if the deleted tree pathname matches
                           // in part the Files Window (Source) pathname.
                           // If so, change the Source to the remaining
                           // viable stub.

                           match = (SHORT) strnicmp (sznamebuffer, szFilesTitle, pathlen);
                           if (match == 0)
                           {
                              // Forshorten the destination path to the length of the
                              // matching directory name.

                              sznamebuffer [pathlen] = 0;

                              // Find where the viable directory name begins (foreshorten
                              // the path one more level)
                              // and use this name as the new directory name (pDirname).

                              int stub, findbackslash;
                              stub = 0;
                              for (findbackslash = 0; findbackslash < pathlen; findbackslash ++)
                              {
                                 if (sznamebuffer [findbackslash] == 92) stub = findbackslash;
                              }
                              sznamebuffer [stub] = 0;

                              // Refill the files container with this foreshortened path
                              // and rebuild the Files Window title and the Source
                              // status bar.

                              PSZ ppath;

                              // If the pathname is more than "X:", set up a pointer to
                              // the abbreviated version.

                              if (sznamebuffer [1] == 58) ppath = &sznamebuffer [2];
                              else ppath = &sznamebuffer [0];

                              DFiles->PathToOrder (ppath, (SHORT) CurrDrive);
                              DFiles->FillContainer (filefilter);
                              fillStatusbar2 (NULL, NULL);

                              strcpy (szFilesTitle, sznamebuffer);
                              SecondChild->newtitle ((PSZ) szFilesTitle);
                           }
                           // If the subdir path was not part of the Source path, maybe the
                           // Source path is the parent of the Subdir.
                           else
                           {
                              // Find where the viable directory name begins (foreshorten
                              // the path one more level)
                              // and use this name as the new directory name (pDirname).

                              int stub, findbackslash;
                              stub = 0;
                              for (findbackslash = 0; findbackslash < pathlen; findbackslash ++)
                              {
                                 if (sznamebuffer [findbackslash] == 92) stub = findbackslash;
                              }
                              sznamebuffer [stub] = 0;

                              // Get the lenght of the forshortened path and compare it to the length of the
                              // source pathname.

                              pathlen = strlen (sznamebuffer);

                              match = (SHORT) strnicmp (sznamebuffer, szFilesTitle, pathlen);
                              if (match == 0)
                              {
                                 fFilesbuild = 1;

                              }
                           }
                        }
                     }
                     else rc = DosDelete (pszFile);

                     // If there was an error, take the error number to the MessageBox routine
                     // for display of the error.  After display, return to the dialog.

                     ii++;
                     if (rc)
                     {
                        MessageBox (hwnd, rc);
                        pWhatRec = 0;
                        ii--;
                        break;
                     }
                  }
                  WinSendMsg (hwndfocus, CM_SETRECORDEMPHASIS,
                                 MPFROMLONG (pWhatRec),
                                 MPFROM2SHORT (FALSE, CRA_SELECTED));

                  if (!pWhatRec) i = cItems;
               }
               // Remove the records from the container and invalidate the display if
               // there have been deletions.

               if (ii) WinSendMsg (hwndfocus, CM_REMOVERECORD, MPFROMP (pRecArray),
                                        MPFROM2SHORT (ii, CMA_FREE | CMA_INVALIDATE));

               // If a subdirectory was removed, this rebuild is necessary to update the display.

               if (fRebuild)
               {
                  // This routine reloads the Tree window and also reopens any open folders
                  // and places the cursor on them.

                  RefillTree (CurrDrive, USEPATH, hwnddircntr, DTree, NULL);
               }
               // Update the root window titlebar byte count.

               getFSInfo (CurrDrive);

               // Update the Destination status bar info with new bytes free etc.

               fillStatusbar1 (DestDrive, (PSZ) szDestPath);

               if (fFilesbuild)
               {
                  // Save the filename so that we can returnto () it below.

                  pWhatRec = (PCNRITEM) WinSendMsg (hwndfilescntr,
                                        CM_QUERYRECORDEMPHASIS, MPFROMP (CMA_FIRST),
                                                              MPFROMSHORT (CRA_CURSORED));
                  // Protection is needed here in case the drive or subdir has disappeared.
                  // In this case no file will be found with the cursor on it or otherwise.

                  if (pWhatRec)
                     strcpy (tparms7.szSavename, pWhatRec->szFileName);

                  // Refresh the source and destination containers, info, etc.
                  // Do not use threads here since it is uncertain when the user will
                  // preform a refresh and the usual designated threads could still be running.

                  // Put up the hourglass pointer.

                  WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP,
                                                              SPTR_WAIT, FALSE));
                  // Refill the files container to the original path.

                  DFiles->EmptyContainer ();
                  DFiles->FillContainer (filefilter);
                  fillStatusbar2 (NULL, NULL);

                  // Use the returnto thread to move the cursor back on to the last file it was on.

                  tparms7.Hab = NULL;
                  tparms7.Hmq = NULL;
                  tparms7.hwnd = hwndfilescntr;
                  tparms7.job = RETURNTO;
                  tid7 = _beginthread (threadall, NULL, 8192, &tparms7);
               }
               delete pRecArray;

               // Put up the regular cursor.

               WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP,
                                                                                 SPTR_ARROW, FALSE));
               WinDismissDlg (hwnd, TRUE);
               break;
         }

      case WM_CONTROL:
         switch (SHORT2FROMMP (mp1))
         {
            case LN_ENTER:

               // Remove a filename from the list in the list box.  Also unselect the
               // record from the files container.

               SHORT index;
               ULONG pRec;

               hwndlistbox = WinWindowFromID (hwnd, DELETELISTID);
               index = (SHORT) WinSendMsg (hwndlistbox, LM_QUERYSELECTION, NULL, NULL);
               if (index != LIT_NONE)
               {
                  pRec = (ULONG) WinSendMsg (hwndlistbox, LM_QUERYITEMHANDLE,
                                                               MPFROMSHORT (index), NULL);
                  WinSendMsg (hwndfocus, CM_SETRECORDEMPHASIS, MPFROMLONG (pRec),
                                                MPFROM2SHORT (FALSE, CRA_SELECTED));
                  WinSendMsg (hwndlistbox, LM_DELETEITEM, MPFROMSHORT (index), NULL);
                  return 0;
               }
         }
         break;
   }
   return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

//--------------------------------------------------------------
// Create Directory procedure
//--------------------------------------------------------------
MRESULT EXPENTRY CreateDirDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   switch (msg)
   {
      // The next three messages are used mainly to set up the help system for the dialog.

      case WM_ACTIVATE:
         WinAssociateHelpInstance (hwndhelp, hwnd);
         break;

      case WM_CLOSE:
         WinAssociateHelpInstance (hwndhelp, hwndframe);
         break;

      case WM_HELP:
         WinSendMsg (hwndhelp, HM_DISPLAY_HELP, MPFROMSHORT (2210),
                                                                    MPFROMSHORT (HM_RESOURCEID));
         break;

      case WM_INITDLG:

         HWND hEntryfield;

         // Set the possible length of the strings in the entry boxes.

         WinSendDlgItemMsg (hwnd, NEWDIRNAMEID, EM_SETTEXTLIMIT,
                                                             MPFROMSHORT (CCHMAXPATH + 1), 0);

         // Change the focus to the entry field so as to keep the default button from changing.
         // Must return TRUE for the change to take effect.

         hEntryfield = WinWindowFromID (hwnd, NEWDIRNAMEID);
         WinSetFocus (HWND_DESKTOP, hEntryfield);
         return (PVOID) TRUE;

      case WM_COMMAND:

         switch (SHORT1FROMMP (mp1))
         {
            case ACCEPTNAMEID:

               // The default pushbutton will pass the focus to the "Create" pushbutton.
               // It is set as the default so as to give the user a chance to pause before
               // pressing OK.  This happens automatically when ENTER is pressed in an
               // entry field.

               hEntryfield = WinWindowFromID (hwnd, CREATEID);
               WinSetFocus (HWND_DESKTOP, hEntryfield);
               return 0;

            case CREATEID:

               // Tack the new directory name onto the current source path.
               // Note:  DosCreateDir does NOT need directory names surrounded in quotes in
               //        the case of embedded spaces etc.

               szpathbuffer [0] = 0;
               strcpy (szpathbuffer, szFilesTitle);
               WinQueryDlgItemText (hwnd, NEWDIRNAMEID, CCHMAXPATH + 1, sznamebuffer);

               if (strlen (szpathbuffer) != 3) strcat (szpathbuffer, "\\");
               else if (szpathbuffer [1] != ':') strcat (szpathbuffer, "\\");
               strcat (szpathbuffer, sznamebuffer);

               // Create the new directory.

               PSZ pName;
               pName = szpathbuffer;

               rc = DosCreateDir (pName, NULL);

               // No need to return after an error since nothing has been done.

               if (rc)
               {
                  MessageBox (hwnd, rc);
                  return 0;
               }
               // Prepare a string for the Returnto thread.
               // This will place the cursor on the newly created subdirectory.

               strcpy (tparms7.szSavename, sznamebuffer);
               strcat (tparms7.szSavename, "\\");

               // Completely rebuild the root tree.  This can be done without triggering a
               // refreshing of the files container.

               RefillTree (CurrDrive, USEPATH, hwnddircntr, DTree, NULL);

               // Refill the files container

               DFiles->EmptyContainer ();
               DFiles->FillContainer (filefilter);
               fillStatusbar2 (NULL, NULL);

               // Update the root window titlebar byte count.

               getFSInfo (CurrDrive);

               // Use the returnto thread to move the cursor back on to the last file it was on.

               tparms7.Hab = NULL;
               tparms7.Hmq = NULL;
               tparms7.hwnd = hwndfilescntr;
               tparms7.job = RETURNTO;
               tid7 = _beginthread (threadall, NULL, 8192, &tparms7);

               WinDismissDlg (hwnd, TRUE);
               break;

            case CANCELNEWDIRID:

               WinDismissDlg (hwnd, TRUE);
               break;
         }
   }
   return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

MRESULT EXPENTRY RenameDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   switch (msg)
   {
      // The next three messages are used mainly to set up the help system for the dialog.

      case WM_ACTIVATE:
         WinAssociateHelpInstance (hwndhelp, hwnd);
         break;

      case WM_CLOSE:
         WinAssociateHelpInstance (hwndhelp, hwndframe);
         break;

      case WM_HELP:
         WinSendMsg (hwndhelp, HM_DISPLAY_HELP, MPFROMSHORT (2140),
                                                                    MPFROMSHORT (HM_RESOURCEID));
         break;

      case WM_INITDLG:

         HWND hEntryfield, hRenamefield;

         // Set the possible length of the strings in the entry boxes.

         hRenamefield = WinWindowFromID (hwnd, ENTERRENAMEID);
         WinSendDlgItemMsg (hwnd, ENTERRENAMEID, EM_SETTEXTLIMIT,
                                                             MPFROMSHORT (CCHMAXPATH + 1), 0);

         // Change the focus to the entry field so as to keep the default button from changing.
         // Must return TRUE for the change to take effect.

         WinSetFocus (HWND_DESKTOP, hRenamefield);

         hEntryfield = WinWindowFromID (hwnd, OLDNAMEID);
         WinSendDlgItemMsg (hwnd, OLDNAMEID, EM_SETTEXTLIMIT,
                                                             MPFROMSHORT (CCHMAXPATH + 1), 0);

         // Find the record in the files or tree container that has been selected.

         pcnrTemp = (PCNRITEM) WinSendMsg (hwndfocus,
                                        CM_QUERYRECORDEMPHASIS, MPFROMP (CMA_FIRST),
                                                               MPFROMSHORT (CRA_SELECTED));
         // Check for a valid record.

         if (pcnrTemp)
         {
            // If it is a subdirectory from the files container, cut off the trailing backslash
            // on the original name.

            strcpy (szpathbuffer, pcnrTemp->szFileName);
            SHORT namelen;
            namelen = (SHORT) strlen (pcnrTemp->szFileName);

            // Stick the name in the 'Current Name' entry box and also in the Rename entry field
            // box since usually the original name is just edited for the new name.

            if (pcnrTemp->attrFile & 16)
            {
               if (hwndfocus == hwndfilescntr) szpathbuffer [namelen - 1] = 0;
            }
            WinSetWindowText (hEntryfield, szpathbuffer);
            WinSetWindowText (hRenamefield, szpathbuffer);
         }

         // Must return TRUE for focus change to take effect.

         return (PVOID) TRUE;

      case WM_COMMAND:

         switch (SHORT1FROMMP (mp1))
         {
            case ACCEPTRENAMEID:

               // The default pushbutton will pass the focus to the "OK" pushbutton.
               // It is set as the default so as to give the user a chance to pause before
               // pressing OK.  This happens automatically when ENTER is pressed in an
               // entry field.

               hEntryfield = WinWindowFromID (hwnd, RENAMEOKID);
               WinSetFocus (HWND_DESKTOP, hEntryfield);
               return 0;

            case RENAMEOKID:

               // If a record was not selected, just exit.

               if (!pcnrTemp) break;

               PSZ pszOldname, pszNewname;
               HWND hFocus;

               // Save the focus handle locally, since other calls used below will corrupt the storage.

               hFocus = hwndfocus;

               // Retrieve the new name from the entry field.  This buffer will also be used
               // to locate the cursor over the new file.

               WinQueryDlgItemText (hwnd, ENTERRENAMEID, CCHMAXPATH + 1, sznamebuffer);

               // Filter the new and old names through the DosEditName filter to handle the
               // wildcards and produce a new filename.

               static CHAR szEditbuffer [CCHMAXPATH + 1];

               ULONG pathlen;

               WinQueryDlgItemText (hwnd, OLDNAMEID, CCHMAXPATH + 1, szpathbuffer);

               DosEditName (1, szpathbuffer, sznamebuffer, szEditbuffer, CCHMAXPATH +1);

               // Set up these strings now.  They are used as is or modified below.
               // The original pathname must be saved to be used to check against the
               // Files window title and the Destination pathname in order to keep them
               // current.

               szpathbuffer [0] = CHAR (CurrDrive + 64);
               szpathbuffer [1] = 58;
               szpathbuffer [2] = 0;
               strcat (szpathbuffer, pcnrTemp->szDirPath);
               strcpy (sznamebuffer, szpathbuffer);

               // Tack the new name onto the path and send it to DosMove which also renames.
               // Remember that DirPath holds the fully qualified pathname without any
               // trailing backslashs.
               // If the focus window was the Files container, the record could be a file or
               // a directory.

               if (hFocus == hwndfilescntr)
               {
                  strcpy (szpathbuffer, szFilesTitle);
                  if (strlen (szpathbuffer) != 3) strcat (szpathbuffer, "\\");
                  else if (szpathbuffer [1] != ':') strcat (szpathbuffer, "\\");
                  strcat (szpathbuffer, szEditbuffer);
               }
               else  // If it's the Tree container, it can only be a directory.
               {
                  int stub, findbackslash;
                  stub = 0;
                  pathlen = strlen (szpathbuffer);
                  for (findbackslash = 0; findbackslash < pathlen; findbackslash ++)
                  {
                     if (szpathbuffer [findbackslash] == 92) stub = findbackslash;
                  }
                  szpathbuffer [stub] = 0;
                  strcat (szpathbuffer, "\\");
                  strcat (szpathbuffer, szEditbuffer);
               }
               pszNewname = &szpathbuffer [2];
               pszOldname = pcnrTemp->szDirPath;

               // Rename the file or Directory.

               rc = DosMove (pszOldname, pszNewname);

               // If there was an error, no need to return to the dialog since nothing was done.

               if (rc)
               {
                  MessageBox (hwnd, rc);
                  return 0;
               }

               // If the renameing was successful, the display will have to be updated.
               // Test to see if it is a subdirectory or just a file.

               if (pcnrTemp->attrFile & 16)
               {
                  // If it is a subdirectory name, it is more complicated.
                  // Check to find if the original pathname matches a leading part of
                  // the destination pathname.
                  // If so, change the destination pathname also.

                  PSZ pszpathbuffer, psznamebuffer;
                  pszpathbuffer = new CHAR [CCHMAXPATH *2];
                  psznamebuffer = new CHAR [CCHMAXPATH *2];
                  SHORT match;

                  // Copy szpathbuffer and sznamebuffer to the new strings since there must
                  // be some local storage that won't be corrupted in the routines called to
                  // rework the status bars etc.

                  strcpy (pszpathbuffer, szpathbuffer);
                  strcpy (psznamebuffer, sznamebuffer);
                  pathlen = strlen (sznamebuffer);
                  match = (SHORT) strnicmp (sznamebuffer, szDestPath, pathlen);
                  if (match == 0)
                  {
                     // The pathname of the record partially matches the Destination path.

                     PSZ pTail;
                     pTail = &szDestPath [pathlen];
                     int stub;
                     stub = 0;
                     for (i = 0; i < pathlen; i++)
                     {
                        if (sznamebuffer [i] == 92) stub = i;
                     }
                     if (stub > 2) sznamebuffer [stub + 1] = 0;
                     strcat (sznamebuffer, szEditbuffer);
                     strcat (sznamebuffer, pTail);

                     // Copy the path into the destination status bar string and update info.

                     strcpy (szDestPath, sznamebuffer);
                     fillStatusbar1 (DestDrive, (PSZ) szDestPath);
                  }
                  // If the directory was in the Files container, the files window will have to be
                  // updated to show the name change.

                  if (hFocus == hwndfilescntr)
                  {
                     // Fill the files container and update the source status bar.  Don't use a thread
                     // here because of the returnto () function at the end of this section.

                     DFiles->EmptyContainer ();
                     DFiles->FillContainer (filefilter);

                     // This statusbar information can't be gathered until the container is filled.

                     fillStatusbar2 (NULL, NULL);
                  }
                  // Add a backslash to the newname so that the returnto () function can
                  // find a match (below).

                  strcat (szEditbuffer, "\\");

                  // Only if the focus window is the Tree window, does this need to be done
                  // since the Files window pathnames are checked above.

                  if (hFocus == hwnddircntr)
                  {
                     // Check to find if the renamed directory pathname matches
                     // in part the Files Window (Source) pathname.
                     // If so, change the Source to the remaining
                     // viable stub.

                     pathlen = strlen (psznamebuffer);
                     match = (SHORT) strnicmp (psznamebuffer, szFilesTitle, pathlen);
                     if (match == 0)
                     {
                        // Forshorten the destination path to the length of the
                        // matching directory name.

                        PSZ pNametail;
                        pNametail = &szFilesTitle [pathlen];
                        strcat (pszpathbuffer, pNametail);
                        strcpy (szFilesTitle, pszpathbuffer);

                        pNametail = &szFilesTitle [2];
                        DFiles->PathToOrder (pNametail, (SHORT) CurrDrive);
                        DFiles->FillContainer (filefilter);

                        SecondChild->newtitle ((PSZ) szFilesTitle);
                     }
                  }
                  // Completely rebuild the root tree.

                  RefillTree (CurrDrive, USEPATH, hwnddircntr, DTree, NULL);


                  delete pszpathbuffer;
                  delete psznamebuffer;
               }

               // If it is not a subdirectory name (just a file name) rebuild the files container only.
               // If it is a subdirectory, the files container is refreshed in the CM_EXPANDTREE
               // message above or the (!pOpenrec) section.

               else
               {
                  DFiles->EmptyContainer ();

                  // Fill the files container and update the source status bar.  Don't use a thread
                  // here because of the returnto () function at the end of this section.

                  DFiles->FillContainer (filefilter);

                  // This statusbar information can't be gathered until the container is filled.

                  fillStatusbar2 (NULL, NULL);
               }
               // Find the file and put the cursor on it.

                  strcpy (tparms7.szSavename, szEditbuffer);

                  tparms7.Hab = NULL;
                  tparms7.Hmq = NULL;
                  tparms7.hwnd = hwndfilescntr;
                  tparms7.job = RETURNTO;
                  tid7 = _beginthread (threadall, NULL, 8192, &tparms7);

               WinDismissDlg (hwnd, TRUE);
               break;

            case RENAMECANCELID:

               WinDismissDlg (hwnd, TRUE);
               break;
         }
      }

   return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

//--------------------------------------------
// Move Files dialog procedure
//--------------------------------------------
MRESULT EXPENTRY MoveDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   switch (msg)
   {
      // The next three messages are used mainly to set up the help system for the dialog.

      case WM_ACTIVATE:
         WinAssociateHelpInstance (hwndhelp, hwnd);
         break;

      case WM_CLOSE:
         WinAssociateHelpInstance (hwndhelp, hwndframe);
         break;

      case WM_HELP:
         WinSendMsg (hwndhelp, HM_DISPLAY_HELP, MPFROMSHORT (2120),
                                                                    MPFROMSHORT (HM_RESOURCEID));
         break;

      case WM_INITDLG:

         HWND hEntryfield;

         // Set the possible length of the strings in the entry boxes.
         // Load the required text into the entry fields.
         // Pass a 'END' char to the path entry field so that the end of the path is visible.

         sznamebuffer [0] = 0;
         strcpy (sznamebuffer, szDestPath);

         hEntryfield = WinWindowFromID (hwnd, MOVEDESTID);
         WinSendDlgItemMsg (hwnd, MOVEDESTID, EM_SETTEXTLIMIT,
                                                             MPFROMSHORT (CCHMAXPATH + 1), 0);
         WinSetWindowText (hEntryfield, sznamebuffer);
         WinSendMsg (hEntryfield, WM_CHAR, MPFROMSH2CH
                                       (KC_VIRTUALKEY, 0, 0), MPFROM2SHORT (0, VK_END));

         HWND hwndlistbox;
         hwndlistbox = WinWindowFromID (hwnd, MOVELISTID);

         SHORT Rindex;
         pWhatRec = (PCNRITEM) CMA_FIRST;

         // Scan for the selected records and then insert them into the listbox.  Attach the
         // Record pointer to each item as a handle so that it can be recovered later.

         while (pWhatRec)
         {
            pWhatRec = (PCNRITEM) WinSendMsg (hwndfilescntr,
                                  CM_QUERYRECORDEMPHASIS, MPFROMP (pWhatRec),
                                                         MPFROMSHORT (CRA_SELECTED));
            if (pWhatRec)
            {
               Rindex = (SHORT) WinSendMsg (hwndlistbox, LM_INSERTITEM,
                      MPFROMSHORT (LIT_SORTASCENDING), MPFROMP (pWhatRec->pszFileName));
               WinSendMsg (hwndlistbox, LM_SETITEMHANDLE, MPFROMSHORT (Rindex),
                                                                       MPFROMLONG (pWhatRec));
            }
         }
      return 0;

      case WM_COMMAND:

         switch (SHORT1FROMMP (mp1))
         {
            case CANCELMOVEID:

               WinDismissDlg (hwnd, TRUE);
               break;

            case MOVEID:

               // Check to be sure there are items in the listbox.  If there are not, there is
               // nothing to move so exit with a beep!

               SHORT cItems;

               hwndlistbox = WinWindowFromID (hwnd, MOVELISTID);
               cItems = (SHORT) WinSendMsg (hwndlistbox, LM_QUERYITEMCOUNT, NULL, NULL);
               if (!cItems)
               {
                  WinDismissDlg (hwnd, TRUE);
                  DosBeep (100,50);
                  return 0;
               }

               ccPath = (USHORT) WinQueryDlgItemText (hwnd, MOVEDESTID,
                                               CCHMAXPATH + 1, szpathbuffer);

               HWND hwndcheckbox;

               // Examine the 'Replace Existing Files' check box in the dialog to find out
               // if the files are to be overwritten if they already exist.

               hwndcheckbox = WinWindowFromID (hwnd, FORCEDMOVEID);
               USHORT replacestate = (USHORT) WinSendMsg (hwndcheckbox,
                                                            BM_QUERYCHECK, NULL, NULL);
               // Make sznamebuffer empty, since it won't be used in the Move mode
               // of the DoCopyOp routine.

               sznamebuffer [0] = 0;

               DoCopy (sznamebuffer, szpathbuffer, replacestate, ccPath, MOVE, hwndfocus);

               WinDismissDlg (hwnd, TRUE);
               break;
         }

      case WM_CONTROL:
         switch (SHORT2FROMMP (mp1))
         {
            case LN_ENTER:

               // Remove a filename from the list in the list box.  Also unselect the
               // record from the files container.

               SHORT index;
               ULONG pRec;

               hwndlistbox = WinWindowFromID (hwnd, MOVELISTID);
               index = (SHORT) WinSendMsg (hwndlistbox, LM_QUERYSELECTION, NULL, NULL);
               if (index != LIT_NONE)
               {
                  pRec = (ULONG) WinSendMsg (hwndlistbox, LM_QUERYITEMHANDLE,
                                                               MPFROMSHORT (index), NULL);
                  WinSendMsg (hwndfilescntr, CM_SETRECORDEMPHASIS, MPFROMLONG (pRec),
                                                MPFROM2SHORT (FALSE, CRA_SELECTED));
                  WinSendMsg (hwndlistbox, LM_DELETEITEM, MPFROMSHORT (index), NULL);
                  return 0;
               }
         }
         break;
   }
   return WinDefDlgProc (hwnd, msg, mp1, mp2);
}
//------------------------------------------------------------------------
// This dialog allows the user to modify File Attributes
//------------------------------------------------------------------------
MRESULT EXPENTRY AttribDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   switch (msg)
   {
      // The next three messages are used mainly to set up the help system for the dialog.

      case WM_ACTIVATE:
         WinAssociateHelpInstance (hwndhelp, hwnd);
         break;

      case WM_CLOSE:
         WinAssociateHelpInstance (hwndhelp, hwndframe);
         break;

      case WM_HELP:
         WinSendMsg (hwndhelp, HM_DISPLAY_HELP, MPFROMSHORT (2170),
                                                                    MPFROMSHORT (HM_RESOURCEID));
         break;

      case WM_INITDLG:

        static HWND hwndlistbox;
        hwndlistbox = WinWindowFromID (hwnd, ATTRIBFILEID);

         static SHORT Rindex;
         pWhatRec = (PCNRITEM) CMA_FIRST;

         // Scan for the selected records and then insert them into the listbox.  Attach the
         // Record pointer to each item as a handle so that it can be recovered later.
         // Format a string that contains the current attributes as well as the file name to
         // display in the listbox.

         while (pWhatRec)
         {
            pWhatRec = (PCNRITEM) WinSendMsg (hwndfilescntr,
                                  CM_QUERYRECORDEMPHASIS, MPFROMP (pWhatRec),
                                                         MPFROMSHORT (CRA_SELECTED));
            if (pWhatRec)
            {
               strcpy (sznamebuffer, pWhatRec->szAttrString);
               sznamebuffer [4] = 32;
               sznamebuffer [5] = 32;
               sznamebuffer [6] = 0;
               strcat (sznamebuffer, pWhatRec->szFileName);

               Rindex = (SHORT) WinSendMsg (hwndlistbox, LM_INSERTITEM,
                      MPFROMSHORT (LIT_SORTASCENDING), MPFROMP (sznamebuffer));

               WinSendMsg (hwndlistbox, LM_SETITEMHANDLE, MPFROMSHORT (Rindex),
                                                                       MPFROMLONG (pWhatRec));
            }
         }
         static HWND hwndspinbutton;
         static CHAR *ampm [2];
         ampm [0] = "AM";
         ampm [1] = "PM";

         static struct dostime_t dtime;
         static struct dosdate_t ddate;
         _dos_gettime (&dtime);
         _dos_getdate (&ddate);

         hwndspinbutton = WinWindowFromID (hwnd, MONTHID);
         WinSendMsg (hwndspinbutton, SPBM_OVERRIDESETLIMITS, MPFROMLONG (12),
                                                               MPFROMLONG (1));
         WinSendMsg (hwndspinbutton, SPBM_SETCURRENTVALUE, MPFROMLONG (ddate.month),
                                                                               NULL);

         hwndspinbutton = WinWindowFromID (hwnd, DAYID);
         WinSendMsg (hwndspinbutton, SPBM_OVERRIDESETLIMITS, MPFROMLONG (31),
                                                               MPFROMLONG (1));
         WinSendMsg (hwndspinbutton, SPBM_SETCURRENTVALUE, MPFROMLONG (ddate.day),
                                                                                NULL);

         hwndspinbutton = WinWindowFromID (hwnd, YEARID);
         WinSendMsg (hwndspinbutton, SPBM_OVERRIDESETLIMITS, MPFROMLONG (99),
                                                               MPFROMLONG (0));
         static ULONG dtemp;
         if (ddate.year > 1999) dtemp = ddate.year -2000;
         else dtemp = ddate.year -1900;
         WinSendMsg (hwndspinbutton, SPBM_SETCURRENTVALUE, MPFROMLONG (dtemp),
                                                                        NULL);

         hwndspinbutton = WinWindowFromID (hwnd, HOURID);
         WinSendMsg (hwndspinbutton, SPBM_OVERRIDESETLIMITS, MPFROMLONG (12),
                                                               MPFROMLONG (1));
         if (dtime.hour > 12) dtemp = dtime.hour -12;
         else dtemp = dtime.hour;
         WinSendMsg (hwndspinbutton, SPBM_SETCURRENTVALUE, MPFROMLONG (dtemp),
                                                                             NULL);

         hwndspinbutton = WinWindowFromID (hwnd, MINUTEID);
         WinSendMsg (hwndspinbutton, SPBM_OVERRIDESETLIMITS, MPFROMLONG (59),
                                                               MPFROMLONG (0));
         WinSendMsg (hwndspinbutton, SPBM_SETCURRENTVALUE, MPFROMLONG (dtime.minute),
                                                                          NULL);

         hwndspinbutton = WinWindowFromID (hwnd, AMPMID);
         WinSendMsg (hwndspinbutton, SPBM_SETARRAY, MPFROMP (ampm),
                                                               MPFROMLONG (2));
         if (dtime.hour > 12) dtemp = 1;
         else dtemp = 0;
         WinSendMsg (hwndspinbutton, SPBM_SETCURRENTVALUE, MPFROMLONG (dtemp),
                                                                          NULL);

      return 0;

      case WM_COMMAND:

         switch (SHORT1FROMMP (mp1))
         {
            case ATTRCANCELID:

               WinDismissDlg (hwnd, TRUE);
               break;

            case BOTHCHANGEID:
            case ATTRCHANGEID:
            case DAYCHANGEID:

               // The three main pushbuttons are all processed here to cut down on space.

               // Check to be sure there are items in the listbox.  If there are not, there is
               // nothing to change so exit with a beep!

               static SHORT cItems;

               hwndlistbox = WinWindowFromID (hwnd, ATTRIBFILEID);
               cItems = (SHORT) WinSendMsg (hwndlistbox, LM_QUERYITEMCOUNT, NULL, NULL);
               if (!cItems)
               {
                  WinDismissDlg (hwnd, TRUE);
                  DosBeep (100,50);
                  return 0;
               }

               USHORT id;
               id = SHORT1FROMMP (mp1);
               ULONG fileattributes = 0;
               unsigned tempattributes;
               unsigned oldattributes;
               ULONG state;

               HWND hwndspinbutton;

               USHORT value;
               USHORT btime;
               USHORT bdate;

               HWND hwndcheckbox;
               int hFile;

               // Since the file attribute flags and the date and time settings are set the
               // same for all files, they are all examined and set up first regardless
               // of whether the settings are going to be changed.  This will save some space
               // with a modest time penalty that should not be noticable here.

               // Examine the 'Read Only' check box in the dialog.
               // Adjust the file attribute flag.

               hwndcheckbox = WinWindowFromID (hwnd, ATTRIBROID);
               state = (ULONG) WinSendMsg (hwndcheckbox, BM_QUERYCHECK, NULL, NULL);
               if (state) fileattributes |= 1;

               // Examine the 'Hidden' check box in the dialog.
               // Adjust the file attribute flag.

               hwndcheckbox = WinWindowFromID (hwnd, ATTRIBHID);
               state = (ULONG) WinSendMsg (hwndcheckbox, BM_QUERYCHECK, NULL, NULL);
               if (state) fileattributes |= 2;

               // Examine the 'System' check box in the dialog.
               // Adjust the file attribute flag.

               hwndcheckbox = WinWindowFromID (hwnd, ATTRIBSID);
               state = (ULONG) WinSendMsg (hwndcheckbox, BM_QUERYCHECK, NULL, NULL);
               if (state) fileattributes |= 4;

               // Examine the 'Archive' check box in the dialog.
               // Adjust the file attribute flag.

               hwndcheckbox = WinWindowFromID (hwnd, ATTRIBAID);
               state = (ULONG) WinSendMsg (hwndcheckbox, BM_QUERYCHECK, NULL, NULL);
               if (state) fileattributes |= 32;

               bdate = 0;
               btime = 0;

               // Examine the Year spin button and shift it's value into the bdate variable.

               hwndspinbutton = WinWindowFromID (hwnd, YEARID);
               WinSendMsg (hwndspinbutton, SPBM_QUERYVALUE, MPFROMP (&value),
                                                         MPFROM2SHORT (NULL, NULL));
               if (value > 79) value -= 80;
               else value += 20;
               bdate = USHORT (value << 4);

               // Examine the Month spin button and shift it's value into the bdate variable.

               hwndspinbutton = WinWindowFromID (hwnd, MONTHID);
               WinSendMsg (hwndspinbutton, SPBM_QUERYVALUE, MPFROMP (&value),
                                                         MPFROM2SHORT (NULL, NULL));
               value |= bdate;
               bdate = USHORT (value << 5);

               // Examine the Day spin button and shift it's value into the bdate variable.

               hwndspinbutton = WinWindowFromID (hwnd, DAYID);
               WinSendMsg (hwndspinbutton, SPBM_QUERYVALUE, MPFROMP (&value),
                                                         MPFROM2SHORT (NULL, NULL));
               bdate |= value;

               // Examine the Hour spin button and shift it's value into the btime variable.
               // It is also adjusted according to the setting of the AMPM spin button.

               hwndspinbutton = WinWindowFromID (hwnd, HOURID);
               WinSendMsg (hwndspinbutton, SPBM_QUERYVALUE, MPFROMP (&value),
                                                         MPFROM2SHORT (NULL, NULL));
               btime = value;

               hwndspinbutton = WinWindowFromID (hwnd, AMPMID);
               WinSendMsg (hwndspinbutton, SPBM_QUERYVALUE, MPFROMP (&value),
                                                         MPFROM2SHORT (NULL, NULL));

               if (value == 1)
               {
                  if (btime != 12) value = USHORT (btime + 12);
                  else value = btime;
               }
               else
               {
                  if (btime == 12) value = 0;
                  else value = btime;
               }
               btime = SHORT (value << 6);

               // Examine the Minute spin button and shift it's value into the btime variable.

               hwndspinbutton = WinWindowFromID (hwnd, MINUTEID);
               WinSendMsg (hwndspinbutton, SPBM_QUERYVALUE, MPFROMP (&value),
                                                         MPFROM2SHORT (NULL, NULL));
               btime = SHORT ((btime | value) << 5);


               // This loop will process all of the selected files.

               for (int i = 0; i < cItems; i++)
               {
                  pcnrTemp = (PCNRITEM) WinSendMsg (hwndlistbox, LM_QUERYITEMHANDLE,
                                                               MPFROMSHORT (i), NULL);

                  // This is where the distinction between the three pushbuttons begins.

                  if ((id == BOTHCHANGEID) || (id == DAYCHANGEID))
                  {
                     // First, the file attributes are read and saved.  If the file has a
                     // Read-Only attribute, it cannot be opened, so it is replaced with
                     // a compatible attribute.  The original attribute will be replaced later.
                     // This also solves the problem of the archive attribute being set at the
                     // end of this routine which would be normal when a file is written to
                     // or has its date changed.
                     // This relates only to this routine for changing the date and time.

                     rc = _dos_getfileattr (pcnrTemp->szDirPath, &oldattributes);
                     tempattributes = oldattributes;
                     oldattributes &= 62;

                     if (!rc) rc = _dos_setfileattr (pcnrTemp->szDirPath, oldattributes);

                     // Now, the file is opened.  This is required for changing the date.

                     rc = _dos_open (pcnrTemp->szDirPath, O_RDWR | SH_DENYWR, &hFile);

                     // If it opened without error, rewrite the date and time.

                     if (!rc) rc = _dos_setftime (hFile, bdate, btime);

                     // If there was an error, put up a message.

                     if (rc)
                     {
                        MessageBox (hwnd, rc);
                        i = cItems;
                     }
                     DosClose (hFile);

                     // Restore the original file attributes (it may include a Read-Only bit).

                     _dos_setfileattr (pcnrTemp->szDirPath, tempattributes);
                  }

                  // Was the 'Change Attributes' or the 'Change Both' buttons pushed?

                  if ((id == ATTRCHANGEID) || (id == BOTHCHANGEID))
                  {
                     // Get the current attributes so that the Volume attribute and the
                     // Subdirectory attribute can be preserved.

                     rc = _dos_getfileattr (pcnrTemp->szDirPath, &oldattributes);
                     oldattributes &= 24;
                     oldattributes |= fileattributes;

                     // If no errors, write the new attributes.

                     if (!rc) rc = _dos_setfileattr (pcnrTemp->szDirPath, oldattributes);
                     if (rc)
                     {
                        MessageBox (hwnd, rc);
                        i = cItems;
                     }
                  }

               }

               // Find the record that has the cursor on it and copy the filename to a buffer.
               // It will be needed below in the returnto () function.

               pWhatRec = (PCNRITEM) WinSendMsg (hwndfilescntr,
                                     CM_QUERYRECORDEMPHASIS, MPFROMP (CMA_FIRST),
                                                            MPFROMSHORT (CRA_CURSORED));
               strcpy (tparms7.szSavename, pWhatRec->szFileName);

               // Refill the files container to show the changes.

               DFiles->EmptyContainer ();

               // Fill the files container and update the source status bar.
               // Don't use a thread here because of the returnto () function below.

               DFiles->FillContainer (filefilter);

               // This statusbar information can't be gathered until the container is filled.

               fillStatusbar2 (NULL, NULL);

               // Place the cursor back on the file it was on before the window was refreshed.

                  tparms7.Hab = NULL;
                  tparms7.Hmq = NULL;
                  tparms7.hwnd = hwndfilescntr;
                  tparms7.job = RETURNTO;
                  tid7 = _beginthread (threadall, NULL, 8192, &tparms7);

               break;

         }

      case WM_CONTROL:
         switch (SHORT2FROMMP (mp1))
         {
            case LN_ENTER:

               // Remove a filename from the list in the list box.  Also unselect the
               // record from the files container.

               static SHORT index;
               static ULONG pRec;

               hwndlistbox = WinWindowFromID (hwnd, ATTRIBFILEID);
               index = (SHORT) WinSendMsg (hwndlistbox, LM_QUERYSELECTION, NULL, NULL);
               if (index != LIT_NONE)
               {
                  pRec = (ULONG) WinSendMsg (hwndlistbox, LM_QUERYITEMHANDLE,
                                                               MPFROMSHORT (index), NULL);
                  WinSendMsg (hwndfilescntr, CM_SETRECORDEMPHASIS, MPFROMLONG (pRec),
                                                MPFROM2SHORT (FALSE, CRA_SELECTED));
                  WinSendMsg (hwndlistbox, LM_DELETEITEM, MPFROMSHORT (index), NULL);
                  return 0;
               }
         }
         break;
      }

   return WinDefDlgProc (hwnd, msg, mp1, mp2);
}
//------------------------------------------------------------------------
// This dialog allows the user to specify and modify Viewers for different file extensions.
//------------------------------------------------------------------------
MRESULT EXPENTRY ViewerDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   switch (msg)
   {
      case WM_CLOSE:
         delete pszVN;
         delete pszVT;
         return 0;

      case WM_INITDLG:

         Viewersetflag = 0;

         hwndvcombobox = WinWindowFromID (hwnd, VIEWERTYPEID);
         hwndentryfield = WinWindowFromID (hwnd, VIEWEREDITID);
         hwndentryadd = WinWindowFromID (hwnd, VIEWERADDEXTID);

         // Set the text length limit on the entryfield of 'Add Extension' to 6 characters
         // and set the text length of the entry field of 'Edit Viewer' to (*NEW*) 50 characters.

         WinSendMsg (hwndentryfield, EM_SETTEXTLIMIT, MPFROMSHORT (50), NULL);
         WinSendMsg (hwndentryadd, EM_SETTEXTLIMIT, MPFROMSHORT (6), NULL);

         // Create an array of pointers to strings.  By making the array count equal to the
         // the count of chars in the source string, there are sure to be enough pointers
         // without going overboard.  These strings are for the viewer extension.
         // Also delimit the strings within the string with zeros in place of spaces.
         // The original string has to end with a space or the loop below will not end.

         SetupViewers ();

         addflag = 0;

         // Declare some variables here that are used in other spots in this dialog proc.

         USHORT Vindex;
         USHORT Lindex;
         int i;

         WinSetFocus (HWND_DESKTOP, hwndvcombobox);
         Viewersetflag = 1;

         return (PVOID) TRUE;

      case WM_CHAR:

            if ((SHORT1FROMMP (mp2)) & KC_VIRTUALKEY)
            {
               // Throw away all up-stroke keys.

               if  ((SHORT1FROMMP (mp1)) & KC_KEYUP) break;
               if  ((SHORT2FROMMP (mp2)) & VK_ESC)
               {
                  WinSendMsg (hwndsettings, WM_CLOSE, NULL, NULL);
                  return 0;
               }
            }
            break;

      case WM_COMMAND:

         switch (SHORT1FROMMP (mp1))
         {
            case VIEWERCANCELID:

               // This will remove the 'add extension' text if the return key has not
               // been pressed yet.

               WinSendMsg (hwndentryadd, EM_SETSEL, MPFROM2SHORT (0, 6), NULL);
               WinSendMsg (hwndentryadd, EM_CLEAR, NULL, NULL);

               addflag = 0;
               return 0;

            case VIEWERACCEPTID:

               // This is the default pushbutton routine and is activated any time a change
               // is made to either the 'Viewer Name' entryfield or the 'Add Extension'
               // entryfield.  The 'addflag' distinguishes between the circumstances.
               // If a change has been made to the viewer, the change will be written to
               // the Viewer string.
               // If an extension has been added, focus will change to the
               // 'Viewer Name' entryfield so that a viewer can be inserted.
               // Provisions are also made to discern an empty 'Add Extension'
               // entryfield with the 'charcount' variable.
               // When an extension is added, both the viewer and extension strings
               // must be expanded and rebuilt as well as the pointer arrays.

               if (addflag)
               {
                  ULONG charcount;
                  static CHAR sztypename [7];
                  charcount = (ULONG) WinQueryDlgItemText (hwnd, VIEWERADDEXTID,
                                                                             7, (PSZ)sztypename);
                  if (charcount)
                  {
                     static SHORT arraysize;
                     arraysize = (SHORT) WinSendMsg (hwndvcombobox,
                                                LM_QUERYITEMCOUNT, NULL, NULL);

                     // Rebuild the name and type strings to be one item longer.

                     pszNbuffer = new CHAR [(arraysize + 1) * VIEWERSTRLEN +1];
                     memcpy (pszNbuffer, pszViewers, arraysize * VIEWERSTRLEN + 1);
                     delete pszViewers;
                     pszViewers = new CHAR [(arraysize + 1) * VIEWERSTRLEN + 1];
                     memcpy (pszViewers, pszNbuffer, (arraysize + 1) * VIEWERSTRLEN + 1);

                     // Set the string to be NULL.

                     pszViewers [arraysize * VIEWERSTRLEN] = 0;
                     delete pszNbuffer;

                     pszTbuffer = new CHAR [(arraysize + 1) * 7 +1];
                     memcpy (pszTbuffer, pszViewerTypes, arraysize * 7 + 1);
                     delete pszViewerTypes;
                     pszViewerTypes = new CHAR [(arraysize + 1) * 7 + 1];
                     memcpy (pszViewerTypes, pszTbuffer, (arraysize + 1) * 7 + 1);

                     delete pszTbuffer;

                     // Copy over the new string from the entryfield.

                     pszTbuffer = &pszViewerTypes [arraysize * 7];
                     strcpy (pszTbuffer, sztypename);

                     // Re-size both arrays to be one pointer longer and set the new pointer
                     // to point to the added item in the appropriate string.

                     delete pszVT;
                     pszVT = new PSZ [arraysize + 1];

                     for (i = 0; i < arraysize + 1; i++)
                     {
                        pszVT [i] = &pszViewerTypes [i * 7];
                     }

                     delete pszVN;
                     pszVN = new PSZ [arraysize + 1];
                     for (i = 0; i < arraysize + 1; i++)
                     {
                        pszVN [i] = &pszViewers [i * VIEWERSTRLEN];
                     }

                     // Add another item to the listbox and set it as the selected item.

                     Lindex = (USHORT) WinSendMsg (hwndvcombobox, LM_INSERTITEM,
                                                           MPFROMSHORT (LIT_SORTASCENDING),
                                                                    MPFROMP (pszVT [arraysize]));
                     WinSendMsg (hwndvcombobox, LM_SETITEMHANDLE,
                                                                       MPFROMSHORT (Lindex),
                                                                        MPFROMLONG (arraysize));

                     WinSendMsg (hwndvcombobox, LM_SELECTITEM, MPFROMSHORT (Lindex),
                                                                         MPFROMSHORT (TRUE));
                     comboitem = Lindex;

                     // Load the Viewer entry field with the blank viewer that matches the
                     // 'ext' array entry.

                     WinSetWindowText (hwndentryfield, pszVN [arraysize]);
                     WinSetFocus (HWND_DESKTOP, hwndentryfield);
                     WinSendMsg (hwndentryadd, EM_SETSEL, MPFROM2SHORT (0, 6), NULL);
                     WinSendMsg (hwndentryadd, EM_CLEAR, NULL, NULL);

                     // Adjust the string length variable.  This is used each time this dialog
                     // is opened.

                     ViewerTypeslen = (arraysize + 1) * 7;

                     WinSetFocus (HWND_DESKTOP, hwndentryfield);
                     addflag = 0;
                  }
               }
               else
               {
                  Vindex = (USHORT) WinSendMsg (hwndvcombobox, LM_QUERYITEMHANDLE,
                                                               MPFROMSHORT (comboitem), NULL);
                  PSZ psztemp;
                  psztemp = &pszViewers [Vindex * VIEWERSTRLEN];
                  WinQueryDlgItemText (hwnd, VIEWEREDITID, VIEWERSTRLEN, psztemp);

               }
               return 0;

            case VIEWERDELETEID:

               // In the case of a 'Delete Item' this routine will delete all the items from
               // the combobox list and totally rebuild the list of both extensions and
               // viewers.  This is necessary because a deleted item might be placed in
               // the middle of the string and so the string and the array of pointers would
               // be skewed if the list was just fore-shortened or trimmed.
               // The first extension ("<none>") is protected from removal.  This is the
               // default viewer so extensions don't have to be entered for it.
               // If a matching extension is not found in the list, the default viewer is used.

               static SHORT delitem, delindex;
               delitem = (SHORT) WinSendMsg (hwndvcombobox, LM_QUERYSELECTION, NULL, NULL);

               delindex = (USHORT) WinSendMsg (hwndvcombobox, LM_QUERYITEMHANDLE,
                                                               MPFROMSHORT (delitem), NULL);
               if (!delindex) return 0;

               static SHORT itemtotal;
               itemtotal = (SHORT) WinSendMsg (hwndvcombobox, LM_QUERYITEMCOUNT,
                                                                                          NULL, NULL);

               // Rebuild the name and type strings to be one item shorter.

               PSZ ptemp;
               pszNbuffer = new CHAR [(itemtotal - 1) * VIEWERSTRLEN + 1];
               if (!delindex)
               {
                  pszTbuffer = &pszViewers [VIEWERSTRLEN];
                  memcpy (pszNbuffer, pszTbuffer, (itemtotal - 1) * VIEWERSTRLEN + 1);
                  delete pszViewers;
                  pszViewers = new CHAR [(itemtotal - 1) * VIEWERSTRLEN + 1];
                  memcpy (pszViewers, pszNbuffer, (itemtotal - 1) * VIEWERSTRLEN + 1);
                  delete pszNbuffer;
               }
               else
               {
                  if (delindex ==1) memcpy (pszNbuffer, pszViewers, VIEWERSTRLEN);
                  else memcpy (pszNbuffer, pszViewers, delindex * VIEWERSTRLEN);
                  pszTbuffer = &pszViewers [(delindex + 1) * VIEWERSTRLEN];
                  ptemp = &pszNbuffer [delindex * VIEWERSTRLEN];
                  memcpy (ptemp, pszTbuffer, (itemtotal - delindex - 1) * VIEWERSTRLEN + 1);
                  delete pszViewers;
                  pszViewers = new CHAR [(itemtotal - 1) * VIEWERSTRLEN + 1];
                  memcpy (pszViewers, pszNbuffer, (itemtotal - 1) * VIEWERSTRLEN + 1);
                  delete pszNbuffer;
               }

               pszNbuffer = new CHAR [(itemtotal - 1) * 7 + 1];
               if (!delindex)
               {
                  pszTbuffer = &pszViewerTypes [7];
                  memcpy (pszNbuffer, pszTbuffer, (itemtotal - 1) * 7 + 1);
                  delete pszViewerTypes;
                  pszViewerTypes = new CHAR [(itemtotal - 1) * 7 + 1];
                  memcpy (pszViewerTypes, pszNbuffer, (itemtotal - 1) * 7 + 1);
                  delete pszNbuffer;
               }
               else
               {
                  if (delindex ==1) memcpy (pszNbuffer, pszViewerTypes, 7);
                  else memcpy (pszNbuffer, pszViewerTypes, delindex * 7);
                  pszTbuffer = &pszViewerTypes [(delindex + 1) * 7];
                  ptemp = &pszNbuffer [delindex * 7];
                  memcpy (ptemp, pszTbuffer, (itemtotal - delindex - 1) * 7 + 1);
                  delete pszViewerTypes;
                  pszViewerTypes = new CHAR [(itemtotal - 1) * 7 + 1];
                  memcpy (pszViewerTypes, pszNbuffer, (itemtotal - 1) * 7 + 1);
                  delete pszNbuffer;
                }

               // Re-size both arrays to be one pointer shorter and delete the
               // selected pointer.

               delete pszVT;
               pszVT = new PSZ [itemtotal - 1];
               for (i = 0; i < itemtotal - 1; i++)
               {
                  pszVT [i] = &pszViewerTypes [i * 7];
               }

               delete pszVN;
               pszVN = new PSZ [itemtotal - 1];
               for (i = 0; i < itemtotal - 1; i++)
               {
                  pszVN [i] = &pszViewers [i * VIEWERSTRLEN];
               }

               // Clear out all of the old entries.

               WinSendMsg (hwndvcombobox, LM_DELETEALL, NULL, NULL);

               // Load the new list of Viewer Types.

               for (i = 0; i < itemtotal - 1; i++)
               {
                  pszVT [i] = &pszViewerTypes [i * 7];

                  // Insert the '.ext' string into the listbox and attach the array index to the
                  // item in so that the string can be indexed properly when retrieved.

                  delindex = (SHORT) WinSendMsg (hwndvcombobox, LM_INSERTITEM,
                                      MPFROMSHORT (LIT_SORTASCENDING), MPFROMP (pszVT [i]));
                  WinSendMsg (hwndvcombobox, LM_SETITEMHANDLE, MPFROMSHORT (delindex),
                                                                          MPFROMLONG (i));
               }

               // It is necessary to clear the entry field for the last entry so it will be
               // done for all.

               WinSendMsg (hwndvcombobox, EM_SETSEL, MPFROM2SHORT (0, 6), NULL);
               WinSendMsg (hwndvcombobox, EM_CLEAR, NULL, NULL);

               // Display the first item in the listbox.

               WinSendMsg (hwndvcombobox, LM_SELECTITEM, MPFROMSHORT (0),
                                                                   MPFROMSHORT (TRUE));
               delindex = (SHORT) WinSendMsg (hwndvcombobox, LM_QUERYITEMHANDLE,
                                                               MPFROMSHORT (0), NULL);
               // Also display the linked viewer.

               WinSetWindowText (hwndentryfield, pszVN [delindex]);

               ViewerTypeslen = (itemtotal - 1) * 7;

               return 0;

            case VIEWERDEFAULTSID:

               // This method will restore the viewer defaults which may be necessary
               // from time to time since deleting the ini file will not work to restore
               // the defaults if it is deleted from file freedom.  File Freedom re-writes
               // the current buffer on exit.

               delete pszViewers;
               delete pszViewerTypes;
               SetViewerDefaults ();

               // Clear out all of the old entries and set up the defaults.

               WinSendMsg (hwndvcombobox, LM_DELETEALL, NULL, NULL);

               delete pszVT;
               delete pszVN;
               SetupViewers ();

               // Display the first item in the listbox.

               WinSendMsg (hwndvcombobox, LM_SELECTITEM, MPFROMSHORT (0),
                                                                   MPFROMSHORT (TRUE));
               delindex = (SHORT) WinSendMsg (hwndvcombobox, LM_QUERYITEMHANDLE,
                                                               MPFROMSHORT (0), NULL);
               // Also display the linked viewer.

               WinSetWindowText (hwndentryfield, pszVN [delindex]);

               return 0;
         }
         break;

      case WM_CONTROL:

            switch (SHORT2FROMMP (mp1))
            {

            // Determine if the combobox or the entryfield has had an enter key pressed.
            // If it was the combobox, shift focus to the entryfield.  If it was the
            // entryfield, shift focus to the 'Done' button.

               case EN_KILLFOCUS:

                  // If the focus changed from an entryfield, something could have
                  // been edited so copy over the contents to a string.

                  switch (SHORT1FROMMP (mp1))
                  {
                     case VIEWEREDITID:

                        // This is the viewer editing entryfield.  Any changes to this item
                        // must be stored in the Viewer string.

                        // A flag that will allow the accept button to process the edited
                        // name is not needed here since it is the default action in the
                        // Accept routine.  This is just here for information and later use.

                        break;

                     case VIEWERADDEXTID:

                        // This is the "Add Extension" entryfield.  If a character has been
                        // left in the entryfield, this must be interpreted as a request for an
                        // add-on for the extension list.
                        // There is a provision for cancelling that requires that this add-on
                        // be postponed until a choice is made between the 'Cancel' and
                        // the 'Accept' buttons.  This choice is discerned by use of a flag.

                        // Set the flag that will allow the 'Accept' button to process the add-on.

                        addflag = 1;
                        break;
                  }
                  return 0;

               case LN_SELECT:

                  // This message is sent upon loading of the dialog, before the pszVN array
                  // is initialized.  If the array is still a NULL pointer, the program will crash.
                  // This flag will stop the array from being accessed until the Setupviewers ()
                  // function has been called.

                  if (Viewersetflag)
                  {

                     // If an item name is highlighted in the list box, it has been selected.
                     // If an item name has had it's first character changed, the item selection
                     // is triggered again with a selection code of -1 (LIT_NONE).
                     // This LIT_NONE possibility is filtered out here since what is required
                     // is the index of the last item selected before it was changed.

                     SHORT tempitem;
                     tempitem = (SHORT) WinSendMsg (hwndvcombobox, LM_QUERYSELECTION, NULL,
                                                                                                      NULL);
                     USHORT tindex;

                     tindex = (USHORT) WinSendMsg (hwndvcombobox, LM_QUERYITEMHANDLE,
                                                                  MPFROMSHORT (tempitem), NULL);
                     WinSetWindowText (hwndentryfield, pszVN [tindex]);

                     if (tempitem != LIT_NONE) comboitem = tempitem;
                  }
                  break;

               case CBN_ENTER:

                  USHORT Hindex;

                  Hindex = (USHORT) WinSendMsg (hwndvcombobox, LM_QUERYITEMHANDLE,
                                                               MPFROMSHORT (comboitem), NULL);

                  WinSetWindowText (hwndentryfield, pszVN [Hindex]);
                  WinSetFocus (HWND_DESKTOP, hwndentryfield);
                  return 0;

            }
            return 0;
   }
   return WinDefDlgProc (hwnd, msg, mp1, mp2);
}
//------------------------------------------------------------------------
// This dialog allows the user to specify a default Source and default Destination Path
//------------------------------------------------------------------------
MRESULT EXPENTRY PathsDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   switch (msg)
   {
      case WM_INITDLG:

         // Set the possible length of the strings in the entry boxes.
         // Load the required text into the entry fields.

         HWND hEntryfield;

         hEntryfield = WinWindowFromID (hwnd, PATHDESTINATIONID);
         WinSendDlgItemMsg (hwnd, PATHDESTINATIONID, EM_SETTEXTLIMIT,
                                                              MPFROMSHORT (CCHMAXPATH + 1), 0);
         WinSetWindowText (hEntryfield, szDefaultDest);

         hEntryfield = WinWindowFromID (hwnd, PATHSOURCEID);
         WinSendDlgItemMsg (hwnd, PATHSOURCEID, EM_SETTEXTLIMIT,
                                                             MPFROMSHORT (CCHMAXPATH + 1), 0);
         WinSetWindowText (hEntryfield, szDefaultSource);

         return 0;

         case WM_CHAR:

            if ((SHORT1FROMMP (mp2)) & KC_VIRTUALKEY)
            {
               // Throw away all up-stroke keys.

               if  ((SHORT1FROMMP (mp1)) & KC_KEYUP) break;
               if  ((SHORT2FROMMP (mp2)) & VK_ESC)
               {
                  WinSendMsg (hwndsettings, WM_CLOSE, NULL, NULL);
                  return 0;
               }
            }
            break;

      case WM_CONTROL:

         switch(SHORT2FROMMP (mp1))
         {
            // Determine which entryfield has had an enter key pressed.

            case EN_KILLFOCUS:

            // If the focus changed from an entryfield, something could have
            // been edited so copy over the contents to a string.

            switch (SHORT1FROMMP (mp1))
            {
               case PATHSOURCEID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, PATHSOURCEID, CCHMAXPATH + 1, szDefaultSource);
                  return 0;

               case PATHDESTINATIONID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, PATHDESTINATIONID, CCHMAXPATH + 1,
                                                                              szDefaultDest);
                  return 0;
            }
            break;
         }
         break;
      }
   return WinDefDlgProc (hwnd, msg, mp1, mp2);
}
//------------------------------------------------------------------------
// This dialog allows the user to specify a default Editor in the Settings Notebook.
//------------------------------------------------------------------------
MRESULT EXPENTRY EditorDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   switch (msg)
   {
   case WM_INITDLG:

         // Set the possible length of the strings in the entry boxes.
         // Load the required text into the entry fields.

         HWND hEntryfield;

         hEntryfield = WinWindowFromID (hwnd, EDITORNAMEID);
         WinSendDlgItemMsg (hwnd, EDITORNAMEID, EM_SETTEXTLIMIT,
                                                              MPFROMSHORT (CCHMAXPATH + 1), 0);
         WinSetWindowText (hEntryfield, szDefaultEditor);

         hEntryfield = WinWindowFromID (hwnd, EDITORPARMSID);
         WinSendDlgItemMsg (hwnd, EDITORPARMSID, EM_SETTEXTLIMIT,
                                                             MPFROMSHORT (CCHMAXPATH + 1), 0);
         WinSetWindowText (hEntryfield, szDefaultEdParms);

         return 0;

   case WM_CHAR:

         if ((SHORT1FROMMP (mp2)) & KC_VIRTUALKEY)
         {
            // Throw away all up-stroke keys.

            if  ((SHORT1FROMMP (mp1)) & KC_KEYUP) break;
            if  ((SHORT2FROMMP (mp2)) & VK_ESC)
            {
               WinSendMsg (hwndsettings, WM_CLOSE, NULL, NULL);
               return 0;
            }
         }
         break;

      case WM_CONTROL:

         switch(SHORT2FROMMP (mp1))
         {
            // Determine which entryfield has had an enter key pressed.

            case EN_KILLFOCUS:

            // If the focus changed from an entryfield, something could have
            // been edited so copy over the contents to a string.

            switch (SHORT1FROMMP (mp1))
            {
               case EDITORNAMEID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, EDITORNAMEID, CCHMAXPATH + 1, szDefaultEditor);
                  return 0;

               case EDITORPARMSID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, EDITORPARMSID, CCHMAXPATH + 1,
                                                                              szDefaultEdParms);
                  return 0;
            }
            break;
         }
         break;
      }

   return WinDefDlgProc (hwnd, msg, mp1, mp2);
}
//------------------------------------------------------------------------
// This dialog allows the user to specify a default Editor in the Settings Notebook.
//------------------------------------------------------------------------
MRESULT EXPENTRY SearchToolDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   switch (msg)
   {
   case WM_INITDLG:

         // Set the possible length of the strings in the entry boxes.
         // Load the required text into the entry fields.

         HWND hEntryfield;

         hEntryfield = WinWindowFromID (hwnd, SEARCHTOOLNAMEID);
         WinSendDlgItemMsg (hwnd, SEARCHTOOLNAMEID, EM_SETTEXTLIMIT,
                                                              MPFROMSHORT (CCHMAXPATH + 1), 0);
         WinSetWindowText (hEntryfield, szDefaultSearchTool);

         hEntryfield = WinWindowFromID (hwnd, STPARMSID);
         WinSendDlgItemMsg (hwnd, STPARMSID, EM_SETTEXTLIMIT,
                                                             MPFROMSHORT (CCHMAXPATH + 1), 0);
         WinSetWindowText (hEntryfield, szDefaultSTParms);

         return 0;

   case WM_CHAR:

         if ((SHORT1FROMMP (mp2)) & KC_VIRTUALKEY)
         {
            // Throw away all up-stroke keys.

            if  ((SHORT1FROMMP (mp1)) & KC_KEYUP) break;
            if  ((SHORT2FROMMP (mp2)) & VK_ESC)
            {
               WinSendMsg (hwndsettings, WM_CLOSE, NULL, NULL);
               return 0;
            }
         }
         break;

      case WM_CONTROL:

         switch(SHORT2FROMMP (mp1))
         {
            // Determine which entryfield has had an enter key pressed.

            case EN_KILLFOCUS:

            // If the focus changed from an entryfield, something could have
            // been edited so copy over the contents to a string.

            switch (SHORT1FROMMP (mp1))
            {
               case SEARCHTOOLNAMEID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, SEARCHTOOLNAMEID, CCHMAXPATH + 1,
                                                                                  szDefaultSearchTool);
                  return 0;

               case STPARMSID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, STPARMSID, CCHMAXPATH + 1,
                                                                              szDefaultSTParms);
                  return 0;
            }
            break;
         }
         break;
      }

   return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

//-------------------------------------------------
// The dialog procedure for the settings notebook.
//-------------------------------------------------
MRESULT EXPENTRY SettingsDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{

   switch (msg)
   {
      case WM_ACTIVATE:
         break;

      case WM_CLOSE:
         WinAssociateHelpInstance (hwndhelp, hwndframe);
         break;

      case WM_HELP:
               WinSendMsg (hwndhelp, HM_DISPLAY_HELP, MPFROMSHORT (1000),
                                                                    MPFROMSHORT (HM_RESOURCEID));
         break;

      case WM_INITDLG:

         // Switch the active Help to this window.

         WinAssociateHelpInstance (hwndhelp, hwnd);

         hwndsettings = hwnd;
         hwndnotebook = WinWindowFromID (hwnd, NOTEBOOKID);

         // Create some pages for the notebook.

         settingsPage1 = (ULONG) WinSendMsg (hwndnotebook, BKM_INSERTPAGE, NULL,
                                 MPFROM2SHORT (BKA_AUTOPAGESIZE | BKA_MAJOR, BKA_FIRST));
         settingsPage2 = (ULONG) WinSendMsg (hwndnotebook, BKM_INSERTPAGE, NULL,
                                 MPFROM2SHORT (BKA_AUTOPAGESIZE | BKA_MAJOR, BKA_LAST));
         settingsPage3 = (ULONG) WinSendMsg (hwndnotebook, BKM_INSERTPAGE, NULL,
                                 MPFROM2SHORT (BKA_AUTOPAGESIZE | BKA_MAJOR, BKA_LAST));
         settingsPage4 = (ULONG) WinSendMsg (hwndnotebook, BKM_INSERTPAGE, NULL,
                                 MPFROM2SHORT (BKA_AUTOPAGESIZE | BKA_MAJOR, BKA_LAST));
         settingsPage5 = (ULONG) WinSendMsg (hwndnotebook, BKM_INSERTPAGE, NULL,
                                 MPFROM2SHORT (BKA_AUTOPAGESIZE | BKA_MAJOR |
                                 BKA_STATUSTEXTON, BKA_LAST));
         settingsPage6 = (ULONG) WinSendMsg (hwndnotebook, BKM_INSERTPAGE, NULL,
                                 MPFROM2SHORT (BKA_AUTOPAGESIZE |
                                 BKA_STATUSTEXTON, BKA_LAST));
         settingsPage7 = (ULONG) WinSendMsg (hwndnotebook, BKM_INSERTPAGE, NULL,
                                 MPFROM2SHORT (BKA_AUTOPAGESIZE |
                                 BKA_STATUSTEXTON, BKA_LAST));
         settingsPage8 = (ULONG) WinSendMsg (hwndnotebook, BKM_INSERTPAGE, NULL,
                                 MPFROM2SHORT (BKA_AUTOPAGESIZE |
                                 BKA_STATUSTEXTON, BKA_LAST));
         settingsPage9 = (ULONG) WinSendMsg (hwndnotebook, BKM_INSERTPAGE, NULL,
                                 MPFROM2SHORT (BKA_AUTOPAGESIZE |
                                 BKA_STATUSTEXTON, BKA_LAST));
         settingsPage10 = (ULONG) WinSendMsg (hwndnotebook, BKM_INSERTPAGE, NULL,
                                 MPFROM2SHORT (BKA_AUTOPAGESIZE |
                                 BKA_STATUSTEXTON, BKA_LAST));
         settingsPage11 = (ULONG) WinSendMsg (hwndnotebook, BKM_INSERTPAGE, NULL,
                                 MPFROM2SHORT (BKA_AUTOPAGESIZE |
                                 BKA_STATUSTEXTON, BKA_LAST));
         settingsPage12 = (ULONG) WinSendMsg (hwndnotebook, BKM_INSERTPAGE, NULL,
                                 MPFROM2SHORT (BKA_AUTOPAGESIZE |
                                 BKA_STATUSTEXTON, BKA_LAST));
         settingsPage13 = (ULONG) WinSendMsg (hwndnotebook, BKM_INSERTPAGE, NULL,
                                 MPFROM2SHORT (BKA_AUTOPAGESIZE | BKA_MAJOR, BKA_LAST));
         settingsPage14 = (ULONG) WinSendMsg (hwndnotebook, BKM_INSERTPAGE, NULL,
                                 MPFROM2SHORT (BKA_AUTOPAGESIZE | BKA_MAJOR, BKA_LAST));


         // Set the tab text for the pages.

         WinSendMsg (hwndnotebook, BKM_SETTABTEXT, MPFROMLONG (settingsPage1),
                                                                MPFROMP ("Paths"));
         WinSendMsg (hwndnotebook, BKM_SETTABTEXT, MPFROMLONG (settingsPage2),
                                                                MPFROMP ("Viewers"));
         WinSendMsg (hwndnotebook, BKM_SETTABTEXT, MPFROMLONG (settingsPage3),
                                                                MPFROMP ("Editor"));
         WinSendMsg (hwndnotebook, BKM_SETTABTEXT, MPFROMLONG (settingsPage4),
                                                                MPFROMP ("Archiver"));
         WinSendMsg (hwndnotebook, BKM_SETTABTEXT, MPFROMLONG (settingsPage5),
                                                                MPFROMP ("User"));
         WinSendMsg (hwndnotebook, BKM_SETTABTEXT, MPFROMLONG (settingsPage13),
                                                                MPFROMP ("Refresh"));
         WinSendMsg (hwndnotebook, BKM_SETTABTEXT, MPFROMLONG (settingsPage14),
                                                                MPFROMP ("Search"));

         // Set the tab dimensions and the dimensions for the page turning buttons.

         WinSendMsg (hwndnotebook, BKM_SETDIMENSIONS,
                               MPFROM2SHORT (90,30), MPFROMSHORT (BKA_MAJORTAB));

         WinSendMsg (hwndnotebook, BKM_SETDIMENSIONS,
                               MPFROM2SHORT (25,30), MPFROMSHORT (BKA_PAGEBUTTON));

         WinSendMsg (hwndnotebook, BKM_SETSTATUSLINETEXT,
                    MPFROMLONG (settingsPage5), MPFROMP ("User - Page 1 of 8"));

         WinSendMsg (hwndnotebook, BKM_SETSTATUSLINETEXT,
                    MPFROMLONG (settingsPage6), MPFROMP ("User - Page 2 of 8"));

         WinSendMsg (hwndnotebook, BKM_SETSTATUSLINETEXT,
                    MPFROMLONG (settingsPage7), MPFROMP ("User - Page 3 of 8"));

         WinSendMsg (hwndnotebook, BKM_SETSTATUSLINETEXT,
                    MPFROMLONG (settingsPage8), MPFROMP ("User - Page 4 of 8"));

         WinSendMsg (hwndnotebook, BKM_SETSTATUSLINETEXT,
                    MPFROMLONG (settingsPage9), MPFROMP ("User - Page 5 of 8"));

         WinSendMsg (hwndnotebook, BKM_SETSTATUSLINETEXT,
                    MPFROMLONG (settingsPage10), MPFROMP ("User - Page 6 of 8"));

         WinSendMsg (hwndnotebook, BKM_SETSTATUSLINETEXT,
                    MPFROMLONG (settingsPage11), MPFROMP ("User - Page 7 of 8"));

         WinSendMsg (hwndnotebook, BKM_SETSTATUSLINETEXT,
                    MPFROMLONG (settingsPage12), MPFROMP ("User - Page 8 of 8"));

         WinSendMsg (hwndnotebook, BKM_SETNOTEBOOKCOLORS,
                          MPFROMLONG (SYSCLR_FIELDBACKGROUND),
                          MPFROMSHORT (BKA_BACKGROUNDMAJORCOLORINDEX));
         WinSendMsg (hwndnotebook, BKM_SETNOTEBOOKCOLORS,
                          MPFROMLONG (SYSCLR_FIELDBACKGROUND),
                          MPFROMSHORT (BKA_BACKGROUNDPAGECOLORINDEX));

         // Declare some handles here so they will be available in the rest of the proc.

         HWND hwndviewerproc;
         HWND hwndpathsproc;
         HWND hwndeditorproc;
         HWND hwnduserproc;
         HWND hwndzipproc;
         HWND hwndrefreshproc;
         HWND hwndsearchproc;

         // Assign the top page here so there will be something showing on startup.
         // The first page is automatically shown on startup.
         // This means to load (modeless) a dialog procedure and associate it with a
         // specific page.

         hwndpathsproc = WinLoadDlg (hwndnotebook, hwndnotebook, PathsDlgProc,
                                                      NULLHANDLE, PATHSID, NULL);

         WinSendMsg (hwndnotebook, BKM_SETPAGEWINDOWHWND,
                               MPFROMLONG (settingsPage1), MPFROMLONG (hwndpathsproc));

         // Now loading the other dialog procedures on demand.  I originally tried loading them
         // on demand but found that this caused crashes when the 'PAGEBUTTON' was used
         // to turn to these pages (as opposed to clicking on the tab for this page) if the dialog
         // had to be loaded at that time (on demand).  Loading the dialog by clicking on the
         // notebook tab worked flawlessly however.  The loading of the dialog somehow
         // overwrote portions of memory.
         // I had other problems with the 'ViewerDlgProc' which I also couldn't
         // explain but which all had to do with memory overwrites to memory referenced
         // by the dialog.
         // All of these problems as well as many other odd, re-occurring problems were
         // caused by the stack size being too small.

         return 0;

      case WM_CONTROL:

            // Trap the 'PagesSelected' notification and other notifications possibly.

            switch(SHORT2FROMMP (mp1))
            {
               case BKN_PAGESELECTED:

                  PAGESELECTNOTIFY *pPageSL;
                  pPageSL = (PAGESELECTNOTIFY *) LONGFROMMP (mp2);

                  // Determine which page has been selected.  If it not the same
                  // page being selected again, process the order.
//---Page One
                  if (pPageSL->ulPageIdNew == settingsPage1)
                  {
                     if (pPageSL->ulPageIdCur != settingsPage1)
                     {
                        // Page One has already had the dialog loaded (in WM_INITDLG)
                        // so just turn to the page.

                        WinSendMsg (hwndnotebook, BKM_TURNTOPAGE,
                                                         MPFROMLONG (settingsPage1), NULL);
                        return 0;
                     }
                  }

                  // Test for Page Two.
//---Page Two
                  else if (pPageSL->ulPageIdNew == settingsPage2)
                  {
                     if (pPageSL->ulPageIdCur != settingsPage2)
                     {
                        // Turn to the page.

                        // Test to determine if the dialog has been loaded before
                       // (ie. does it have a handle to a procedure?).

                        hwndtemp = (HWND) WinSendMsg (hwndnotebook,
                                                              BKM_QUERYPAGEWINDOWHWND,
                                                               MPFROMLONG (settingsPage2), NULL);
                        if (!hwndtemp)
                        {
                         hwndviewerproc = WinLoadDlg (hwndnotebook, hwndnotebook,
                                                   ViewerDlgProc, NULLHANDLE, VIEWERCFGID, NULL);
                          WinSendMsg (hwndnotebook, BKM_SETPAGEWINDOWHWND,
                               MPFROMLONG (settingsPage2), MPFROMLONG (hwndviewerproc));
                         }
                        WinSendMsg (hwndnotebook, BKM_TURNTOPAGE,
                                                                MPFROMLONG (settingsPage2), NULL);
                        return 0;
                     }
                  }

                  // Test for Page Three.
//---Page Three
                  else if (pPageSL->ulPageIdNew == settingsPage3)
                  {
                     if (pPageSL->ulPageIdCur != settingsPage3)
                     {
                        // Test to determine if the dialog has been loaded before
                       // (ie. does it have a handle to a procedure?).

                        hwndtemp = (HWND) WinSendMsg (hwndnotebook,
                                                              BKM_QUERYPAGEWINDOWHWND,
                                                               MPFROMLONG (settingsPage3), NULL);
                        if (!hwndtemp)
                        {
                         hwndeditorproc = WinLoadDlg (hwndnotebook, hwndnotebook,
                                                       EditorDlgProc, NULLHANDLE, EDITORID, NULL);
                          WinSendMsg (hwndnotebook, BKM_SETPAGEWINDOWHWND,
                               MPFROMLONG (settingsPage3), MPFROMLONG (hwndeditorproc));
                         }
                        WinSendMsg (hwndnotebook, BKM_TURNTOPAGE,
                                                              MPFROMLONG (settingsPage3), NULL);
                        return 0;
                     }
                  }

                  // Test for Page four.
//---Page Four
                  else if (pPageSL->ulPageIdNew == settingsPage4)
                  {
                     if (pPageSL->ulPageIdCur != settingsPage4)
                     {
                        // Test to determine if the dialog has been loaded before
                       // (ie. does it have a handle to a procedure?).

                        hwndtemp = (HWND) WinSendMsg (hwndnotebook,
                                                              BKM_QUERYPAGEWINDOWHWND,
                                                               MPFROMLONG (settingsPage4), NULL);
                        if (!hwndtemp)
                        {
                         hwndzipproc = WinLoadDlg (hwndnotebook, hwndnotebook,
                                                       ZipDlgProc, NULLHANDLE, ARCHIVERID, NULL);
                          WinSendMsg (hwndnotebook, BKM_SETPAGEWINDOWHWND,
                               MPFROMLONG (settingsPage4), MPFROMLONG (hwndzipproc));
                         }
                        WinSendMsg (hwndnotebook, BKM_TURNTOPAGE,
                                                              MPFROMLONG (settingsPage4), NULL);
                        return 0;
                     }
                  }

                  // Test for Page five.
//---Page Five (1 of 8)
                  else if (pPageSL->ulPageIdNew == settingsPage5)
                  {
                     if (pPageSL->ulPageIdCur != settingsPage5)
                     {
                        // Test to determine if the dialog has been loaded before
                       // (ie. does it have a handle to a procedure?).

                        hwndtemp = (HWND) WinSendMsg (hwndnotebook,
                                                              BKM_QUERYPAGEWINDOWHWND,
                                                               MPFROMLONG (settingsPage5), NULL);
                        if (!hwndtemp)
                        {
                         hwnduserproc = WinLoadDlg (hwndnotebook, hwndnotebook,
                                                       UserDlgProc, NULLHANDLE, USER1ID, NULL);
                          WinSendMsg (hwndnotebook, BKM_SETPAGEWINDOWHWND,
                               MPFROMLONG (settingsPage5), MPFROMLONG (hwnduserproc));
                         }
                        WinSendMsg (hwndnotebook, BKM_TURNTOPAGE,
                                                              MPFROMLONG (settingsPage5), NULL);
                        return 0;
                     }
                  }
//---Page Six (2 of 8)
                  else if (pPageSL->ulPageIdNew == settingsPage6)
                  {
                     if (pPageSL->ulPageIdCur != settingsPage6)
                     {
                        // Test to determine if the dialog has been loaded before
                       // (ie. does it have a handle to a procedure?).

                        hwndtemp = (HWND) WinSendMsg (hwndnotebook,
                                                              BKM_QUERYPAGEWINDOWHWND,
                                                               MPFROMLONG (settingsPage6), NULL);
                        if (!hwndtemp)
                        {
                         hwnduserproc = WinLoadDlg (hwndnotebook, hwndnotebook,
                                                       UserDlgProc, NULLHANDLE, USER2ID, NULL);
                          WinSendMsg (hwndnotebook, BKM_SETPAGEWINDOWHWND,
                               MPFROMLONG (settingsPage6), MPFROMLONG (hwnduserproc));
                         }
                        WinSendMsg (hwndnotebook, BKM_TURNTOPAGE,
                                                              MPFROMLONG (settingsPage6), NULL);
                        return 0;
                     }
                  }
//---Page Seven (3 of 8)
                  else if (pPageSL->ulPageIdNew == settingsPage7)
                  {
                     if (pPageSL->ulPageIdCur != settingsPage7)
                     {
                        // Test to determine if the dialog has been loaded before
                       // (ie. does it have a handle to a procedure?).

                        hwndtemp = (HWND) WinSendMsg (hwndnotebook,
                                                              BKM_QUERYPAGEWINDOWHWND,
                                                               MPFROMLONG (settingsPage7), NULL);
                        if (!hwndtemp)
                        {
                         hwnduserproc = WinLoadDlg (hwndnotebook, hwndnotebook,
                                                       UserDlgProc, NULLHANDLE, USER3ID, NULL);
                          WinSendMsg (hwndnotebook, BKM_SETPAGEWINDOWHWND,
                               MPFROMLONG (settingsPage7), MPFROMLONG (hwnduserproc));
                         }
                        WinSendMsg (hwndnotebook, BKM_TURNTOPAGE,
                                                              MPFROMLONG (settingsPage7), NULL);
                        return 0;
                     }
                  }
//---Page Eight (4 of 8)
                  else if (pPageSL->ulPageIdNew == settingsPage8)
                  {
                     if (pPageSL->ulPageIdCur != settingsPage8)
                     {
                        // Test to determine if the dialog has been loaded before
                       // (ie. does it have a handle to a procedure?).

                        hwndtemp = (HWND) WinSendMsg (hwndnotebook,
                                                              BKM_QUERYPAGEWINDOWHWND,
                                                               MPFROMLONG (settingsPage8), NULL);
                        if (!hwndtemp)
                        {
                         hwnduserproc = WinLoadDlg (hwndnotebook, hwndnotebook,
                                                       UserDlgProc, NULLHANDLE, USER4ID, NULL);
                          WinSendMsg (hwndnotebook, BKM_SETPAGEWINDOWHWND,
                               MPFROMLONG (settingsPage8), MPFROMLONG (hwnduserproc));
                         }
                        WinSendMsg (hwndnotebook, BKM_TURNTOPAGE,
                                                              MPFROMLONG (settingsPage8), NULL);
                        return 0;
                     }
                  }
//---Page 9 (5 of 8)
                  else if (pPageSL->ulPageIdNew == settingsPage9)
                  {
                     if (pPageSL->ulPageIdCur != settingsPage9)
                     {
                        // Test to determine if the dialog has been loaded before
                       // (ie. does it have a handle to a procedure?).

                        hwndtemp = (HWND) WinSendMsg (hwndnotebook,
                                                              BKM_QUERYPAGEWINDOWHWND,
                                                               MPFROMLONG (settingsPage9), NULL);
                        if (!hwndtemp)
                        {
                         hwnduserproc = WinLoadDlg (hwndnotebook, hwndnotebook,
                                                       UserDlgProc, NULLHANDLE, USER5ID, NULL);
                          WinSendMsg (hwndnotebook, BKM_SETPAGEWINDOWHWND,
                               MPFROMLONG (settingsPage9), MPFROMLONG (hwnduserproc));
                         }
                        WinSendMsg (hwndnotebook, BKM_TURNTOPAGE,
                                                              MPFROMLONG (settingsPage9), NULL);
                        return 0;
                     }
                  }
//---Page 10 (6 of 8)
                  else if (pPageSL->ulPageIdNew == settingsPage10)
                  {
                     if (pPageSL->ulPageIdCur != settingsPage10)
                     {
                        // Test to determine if the dialog has been loaded before
                       // (ie. does it have a handle to a procedure?).

                        hwndtemp = (HWND) WinSendMsg (hwndnotebook,
                                                              BKM_QUERYPAGEWINDOWHWND,
                                                               MPFROMLONG (settingsPage10), NULL);
                        if (!hwndtemp)
                        {
                         hwnduserproc = WinLoadDlg (hwndnotebook, hwndnotebook,
                                                       UserDlgProc, NULLHANDLE, USER6ID, NULL);
                          WinSendMsg (hwndnotebook, BKM_SETPAGEWINDOWHWND,
                               MPFROMLONG (settingsPage10), MPFROMLONG (hwnduserproc));
                         }
                        WinSendMsg (hwndnotebook, BKM_TURNTOPAGE,
                                                              MPFROMLONG (settingsPage10), NULL);
                        return 0;
                     }
                  }
//---Page 11 (7 of 8)
                  else if (pPageSL->ulPageIdNew == settingsPage11)
                  {
                     if (pPageSL->ulPageIdCur != settingsPage11)
                     {
                        // Test to determine if the dialog has been loaded before
                       // (ie. does it have a handle to a procedure?).

                        hwndtemp = (HWND) WinSendMsg (hwndnotebook,
                                                              BKM_QUERYPAGEWINDOWHWND,
                                                               MPFROMLONG (settingsPage11), NULL);
                        if (!hwndtemp)
                        {
                         hwnduserproc = WinLoadDlg (hwndnotebook, hwndnotebook,
                                                       UserDlgProc, NULLHANDLE, USER7ID, NULL);
                          WinSendMsg (hwndnotebook, BKM_SETPAGEWINDOWHWND,
                               MPFROMLONG (settingsPage11), MPFROMLONG (hwnduserproc));
                         }
                        WinSendMsg (hwndnotebook, BKM_TURNTOPAGE,
                                                              MPFROMLONG (settingsPage11), NULL);
                        return 0;
                     }
                  }
//---Page 12 (8 of 8)
                  else if (pPageSL->ulPageIdNew == settingsPage12)
                  {
                     if (pPageSL->ulPageIdCur != settingsPage12)
                     {
                        // Test to determine if the dialog has been loaded before
                       // (ie. does it have a handle to a procedure?).

                        hwndtemp = (HWND) WinSendMsg (hwndnotebook,
                                                              BKM_QUERYPAGEWINDOWHWND,
                                                               MPFROMLONG (settingsPage12), NULL);
                        if (!hwndtemp)
                        {
                         hwnduserproc = WinLoadDlg (hwndnotebook, hwndnotebook,
                                                       UserDlgProc, NULLHANDLE, USER8ID, NULL);
                          WinSendMsg (hwndnotebook, BKM_SETPAGEWINDOWHWND,
                               MPFROMLONG (settingsPage12), MPFROMLONG (hwnduserproc));
                         }
                        WinSendMsg (hwndnotebook, BKM_TURNTOPAGE,
                                                              MPFROMLONG (settingsPage12), NULL);
                        return 0;
                     }
                  }
                  // Test for Page 13.
//---Page 13
                  else if (pPageSL->ulPageIdNew == settingsPage13)
                  {
                     if (pPageSL->ulPageIdCur != settingsPage13)
                     {
                        // Turn to the page.

                        // Test to determine if the dialog has been loaded before
                       // (ie. does it have a handle to a procedure?).

                        hwndtemp = (HWND) WinSendMsg (hwndnotebook,
                                                              BKM_QUERYPAGEWINDOWHWND,
                                                               MPFROMLONG (settingsPage13), NULL);
                        if (!hwndtemp)
                        {
                         hwndrefreshproc = WinLoadDlg (hwndnotebook, hwndnotebook,
                                           TimedRefreshDlgProc, NULLHANDLE, REFRESHID, NULL);
                          WinSendMsg (hwndnotebook, BKM_SETPAGEWINDOWHWND,
                               MPFROMLONG (settingsPage13), MPFROMLONG (hwndrefreshproc));
                         }
                        WinSendMsg (hwndnotebook, BKM_TURNTOPAGE,
                                                                MPFROMLONG (settingsPage13), NULL);
                        return 0;
                     }
                  }
                  // Test for Page 14.
//---Page 14
                  else if (pPageSL->ulPageIdNew == settingsPage14)
                  {
                     if (pPageSL->ulPageIdCur != settingsPage14)
                     {
                        // Turn to the page.

                        // Test to determine if the dialog has been loaded before
                       // (ie. does it have a handle to a procedure?).

                        hwndtemp = (HWND) WinSendMsg (hwndnotebook,
                                                              BKM_QUERYPAGEWINDOWHWND,
                                                               MPFROMLONG (settingsPage14), NULL);
                        if (!hwndtemp)
                        {
                         hwndsearchproc = WinLoadDlg (hwndnotebook, hwndnotebook,
                                           SearchToolDlgProc, NULLHANDLE, SEARCHID, NULL);
                          WinSendMsg (hwndnotebook, BKM_SETPAGEWINDOWHWND,
                               MPFROMLONG (settingsPage14), MPFROMLONG (hwndsearchproc));
                         }
                        WinSendMsg (hwndnotebook, BKM_TURNTOPAGE,
                                                                MPFROMLONG (settingsPage14), NULL);
                        return 0;
                     }
                  }
                  break;
         }
         break;

      case WM_COMMAND:

         switch (SHORT1FROMMP (mp1))
         {
            case SETTINGSDONEID:

               // Dismiss the dialog.  This is necessary or a crash can occur if this
               // button is pressed.

               WinDismissDlg (hwnd, TRUE);
               break;

            case SETTINGSHELPID:
               WinSendMsg (hwndhelp, HM_DISPLAY_HELP, MPFROMSHORT (2600),
                                                                    MPFROMSHORT (HM_RESOURCEID));
               return 0;
         }
         break;
   }
   return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

MRESULT EXPENTRY UserDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   switch (msg)
   {
      case WM_INITDLG:

         // Set the possible length of the strings in the entry boxes.
         // Load the required text into the entry fields.

         HWND hEntryfield;

         hEntryfield = WinWindowFromID (hwnd, USERONEID);
         WinSendMsg (hEntryfield, EM_SETTEXTLIMIT, MPFROMSHORT (CCHMAXPATH + 1), NULL);
         WinSetWindowText (hEntryfield, szUser1prog);

         hEntryfield = WinWindowFromID (hwnd, USERONEPARMSID);
         WinSendMsg (hEntryfield, EM_SETTEXTLIMIT, MPFROMSHORT (CCHMAXPATH + 1), NULL);
         WinSetWindowText (hEntryfield, szUser1parms);

         hEntryfield = WinWindowFromID (hwnd, USERONEDIRID);
         WinSendMsg (hEntryfield, EM_SETTEXTLIMIT, MPFROMSHORT (CCHMAXPATH + 1), NULL);
         WinSetWindowText (hEntryfield, szUser1dir);

         hEntryfield = WinWindowFromID (hwnd, USERONEMENUNAMEID);
         WinSendMsg (hEntryfield, EM_SETTEXTLIMIT, MPFROMSHORT (13), NULL);
         WinSetWindowText (hEntryfield, szUser1name);

         hEntryfield = WinWindowFromID (hwnd, USERTWOID);
         WinSendMsg (hEntryfield, EM_SETTEXTLIMIT, MPFROMSHORT (CCHMAXPATH + 1), NULL);
         WinSetWindowText (hEntryfield, szUser2prog);

         hEntryfield = WinWindowFromID (hwnd, USERTWOPARMSID);
         WinSendMsg (hEntryfield, EM_SETTEXTLIMIT, MPFROMSHORT (CCHMAXPATH + 1), NULL);
         WinSetWindowText (hEntryfield, szUser2parms);

         hEntryfield = WinWindowFromID (hwnd, USERTWODIRID);
         WinSendMsg (hEntryfield, EM_SETTEXTLIMIT, MPFROMSHORT (CCHMAXPATH + 1), NULL);
         WinSetWindowText (hEntryfield, szUser2dir);

         hEntryfield = WinWindowFromID (hwnd, USERTWOMENUNAMEID);
         WinSendMsg (hEntryfield, EM_SETTEXTLIMIT, MPFROMSHORT (13), NULL);
         WinSetWindowText (hEntryfield, szUser2name);

         hEntryfield = WinWindowFromID (hwnd, USERTHREEID);
         WinSendMsg (hEntryfield, EM_SETTEXTLIMIT, MPFROMSHORT (CCHMAXPATH + 1), NULL);
         WinSetWindowText (hEntryfield, szUser3prog);

         hEntryfield = WinWindowFromID (hwnd, USERTHREEPARMSID);
         WinSendMsg (hEntryfield, EM_SETTEXTLIMIT, MPFROMSHORT (CCHMAXPATH + 1), NULL);
         WinSetWindowText (hEntryfield, szUser3parms);

         hEntryfield = WinWindowFromID (hwnd, USERTHREEDIRID);
         WinSendMsg (hEntryfield, EM_SETTEXTLIMIT, MPFROMSHORT (CCHMAXPATH + 1), NULL);
         WinSetWindowText (hEntryfield, szUser3dir);

         hEntryfield = WinWindowFromID (hwnd, USERTHREEMENUNAMEID);
         WinSendMsg (hEntryfield, EM_SETTEXTLIMIT, MPFROMSHORT (13), NULL);
         WinSetWindowText (hEntryfield, szUser3name);

         hEntryfield = WinWindowFromID (hwnd, USERFOURID);
         WinSendMsg (hEntryfield, EM_SETTEXTLIMIT, MPFROMSHORT (CCHMAXPATH + 1), NULL);
         WinSetWindowText (hEntryfield, szUser4prog);

         hEntryfield = WinWindowFromID (hwnd, USERFOURPARMSID);
         WinSendMsg (hEntryfield, EM_SETTEXTLIMIT, MPFROMSHORT (CCHMAXPATH + 1), NULL);
         WinSetWindowText (hEntryfield, szUser4parms);

         hEntryfield = WinWindowFromID (hwnd, USERFOURDIRID);
         WinSendMsg (hEntryfield, EM_SETTEXTLIMIT, MPFROMSHORT (CCHMAXPATH + 1), NULL);
         WinSetWindowText (hEntryfield, szUser4dir);

         hEntryfield = WinWindowFromID (hwnd, USERFOURMENUNAMEID);
         WinSendMsg (hEntryfield, EM_SETTEXTLIMIT, MPFROMSHORT (13), NULL);
         WinSetWindowText (hEntryfield, szUser4name);

         hEntryfield = WinWindowFromID (hwnd, USERFIVEID);
         WinSendMsg (hEntryfield, EM_SETTEXTLIMIT, MPFROMSHORT (CCHMAXPATH + 1), NULL);
         WinSetWindowText (hEntryfield, szUser5prog);

         hEntryfield = WinWindowFromID (hwnd, USERFIVEPARMSID);
         WinSendMsg (hEntryfield, EM_SETTEXTLIMIT, MPFROMSHORT (CCHMAXPATH + 1), NULL);
         WinSetWindowText (hEntryfield, szUser5parms);

         hEntryfield = WinWindowFromID (hwnd, USERFIVEDIRID);
         WinSendMsg (hEntryfield, EM_SETTEXTLIMIT, MPFROMSHORT (CCHMAXPATH + 1), NULL);
         WinSetWindowText (hEntryfield, szUser5dir);

         hEntryfield = WinWindowFromID (hwnd, USERFIVEMENUNAMEID);
         WinSendMsg (hEntryfield, EM_SETTEXTLIMIT, MPFROMSHORT (13), NULL);
         WinSetWindowText (hEntryfield, szUser5name);

         hEntryfield = WinWindowFromID (hwnd, USERSIXID);
         WinSendMsg (hEntryfield, EM_SETTEXTLIMIT, MPFROMSHORT (CCHMAXPATH + 1), NULL);
         WinSetWindowText (hEntryfield, szUser6prog);

         hEntryfield = WinWindowFromID (hwnd, USERSIXPARMSID);
         WinSendMsg (hEntryfield, EM_SETTEXTLIMIT, MPFROMSHORT (CCHMAXPATH + 1), NULL);
         WinSetWindowText (hEntryfield, szUser6parms);

         hEntryfield = WinWindowFromID (hwnd, USERSIXDIRID);
         WinSendMsg (hEntryfield, EM_SETTEXTLIMIT, MPFROMSHORT (CCHMAXPATH + 1), NULL);
         WinSetWindowText (hEntryfield, szUser6dir);

         hEntryfield = WinWindowFromID (hwnd, USERSIXMENUNAMEID);
         WinSendMsg (hEntryfield, EM_SETTEXTLIMIT, MPFROMSHORT (13), NULL);
         WinSetWindowText (hEntryfield, szUser6name);

         hEntryfield = WinWindowFromID (hwnd, USERSEVENID);
         WinSendMsg (hEntryfield, EM_SETTEXTLIMIT, MPFROMSHORT (CCHMAXPATH + 1), NULL);
         WinSetWindowText (hEntryfield, szUser7prog);

         hEntryfield = WinWindowFromID (hwnd, USERSEVENPARMSID);
         WinSendMsg (hEntryfield, EM_SETTEXTLIMIT, MPFROMSHORT (CCHMAXPATH + 1), NULL);
         WinSetWindowText (hEntryfield, szUser7parms);

         hEntryfield = WinWindowFromID (hwnd, USERSEVENDIRID);
         WinSendMsg (hEntryfield, EM_SETTEXTLIMIT, MPFROMSHORT (CCHMAXPATH + 1), NULL);
         WinSetWindowText (hEntryfield, szUser7dir);

         hEntryfield = WinWindowFromID (hwnd, USERSEVENMENUNAMEID);
         WinSendMsg (hEntryfield, EM_SETTEXTLIMIT, MPFROMSHORT (13), NULL);
         WinSetWindowText (hEntryfield, szUser7name);

         hEntryfield = WinWindowFromID (hwnd, USEREIGHTID);
         WinSendMsg (hEntryfield, EM_SETTEXTLIMIT, MPFROMSHORT (CCHMAXPATH + 1), NULL);
         WinSetWindowText (hEntryfield, szUser8prog);

         hEntryfield = WinWindowFromID (hwnd, USEREIGHTPARMSID);
         WinSendMsg (hEntryfield, EM_SETTEXTLIMIT, MPFROMSHORT (CCHMAXPATH + 1), NULL);
         WinSetWindowText (hEntryfield, szUser8parms);

         hEntryfield = WinWindowFromID (hwnd, USEREIGHTDIRID);
         WinSendMsg (hEntryfield, EM_SETTEXTLIMIT, MPFROMSHORT (CCHMAXPATH + 1), NULL);
         WinSetWindowText (hEntryfield, szUser8dir);

         hEntryfield = WinWindowFromID (hwnd, USEREIGHTMENUNAMEID);
         WinSendMsg (hEntryfield, EM_SETTEXTLIMIT, MPFROMSHORT (13), NULL);
         WinSetWindowText (hEntryfield, szUser8name);

         return 0;

      case WM_CHAR:

         if ((SHORT1FROMMP (mp2)) & KC_VIRTUALKEY)
         {
            // Throw away all up-stroke keys.

            if  ((SHORT1FROMMP (mp1)) & KC_KEYUP) break;
            if  ((SHORT2FROMMP (mp2)) & VK_ESC)
            {
               WinSendMsg (hwndsettings, WM_CLOSE, NULL, NULL);
               return 0;
            }
         }
         break;

      case WM_CONTROL:

         switch(SHORT2FROMMP (mp1))
         {
            // Determine which entryfield has had an enter key pressed.

            case EN_KILLFOCUS:

            // If the focus changed from an entryfield, something could have
            // been edited so copy over the contents to a string.

            switch (SHORT1FROMMP (mp1))
            {
               case USERONEID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, USERONEID, CCHMAXPATH + 1, szUser1prog);

                  // Write the menu name as entered and if the user defined program entryfield
                  // is blank, disable the menu item.
                  // The user name that has been entered (if it is not blank) will appear in
                  // the menu during this program instance, but on re-starting the program,
                  // it will be replaced with "User Prog" if the program field is still blank.

                  WriteMenuItem (szUser1name, "~1\tAlt+1", MID_USER1, strlen (szUser1prog));
                  return 0;

               case USERONEPARMSID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, USERONEPARMSID, CCHMAXPATH + 1,
                                                                              szUser1parms);
                  return 0;

               case USERONEDIRID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, USERONEDIRID, CCHMAXPATH + 1,
                                                                              szUser1dir);
                  return 0;

               case USERONEMENUNAMEID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, USERONEMENUNAMEID, 14, szUser1name);

                  // Note:  The actual name can be 13 Characters plus a terminating zero.
                  // The second string is the Alt-Key (speed key) symbol and the
                  // Hot Key information for this entry.

                  WriteMenuItem (szUser1name, "~1\tAlt+1", MID_USER1, strlen (szUser1prog));
                  return 0;

               case USERTWOID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, USERTWOID, CCHMAXPATH + 1, szUser2prog);
                  WriteMenuItem (szUser2name, "~2\tAlt+2", MID_USER2, strlen (szUser2prog));
                  return 0;

               case USERTWOPARMSID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, USERTWOPARMSID, CCHMAXPATH + 1,
                                                                              szUser2parms);
                  return 0;

               case USERTWODIRID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, USERTWODIRID, CCHMAXPATH + 1,
                                                                              szUser2dir);
                  return 0;

               case USERTWOMENUNAMEID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, USERTWOMENUNAMEID, 14, szUser2name);

                  // Note:  The actual name can be 13 Characters plus a terminating zero.
                  // The second string is the Alt-Key (speed key) symbol and the
                  // Hot Key information for this entry.

                  WriteMenuItem (szUser2name, "~2\tAlt+2", MID_USER2, strlen (szUser2prog));
                  return 0;

               case USERTHREEID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, USERTHREEID, CCHMAXPATH + 1, szUser3prog);
                  WriteMenuItem (szUser3name, "~3\tAlt+3", MID_USER3, strlen (szUser3prog));
                  return 0;

               case USERTHREEPARMSID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, USERTHREEPARMSID, CCHMAXPATH + 1,
                                                                              szUser3parms);
                  return 0;

               case USERTHREEDIRID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, USERTHREEDIRID, CCHMAXPATH + 1,
                                                                              szUser3dir);
                  return 0;

               case USERTHREEMENUNAMEID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, USERTHREEMENUNAMEID, 14, szUser3name);

                  // Note:  The actual name can be 13 Characters plus a terminating zero.
                  // The second string is the Alt-Key (speed key) symbol and the
                  // Hot Key information for this entry.

                  WriteMenuItem (szUser3name, "~3\tAlt+3", MID_USER3, strlen (szUser3prog));
                  return 0;

               case USERFOURID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, USERFOURID, CCHMAXPATH + 1, szUser4prog);
                  WriteMenuItem (szUser4name, "~4\tAlt+4", MID_USER4, strlen (szUser4prog));
                  return 0;

               case USERFOURPARMSID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, USERFOURPARMSID, CCHMAXPATH + 1,
                                                                              szUser4parms);
                  return 0;

               case USERFOURDIRID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, USERFOURDIRID, CCHMAXPATH + 1,
                                                                              szUser4dir);
                  return 0;

               case USERFOURMENUNAMEID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, USERFOURMENUNAMEID, 14, szUser4name);

                  // Note:  The actual name can be 13 Characters plus a terminating zero.
                  // The second string is the Alt-Key (speed key) symbol and the
                  // Hot Key information for this entry.

                  WriteMenuItem (szUser4name, "~4\tAlt+4", MID_USER4, strlen (szUser4prog));
                  return 0;

               case USERFIVEID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, USERFIVEID, CCHMAXPATH + 1, szUser5prog);
                  WriteMenuItem (szUser5name, "~5\tAlt+5", MID_USER5, strlen (szUser5prog));
                  return 0;

               case USERFIVEPARMSID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, USERFIVEPARMSID, CCHMAXPATH + 1,
                                                                              szUser5parms);
                  return 0;

               case USERFIVEDIRID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, USERFIVEDIRID, CCHMAXPATH + 1,
                                                                              szUser5dir);
                  return 0;

               case USERFIVEMENUNAMEID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, USERFIVEMENUNAMEID, 14, szUser5name);

                  // Note:  The actual name can be 13 Characters plus a terminating zero.
                  // The second string is the Alt-Key (speed key) symbol and the
                  // Hot Key information for this entry.

                  WriteMenuItem (szUser5name, "~5\tAlt+5", MID_USER5, strlen (szUser5prog));
                  return 0;

               case USERSIXID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, USERSIXID, CCHMAXPATH + 1, szUser6prog);
                  WriteMenuItem (szUser6name, "~6\tAlt+6", MID_USER6, strlen (szUser6prog));
                  return 0;

               case USERSIXPARMSID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, USERSIXPARMSID, CCHMAXPATH + 1,
                                                                              szUser6parms);
                  return 0;

               case USERSIXDIRID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, USERSIXDIRID, CCHMAXPATH + 1,
                                                                              szUser6dir);
                  return 0;

               case USERSIXMENUNAMEID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, USERSIXMENUNAMEID, 14, szUser6name);

                  // Note:  The actual name can be 13 Characters plus a terminating zero.
                  // The second string is the Alt-Key (speed key) symbol and the
                  // Hot Key information for this entry.

                  WriteMenuItem (szUser6name, "~6\tAlt+6", MID_USER6, strlen (szUser6prog));
                  return 0;

               case USERSEVENID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, USERSEVENID, CCHMAXPATH + 1, szUser7prog);
                  WriteMenuItem (szUser7name, "~7\tAlt+7", MID_USER7, strlen (szUser7prog));
                  return 0;

               case USERSEVENPARMSID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, USERSEVENPARMSID, CCHMAXPATH + 1,
                                                                              szUser7parms);
                  return 0;

               case USERSEVENDIRID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, USERSEVENDIRID, CCHMAXPATH + 1,
                                                                              szUser7dir);
                  return 0;

               case USERSEVENMENUNAMEID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, USERSEVENMENUNAMEID, 14, szUser7name);

                  // Note:  The actual name can be 13 Characters plus a terminating zero.
                  // The second string is the Alt-Key (speed key) symbol and the
                  // Hot Key information for this entry.

                  WriteMenuItem (szUser7name, "~7\tAlt+7", MID_USER7, strlen (szUser7prog));
                  return 0;

               case USEREIGHTID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, USEREIGHTID, CCHMAXPATH + 1, szUser8prog);
                  WriteMenuItem (szUser8name, "~8\tAlt+8", MID_USER8, strlen (szUser8prog));
                  return 0;

               case USEREIGHTPARMSID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, USEREIGHTPARMSID, CCHMAXPATH + 1,
                                                                              szUser8parms);
                  return 0;

               case USEREIGHTDIRID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, USEREIGHTDIRID, CCHMAXPATH + 1,
                                                                              szUser8dir);
                  return 0;

               case USEREIGHTMENUNAMEID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, USEREIGHTMENUNAMEID, 14, szUser8name);

                  // Note:  The actual name can be 13 Characters plus a terminating zero.
                  // The second string is the Alt-Key (speed key) symbol and the
                  // Hot Key information for this entry.

                  WriteMenuItem (szUser8name, "~8\tAlt+8", MID_USER8, strlen (szUser8prog));
                  return 0;
            }
            break;
         }
         break;
   }
   return WinDefDlgProc (hwnd, msg, mp1, mp2);
}
// ---------------------------------------------------------------
// This is the dialog used in the Settings Notebook under ARCHIVER
//----------------------------------------------------------------
MRESULT EXPENTRY ZipDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   switch (msg)
   {
      case WM_INITDLG:

         // Set the possible length of the strings in the entry boxes.
         // Load the required text into the entry fields.

         HWND hEntryfield;

         hEntryfield = WinWindowFromID (hwnd, ZIPID);
         WinSendMsg (hEntryfield, EM_SETTEXTLIMIT, MPFROMSHORT (CCHMAXPATH + 1), NULL);
         WinSetWindowText (hEntryfield, szZipprog);

         hEntryfield = WinWindowFromID (hwnd, ZIPPARMSID);
         WinSendMsg (hEntryfield, EM_SETTEXTLIMIT, MPFROMSHORT (CCHMAXPATH + 1), NULL);
         WinSetWindowText (hEntryfield, szZipparms);

         hEntryfield = WinWindowFromID (hwnd, UNZIPID);
         WinSendMsg (hEntryfield, EM_SETTEXTLIMIT, MPFROMSHORT (CCHMAXPATH + 1), NULL);
         WinSetWindowText (hEntryfield, szUnzipprog);

         hEntryfield = WinWindowFromID (hwnd, UNZIPPARMSID);
         WinSendMsg (hEntryfield, EM_SETTEXTLIMIT, MPFROMSHORT (CCHMAXPATH + 1), NULL);
         WinSetWindowText (hEntryfield, szUnzipparms);

         return 0;

      case WM_CHAR:

            if ((SHORT1FROMMP (mp2)) & KC_VIRTUALKEY)
            {
               // Throw away all up-stroke keys.

               if  ((SHORT1FROMMP (mp1)) & KC_KEYUP) break;
               if  ((SHORT2FROMMP (mp2)) & VK_ESC)
               {
                  WinSendMsg (hwndsettings, WM_CLOSE, NULL, NULL);
                  return 0;
               }
            }
            break;

      case WM_CONTROL:

         switch(SHORT2FROMMP (mp1))
         {
            // Determine which entryfield has had an enter key pressed.

            case EN_KILLFOCUS:

            // If the focus changed from an entryfield, something could have
            // been edited so copy over the contents to a string.

            switch (SHORT1FROMMP (mp1))
            {
               case ZIPID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, ZIPID, CCHMAXPATH + 1, szZipprog);
                  return 0;

               case ZIPPARMSID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, ZIPPARMSID, CCHMAXPATH + 1, szZipparms);
                  return 0;

               case UNZIPID:

                  // Retrieve the new name from the entry field.

                  WinQueryDlgItemText (hwnd, UNZIPID, CCHMAXPATH + 1, szUnzipprog);
                  return 0;

               case UNZIPPARMSID:

                  // Retrieve the new params from the entry field.

                  WinQueryDlgItemText (hwnd, UNZIPPARMSID, CCHMAXPATH + 1, szUnzipparms);
                  return 0;

            }
            break;
         }
         break;
   }


   return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

//---------------------------------------------------------------------------
// This procedure presents a dialog that requests a name for the zip file that is about to
// be created.  It then creates a file containing a list of all files selected and associates that
// file with 'stdin' which is used by zip as a file list to include in a zip archive.
// This procedure will work only with Info-Zip 2.1 and up since it includes support for filenames
// with embedded spaces (enclose name in single quotes) and Info-Zip 2.01 does not.
//---------------------------------------------------------------------------
MRESULT EXPENTRY ZipnameDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   switch (msg)
   {
      // The next three messages are used mainly to set up the help system for the dialog.

      case WM_ACTIVATE:
         WinAssociateHelpInstance (hwndhelp, hwnd);
         break;

      case WM_CLOSE:
         WinAssociateHelpInstance (hwndhelp, hwndframe);
         break;

      case WM_HELP:
         WinSendMsg (hwndhelp, HM_DISPLAY_HELP, MPFROMSHORT (2530),
                                                                    MPFROMSHORT (HM_RESOURCEID));
         break;

      case WM_INITDLG:

         static HWND hEntryfield;
         PCNRITEM pAddRec;
         static HWND hwndlistbox;
         hwndlistbox = WinWindowFromID (hwnd, ZIPLISTID);
         hEntryfield = WinWindowFromID (hwnd, ZIPNAMEID);

         // Set the possible length of the strings in the entry boxes.

         WinSendDlgItemMsg (hwnd, ZIPNAMEID, EM_SETTEXTLIMIT,
                                                             MPFROMSHORT (CCHMAXPATH + 1), 0);

         // Some static variables are needed in this dialog.

         static CHAR szfname [CCHMAXPATH +1];
         static FILE * fp;

         // Search the destination directory for files with the 'zip' extention and insert
         // them in the listbox.

         // These buffers are used with the DosFindFirst etc. functions.  The large array
         // of buffers holds information on many files at a time.

         PFILEFINDBUF3 pNextbuf;
         PBYTE pbBuf;
         PFILEFINDBUF3 pFilebuf;
         pFilebuf = new FILEFINDBUF3 [50];
         ULONG maxfiles;
         maxfiles = 50;

         rc = 0;
         HDIR hdir;
         ULONG fileattrs;
         ULONG buffer;
         buffer = sizeof (FILEFINDBUF3) * 50;

         fileattrs =  FILE_ARCHIVED | FILE_SYSTEM | FILE_HIDDEN | FILE_READONLY;

         // Create our filespec

         strcpy (szpathbuffer, szDestPath);
         if (strlen (szpathbuffer) != 3) strcat (szpathbuffer, "\\");
         else if (szpathbuffer [1] != ':') strcat (szpathbuffer, "\\");
         strcat (szpathbuffer, "*.zip");

         hdir = HDIR_SYSTEM;
         rc = DosFindFirst ( szpathbuffer, &hdir, fileattrs, pFilebuf, buffer,
                           &maxfiles, FIL_STANDARD);

         // This loop will insert the files in the listbox and it also will
         // keep reading in files if there were more than could be read on the
         // DosFindFirst pass.  When thru, it releases the handle.

         pbBuf = (PBYTE) pFilebuf;
         pNextbuf = pFilebuf;

         while ( !rc)
         {
            for (i = 0; i < maxfiles; i++)
            {
            // Take for the selected records and then insert them into the listbox.  Attach the
            // Record pointer to each item as a handle so that it can be recovered later.

               WinSendMsg (hwndlistbox, LM_INSERTITEM,
                         MPFROMSHORT (LIT_SORTASCENDING), MPFROMP (pNextbuf->achName));

               pbBuf += pNextbuf->oNextEntryOffset;
               pNextbuf = (PFILEFINDBUF3) pbBuf;
            }

            if ( maxfiles == 50)
            {
               rc = DosFindNext (hdir, pFilebuf, buffer, &maxfiles);
               pbBuf = (PBYTE) pFilebuf;
               pNextbuf = pFilebuf;
            }
            else
            {
               rc = 1;
               DosFindClose (hdir);
            }
         }
         return 0;

      case WM_APPTERMINATENOTIFY:

         // When the archiver terminates, this dialog is notified and here the file that has been
         // "typed" to stdin is closed and deleted.  The dialog will be dismissed and
         // focus is changed back to the files window.
         // If the operation was to zip up a directory tree, a file containing a list of filenames
         // would not have been created or, in the case of a zip up drive operation, a wildcard
         // character is used, but trying to delete the file anyway won't cause any
         // harm (it will only return an error) because global filename characters are not accepted.

         DosDelete (szfname);

         WinDismissDlg (hwnd, TRUE);

         // Compare the source and the destination.  If they are the same, the
         // files container will have to be updated to show the any new files.
         // This will only happen if the files in the source directory are being
         // zipped to a zip file in the same directory.

         success = CompareandRefresh ();
         if (success)
         {
            // Get the cursor back on the original file.

            tparms7.Hab = NULL;
            tparms7.Hmq = NULL;
            tparms7.hwnd = hwndfilescntr;
            tparms7.job = RETURNTO;
            tid7 = _beginthread (threadall, NULL, 8192, &tparms7);
         }

         // Update the Destination status bar info with new bytes free etc.

         fillStatusbar1 (DestDrive, (PSZ) szDestPath);

         // If this is bit is set, the origin of this process was another instance of FF.
         // This means that the caller's object window handle is stored and a message
         // must be sent to it telling it to refresh the displays due to a change in contents
         // and byte counts.

         if ((DropDestOp.Operation & 64) == 64)
         {
            WinPostMsg (DropDestOp.hwndCaller, OBJM_REFRESH, NULL, NULL);
         }

         // This variable must be cleared here to keep other ops from screwing up.

         DropDestOp.recundermouse = 0;

         WinSetFocus (HWND_DESKTOP, hwndfilescntr);
         break;

      case WM_CONTROL:
         switch (SHORT2FROMMP (mp1))
         {
            // LN_SELECT is the message given when a filename is highlighted in the
            // listbox.  This is better than LN_ENTER since the enter key doesn't have
            // to be pressed.

            case LN_SELECT:

               // Get a filename from the list in the list box.  Enter it into the entry field below.

               SHORT index;

               index = (SHORT) WinSendMsg (hwndlistbox, LM_QUERYSELECTION, NULL, NULL);
               WinSendMsg (hwndlistbox, LM_QUERYITEMTEXT,
                              MPFROM2SHORT (index, CCHMAXPATH),
                              MPFROMP (sznamebuffer));
               if (index != LIT_NONE)
               {
                  WinSetWindowText (hEntryfield, sznamebuffer);
                  WinSendMsg (hEntryfield, WM_CHAR, MPFROMSH2CH
                                       (KC_VIRTUALKEY, 0, 0), MPFROM2SHORT (0, VK_END));
               }
               WinSetFocus (HWND_DESKTOP, hEntryfield);
               return 0;
         }
         break;

      case WM_COMMAND:

         switch (SHORT1FROMMP (mp1))
         {
            case ZIPNAMEACCEPTID:

               // Some file handles are needed for redirection later.

               HFILE hFile, hStdin, hTemp;

               // A flag is needed to indicate if redirection is being used

               USHORT redirect;
               redirect = 0;

               // The name entered for the zip file has been accepted and is read from the
               // dialog entry field.

               WinQueryDlgItemText (hwnd, ZIPNAMEID, CCHMAXPATH + 1, szpathbuffer);

               // Save the archive name.  It will be used later to return the cursor to if
               // the target directory was the same as the source.
               // The returnto function only compares for as many chars as the length
               // of this saved name, so it doesn't matter if it has a zip extension or not.

               strcpy (tparms7.szSavename, szpathbuffer);

               // Put up the hourglass cursor.

               WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP,
                                                                                 SPTR_WAIT, FALSE));
               // Fill in the PROGDETAILS structure for the WinStartApp function.

               pDetails = &Details;
               pDetails->progt.progc     = PROG_WINDOWABLEVIO;
               pDetails->progt.fbVisible = SHE_VISIBLE;
               pDetails->pszTitle        = "Archiver";
               pDetails->pszStartupDir   = NULL;
               pDetails->pszExecutable   = szZipprog;

// *** Add this line for trouble shooting only, since otherwise, all other
//      pDetail structures will pick up this attribute after this usage.
//               pDetails->swpInitial.fl= SWP_SHOW | SWP_NOAUTOCLOSE;

               // The zip procedure begins here.
               // There are three possible zip routes:
               // 1) a group of files or a single file has been selected for a zip.
               // 2) a Directory has been selected for a zip (only the first one will be zipped).
               // 3) nothing has been selected for a zip in which case a message box
               //    will query the user if they would like to zip the entire drive.
               // The second choice will compress the entire directory tree while the third choice
               // is the procedure to use to compress a whole drive.

               // A search will be made for the first selected record.  The search criteria for
               // the first record is CMA_FIRST, but for the following records, the criteria is the
               // record of the preceeding search.

               strcpy (szfname, szHomedrivepath);
               strcat (szfname, "\\");
               strcat (szfname, "fflist");

               pAddRec = (PCNRITEM) CMA_FIRST;

               // Test to see if a single file under the mouse is being used (that was not
               // selected) in a Drag&Drop manouver.  If so the selected files
               // must be ignored while this file is used.

               if (DropDestOp.recundermouse) pAddRec = (PCNRITEM) DropDestOp.pRec;

               // Otherwise scan for selected files.

               else
                  pAddRec = (PCNRITEM) WinSendMsg (hwndfilescntr,
                                        CM_QUERYRECORDEMPHASIS, MPFROMP (pAddRec),
                                                                  MPFROMSHORT (CRA_SELECTED));
               if (pAddRec)
               {
                     // If this is a sub-directory

                  if (pAddRec->attrFile & 16)
                  {
                     // De-select the highlighted file.

                     WinSendMsg (hwndfilescntr, CM_SETRECORDEMPHASIS,
                               MPFROMLONG (pAddRec), MPFROM2SHORT (FALSE, CRA_SELECTED));

                     // The command and parameter line being built here will look like:
                     //   "zip -rS zipname D:\path\*".

                     strcpy (sznamebuffer, "-rS ");

                     // Now add the destination drive/path/name

                     strcat (sznamebuffer, szDestPath);
                     if (strlen (szDestPath) != 3) strcat (sznamebuffer , "\\");
                     else if (szDestPath [1] != 58) strcat (sznamebuffer, "\\");
                     strcat (sznamebuffer, szpathbuffer);

                     // Add the source with a "*" attached.

                     strcat (sznamebuffer, " ");
                     strcat (sznamebuffer, szFilesTitle);

                     // Test for a drive designator followed by backslash only.

                     if (strlen (szFilesTitle) != 3) strcat (sznamebuffer , "\\");
                     else if (szFilesTitle [1] != 58) strcat (sznamebuffer, "\\");

                     // Test for parent directory only ("..\") in which case only an asterisk is
                     // added.

                     if (strlen (pAddRec->szFileName) == 3)
                     {
                        if (pAddRec->szFileName [0] == 46)
                        {
                           if (pAddRec->szFileName [1] == 46)
                           {
                              if (pAddRec->szFileName [2] == 92)
                              strcat (sznamebuffer, "*");
                           }
                        }
                     }
                     else
                     {
                        strcat (sznamebuffer, pAddRec->szFileName);
                        strcat (sznamebuffer, "\*");
                     }
                  }
                  else
                  {
                     // Open a file "X:\path\fflist" that will be filled with the file list
                     // that is later redirected to stdin.

                     fp = fopen (szfname, "w");
                     sznamebuffer [0] = 0;
                     while (pAddRec)
                     {

                        // De-select the highlighted file.

                        WinSendMsg (hwndfilescntr, CM_SETRECORDEMPHASIS,
                               MPFROMLONG (pAddRec), MPFROM2SHORT (FALSE, CRA_SELECTED));

                        // In order to handle filenames with embedded spaces, the whole name
                        // must be enclosed in single quotes.  This will also work for ordinary names
                        // so it is done for all names.

                        sznamebuffer [0] = 39;
                        sznamebuffer [1] = CHAR (CurrDrive + 64);
                        sznamebuffer [2] = 58;
                        sznamebuffer [3] = 0;

                        strcat (sznamebuffer, pAddRec->szDirPath);
                        strcat (sznamebuffer, "\'");
                        strcat (sznamebuffer, "\n");

                        // Write the file name to the list.

                        fputs (sznamebuffer, fp);
                        sznamebuffer [0] = 0;
                        pAddRec = (PCNRITEM) WinSendMsg (hwndfilescntr,
                                              CM_QUERYRECORDEMPHASIS, MPFROMP( pAddRec ),
                                                                     MPFROMSHORT (CRA_SELECTED));
                        // We need a trap here to break out if this dialog has been called
                        // by Drag/Drop with an unselected file under the mouse.

                        if (DropDestOp.recundermouse)
                        {
                           pAddRec = NULL;
                           DropDestOp.recundermouse = 0;
                        }
                     }
                     fclose (fp);

                     // Copy the parameters to the parameter buffer.  The  "-@" parameter is
                     // used to get the list from stdin.  The user should be told not to put it in.
                     // When the parameter string is complete. it will look like
                     //      "-@[xxx] zipname"

                     strcpy (sznamebuffer, " -@");

                     // Make it possible to concatenate parameters with mine, excepting
                     // "-b" which is the parameter to indicate the drive for a tempory
                     // zip file and must stand alone with a path name.

                     if ((szZipparms [0] == '-') && (szZipparms [1] != 'b'))
                        pszpath = &szZipparms [1];
                     else pszpath = szZipparms;
                     strcat (sznamebuffer, pszpath);

                     // Now add the destination directory to the string following the 'blank'
                     // and the beginning quote mark.

                     strcat (sznamebuffer, " \"");
                     strcat (sznamebuffer, szDestPath);
                     if (strlen (szDestPath) != 3) strcat (sznamebuffer , "\\");
                     else if (szDestPath [1] != 58) strcat (sznamebuffer, "\\");
                     strcat (sznamebuffer, szpathbuffer);

                     // Close the quotes.

                     strcat (sznamebuffer, "\"");

                     // Set up flag to redirect stdin.

                     redirect = 1;
                  }
               }
               else
               {
                  USHORT usResponse;
                  usResponse = (USHORT) WinMessageBox (HWND_DESKTOP, hwnd,
                                      "No Files or Directories were selected.\n"
                                      "Do you want to Zip the entire Drive?", "ZIP QUERY!",
                                                      MESSAGEBOXID, MB_OKCANCEL | MB_QUERY);
                  if (usResponse == MBID_OK)
                  {
                     // Spell out the drive parameter "X:\*"

                     szfname [0] = CHAR (CurrDrive + 64);
                     szfname [1] = 58;
                     szfname [2] = 92;
                     szfname [3] = 42;
                     szfname [4] = 0;

                     // The command and parameter line being built here will look like:
                     //   "zip -rS zipname D:\*".
                     // Paths will be preserved and hidden and system files will be included.

                     strcpy (sznamebuffer, "-rS ");

                     // Now add the destination drive/path/name

                     strcat (sznamebuffer, szDestPath);
                     if (strlen (szDestPath) != 3) strcat (sznamebuffer , "\\");
                     else if (szDestPath [1] != 58) strcat (sznamebuffer, "\\");
                     strcat (sznamebuffer, szpathbuffer);

                     // Add the source with a "*" attached.

                     strcat (sznamebuffer, " ");
                     strcat (sznamebuffer, szfname);
                  }
                  else
                  {
                     WinDismissDlg (hwnd, TRUE);

                     // Put up the regular cursor.

                     WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP,
                                                                              SPTR_ARROW, FALSE));
                     return 0;
                  }
               }

               // If we are looking for a file list from stdin, do this.

               if (redirect)
               {
                  // This parameter is ignored in this instance, but it is still
                  // required.
                  ULONG Action;

                  rc = DosOpen (szfname, &hFile, &Action, NULL, NULL,
                                  OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
                                  OPEN_SHARE_DENYNONE | OPEN_ACCESS_READONLY, NULL);

                  // If error opening "fflist" file, put up message.

                  if (rc != 0)
                  {
                     WinMessageBox (HWND_DESKTOP, hwnd, "Can't open temporary file\n",
                                                       "ERROR!", NULL, MB_OK | MB_ERROR);
                  }
                  // **Set up to redirect the file "fflist" to stdin**
                  // First the file must be opened (as above) so that a handle is available.
                  // Then DosDupHandle is used to:
                  // 1) Duplicate the handle to stdin and save the new handle
                  // 2) Substitute the old handle of stdin for the handle for "fflist"
                  // Next the original file handle is closed and this will force the file to stdin.
                  // After the child app is started (it inherits the environment and outputs from
                  // it's parent) DosDupHandle is again used to exchange the saved substituted
                  // handle for stdin with the original value (0).
                  // When the child app finishes getting input from stdin (the end of file is reached),
                  // it will close stdin before it returns.

                  hStdin = 0;
                  hTemp = -1;
                  DosDupHandle (hStdin, &hTemp);
                  hStdin = 0;
                  DosDupHandle (hFile, &hStdin);
                  DosClose (hFile);
               }

               // Change the current dir to the home dir so that the Info-zip utilities can be
               // found if they are there.

               DosSetDefaultDisk (Homedrive);
               DosSetCurrentDir (szHomepath);

               // Start Info-Zip.  When it is finished, a notification message will
               // be sent to WM_APPTERMINATENOTIFY (above).

               WinStartApp (hwnd, pDetails, sznamebuffer, NULL, SAF_STARTCHILDAPP);

               if (redirect)
               {
                  hStdin = 0;
                  DosDupHandle (hTemp, &hStdin);
               }
               // Change the drive back to where we were.

               DosSetDefaultDisk (CurrDrive);

               // Put up the regular cursor.

               WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP,
                                                                              SPTR_ARROW, FALSE));
               return 0;

            case ZIPNAMECANCELID:
               WinDismissDlg (hwnd, TRUE);
               break;
         }
   }
   return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

//----------------------------------------------
// This dlg presents the ABOUT box and nothing else
//----------------------------------------------
MRESULT EXPENTRY AboutDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   switch (msg)
   {
      // The next three messages are used mainly to set up the help system for the dialog.

      case WM_ACTIVATE:
         WinAssociateHelpInstance (hwndhelp, hwnd);
         break;

      case WM_CLOSE:
         WinAssociateHelpInstance (hwndhelp, hwndframe);
         break;

      case WM_HELP:
         WinSendMsg (hwndhelp, HM_DISPLAY_HELP, MPFROMSHORT (3000),
                                                                    MPFROMSHORT (HM_RESOURCEID));
         break;

      case WM_INITDLG:

         // If there was a valid registration number, display it's name in the About box.

         if (regpassed)
         {
            PSZ pName;
            CHAR szName [27];
            i = (szregnumber [0] -48) + 8;
            pName = &szregnumber [i];
            i = szregnumber [0] - 48;
            ii = szregnumber [i + 2] - 64;
            memcpy (szName, pName, ii);
            szName [ii] = 0;
            WinSetDlgItemText (hwnd, ABOUT1TEXTID, "Registered to:");
            WinSetDlgItemText (hwnd, ABOUT2TEXTID, szName);
         }
         break;
   }
   return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

//-------------------------------------------------------------------------
// A Procedure to handle a Drag&Drop Dialog Box.  It must allow the user to pick a
// directory from a supplied drive and also to indicate which operation should be performed
// on the files that have been previously selected.
//-------------------------------------------------------------------------
MRESULT EXPENTRY DragDropDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   switch (msg)
   {
      // The next three messages are used mainly to set up the help system for the dialog.

      case WM_ACTIVATE:
         WinAssociateHelpInstance (hwndhelp, hwnd);
         break;
         // NOTE: using a return 0 statement for WM_ACTIVATE results in a titlebar that
         // has been disabled.

      case WM_HELP:
         // This general help summons is answered by a General Help screen.
         // There is also a CN_HELP trap below which answers with a General Help screen.
         // The DDHELP_ID message is intercepted in WM_COMMAND and discriminates
         // between help for Copy, Move, Zip or Unzip.

         WinSendMsg (hwndhelp, HM_DISPLAY_HELP, MPFROMSHORT (1000),
                                                                    MPFROMSHORT (HM_RESOURCEID));
         break;

      case WM_INITDLG:

         // Get the pointer to the DragDropOp structure, passed as the user-definable parameter.

         static PDROPDEST pDropDestOp;
         pDropDestOp = (PDROPDEST) PVOIDFROMMP (mp2);

         // Set the possible length of the strings in the entry boxs.

         WinSendDlgItemMsg (hwnd, DDENTRY_ID, EM_SETTEXTLIMIT,
                                                             MPFROMSHORT (CCHMAXPATH + 1), 0);
         WinSendDlgItemMsg (hwnd, DDNAME_ID, EM_SETTEXTLIMIT,
                                                             MPFROMSHORT (CCHMAXPATH + 1), 0);

         // Set up the proper checked state of the radiobuttons according to the state of
         // pDropDestOp->Operation.  We can only check for COPY or MOVE operations since
         // they are the only two that can be identified in the DM_DROP message of the
         // calling procedure.

         if (pDropDestOp->Operation & 2)
         {
            WinSendDlgItemMsg (hwnd, DDCOPY_ID, BM_SETCHECK, MPFROMSHORT (TRUE), NULL);
            WinEnableControl (hwnd, DDNAME_ID, TRUE);
            WinEnableControl (hwnd, DDNAMETITLE_ID, TRUE);
         }
         else
         {
            WinSendDlgItemMsg (hwnd, DDMOVE_ID, BM_SETCHECK, MPFROMSHORT (TRUE), NULL);
            WinEnableControl (hwnd, DDNAME_ID, FALSE);
            WinEnableControl (hwnd, DDNAMETITLE_ID, FALSE);
         }

         // Set up the Drive stub.

         sznamebuffer [0] = CHAR (DropDrive + 64);
         sznamebuffer [1] = 58;
         sznamebuffer [2] = 92;
         sznamebuffer [3] = 0;

         // If the szDropPath string is not of zero length, it means the calling target was the
         // file or tree container and we must copy the destination path to the entry box.

         if (strlen (pDropDestOp->szDropPath))
         {
            PSZ pt;
            pt = &pDropDestOp->szDropPath [1];
            strcat (sznamebuffer, pt);
         }

         hwndtemp = WinWindowFromID (hwnd, DDENTRY_ID);
         WinSetWindowText (hwndtemp, sznamebuffer);

         // Set up a handle for the dialog container window.

         hwnddropcont = WinWindowFromID (hwnd, DDCONT_ID);

         // Fill the container with the root directories.
         // BUT first hide the files window to get rid of the flicker.

         WinSetWindowPos (hwnddropcont, HWND_TOP, 0, 0, 0, 0, SWP_HIDE);

         ddtree = new roottree (hwnddropcont, (SHORT) DropDrive);
         ddtree->FillContainer ();

         // Don't need a drive designator in this path since a discreet container is used
         // and has already been initialized to the drop drive.  The drive is sent as the
         // first parameter.

         if (strlen (pDropDestOp->szDropPath))
            RefillTree (DropDrive, USEPATH, hwnddropcont, ddtree, pDropDestOp->szDropPath );

         WinSetWindowPos (hwnddropcont, HWND_TOP, 0, 0, 0, 0, SWP_SHOW);

         // A flag is required to trigger the CN_SETFOCUS routine (below) ONCE only.

         static USHORT contflag;
         contflag = 0;

         return 0;

      case WM_CLOSE:

         WinAssociateHelpInstance (hwndhelp, hwndframe);
         pDropDestOp->Operation = 32;
         delete ddtree;
         break;


      case WM_CONTROL:

         switch (SHORT2FROMMP (mp1))
         {
            case CN_ENTER:

               PNOTIFYRECORDENTER pNRE;
               pNRE = PNOTIFYRECORDENTER (PVOIDFROMMP (mp2));

               PCNRITEM pEnterRec;
               pEnterRec = (PCNRITEM) pNRE->pRecord;

               // If the record pointer is NULL, the pointer was not over a record when
               // it was clicked so a break is called for.

               if (!pEnterRec) break;

               PSZ psztemp;
               psztemp = new CHAR [CCHMAXPATH * 2];

               strcpy (psztemp, pEnterRec->szDirPath);

               // If the enter key (or mouse double-click) was over a folder when it
               // was pressed, that folder is expanded in the dialog container window.
               // A pointer to the tree object must be sent as the last parameter to
               // distinguish the caller from the non-drag/drop caller which sends a NULL.

               if (pNRE->hwndCnr == hwnddropcont)
               {
                  DoTree (hwnddropcont, pEnterRec, ddtree);
               }

               // Create a path string including drive designator and write it to the
               // dialog entryfield.

               sznamebuffer [0] = CHAR (DropDrive + 64);
               sznamebuffer [1] = 58;
               sznamebuffer [2] = 0;
               if (strlen (psztemp))
                  strcat (sznamebuffer, psztemp);
               else
               {
                  sznamebuffer [2] = 92;
                  sznamebuffer [3] = 0;
               }
               delete psztemp;

               hwndtemp = WinWindowFromID (hwnd, DDENTRY_ID);
               WinSetWindowText (hwndtemp, sznamebuffer);
               return 0;

            case CN_COLLAPSETREE:

               // This section removes child records from the container being closed.
               // This will ensure that the next time this folder is opened, the contents
               // will be up-to-date.

               CollapseTreeFix ((PCNRITEM) PVOIDFROMMP (mp2), hwnddropcont, ddtree);

               return 0;

            case CN_EXPANDTREE:

               ExpandTreeFix ((PCNRITEM) PVOIDFROMMP (mp2), hwnddropcont);
               return 0;

            // We need to take note whenever a radio button is checked in order to control
            // the Enabling/Disabling of the DDNAME_ID entry box.

            case BN_CLICKED:

               if (SHORT1FROMMP (mp1) == DDCOPY_ID)
               {
                  WinEnableControl (hwnd, DDNAME_ID, TRUE);
                  WinEnableControl (hwnd, DDNAMETITLE_ID, TRUE);
               }
               else if (SHORT1FROMMP (mp1) == DDMOVE_ID)
               {
                  WinEnableControl (hwnd, DDNAME_ID, FALSE);
                  WinEnableControl (hwnd, DDNAMETITLE_ID, FALSE);
               }
               else if (SHORT1FROMMP (mp1) == DDZIP_ID)
               {
                  WinEnableControl (hwnd, DDNAME_ID, FALSE);
                  WinEnableControl (hwnd, DDNAMETITLE_ID, FALSE);
               }
               else if (SHORT1FROMMP (mp1) == DDUNZIP_ID)
               {
                  WinEnableControl (hwnd, DDNAME_ID, FALSE);
                  WinEnableControl (hwnd, DDNAMETITLE_ID, FALSE);
               }
               else if (SHORT1FROMMP (mp1) == DDAPPEND_ID)
               {
                  WinEnableControl (hwnd, DDNAME_ID, FALSE);
                  WinEnableControl (hwnd, DDNAMETITLE_ID, FALSE);
               }
               break;

            case CN_HELP:
               WinSendMsg (hwndhelp, HM_DISPLAY_HELP, MPFROMSHORT (1000),
                                                                    MPFROMSHORT (HM_RESOURCEID));
               break;
         }
         // Be sure the operation is set to CANCEL before we leave here (in case of an
         // Escape key.

         pDropDestOp->Operation = 32;
         break;

      case WM_COMMAND:

         switch (SHORT1FROMMP (mp1))
         {
            case DDOK_ID:

               // Fill in the DROPDEST structure which could have members changed during this
               // dialog.  This is needed when the dialog returns to the caller.

               // Examine the radio buttons to find out if the operation should be a Copy, Move,
               // Zip or Unzip.

               USHORT opstate;

               hwndtemp = WinWindowFromID (hwnd, DDCOPY_ID);
               opstate = (USHORT) WinSendMsg (hwndtemp, BM_QUERYCHECK, NULL, NULL);
               if (opstate) pDropDestOp->Operation = 2;

               hwndtemp = WinWindowFromID (hwnd, DDMOVE_ID);
               opstate = (USHORT) WinSendMsg (hwndtemp, BM_QUERYCHECK, NULL, NULL);
               if (opstate) pDropDestOp->Operation = 4;

               hwndtemp = WinWindowFromID (hwnd, DDZIP_ID);
               opstate = (USHORT) WinSendMsg (hwndtemp, BM_QUERYCHECK, NULL, NULL);
               if (opstate) pDropDestOp->Operation = 8;

               hwndtemp = WinWindowFromID (hwnd, DDUNZIP_ID);
               opstate = (USHORT) WinSendMsg (hwndtemp, BM_QUERYCHECK, NULL, NULL);
               if (opstate) pDropDestOp->Operation = 16;

               hwndtemp = WinWindowFromID (hwnd, DDAPPEND_ID);
               opstate = (USHORT) WinSendMsg (hwndtemp, BM_QUERYCHECK, NULL, NULL);
               if (opstate) pDropDestOp->Operation = 128;

               // Examine the 'Replace Existing Files' check box in the dialog to find out
               // if the files are to be overwritten if they already exist.
               // 'OR' this value with the operation value.

               hwndtemp = WinWindowFromID (hwnd, DDREPLACE_ID);
               opstate = (USHORT) WinSendMsg (hwndtemp, BM_QUERYCHECK, NULL, NULL);
               if (opstate) pDropDestOp->Operation |= 1;

               // Get the text from the entry fields which is limited to 'CCHMAXPATH + 1'.

               pDropDestOp->ccPathname = (USHORT) WinQueryDlgItemText (hwnd, DDENTRY_ID,
                                                       CCHMAXPATH + 1, pDropDestOp->szDropPath);
               WinQueryDlgItemText (hwnd, DDNAME_ID, CCHMAXPATH + 1,
                                                                            pDropDestOp->szDropName);
               // Dismiss the dialog and return to the caller.

               WinDismissDlg (hwnd, TRUE);
               break;

            // On a Cancel, make the operation equal 32, no ORs, ANDS, or buts!

            case DDCANCEL_ID:

               pDropDestOp->Operation = 32;
               WinDismissDlg (hwnd, TRUE);
               break;

            case DDHELP_ID:
               // Using brackets here to keep these variables local and temporary.
               {
                  USHORT helpstate, helpcode;

                  hwndtemp = WinWindowFromID (hwnd, DDCOPY_ID);
                  helpstate = (USHORT) WinSendMsg (hwndtemp, BM_QUERYCHECK, NULL, NULL);
                  if (helpstate) helpcode = 2110;

                  hwndtemp = WinWindowFromID (hwnd, DDAPPEND_ID);
                  helpstate = (USHORT) WinSendMsg (hwndtemp, BM_QUERYCHECK, NULL, NULL);
                  if (helpstate) helpcode = 2115;

                  hwndtemp = WinWindowFromID (hwnd, DDMOVE_ID);
                  helpstate = (USHORT) WinSendMsg (hwndtemp, BM_QUERYCHECK, NULL, NULL);
                  if (helpstate) helpcode = 2120;

                  hwndtemp = WinWindowFromID (hwnd, DDZIP_ID);
                  helpstate = (USHORT) WinSendMsg (hwndtemp, BM_QUERYCHECK, NULL, NULL);
                  if (helpstate) helpcode = 2530;

                  hwndtemp = WinWindowFromID (hwnd, DDUNZIP_ID);
                  helpstate = (USHORT) WinSendMsg (hwndtemp, BM_QUERYCHECK, NULL, NULL);
                  if (helpstate) helpcode = 2540;

                  WinSendMsg (hwndhelp, HM_DISPLAY_HELP, MPFROMSHORT (helpcode),
                                                                    MPFROMSHORT (HM_RESOURCEID));
               }
               return 0;
         }
         break;
      }
   return WinDefDlgProc (hwnd, msg, mp1, mp2);
}
//-------------------------------------------------------------------------
// This dialog presents a list of ten directoriy bookmarks so that the user can choose
// from them to reload as the new source directory.
// The user can also modify them by inserting the current dir into the selected bookmark.
//-------------------------------------------------------------------------
MRESULT EXPENTRY DirBookmarkDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   switch (msg)
   {
      // The next three messages are used mainly to set up the help system for the dialog.

      case WM_ACTIVATE:
         WinAssociateHelpInstance (hwndhelp, hwnd);
         break;

      case WM_CLOSE:
         WinAssociateHelpInstance (hwndhelp, hwndframe);
         break;

      case WM_HELP:
         WinSendMsg (hwndhelp, HM_DISPLAY_HELP, MPFROMSHORT (2250),
                                                                    MPFROMSHORT (HM_RESOURCEID));
         break;

      case WM_INITDLG:

         LONG ix, iy;
         SWP   tmpswp;
         RECTL tmprctl;
         POINTL tmppointl;

         // This opening code is to position the dialog window closer to the bookmark
         // button so that the user doesn't have to move the cursor so far to pick a bookmark.

         // Query width and height of dialog box

         WinQueryWindowPos (hwnd, &tmpswp);

         // Query width and height of main window frame

         WinQueryWindowRect (hwndframe, &tmprctl);

         // Adjust height and width so that dialog can be positioned starting about the
         // centre of the window.

         ix = (tmprctl.xRight - tmprctl.xLeft) - tmpswp.cx;
         iy = (tmprctl.yTop - tmprctl.yBottom) - tmpswp.cy;

         if (ix < 0) ix = 0;
         if (iy < 0) iy = 0;

         // Map these window coordinates to screen coordinates.

         tmppointl.x = ix;
         tmppointl.y = iy;
         WinMapWindowPoints (hwndframe, HWND_DESKTOP, &tmppointl, 1);
         WinSetWindowPos (hwnd, HWND_TOP, tmppointl.x, tmppointl.y, 0, 0, SWP_MOVE );

         static HWND hwndlistbox;
         hwndlistbox = WinWindowFromID (hwnd, HISTORYLISTID);

         // Load the bookmarks from that have been saved in the FF ini file.  If the bookmarks
         // are blank, it won't matter.
         // Also keep track of the first open bookmark and move the selection cursor there
         // if there is one.  Otherwise place the selection bar on the first entry.
         // This will facilitate adding the next bookmark since
         // new bookmarks are inserted wherever the selection bar is resting.

         ii = 0;
         for (i = 0; i < 10; i++)
         {
            WinSendMsg (hwndlistbox, LM_INSERTITEM,
                               MPFROMSHORT (LIT_END), MPFROMP (szDirHist [i]));
            if (szDirHist [i][0] != 0) ii++;
         }
         if (ii == 10) ii = 0;
            WinSendMsg (hwndlistbox, LM_SELECTITEM, MPFROMSHORT (ii),
                                                                              MPFROMSHORT (TRUE));
         return 0;

      case WM_COMMAND:

         switch (SHORT1FROMMP (mp1))
         {
            case CANCELHISTORYID:

               WinDismissDlg (hwnd, TRUE);
               break;

            case ADDBOOKMARKID:

               // Bookmarks are inserted into the position that the selection cursor rests on.

               i = (SHORT) WinSendMsg (hwndlistbox, LM_QUERYSELECTION, NULL, NULL);

               WinSendMsg (hwndlistbox, LM_INSERTITEM, MPFROMSHORT (i),
                                                                              MPFROMP (szFilesTitle));
               // Copy over the storage string for this bookmark so the new one is saved.

               strcpy (szDirHist [i], szFilesTitle);
               break;

            case BOOKMARKHELPID:
               // This help is here so that users can access it with just the mouse.

               WinPostMsg (hwndhelp, HM_DISPLAY_HELP, MPFROMSHORT (2250),
                                                                    MPFROMSHORT (HM_RESOURCEID));
               return 0;
         }
         break;

      case WM_CONTROL:
         switch (SHORT2FROMMP (mp1))
         {
            case LN_ENTER:
               // When the bookmark is double-clicked on, the path selected must be loaded
               // into the files window and the drive must be loaded into the tree window.

               i = (SHORT) WinSendMsg (hwndlistbox, LM_QUERYSELECTION, NULL, NULL);

               WinSendMsg (hwndlistbox, LM_QUERYITEMTEXT,
                            MPFROM2SHORT (i, CCHMAXPATH), MPFROMP (sznamebuffer));
               // Trap unset bookmarks

               if (strlen (sznamebuffer))
               {
                  WinDismissDlg (hwnd, TRUE);

                  strcpy (szFilesTitle, sznamebuffer);
                  CHAR *pTitle = szFilesTitle;
                  SecondChild->newtitle (pTitle);

                  // This section fills the Files container with the files in this
                  // new folder that is being opened in the Directory container.

                  i = szFilesTitle [0];
                  if (i > 96) i -= 96;
                  else i -= 64;

                  if (i != CurrDrive)
                  {
                     CurrDrive = i;
                     DosSetDefaultDisk (CurrDrive);

                     // Rebuild the Tree window and indicate for the refilling routine that it is a
                     // a Bookmark path so that the folders can be opened.

                     RefillTree (CurrDrive, USEPATH, hwnddircntr, DTree, NULL);

                     // Get the file system info and format a title for the container with
                     // the drive designation and the bytes free.

                     getFSInfo (CurrDrive);
                  }
                  pTitle = &szFilesTitle [2];
                  if (szFilesTitle [3] == 0) pTitle = NULL;
                  DFiles->PathToOrder (pTitle, (SHORT) CurrDrive);

                  tparms3.Hab = hab3;
                  tparms3.Hmq = hmq3;
                  tparms3.job = FILES;
                  tid3 = _beginthread (threadall, NULL, 8192, &tparms3);
               }
               break;
         }
         break;
   }
   return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

//------------------------------------------------------------------------------
// This dialog handles the writing of Volume labels.
//------------------------------------------------------------------------------
MRESULT EXPENTRY VolumeLabelDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   switch (msg)
   {
      // The next three messages are used mainly to set up the help system for the dialog.

      case WM_ACTIVATE:
         WinAssociateHelpInstance (hwndhelp, hwnd);
         break;

      case WM_CLOSE:
         WinAssociateHelpInstance (hwndhelp, hwndframe);
         break;

      case WM_HELP:
         WinSendMsg (hwndhelp, HM_DISPLAY_HELP, MPFROMSHORT (2340),
                                                                    MPFROMSHORT (HM_RESOURCEID));
         break;

      case WM_INITDLG:

         static HWND hwndcombobox;
         static HWND hwndEntryfield;

         hwndcombobox = WinWindowFromID (hwnd, DRIVELISTID);

         // Set the possible length of the volume label string in the entry box.

         hwndEntryfield = WinWindowFromID (hwnd, LABELFIELDID);
         WinSendDlgItemMsg (hwnd, LABELFIELDID, EM_SETTEXTLIMIT,
                                                             MPFROMSHORT (12), 0);
         // Load the drives from from the drive button list.

         int drives;
         drives = totaldrives - 1; // Subtract the wild card for directory bookmarks.
         ii = 0;

         for (i = 0; i < drives; i++)
         {

            // Szctemp is massaged to hold the drive designation.
            // The drive designation is derived from the label since there may be
            // gaps in the order.  The labels are mapped to the actual drive array.

            CHAR szctemp [2];
            szctemp [0] = (CHAR) pLabelarray [i * 4];
            szctemp [1] = 0;

            WinSendMsg (hwndcombobox, LM_INSERTITEM,
                               MPFROMSHORT (LIT_END), MPFROMP (szctemp));
         }
         // Select the first item in the list box so that something shows up.

         WinSendMsg (hwndcombobox, LM_SELECTITEM, MPFROMSHORT (0), MPFROMSHORT (TRUE));

         // Copy the current Volume label into the entryfield.

         static UCHAR * cptr;
         cptr = &ucVoldat [0][5];

         WinSetWindowText (hwndEntryfield, (PSZ) cptr);

         return 0;

      case WM_COMMAND:

         switch (SHORT1FROMMP (mp1))
         {
            case CANCELLABELID:

               WinDismissDlg (hwnd, TRUE);
               break;

            case ACCEPTLABELID:

               // The volume label has been edited or created.
               // Get this new label and write it to the partition.

               sznamebuffer [0] = 0;
               PSZ ptemp;
               ptemp = &sznamebuffer [1];

               WinQueryDlgItemText (hwnd, LABELFIELDID, 12, ptemp);
               sznamebuffer [0] = (CHAR) strlen (ptemp);

               // Convert all characters to uppercase.  They won't be accepted otherwise.

               strupr (ptemp);

               // Find out which partition is being written to.

               i = (SHORT) WinSendMsg (hwndcombobox, LM_QUERYSELECTION, NULL, NULL);

               WinSendMsg (hwndcombobox, LM_QUERYITEMTEXT, MPFROM2SHORT (i, 1),
                                                        MPFROMP (szpathbuffer));
               i = szpathbuffer [0] - 64;

               // Write the label.

               rc = DosSetFSInfo (i, 2, sznamebuffer, 20);

               if (rc)
               {
                  DosBeep (200,100);
                  strcpy (sznamebuffer, "Error writing Volume Label");
                  if (rc == 123)
                     strcpy (sznamebuffer, "Error writing Volume Label - Invalid Name!");

                  WinMessageBox (HWND_DESKTOP, hwndframe, sznamebuffer,
                                                    "ERROR!", MESSAGEBOXID, MB_OK | MB_ERROR);
                  // Return to the dialog.
                  return 0;
               }
               else
                  // Refresh all of the volume labels including the floppies.

                  RefreshVolLabels (1, FALSE);
               break;
         }
         break;

      case WM_CONTROL:

         switch (SHORT2FROMMP (mp1))
         {
            // NOTE:
            // The combobox control uses most of the same messages as the entryfield control
            // and the listbox control as well as it's own few particular messages.
            // Here I have used a listbox notification message and a specific combobox
            // notification message.

            case LN_SELECT:

               // Whenever a drive letter is highlighted, this bit will display the current
               // Volume label for that drive dynamically.

               i = (SHORT) WinSendMsg (hwndcombobox, LM_QUERYSELECTION, NULL, NULL);

               WinSendMsg (hwndcombobox, LM_QUERYITEMTEXT, MPFROM2SHORT (i, 1),
                                                        MPFROMP (szpathbuffer));
               i = szpathbuffer [0] - 65;

               // Copy the current Volume label into the entryfield.

               cptr = &ucVoldat [i][5];

               WinSetWindowText (hwndEntryfield, (PSZ) cptr);

               break;

            case CBN_ENTER:

               // Set the focus to the entryfield now that a drive has been selected.

               WinSetFocus (HWND_DESKTOP, hwndEntryfield);
               break;
         }
         break;
   }
   return WinDefDlgProc (hwnd, msg, mp1, mp2);
}
MRESULT EXPENTRY TimedRefreshDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   switch (msg)
   {
      case WM_INITDLG:

         static HWND hwndspinbutton1;
         static HWND hwndspinbutton2;
         static HWND hwndcheckbox1;
         static HWND hwndcheckbox2;
         ULONG tempinterval;

         hwndcheckbox2 = WinWindowFromID (hwnd, DRIVELISTINGID);
         hwndcheckbox1 = WinWindowFromID (hwnd, FILESLISTINGID);

         // Set up the spin buttons and the checkboxes.
         // If there was a value saved in the ini file for a timer, check the checkbox
         // and stick that value in the spin button.
         // interval1 (for DirRefresh) and interval2 (for DriveRefresh) have been retrieved from
         // the ini file.

         hwndspinbutton1 = WinWindowFromID (hwnd, DIRECTORYINTERVALID);
         WinSendMsg (hwndspinbutton1, SPBM_OVERRIDESETLIMITS, MPFROMLONG (600),
                                                               MPFROMLONG (1));
         tempinterval = interval1 > 0 ? interval1 : 30;
         WinSendMsg (hwndspinbutton1, SPBM_SETCURRENTVALUE, MPFROMLONG (tempinterval),
                                                                               NULL);
         if (interval1) WinSendMsg (hwndcheckbox1, BM_SETCHECK, MPFROMSHORT (TRUE), NULL);

         hwndspinbutton2 = WinWindowFromID (hwnd, DRIVEINTERVALID);
         WinSendMsg (hwndspinbutton2, SPBM_OVERRIDESETLIMITS, MPFROMLONG (600),
                                                               MPFROMLONG (1));
         tempinterval = interval2 > 0 ? interval2 : 30;
         WinSendMsg (hwndspinbutton2, SPBM_SETCURRENTVALUE, MPFROMLONG (tempinterval),
                                                                                NULL);
         if (interval2) WinSendMsg (hwndcheckbox2, BM_SETCHECK, MPFROMSHORT (TRUE), NULL);
         return 0;

      case WM_CHAR:

         // Note:
         // The escape key is intercepted here since this dialog is just a page in the
         // settings notebook and does not receive it's own WM_CLOSE message when
         // the pages are turned or the escape key is pressed.  This interception allows the
         // the notebook to close when an escape key is pressed on any page.

         if ((SHORT1FROMMP (mp2)) & KC_VIRTUALKEY)
         {
            // Throw away all up-stroke keys.

            if  ((SHORT1FROMMP (mp1)) & KC_KEYUP) break;
            if  ((SHORT2FROMMP (mp2)) & VK_ESC)
            {
               WinSendMsg (hwndsettings, WM_CLOSE, NULL, NULL);
               return 0;
            }
         }
         break;

      case WM_CONTROL:

         switch(SHORT2FROMMP (mp1))
         {
            // Each time a spin button has its value changed, a message is received here.

            case SPBN_CHANGE:

               USHORT state;
               if (SHORT1FROMMP (mp1) == DIRECTORYINTERVALID)
               {
                  state = (USHORT) WinSendMsg (hwndcheckbox1, BM_QUERYCHECK, NULL, NULL);
                  if (state) SetTimers (hwndcheckbox1, hwndspinbutton1, DIRTIMER, &interval1);
               }
               // If it's not the directories checkbox, it's the Drives checkbox.
               else
               {
                  state = (USHORT) WinSendMsg (hwndcheckbox2, BM_QUERYCHECK, NULL, NULL);
                  if (state) SetTimers (hwndcheckbox2, hwndspinbutton2, DRIVETIMER, &interval2);
               }
               return 0;

            case BN_CLICKED:

               // Each time a checkbox is clicked on, a message is received here.
               // If the checkbox is checked or unchecked, either way a timer must
               // be reset (either to zero or some value).

               if (LONGFROMMP (mp2) == hwndcheckbox1)
                  SetTimers (hwndcheckbox1, hwndspinbutton1, DIRTIMER, &interval1);

               if (LONGFROMMP (mp2) == hwndcheckbox2)
                  SetTimers (hwndcheckbox2, hwndspinbutton2, DRIVETIMER, &interval2);

               return 0;
         }
         break;
   }
   return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

//-------------------------------------------------------------------------
// The Append function will take a list of files and append them to the target file.
// The target may be selected from the list of selected files.
// This is intended for ascii files and the files will be separated by a line of asterisks
// that include the original file name.
//-------------------------------------------------------------------------
MRESULT EXPENTRY AppendDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   switch (msg)
   {
      // The next three messages are used mainly to set up the help system for the dialog.

      case WM_ACTIVATE:
         WinAssociateHelpInstance (hwndhelp, hwnd);
         break;

      case WM_CLOSE:
         WinAssociateHelpInstance (hwndhelp, hwndframe);
         break;

      case WM_HELP:
         WinSendMsg (hwndhelp, HM_DISPLAY_HELP, MPFROMSHORT (2115),
                                                                    MPFROMSHORT (HM_RESOURCEID));
         break;

      case WM_INITDLG:

         static HWND hwndlistbox;
         static HWND hEntryfield;

         hwndlistbox = WinWindowFromID (hwnd, APPENDLISTID);
         hEntryfield = WinWindowFromID (hwnd, APPENDNAMEID);

         // Set the string length of the Target name entryfield.

         WinSendDlgItemMsg (hwnd, APPENDNAMEID, EM_SETTEXTLIMIT,
                                                              MPFROMSHORT (CCHMAXPATH + 1), 0);

         // Check for notification that the record being sent was a single unselected record
         // under the mouse during a Drag/Drop operation.  If this is the case, this record
         // must be used and all selected records ignored.

         if (DropDestOp.recundermouse)
         {
            pWhatRec = (PCNRITEM) DropDestOp.pRec;
            WinSendMsg (hwndlistbox, LM_INSERTITEM,
                      MPFROMSHORT (LIT_SORTASCENDING), MPFROMP (pWhatRec->pszFileName));
            WinSendMsg (hwndlistbox, LM_SETITEMHANDLE, MPFROMSHORT (0),
                                                                       MPFROMLONG (pWhatRec));
            DropDestOp.recundermouse = 0;
         }
         else
         {
            SHORT Rindex;
            pWhatRec = (PCNRITEM) CMA_FIRST;

            // Scan for the selected records and then insert them into the listbox.  Attach the
            // Record pointer to each item as a handle so that it can be recovered later.

            while (pWhatRec)
            {
               pWhatRec = (PCNRITEM) WinSendMsg (hwndfocus,
                                     CM_QUERYRECORDEMPHASIS, MPFROMP (pWhatRec),
                                                            MPFROMSHORT (CRA_SELECTED));
               if (pWhatRec)
               {
                  Rindex = (SHORT) WinSendMsg (hwndlistbox, LM_INSERTITEM,
                         MPFROMSHORT (LIT_SORTASCENDING), MPFROMP (pWhatRec->pszFileName));
                  WinSendMsg (hwndlistbox, LM_SETITEMHANDLE, MPFROMSHORT (Rindex),
                                                                          MPFROMLONG (pWhatRec));
               }
            }
         }
         // Fill in a default Target file name for the entryfield.

         strcpy (szpathbuffer, szDestPath);
         if (strlen (szDestPath) != 3)
            strcat (szpathbuffer, "\\");
         strcat (szpathbuffer, "FFappend.txt");
         WinSetWindowText (hEntryfield, szpathbuffer);

         // Change the focus to the entry field so as to keep the default button from changing.
         // Must return TRUE for the change to take effect.

         WinSetFocus (HWND_DESKTOP, hEntryfield);
         return (PVOID) TRUE;

      case WM_COMMAND:

         switch (SHORT1FROMMP (mp1))
         {
            case APPENDCANCELID:

               WinDismissDlg (hwnd, TRUE);
               break;

            case APPENDHELPID:

               WinSendMsg (hwndhelp, HM_DISPLAY_HELP, MPFROMSHORT (2115),
                                                                    MPFROMSHORT (HM_RESOURCEID));
               return 0;

            case APPENDOKID:

               // Check to be sure there are items in the listbox.  If there are not, there is
               // nothing to append so exit with a beep!

               SHORT cItems;

               cItems = (SHORT) WinSendMsg (hwndlistbox, LM_QUERYITEMCOUNT, NULL, NULL);
               if (!cItems)
               {
                  WinDismissDlg (hwnd, TRUE);
                  DosBeep (100,50);
                  return 0;
               }
               // This array will have to be deleted if the routine bombs out.  It's used to
               // remove records from the container if the source files are marked for
               // removal after appending.

               ULONG *pRecArray;
               pRecArray = new ULONG [cItems];

               // Put up the hourglass cursor.

               WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP,
                                                                                 SPTR_WAIT, FALSE));

               // Get the Target file name.

               WinQueryDlgItemText (hwnd, APPENDNAMEID, CCHMAXPATH + 1, sznamebuffer);

               // Save the name of the target file so that we can return to it if the
               // screen needs rebuilt.

               PSZ pName;
               int iLength;

               iLength = strlen (szDestPath);
               pName = &sznamebuffer [iLength + 1];  // Allow for the backslash
               strcpy (tparms7.szSavename, pName);

               // Check the source and destination drives to be sure they are ready.
               // The single error trap will catch either error.

               ULONG dDrive;
               dDrive = sznamebuffer [0];
               if (dDrive > 96) dDrive -= 96;
               else dDrive -= 64;
               rc = IsDriveReady (dDrive);

               // Test for source drive ready (media inserted etc.).

               if (rc)
                  rc = IsDriveReady (CurrDrive);
               if (!rc)
               {
                           WinDismissDlg (hwnd, TRUE);
                           DosBeep (100,50);
                           return 0;
               }

               // Retrieve the selected record's Record pointers from the listbox.
               // Append each file in turn and add a separator to the mother file that includes
               // the child file's name in a line formatted like this:
               // +-+-+-+-+-filename-+-+-+-+-+
               // Examine the 'Delete Source Files' check box in the dialog to find out
               // if the files are to be deleted once appended.

               hwndtemp = WinWindowFromID (hwnd, APPENDDELETEID);
               static USHORT deletestate;
               deletestate = (USHORT) WinSendMsg (hwndtemp, BM_QUERYCHECK, NULL, NULL);

               HFILE hFile;
               ULONG Action;
               ULONG outlength, ulindex;

               int i;
               int ii;
               ii = 0;
               for (i =0; i < cItems; i++)
               {

                  pWhatRec = (PCNRITEM) WinSendMsg (hwndlistbox, LM_QUERYITEMHANDLE,
                                                                        MPFROMSHORT (i), NULL);
                  if (pWhatRec)
                  {
                     pRecArray [ii] = (ULONG) pWhatRec;

                     // Open the target file (mother) so that a separator line can be written
                     // to it before the file being appended is added.

                     rc = DosOpen (sznamebuffer, &hFile, &Action, NULL, NULL,
                                 OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
                                  OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE
                                  | OPEN_FLAGS_WRITE_THROUGH , NULL);

                     if (rc)
                     {
                        MessageBox (hwnd, rc);
                        pWhatRec = 0;
                        ii--;
                        break;
                     }
                     // The pointer must be set to the end of the file each time
                     // because the pointer always stays at the start of the file
                     // for some reason.

                     DosSetFilePtr (hFile, NULL, FILE_END, &ulindex);

                    // Format a separator which will include the original file name
                    // of the current file being appended.
                    // This separator will be written to the file at the beginning of
                    // each append.

                     strcpy (szpathbuffer, "\n\n+-+-+-+-+-");
                     strcat (szpathbuffer, pWhatRec->szFileName);
                     strcat (szpathbuffer, "-+-+-+-+-+\n");

                     // Write the separator line and close the file.

                     rc = DosWrite (hFile, szpathbuffer, strlen (szpathbuffer), &outlength);
                     if (rc)
                     {
                        MessageBox (hwnd, rc);
                        pWhatRec = 0;
                        ii--;
                        break;
                     }

                     DosClose (hFile);

                     // Now we can do the append operation.

                     szpathbuffer [0] = CHAR (CurrDrive + 64);
                     szpathbuffer [1] = 58;
                     szpathbuffer [2] = 0;
                     strcat (szpathbuffer, pWhatRec->szDirPath);

                     rc = DosCopy (szpathbuffer, sznamebuffer, DCPY_APPEND | DCPY_EXISTING);

                     // If there was an error, take the error number to the MessageBox routine
                     // for display of the error.  After display, return to the dialog.

                     ii++;
                     if (rc)
                     {
                        MessageBox (hwnd, rc);
                        pWhatRec = 0;
                        ii--;
                        break;
                     }
                     else
                     {
                        if (deletestate) DosDelete (szpathbuffer);
                     }

                  }
                  WinSendMsg (hwndfocus, CM_SETRECORDEMPHASIS, MPFROMLONG (pWhatRec),
                                 MPFROM2SHORT (FALSE, CRA_SELECTED));

                  if (!pWhatRec) i = cItems;
               }
               if (deletestate)
               {
                  // Remove the records from the container and invalidate the display if
                  // there have been deletions.

                  if (ii) WinSendMsg (hwndfocus, CM_REMOVERECORD, MPFROMP (pRecArray),
                                           MPFROM2SHORT (ii, CMA_FREE | CMA_INVALIDATE));

                  // Update the root window titlebar byte count.

                  getFSInfo (CurrDrive);

                  // Update the Destination status bar info with new bytes free etc.

                  fillStatusbar1 (DestDrive, (PSZ) szDestPath);
               }
               delete pRecArray;

               success = CompareandRefresh ();
               if (success)
               {
                  // Get the cursor back on the original file.

                  tparms7.Hab = NULL;
                  tparms7.Hmq = NULL;
                  tparms7.hwnd = hwndfilescntr;
                  tparms7.job = RETURNTO;
                  tid7 = _beginthread (threadall, NULL, 8192, &tparms7);
               }

               // Put up the regular cursor.

               WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP,
                                                                                 SPTR_ARROW, FALSE));
               WinDismissDlg (hwnd, TRUE);
               break;
         }

      case WM_CONTROL:
         switch (SHORT2FROMMP (mp1))
         {
            case LN_ENTER:

               // Remove a filename from the list in the list box.  Also unselect the
               // record from the files container.

               SHORT index;
               PCNRITEM pRec;

               index = (SHORT) WinSendMsg (hwndlistbox, LM_QUERYSELECTION, NULL, NULL);
               if (index != LIT_NONE)
               {
                  pRec = (PCNRITEM) WinSendMsg (hwndlistbox, LM_QUERYITEMHANDLE,
                                                               MPFROMSHORT (index), NULL);
                  WinSendMsg (hwndfocus, CM_SETRECORDEMPHASIS, MPFROMLONG (pRec),
                                                MPFROM2SHORT (FALSE, CRA_SELECTED));
                  strcpy (szpathbuffer, szDestPath);
                  strcat (szpathbuffer, "\\");
                  strcat (szpathbuffer, pRec->szFileName);

                  WinSetWindowText (hEntryfield, szpathbuffer);
                  WinSendMsg (hwndlistbox, LM_DELETEITEM, MPFROMSHORT (index), NULL);
                  return 0;
               }
         }
         break;
   }
   return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

//-------------------------------------------------------------------------
//This function is called to initially make all the modeless windows fit the client window
//and to make them visible.  It is also called whenever the window has been resized.
//-------------------------------------------------------------------------
void resize ()
{
      WinQueryWindowPos (hwndclient, (PSWP)&swp);
      if (flag&8) pdrivedim = FFdrivebar->resizewin();
      if (flag&4) ptooldim = FFtoolbar->resizewin();

      if ((flag&1) && (flag&2))
         {
            int newvertdim = pdrivedim->vert + ptooldim->vert;
            int newhorzdim = pdrivedim->horz + ptooldim->horz;
            FirstChild->resizewin(newhorzdim, newvertdim);
            SecondChild->resizewin(newhorzdim, newvertdim);
         }
}

//---------------------------------------------------------------------------
// This function would be declared with a __syscall modifier if it was being called by
//  DosCreateThread.  This is the thread dispatcher.
//----------------------------------------------------------------------------
void threadall (PVOID tparmref)
{
   tp = (threadparms *) tparmref;

   switch (tp->job)
   {
      case TREE:

         flag &= 191;           // Flag is used in ChangeDrive routine.

         tp1 = (threadparms *) tparmref;
         tp1->Hab = WinInitialize (0);
         tp1->Hmq = WinCreateMsgQueue (tp1->Hab, 0);

         // Fill the directory tree.

         DTree->FillContainer ();

         WinDestroyMsgQueue (tp1->Hmq);
         WinTerminate (tp1->Hab);

         flag |= 64;
         break;

      case FILES:

         flag &= 127;      // Used in ChangeDrive routine.

         tp2 = (threadparms *) tparmref;
         tp2->Hab = WinInitialize (0);
         tp2->Hmq = WinCreateMsgQueue (tp2->Hab, 0);

         // Fill the files container and update the source status bar.

         DFiles->FillContainer (filefilter);

         // This statusbar information can't be gathered until the container is filled so it must
         // follow it in this thread.  It can't be done outside of the thread unless the container
         // is filled outside of the thread.

         fillStatusbar2 (NULL, NULL);

         WinDestroyMsgQueue (tp2->Hmq);
         WinTerminate (tp2->Hab);

         flag |= 128;
         break;

      case INI:

         tp3 = (threadparms *) tparmref;
         tp3->Hab = WinInitialize (0);
         tp3->Hmq = WinCreateMsgQueue (tp3->Hab, 0);

         // Set the current drive and directory to the starting path (where the executable
         // file was located).

         DosSetDefaultDisk (Homedrive);
         DosSetCurrentDir (szHomepath);

         // Initialize the last selected viewer item to zero;

         comboitem = 0;

         // Set the various menu view items as checked or unchecked.

         if (!(filefilter & FILE_HIDDEN))
            WinSendMsg (hwndmenu, MM_SETITEMATTR, MPFROM2SHORT
                                                       (MID_SHOWHIDDEN, TRUE), MPFROM2SHORT
                                                                                (MIA_CHECKED, NULL));
         if (!(filefilter & FILE_SYSTEM))
            WinSendMsg (hwndmenu, MM_SETITEMATTR, MPFROM2SHORT
                                                       (MID_SHOWSYSTEM, TRUE), MPFROM2SHORT
                                                                                (MIA_CHECKED, NULL));
         if (toolbarorient & 2)
            WinSendMsg (hwndmenu, MM_SETITEMATTR, MPFROM2SHORT
                                                       (MID_TOOLBAR, TRUE), MPFROM2SHORT
                                                                                (MIA_CHECKED, NULL));
         // Set some of the unchanging items in the PROGDETAILS structure.

         pDetails = &Details;
         pDetails->Length          = sizeof(PROGDETAILS);
         pDetails->pszParameters   = NULL;
         pDetails->pszStartupDir   = NULL;
         pDetails->pszIcon         = NULL;
         pDetails->pszEnvironment  = "\0\0";
         pDetails->swpInitial.fl   = SWP_ACTIVATE; // window positioning //
         pDetails->swpInitial.cy   = 0;    // width of window //
         pDetails->swpInitial.cx   = 0;    // height of window //
         pDetails->swpInitial.y    = 0;    // lower edge of window //
         pDetails->swpInitial.x    = 0;    // left edge of window //
         pDetails->swpInitial.hwndInsertBehind = HWND_TOP;
         pDetails->swpInitial.hwnd             = hwndfilescntr;
         pDetails->swpInitial.ulReserved1      = 0;
         pDetails->swpInitial.ulReserved2      = 0;

         // Set up the unchanging data members in the STARTDATA structure.

         StartData.Length = sizeof(STARTDATA); // Length of STARTDATA structure //
         StartData.TraceOpt = SSF_TRACEOPT_NONE; // Don't trace session //
         StartData.TermQ = 0;   // Assume no termination queue //
         StartData.Environment = 0; // Assume no environment string //
         StartData.InheritOpt = 1;   // Inherit programs environment//
         StartData.IconFile = 0; // Assume no specific icon file is provided   //
         StartData.PgmHandle = 0; // Do not use the installation file //
         StartData.Reserved = 0; // Reserved, must be zero //
         StartData.ObjectBuffer = ObjBuf; // Object buffer to hold failure causes //
         StartData.ObjectBuffLen = 100; // Size of object buffer //

         // Load the initialization info from the ini file.

         hini = PrfOpenProfile (hab1, "FileFrdm.ini");
         PrfQueryProfileSize (hini, "FileFrdm.exe", "ViewerType", &ViewerTypeslen);

         // If there is no data registered to 'ViewerType' the defaults will be reloaded
         // for the Viewer Types and the Viewer Names.

         if (!ViewerTypeslen)
         {
            SetViewerDefaults ();
         }
         else
         {
            // See my notes on the PrfOpenProfile function in notebook.

            pszViewerTypes = new CHAR [ViewerTypeslen + 1];
            PrfQueryProfileData (hini, "FileFrdm.exe", "ViewerType", pszViewerTypes,
                                                                                &ViewerTypeslen);
            APIRET testforoldformat;
            ULONG Datalen;
            testforoldformat = (BOOL) PrfQueryProfileSize (hini, "FileFrdm.exe", "ViewerName",
                                                                                              &Datalen);
//**       // This little section will update the format of the viewers stored in the INI file.
            // Formerly, only 13 chars were allowed for each viewer name, but now I've increased
            // it to 51.  This can be modified after about version 2.1 or so.

            if (testforoldformat)
            {
               ULONG viewercount;
               PSZ tempstorage;
               tempstorage = new CHAR [Datalen];
               viewercount = Datalen/13;
               PrfQueryProfileData (hini, "FileFrdm.exe", "ViewerName", tempstorage, &Datalen);

               pszViewers = new CHAR [viewercount * VIEWERSTRLEN + 1];
               PSZ pszold, psznew;
               for (int v = 0; v < viewercount; v++)
               {
                  psznew = &pszViewers [v * VIEWERSTRLEN];
                  pszold = &tempstorage [v * 13];
                  memcpy (psznew, pszold, 13);
               }
               PrfWriteProfileData (hini, "FileFrdm.exe", "ViewerName", NULL, NULL);
               delete tempstorage;
            }
            else
            {
               PrfQueryProfileSize (hini, "FileFrdm.exe", "Viewers", &Datalen);
               pszViewers = new CHAR [Datalen + 1];
               PrfQueryProfileData (hini, "FileFrdm.exe", "Viewers", pszViewers, &Datalen);
            }
//**
         }
         // Check the ini file for default editor strings.  If none exists (should happen on
         // installation) set EPM as the default editor.

         szDefaultEditor [0] = 0;
         Strlength = (ULONG) PrfQueryProfileString (hini, "FileFrdm.exe", "EdName", NULL,
                                                             (PSZ) szDefaultEditor, CCHMAXPATH + 1);

         // Normally the stringlength returned includes the null terminator so a zero-length string
         // returns 1.  If the key doesn't exist, however, the string length returned is NULL.

         if (Strlength < 2) strcpy (szDefaultEditor, "EPM.EXE");
         szDefaultEdParms [0] = 0;
         PrfQueryProfileString (hini, "FileFrdm.exe", "EdParms", NULL,
                                                          (PSZ) szDefaultEdParms, CCHMAXPATH + 1);

         // Check the ini file for default Search Tool strings.  If none exists (should happen on
         // installation) set PMSeek as the default search tool.

         szDefaultSearchTool [0] = 0;
         Strlength = (ULONG) PrfQueryProfileString (hini, "FileFrdm.exe", "SearchToolName", NULL,
                                                        (PSZ) szDefaultSearchTool, CCHMAXPATH + 1);

         // Normally the stringlength returned includes the null terminator so a zero-length string
         // returns 1.  If the key doesn't exist, however, the string length returned is NULL.

         if (Strlength < 2) strcpy (szDefaultSearchTool, "PMSeek.EXE");
         szDefaultSTParms [0] = 0;
         PrfQueryProfileString (hini, "FileFrdm.exe", "STParms", NULL,
                                                          (PSZ) szDefaultSTParms, CCHMAXPATH + 1);

         // Check the ini file for a default zipper.  If none exists, set 'zip.exe' as the default.
         // Normally the stringlength returned includes the null terminator so a zero-length string
         // returns 1.  If the key doesn't exist, however, the string length returned is NULL.

         szZipprog [0] = 0;
         Strlength = (ULONG) PrfQueryProfileString (hini, "FileFrdm.exe", "Archiver", NULL,
                                                             (PSZ) szZipprog, CCHMAXPATH + 1);
         if (Strlength < 2) strcpy (szZipprog, "ZIP.EXE");

         szZipparms [0] = 0;
         Strlength = (ULONG) PrfQueryProfileString (hini, "FileFrdm.exe", "ArchiverParms", NULL,
                                                          (PSZ) szZipparms, CCHMAXPATH + 1);

         // Normally the stringlength returned includes the null terminator so a zero-length string
         // returns 1.  If the key doesn't exist, however, the string length returned is NULL.
         // In  this case we only want to put in a default string on boot up.  If the user wants
         // to leave this string blank, they can without it being upgraded to "-j8".

         if (!Strlength) strcpy (szZipparms, "-j8");

         // Check the ini file for an unzipper. If none exists set 'unzip.exe' as the default.

         szUnzipprog [0] = 0;
         Strlength = (ULONG) PrfQueryProfileString (hini, "FileFrdm.exe", "UnArchiver", NULL,
                                                             (PSZ) szUnzipprog, CCHMAXPATH + 1);
         if (Strlength < 2) strcpy (szUnzipprog, "UNZIP.EXE");

         szUnzipparms [0] = 0;
         Strlength = (ULONG) PrfQueryProfileString (hini, "FileFrdm.exe", "UnArchiverParms",
                                                    NULL, (PSZ) szUnzipparms, CCHMAXPATH + 1);
         // See note above for ZIP params.

         if (!Strlength) strcpy (szUnzipparms, "-j");

         // Check the user-definable program settings in the ini file. (No defaults).

         szUser1prog [0] = 0;
         PrfQueryProfileString (hini, "FileFrdm.exe", "User1Prog", NULL,
                                                             (PSZ) szUser1prog, CCHMAXPATH + 1);
         szUser1parms [0] = 0;
         PrfQueryProfileString (hini, "FileFrdm.exe", "User1Parms", NULL,
                                                          (PSZ) szUser1parms, CCHMAXPATH + 1);
         szUser1dir [0] = 0;
         PrfQueryProfileString (hini, "FileFrdm.exe", "User1Dir", NULL,
                                                          (PSZ) szUser1dir, CCHMAXPATH + 1);
         szUser1name [0] = 0;
         PrfQueryProfileString (hini, "FileFrdm.exe", "User1Name", NULL,
                                                          (PSZ) szUser1name, 14);

         // For the User-Defined programs, if the program name isn't entered,
         // substitute "User Name" for the name string and disable the User Name from the menu.

         Strlength = strlen (szUser1prog);
         if (!Strlength)
            strcpy (szUser1name, "User Prog");
         WriteMenuItem (szUser1name, "~1\tAlt+1", MID_USER1, Strlength);

         szUser2prog [0] = 0;
         PrfQueryProfileString (hini, "FileFrdm.exe", "User2Prog", NULL,
                                                             (PSZ) szUser2prog, CCHMAXPATH + 1);
         szUser2parms [0] = 0;
         PrfQueryProfileString (hini, "FileFrdm.exe", "User2Parms", NULL,
                                                          (PSZ) szUser2parms, CCHMAXPATH + 1);
         szUser2dir [0] = 0;
         PrfQueryProfileString (hini, "FileFrdm.exe", "User2Dir", NULL,
                                                          (PSZ) szUser2dir, CCHMAXPATH + 1);
         szUser2name [0] = 0;
         PrfQueryProfileString (hini, "FileFrdm.exe", "User2Name", NULL,
                                                          (PSZ) szUser2name, 14);
         Strlength = strlen (szUser2prog);
         if (!Strlength)
            strcpy (szUser2name, "User Prog");
         WriteMenuItem (szUser2name, "~2\tAlt+2", MID_USER2, Strlength);

         szUser3prog [0] = 0;
         PrfQueryProfileString (hini, "FileFrdm.exe", "User3Prog", NULL,
                                                             (PSZ) szUser3prog, CCHMAXPATH + 1);
         szUser3parms [0] = 0;
         PrfQueryProfileString (hini, "FileFrdm.exe", "User3Parms", NULL,
                                                          (PSZ) szUser3parms, CCHMAXPATH + 1);
         szUser3dir [0] = 0;
         PrfQueryProfileString (hini, "FileFrdm.exe", "User3Dir", NULL,
                                                          (PSZ) szUser3dir, CCHMAXPATH + 1);
         szUser3name [0] = 0;
         PrfQueryProfileString (hini, "FileFrdm.exe", "User3Name", NULL,
                                                          (PSZ) szUser3name, 14);
         Strlength = strlen (szUser3prog);
         if (!Strlength)
            strcpy (szUser3name, "User Prog");
         WriteMenuItem (szUser3name, "~3\tAlt+3", MID_USER3, Strlength);

         szUser4prog [0] = 0;
         PrfQueryProfileString (hini, "FileFrdm.exe", "User4Prog", NULL,
                                                             (PSZ) szUser4prog, CCHMAXPATH + 1);
         szUser4parms [0] = 0;
         PrfQueryProfileString (hini, "FileFrdm.exe", "User4Parms", NULL,
                                                          (PSZ) szUser4parms, CCHMAXPATH + 1);
         szUser4dir [0] = 0;
         PrfQueryProfileString (hini, "FileFrdm.exe", "User4Dir", NULL,
                                                          (PSZ) szUser4dir, CCHMAXPATH + 1);
         szUser4name [0] = 0;
         Strlength = (ULONG) PrfQueryProfileString (hini, "FileFrdm.exe", "User4Name", NULL,
                                                          (PSZ) szUser4name, 14);
         Strlength = strlen (szUser4prog);
         if (!Strlength)
            strcpy (szUser4name, "User Prog");
         WriteMenuItem (szUser4name, "~4\tAlt+4", MID_USER4, Strlength);

         szUser5prog [0] = 0;
         PrfQueryProfileString (hini, "FileFrdm.exe", "User5Prog", NULL,
                                                             (PSZ) szUser5prog, CCHMAXPATH + 1);
         szUser5parms [0] = 0;
         PrfQueryProfileString (hini, "FileFrdm.exe", "User5Parms", NULL,
                                                          (PSZ) szUser5parms, CCHMAXPATH + 1);
         szUser5dir [0] = 0;
         PrfQueryProfileString (hini, "FileFrdm.exe", "User5Dir", NULL,
                                                          (PSZ) szUser5dir, CCHMAXPATH + 1);
         szUser5name [0] = 0;
         Strlength = (ULONG) PrfQueryProfileString (hini, "FileFrdm.exe", "User5Name", NULL,
                                                          (PSZ) szUser5name, 14);
         Strlength = strlen (szUser5prog);
         if (!Strlength)
            strcpy (szUser5name, "User Prog");
         WriteMenuItem (szUser5name, "~5\tAlt+5", MID_USER5, Strlength);

         szUser6prog [0] = 0;
         PrfQueryProfileString (hini, "FileFrdm.exe", "User6Prog", NULL,
                                                             (PSZ) szUser6prog, CCHMAXPATH + 1);
         szUser6parms [0] = 0;
         PrfQueryProfileString (hini, "FileFrdm.exe", "User6Parms", NULL,
                                                          (PSZ) szUser6parms, CCHMAXPATH + 1);
         szUser6dir [0] = 0;
         PrfQueryProfileString (hini, "FileFrdm.exe", "User6Dir", NULL,
                                                          (PSZ) szUser6dir, CCHMAXPATH + 1);
         szUser6name [0] = 0;
         Strlength = (ULONG) PrfQueryProfileString (hini, "FileFrdm.exe", "User6Name", NULL,
                                                          (PSZ) szUser6name, 14);
         Strlength = strlen (szUser6prog);
         if (!Strlength)
            strcpy (szUser6name, "User Prog");
         WriteMenuItem (szUser6name, "~6\tAlt+6", MID_USER6, Strlength);

         szUser7prog [0] = 0;
         PrfQueryProfileString (hini, "FileFrdm.exe", "User7Prog", NULL,
                                                             (PSZ) szUser7prog, CCHMAXPATH + 1);
         szUser7parms [0] = 0;
         PrfQueryProfileString (hini, "FileFrdm.exe", "User7Parms", NULL,
                                                          (PSZ) szUser7parms, CCHMAXPATH + 1);
         szUser7dir [0] = 0;
         PrfQueryProfileString (hini, "FileFrdm.exe", "User7Dir", NULL,
                                                          (PSZ) szUser7dir, CCHMAXPATH + 1);
         szUser7name [0] = 0;
         Strlength = (ULONG) PrfQueryProfileString (hini, "FileFrdm.exe", "User7Name", NULL,
                                                          (PSZ) szUser7name, 14);
         Strlength = strlen (szUser7prog);
         if (!Strlength)
            strcpy (szUser7name, "User Prog");
         WriteMenuItem (szUser7name, "~7\tAlt+7", MID_USER7, Strlength);

         szUser8prog [0] = 0;
         PrfQueryProfileString (hini, "FileFrdm.exe", "User8Prog", NULL,
                                                             (PSZ) szUser8prog, CCHMAXPATH + 1);
         szUser8parms [0] = 0;
         PrfQueryProfileString (hini, "FileFrdm.exe", "User8Parms", NULL,
                                                          (PSZ) szUser8parms, CCHMAXPATH + 1);
         szUser8dir [0] = 0;
         PrfQueryProfileString (hini, "FileFrdm.exe", "User8Dir", NULL,
                                                          (PSZ) szUser8dir, CCHMAXPATH + 1);
         szUser8name [0] = 0;
         Strlength = (ULONG) PrfQueryProfileString (hini, "FileFrdm.exe", "User8Name", NULL,
                                                          (PSZ) szUser8name, 14);
         Strlength = strlen (szUser8prog);
         if (!Strlength)
            strcpy (szUser8name, "User Prog");
         WriteMenuItem (szUser8name, "~8\tAlt+8", MID_USER8, Strlength);

         // Initialize the Directory Bookmarks to "".

         szDirHist [0] [0] = 0;
         szDirHist [1] [0] = 0;
         szDirHist [2] [0] = 0;
         szDirHist [3] [0] = 0;
         szDirHist [4] [0] = 0;
         szDirHist [5] [0] = 0;
         szDirHist [6] [0] = 0;
         szDirHist [7] [0] = 0;
         szDirHist [8] [0] = 0;
         szDirHist [9] [0] = 0;

         // Search the INI file for Directory Bookmarks.  If the first one is blank, put in
         // the Source directory.

         Strlength = (ULONG) PrfQueryProfileString (hini, "FileFrdm.exe", "Dir1", NULL,
                                                          (PSZ) szDirHist [0], CCHMAXPATH + 1);
         if (Strlength < 2) strcpy (szDirHist [0], szFilesTitle);

         PrfQueryProfileString (hini, "FileFrdm.exe", "Dir2", NULL, (PSZ) szDirHist [1],
                                                                                    CCHMAXPATH + 1);
         PrfQueryProfileString (hini, "FileFrdm.exe", "Dir3", NULL, (PSZ) szDirHist [2],
                                                                                    CCHMAXPATH + 1);
         PrfQueryProfileString (hini, "FileFrdm.exe", "Dir4", NULL, (PSZ) szDirHist [3],
                                                                                    CCHMAXPATH + 1);
         PrfQueryProfileString (hini, "FileFrdm.exe", "Dir5", NULL, (PSZ) szDirHist [4],
                                                                                    CCHMAXPATH + 1);
         PrfQueryProfileString (hini, "FileFrdm.exe", "Dir6", NULL, (PSZ) szDirHist [5],
                                                                                    CCHMAXPATH + 1);
         PrfQueryProfileString (hini, "FileFrdm.exe", "Dir7", NULL, (PSZ) szDirHist [6],
                                                                                    CCHMAXPATH + 1);
         PrfQueryProfileString (hini, "FileFrdm.exe", "Dir8", NULL, (PSZ) szDirHist [7],
                                                                                    CCHMAXPATH + 1);
         PrfQueryProfileString (hini, "FileFrdm.exe", "Dir9", NULL, (PSZ) szDirHist [8],
                                                                                    CCHMAXPATH + 1);
         PrfQueryProfileString (hini, "FileFrdm.exe", "Dir10", NULL, (PSZ) szDirHist [9],
                                                                                    CCHMAXPATH + 1);
         // For the refresh timer intervals, get the time saved in seconds.
         // If the interval is zero, the timer is not activated.

         ltemp = sizeof (ULONG);
         interval1 = 0;
         PrfQueryProfileData (hini, "FileFrdm.exe", "DirRefresh", &interval1, &ltemp);
         if (interval1)
         {
            ULONG timer;

            // Multiply the number of seconds by 1000 since the timer must be programmed
            // in milliseconds.

            ULONG ulTime;
            ulTime = interval1 * 1000;
            timer = WinStartTimer (hab1, hwndclient, DIRTIMER, ulTime);
         }

         interval2 = 0;
         PrfQueryProfileData (hini, "FileFrdm.exe", "DriveRefresh", &interval2, &ltemp);
         if (interval2)
         {
            ULONG timer;

            // Multiply the number of seconds by 1000 since the timer must be programmed
            // in milliseconds.

            ULONG ulTime;
            ulTime = interval2 * 1000;
            timer = WinStartTimer (hab1, hwndclient, DRIVETIMER, ulTime);
         }


         PrfCloseProfile (hini);
         DosSetDefaultDisk (CurrDrive);

         // Read the volume labels of the discovered drives, excluding the floppies.

         RefreshVolLabels (3, FALSE);

         // Set up an object window to handle a seperate thread.  This thread will be used
         // for Drag and Drop manouvers between multiple instances of FF.

         QMSG objQmsg;
         WinRegisterClass (tp3->Hab, "FFObject", ObjectWindowProc, NULL, NULL);

         // An application can create an object window by using the WinCreateWindow function and
         // setting the desktop-object window as the parent window.

         hwndobject = WinCreateWindow (
            HWND_OBJECT,        /* Parent is object window.            */
            "FFObject",       /* Window class for client             */
            NULL,               /* Window text                         */
            0,                  /* No styles for object window         */
            0, 0,               /* Lower-left corner                   */
            0, 0,               /* Width and height                    */
            NULL,               /* No owner                            */
            HWND_TOP,        /* Inserts window at bottom of z-order */
            OBJECTWINDOWID,       /* Window identifier                   */
            NULL,               /* No class-specific data              */
            NULL);              /* No presentation data                */

         // Now, store the object window handle in the frame window's window words
         // so that it can be retrieved by by a conncurrent process (another FF instance).
         // This index into the window words corresponds to QWL_USER, but I have reserved
         // extra bytes so that I can store more words.

         rc = (BOOL) WinSetWindowULong (hwndframe, 0, (ULONG) hwndobject);

         // This is the "wait for input" loop for this window procedure.

         while (WinGetMsg (tp3->Hab, &objQmsg, NULLHANDLE, 0, 0))
         {
            WinDispatchMsg (tp3->Hab, &objQmsg);
         }

         WinDestroyMsgQueue (tp3->Hmq);
         WinTerminate (tp3->Hab);
         break;

      // This is used for the Copy operations thread.  This is to prevent long copy operations
      // to slow peripherals from blocking the desktop.

      case DIVERGE:

         cp = (CopyParms *) tparmref;
         cp->Hab = WinInitialize (0);
         cp->Hmq = WinCreateMsgQueue (cp->Hab, 0);

         // Grab the mutex semaphore so that any other routine that queries it
         // can tell if this process is current.

         DosRequestMutexSem (hmtx, SEM_INDEFINITE_WAIT);

         // Call the DoCopyOp routine.
         // This thread places a call to DoCopyOp with the following paramters:

         DoCopyOp (cp->pszName, cp->pszPath, cp->usReplace, cp->cPath,
                                                                           cp->usOp, cp->hwndfocus);

         // Release the semaphore to signal that the Copy process is complete.

         DosReleaseMutexSem (hmtx);

         WinDestroyMsgQueue (cp->Hmq);
         WinTerminate (cp->Hab);

         break;

      // Scrolldown and Scrollup are used for the dynamic scrolling while swiping operations.

      case SCROLLDOWN:

         tp6 = (threadparms *) tparmref;
         tp6->Hab = WinInitialize (0);
         tp6->Hmq = WinCreateMsgQueue (tp6->Hab, 0);


         PCNRITEM pOrgRec;
         SHORT selectedstate;

         pOrgRec = (PCNRITEM) WinSendMsg (hwndfilescntr,
                               CM_QUERYRECORDEMPHASIS, MPFROMP (CMA_FIRST),
                                                      MPFROMSHORT (CRA_CURSORED));

         if ((pOrgRec->rc.flRecordAttr & CRA_SELECTED) == 1) selectedstate = TRUE;
         else selectedstate = FALSE;

         pOrgRec = (PCNRITEM) WinSendMsg (hwndfilescntr, CM_QUERYRECORD,
                                                                MPFROMP (pOrgRec ),
                                                MPFROM2SHORT (CMA_NEXT, CMA_ITEMORDER));
         if (pOrgRec)
         {
            WinSendMsg (hwndfilescntr, WM_CHAR, MPFROMSH2CH
                                       (KC_VIRTUALKEY, 0, 0), MPFROM2SHORT (0, VK_DOWN));

                WinSendMsg (hwndfilescntr, CM_SETRECORDEMPHASIS,
                   MPFROMLONG (pOrgRec), MPFROM2SHORT (selectedstate, CRA_SELECTED));
         }
         DosReleaseMutexSem (hmtx2);
         WinDestroyMsgQueue (tp6->Hmq);
         WinTerminate (tp6->Hab);

         inhibitrefreshflag = 0;
         break;

      // Scrolldown and Scrollup are used for the dynamic scrolling while swiping operations.

      case SCROLLUP:

         tp6 = (threadparms *) tparmref;
         tp6->Hab = WinInitialize (0);
         tp6->Hmq = WinCreateMsgQueue (tp6->Hab, 0);


         PCNRITEM pOrgRec2;
         SHORT selectedstate2;

         pOrgRec2 = (PCNRITEM) WinSendMsg (hwndfilescntr,
                               CM_QUERYRECORDEMPHASIS, MPFROMP (CMA_FIRST),
                                                      MPFROMSHORT (CRA_CURSORED));

         if ((pOrgRec2->rc.flRecordAttr & CRA_SELECTED) == 1) selectedstate2 = TRUE;
         else selectedstate2 = FALSE;

         pOrgRec2 = (PCNRITEM) WinSendMsg (hwndfilescntr, CM_QUERYRECORD,
                                                                MPFROMP (pOrgRec2),
                                                MPFROM2SHORT (CMA_PREV, CMA_ITEMORDER));

         if (pOrgRec2)
         {

            WinSendMsg (hwndfilescntr, WM_CHAR, MPFROMSH2CH
                                       (KC_VIRTUALKEY, 0, 0), MPFROM2SHORT (0, VK_UP));

               WinSendMsg (hwndfilescntr, CM_SETRECORDEMPHASIS,
                   MPFROMLONG (pOrgRec2), MPFROM2SHORT (selectedstate2, CRA_SELECTED));

         }
         DosReleaseMutexSem (hmtx2);
         WinDestroyMsgQueue (tp6->Hmq);
         WinTerminate (tp6->Hab);

         inhibitrefreshflag = 0;
         break;

      case RETURNTO:

         //------------------------------------------------------------
         // This function will cause the container window to scroll until the original
         // record that was in the cursor box is again in the window view with the
         // cursor box around it.
         // This is used when the files display is resorted or the window is rebuilt.
         // If a matching record isn't found, no harm will be done.
         //------------------------------------------------------------

         tp7 = (threadparms *) tparmref;
         tp7->Hab = WinInitialize (0);
         tp7->Hmq = WinCreateMsgQueue (tp7->Hab, 0);

         HWND hwnd;
         hwnd = tp7->hwnd;
         PSZ psztemp;
         psztemp = tp7->szSavename;

         SHORT sResult;
         RECTL contrectl;
         RECTL queryrect;
         QUERYRECORDRECT qRecordrect;
         PQUERYRECORDRECT pqRecordrect;
         pqRecordrect = &qRecordrect;

         sResult = 1;

         // Get the container type:  it will be filtered to one of four
         //                               - CV_TEXT | CV_FLOW
         //                               - CV_TEXT
         //                               - CV_NAME
         //                               - CV_DETAIL

         PCNRINFO pcnrinfo;
         pcnrinfo = new (CNRINFO);
         WinSendMsg (hwnd, CM_QUERYCNRINFO, MPFROMP (pcnrinfo),
                                                           MPFROMSHORT (sizeof (CNRINFO)+100));
         // Find the record that matchs the name.

         pWhatRec = (PCNRITEM) WinSendMsg (hwnd, CM_QUERYRECORD, MPFROMP (pWhatRec),
                                               MPFROM2SHORT (CMA_FIRST, CMA_ITEMORDER ));
         int iLen;
         while (pWhatRec)
         {
            // Use the case insensitive 'strnicmp' here and only compare as many chars
            // as the length of the saved name.

            iLen = strlen (psztemp);
            sResult = (SHORT) strnicmp (psztemp, pWhatRec->szFileName, iLen);
            if (!sResult) break;

            pWhatRec = (PCNRITEM) WinSendMsg (hwnd, CM_QUERYRECORD,
                           MPFROMP( pWhatRec), MPFROM2SHORT (CMA_NEXT, CMA_ITEMORDER ));
         }
         // This section resets the cursor selection box around the chosen file.  This is because
         // if a directory is being reloaded, the cursor will initially be set to the first file which
         // then won't be where we want it if it is swapped again.  It is the file that is contained
         // within the cursor selection box that is saved as the bookmark.
         // This is also used as a workaround for scrolling the CV_TEXT views (below).
         // This works since once the record is selected, even though the view doesn't move to that
         // record, sending a WM_CHAR down arrow message
         // causes the container to jump the view to that record in the window.
         // The proper method of scrolling as used with the CV_DETAIL view works better however,
         // since it also allows for scrolling the record to the center of the screen.

         // If a matching record isn't found, everything will be ignored.

         if (pWhatRec)
         {
            // Don't toggle the rc.flRecordAttr bit since it doesn't stick.  Setting the record
            // emphasis also makes it unnecessary to use the cm_invalidaterecord command.

            WinSendMsg (hwnd, CM_SETRECORDEMPHASIS,
                         MPFROMLONG (pWhatRec), MPFROM2SHORT (TRUE, CRA_CURSORED));

            // Get the container window rectangle.  This is used later to scroll 1/2 of the window
            // size to bring the record to the middle of the window.

            rc = (BOOL) WinSendMsg (hwnd, CM_QUERYVIEWPORTRECT, MPFROMP (&contrectl),
                                               MPFROM2SHORT (CMA_WINDOW, NULL));
            // Set up to get the virtual position of the record in the container.

            qRecordrect.cb = sizeof (QUERYRECORDRECT);
            qRecordrect.pRecord = (PRECORDCORE) pWhatRec;
            qRecordrect.fsExtent = CMA_TEXT | CMA_ICON;

            if (rc)
            {
            // Get the virtual position of the record in the container.  This virtual position can be a
            // negative value which means that it is not visible in the opening viewport rectangle.
            // This value will not change unless records are added or subtracted from the container.

               rc = (BOOL) WinSendMsg (hwnd, CM_QUERYRECORDRECT, MPFROMP (&queryrect),
                                                                               MPFROMP (pqRecordrect));
               if (rc)
               {
                  // If the position is below the opening window, we must scroll to it.
                  // Allow 32 pels for container bug.
                  if ((pcnrinfo->flWindowAttr & CV_FLOW) || (pcnrinfo->flWindowAttr & CV_TEXT))
                  {
         //********NOTE******
         // I tried to use the same routine to scroll to the record with the matching filename, but found
         // that I could not use it.  It works perfectly with the Details view of the container (below), but
         // wouldn't work with the CV_TEXT views.  There were two main bugs -
         // First, the CM_SCROLLWINDOW messages had to be sent with a WinPostMsg, and at that
         // only every second message was received so scroll messages with small values had to be
         // inserted between meaningful messages.
         // Secondly, the spacing between records is different in CV_TEXT views, and althought the position
         // of the record rectangle was returned, scrolling that amount would always be short of the goal.

         // Also NOTE:
         // WinPostMsg is used here as WinSendMsg proved unreliable.  Also, two key events
         // are sent (one would seem to be enough since it is followed by another
         // CM_SETRECORDEMPHASIS message) because only one also proved unreliable.
         // When the messages don't get through, the window opens at the first page and
         // stays there even though the cursor has positioned itself over the correct record.
         // This is a very buggy API.

                     WinPostMsg (hwnd, WM_CHAR, MPFROMSH2CH
                                          (KC_VIRTUALKEY, 0, 0), MPFROM2SHORT (0, VK_DOWN));
                     WinPostMsg (hwnd, WM_CHAR, MPFROMSH2CH
                                          (KC_VIRTUALKEY, 0, 0), MPFROM2SHORT (0, VK_UP));

                     WinPostMsg (hwnd, CM_SETRECORDEMPHASIS,
                            MPFROMLONG (pWhatRec), MPFROM2SHORT (TRUE, CRA_CURSORED));
                  }

                  if (pcnrinfo->flWindowAttr & CV_DETAIL)
                  {
                     if (queryrect.yBottom < 32)
                     {
                        // Make the negative number absolute. (positive)
                        i = labs (queryrect.yBottom);

                        // Scroll because of the buggy api.
                        WinPostMsg (hwnd, CM_SCROLLWINDOW, MPFROMSHORT (CMA_VERTICAL),
                                                 MPFROMLONG (1));

                        // Scroll to the record.
                        WinPostMsg (hwnd, CM_SCROLLWINDOW, MPFROMSHORT (CMA_VERTICAL),
                                                 MPFROMLONG (i));
                        // Scroll because of the buggy api.
                        WinPostMsg (hwnd, CM_SCROLLWINDOW, MPFROMSHORT (CMA_VERTICAL),
                                                 MPFROMLONG (-1));

                        // Scroll another half screen to bring the record to the middle of the screen.
                        WinPostMsg (hwnd, CM_SCROLLWINDOW, MPFROMSHORT (CMA_VERTICAL),
                                                 MPFROMLONG ((contrectl.yTop - contrectl.yBottom)/2));
                     }
                  }
               }
            }
         }
         delete pcnrinfo;
         WinDestroyMsgQueue (tp7->Hmq);
         WinTerminate (tp7->Hab);

         // Note:
         // This WinSetWindow message must not be sent until after the thread queue is
         // destroyed, otherwise the message is not processed soon enough and the screen
         // jumping caused by the various returnto manipulations are visible to the user.
         // I don't know why this works this way.

         WinSetWindowPos (hwnd, HWND_TOP, 0, 0, 0, 0, SWP_SHOW);
         break;
   }
   _endthread ();
}

//--------------------------------------------------------------------------
// Get the File System Info for the designated disk and set-up the free bytes into the title string
// of the directory tree container and print it.
//--------------------------------------------------------------------------
void getFSInfo (LONG newdrive)
{
      CurrDrive = newdrive;
      pFSalloc = new FSALLOCATE;
      DosQueryFSInfo (CurrDrive, 1, pFSalloc, sizeof (FSALLOCATE));

      ULONG freebytes = pFSalloc->cSectorUnit * pFSalloc->cUnitAvail * pFSalloc->cbSector;

      // Create a title text banner for the directory container.  This will name the drive
      // and the number of bytes free on that drive.
      // Copy the title text and the ascii byte string to the title text
      // banner, inserting some commas into the byte string on the way.

      delete pFSalloc;
      CHAR Roottitle[31] = " :\\  ";
      LONG ctemp = CurrDrive + 64;

      Roottitle[0] = (CHAR) ctemp;

      // Insert some commas into the byte count string after every three digits.

      static CHAR betterstring [17] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
      CHAR *cString = betterstring;
      InsertCommas (freebytes, cString);

      strcat (Roottitle, betterstring);
      strcat (Roottitle, " bytes free");

      CHAR *pRoottitle = Roottitle;
      FirstChild->newtitle (pRoottitle);
}

//---------------------------------------------------------
//A function to format the first status bar with the required information
//---------------------------------------------------------
void fillStatusbar1 (ULONG drive, PSZ pszdestpath)
{
   DestDrive = drive;
   strcpy (szDestPath, pszdestpath);

   memset (szDestination, 0, 80);
   strcpy (szDestination, "Destination: ");
   USHORT destpathlen = (USHORT) strlen (szDestPath);

   // The maximum length that is allowed for the status bar text is 80 chars, so the path
   // must be trimmed if it is over 67 chars so that it will fit.

   if (destpathlen > 40)
   {
      for (int i = 0; i < 3; i++) szDestination [i + 13] = szDestPath [i];
      szDestination [16] = '.';
      szDestination [17] = '.';
      szDestination [18] = '.';
      USHORT excesslen = USHORT (destpathlen - 34);
      CHAR *pszTail = &szDestPath [excesslen];
      strcat (szDestination, pszTail);
   }
   else strcat (szDestination, szDestPath);

   pFSalloc = new FSALLOCATE;
   DosQueryFSInfo (DestDrive, 1, pFSalloc, sizeof (FSALLOCATE));

   ULONG freebytes = pFSalloc->cSectorUnit * pFSalloc->cUnitAvail * pFSalloc->cbSector;

   // Create a title text banner for the directory container.  This will name the drive
   // and the number of bytes free on that drive.
   // Copy the title text and the ascii byte string to the title text
   // banner, inserting some commas into the byte string on the way.

   delete pFSalloc;

   // Insert some commas into the byte count string after every three digits.

   static CHAR betterstring [17] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
   CHAR *cString = betterstring;
   InsertCommas (freebytes, cString);

   strcat (szDestination, " ");
   strcat (szDestination, betterstring);
   strcat (szDestination, " bytes free");

   PSZ pString = szDestination;
   Statusbar->newtitle1 (pString);
}

//----------------------------------------------------------
//A function to format the second status bar with the required information
//----------------------------------------------------------
void fillStatusbar2 (USHORT files, ULONG bytes)
{
   strcpy (szSource, "Source: ");

   pFB = DFiles->Getfilesandbytes ();

   // Get the total files in the container translated into ascii
   // and get the total bytes in the files translated into ascii.

   static CHAR tempstring [13];
   static CHAR betterstring [17] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
   CHAR *cString = betterstring;

   // If files have been selected for operations, use this routine which displays files
   // and bytes selected as well as files and bytes in the directory.

   if (files)
   {
      utoa (files, tempstring, 10);
      strcat (szSource, tempstring);
      strcat (szSource, " of ");
      utoa (pFB->cFiles, tempstring, 10);
      strcat (szSource, tempstring);
      strcat (szSource, " Files selected using ");
      if (bytes)
      {
         InsertCommas (bytes, cString);
         strcat (szSource, betterstring);
      }
      else strcat (szSource, "0");
      strcat (szSource, " of ");
      if (pFB->cBytes)
      {
         InsertCommas (pFB->cBytes, cString);
         strcat (szSource, betterstring);
      }
      else strcat (szSource, "0");
      strcat (szSource, " bytes");
   }
   else

   // If no files are currently selected use this routine which only displays files
   // and bytes in the directory.
   {
      utoa (pFB->cFiles, tempstring, 10);
      strcat (szSource, tempstring);
      strcat (szSource, " File(s) using ");

      // Insert commas into the byte count. Leave the files count as is since it usually
      // won't be more than 3 digits so the lack of a comma there is not a big deal.

      if (pFB->cBytes)
      {
         InsertCommas (pFB->cBytes, cString);
         strcat (szSource, betterstring);
      }
      else strcat (szSource, "0");
      strcat (szSource, " bytes ");
   }
   // Send this formatted string to the status bar.

   Statusbar->newtitle2 ((PSZ) szSource);
}
//---------------------------------------------------------------------
// A function to set up the correct string and then put up a message box with the error
// displayed.
//---------------------------------------------------------------------
void MessageBox (HWND hwnd, APIRET error)
{
   PSZ pszErrormessage;

   // If the error is not listed here, the message box will not be activated.

   USHORT myerror = 1;

   switch (error)
   {
      case ERROR_PATH_NOT_FOUND:
          pszErrormessage = "Path not Found!";
          break;

      case ERROR_FILE_NOT_FOUND:
         pszErrormessage = "File not Found!";
         break;

      case ERROR_ACCESS_DENIED:
         pszErrormessage = "Access Denied!";
         break;

      case ERROR_NOT_DOS_DISK:
         pszErrormessage = "Not a Dos Disk!";
         break;

      case ERROR_SHARING_VIOLATION:
         pszErrormessage = "Sharing Violation!";
         break;

      case ERROR_SHARING_BUFFER_EXCEEDED:
         pszErrormessage = "Sharing Buffer Exceeded!";
         break;

      case ERROR_INVALID_PARAMETER:
         pszErrormessage = "Invalid Parameter!";
         break;

      case ERROR_DRIVE_LOCKED:
         pszErrormessage = "Drive is Locked!";
         break;

      case ERROR_DISK_FULL:
         pszErrormessage = "Disk is Full!";
         break;

      case ERROR_FILENAME_EXCED_RANGE:
         pszErrormessage = "Path/Filename Exceeded Maximum Length!";
         break;

      case ERROR_DIRECTORY:
         pszErrormessage = "Directory Error!";
         break;

      case ERROR_EAS_NOT_SUPPORTED:
         pszErrormessage = "EAs Not Supported!";
         break;

      case ERROR_NEED_EAS_FOUND:
         pszErrormessage = "EAs are Required!";
         break;

      case ERROR_WRITE_PROTECT:
         pszErrormessage = "Disk is Write Protected!";
         break;

      case ERROR_INVALID_DRIVE:
         pszErrormessage = "Invalid Drive!";
         break;

      case ERROR_SECTOR_NOT_FOUND:
         pszErrormessage = "Sector Not Found!";
         break;

      case ERROR_OUT_OF_PAPER:
         pszErrormessage = "Printer Out of Paper!";
         break;

      case ERROR_WRITE_FAULT:
         pszErrormessage = "Write Fault!";
         break;

      case ERROR_READ_FAULT:
         pszErrormessage = "Read Fault!";
         break;

      case ERROR_GEN_FAILURE:
         pszErrormessage = "General Failure!";
         break;

      case ERROR_INVALID_NAME:
         pszErrormessage = "Invalid Name!";
         break;

      case ERROR_NOT_SAME_DEVICE:
         pszErrormessage = "Different Device!";
         break;

      case CUSTOM_NO_FILES_SELECTED:
         pszErrormessage = "No Files Selected!";
         break;

      default:
         myerror = 0;
   }
   if (myerror) WinMessageBox (HWND_DESKTOP, hwnd, pszErrormessage, "ERROR!",
                                                         MESSAGEBOXID, MB_CANCEL | MB_ERROR);

}

//-------------------------------------------------------------------
// This function searches for the last open record in the root tree and returns it.
// It will be used as the parent in a search for children that match root entries
// to be deleted or as the parent of a new subdirectory.
//-------------------------------------------------------------------
PCNRITEM SearchForOpen ()
{
   PCNRITEM pOpenRec, pAnyRec;
   pAnyRec = (PCNRITEM) 1;
   pOpenRec = NULL;
   USHORT search = CMA_FIRST;

   while (pAnyRec)
   {
      pAnyRec = (PCNRITEM) WinSendMsg (hwnddircntr,
                            CM_QUERYRECORD, MPFROMP (pAnyRec),
                                         MPFROM2SHORT (search, CMA_ITEMORDER));
      search = CMA_NEXT;
      while (pAnyRec)
      {
         // This search will pick the first folder indicated as expanded and then search
         // its children for further expanded folders and so on until no further open folders
         // are found.  The last one found open is saved and returned.

         if (pAnyRec->rc.flRecordAttr & CRA_EXPANDED)
         {
            pOpenRec = pAnyRec;
            search = CMA_FIRSTCHILD;
            break;
         }
         pAnyRec = (PCNRITEM) WinSendMsg (hwnddircntr,
                                           CM_QUERYRECORD, MPFROMP (pAnyRec),
                                         MPFROM2SHORT (search, CMA_ITEMORDER));
      }
   }
   // Exclude the parent folder symbol (the open folder marked "in Use").
   // In the case of a FAT drive, no parent folder exists and so it is possible
   // that pOpenRec could be returned NULL.

   if (pOpenRec)
   {
      if (pOpenRec->rc.flRecordAttr & CRA_INUSE) pOpenRec = NULL;
   }
   return pOpenRec;
}

//-----------------------------------------------------------
// This is a routine to produce an ascii string from a ULONG with commas
// inserted every 3 digits as is common to North America.
// The ascii string will be inserted into the string provided by PSZ newstring.
// The string provided has to be at least 14 chars long.
//-----------------------------------------------------------
void InsertCommas (ULONG number, PSZ newstring)
{
   static CHAR tempstring [13];
   ultoa (number, tempstring, 10);

   // Insert commas into the byte count. Leave the files count as is since it usually
   // won't be more than 3 digits so the lack of a comma there is not a big deal.

   static CHAR commastring [14] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};
   int cbFree = strlen (tempstring);
   int commas = cbFree / 3;
   int firstpart = cbFree % 3;

   int i;
   for (i = 0; i < firstpart; i++)
   {
      commastring [i] = tempstring [i];
   }
   int ii = (i - 1);

   if ((commas) && (firstpart)) commastring [i] = ',';
   else i--;

   int q;
   while (commas)
   {
      for (q = 1; q < 4; q++)
      {
         commastring [q + i] = tempstring [ii + q];
      }
      commas--;
      if (commas) commastring [q + i] = ',';
      i += q;
      ii +=3;
   }
   if (cbFree > 2)
      commastring [i] = 0;
   else
      commastring [cbFree] = 0;
   strcpy (newstring, commastring);
}

//-----------------------------------------------------------------
// A function to change the current drive in the files container, to the passed drive
//-----------------------------------------------------------------
void ChangeDrive (SHORT drive)
{
         // Check if thread 2 and 3 have finished.  If they haven't, skip this routine.
         // Found I couldn't use threads in this routine.  If the Drive changes were well
         // spaced it was alright, but if they were close together (key held down) the
         // program would crash.

         if ((flag & 192) == 192)
         {
            // Put up the hourglass pointer.

            WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP,
                                                                                SPTR_WAIT, FALSE));

            // Start over with a new drive.
            // Don't use the RefillTree function here because there is no open folder in a new drive.

            DTree->RenewCont (drive);
            DTree->FillContainer ();

            // Get the file system info and format a title for the container with
            // the drive designation and the bytes free.

            getFSInfo (drive);

            // This first section fills the Files container with the files in this
            // new drive that has been opened in the Directory container.
            // Since it is the root, pass a NULL value in PathToOrder.

            DFiles->PathToOrder (NULL, (SHORT) CurrDrive);
            DFiles->FillContainer (filefilter);

            // This statusbar information can't be gathered until the container is filled so it must
            // follow it in this thread.  It can't be done outside of the thread unless the container
            // is filled outside of the thread.

            fillStatusbar2 (NULL, NULL);

            // Update the title of the Files container with the new path

            szFilesTitle [0] = CHAR (CurrDrive + 64);
            szFilesTitle [1] = 58;
            szFilesTitle [2] = 92;
            szFilesTitle [3] = 0;
            CHAR *pTitle = szFilesTitle;
            SecondChild->newtitle (pTitle);

            // If the drive is 'A' or 'B', read the volume label as well.

            if (drive == 1 || drive == 2) RefreshVolLabels (drive, TRUE);

            // Restore the arrow pointer.

            WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP,
                                                                              SPTR_ARROW, FALSE));
         }
}

//-------------------------------------------------------------
// A small routine to set-up the Viewer default strings and write them to the
// FileFrdm.ini file
//-------------------------------------------------------------
void SetViewerDefaults ()
{
         DosSetDefaultDisk (Homedrive);
         DosSetCurrentDir (szHomepath);

         hini = PrfOpenProfile (hab1, "FileFrdm.ini");

         // These strings are peppered with zeros and so must be handled as binary data.
         // Each item within the string must be delimited with a zero.

         pszViewerTypes = new CHAR [92];
         memcpy (pszViewerTypes, "<none>\0DOC\0   ICO\0   GIF\0   BMP\0   PCX\0   "
                                  "TIF\0   AVI\0   WAV\0   "
                                    "JPG\0   MPG\0   INF\0   SND\0   ", 91);

         PrfWriteProfileData (hini, "FileFrdm.exe", "ViewerType", pszViewerTypes, 91);

         pszViewers = new CHAR [664];
         memcpy (pszViewers, "<epm>\0                                             "
                     "<epm>\0                                             "
                     "ICONEDIT.EXE\0                                      "
                     "IB.EXE\0                                            "
                     "IB.EXE\0                                            "
                     "IB.EXE\0                                            "
                     "IB.EXE\0                                            "
                     "VB.EXE\0                                            "
                     "AB.EXE\0                                            "
                     "IB.EXE\0                                            "
                     "VB.EXE\0                                            "
                     "VIEW.EXE\0                                          "
                     "AB.EXE\0                                            ", 663);

         PrfWriteProfileData (hini, "FileFrdm.exe", "Viewers", pszViewers, 663);
         PrfCloseProfile (hini);
         ViewerTypeslen = 91;

         DosSetDefaultDisk (CurrDrive);
}

//--------------------------------------------------------------
// A small routine to set-up the viewer pointers and stick them in the listbox.
//--------------------------------------------------------------
void SetupViewers ()
{
         USHORT vtlen;
         USHORT vnlen;
         vtlen = USHORT (ViewerTypeslen/7);
         vnlen = vtlen;

         pszVT = new PSZ [vtlen];
         USHORT Vindex;
         USHORT Lindex;
         Vindex = 0;

         int i;
         for (i = 0; i < vtlen; i++)
         {
            pszVT [i] = &pszViewerTypes [i * 7];

            // Insert the '.ext' string into the listbox and attach the array index to the
            // item in so that the string can be indexed properly when retrieved.

            Lindex = (USHORT) WinSendMsg (hwndvcombobox, LM_INSERTITEM,
                                MPFROMSHORT (LIT_SORTASCENDING), MPFROMP (pszVT [i]));
            WinSendMsg (hwndvcombobox, LM_SETITEMHANDLE, MPFROMSHORT (Lindex),
                                                                       MPFROMLONG (i));
         }
         WinSendMsg (hwndvcombobox, LM_SELECTITEM, MPFROMSHORT (comboitem),
                                                                   MPFROMSHORT (TRUE));

         // Now create an array of pointers to strings within the string buffer for the
         // Viewer names.  This array will match to the array of extension types one-to-one.

         pszVN = new PSZ [vnlen];

         for (i = 0; i < vnlen ; i++)
         {
            pszVN [i] = &pszViewers [i * VIEWERSTRLEN];
         }

         // Load the Viewer entry field with the viewer that matches the 'ext' array entry.

         Vindex = (USHORT) WinSendMsg (hwndvcombobox, LM_QUERYITEMHANDLE,
                                                               MPFROMSHORT (comboitem), NULL);
         WinSetWindowText (hwndentryfield, pszVN [Vindex]);
}

//----------------------------------------------------------------------------
// This routine does two things.
// First, it determines whether there is enough room for the tool bar to stretch out as one long
// row (either vertically or horizontally) and adjusts the toolbar to be one of four possible
// configurations - two rows horizontally, one row horizontally, two columns vertically or
// one column vertically.  It also re-orients the drivebar using the same criteria.
// Secondly, it takes these new positions and calculates the amount of space necessary
// to reserve for them before resizing the two container windows to suit.
// Since this function is also called in response to a WM_SIZE message, it must be able
// to reconfigure the bars even if they are not changing orientation.
//-----------------------------------------------------------------------------
void OrientToolbar ()
{
   WinQueryWindowPos (hwndclient, (PSWP)&swp);

   if (buttoncolumns > buttonrows)  // This means the bar is horizontal.
   {
      if (swp.cx < (buttonrows * buttoncolumns * (TBwidth + 2)))
      {
            buttonrows = 2;
            buttoncolumns = SHORT (totalbuttons / 2);
            if ((buttoncolumns * 2) < totalbuttons) buttoncolumns++;
      }
      else
      {
            buttonrows = 1;
            buttoncolumns = totalbuttons;
      }
   }
   else  // The bar is vertical.
   {
      if (swp.cy < (buttonrows * buttoncolumns * (TBheight + 2)))
      {
            buttoncolumns = 2;
            buttonrows = SHORT (totalbuttons / 2);
            if ((buttonrows * 2) < totalbuttons) buttonrows++;
      }
      else
      {
            buttoncolumns = 1;
            buttonrows = totalbuttons;
      }
   }
// The orientation logic for the two bars is kept separate here in case I want to have them
// orient independantly later.

   if (drivecolumns > driverows)  // This means the bar is horizontal
   {
      if (swp.cx < (totaldrives * (DBwidth + 2)))
      {
            driverows = 2;
            drivecolumns = SHORT (totaldrives / 2);
            if ((drivecolumns * 2) < totaldrives) drivecolumns++;
      }
      else
      {
         driverows = 1;
         drivecolumns = totaldrives;
      }
   }
   else  // The bar is vertical.
   {
      if (swp.cy < (totaldrives * (DBheight + 2)))
      {
            drivecolumns = 2;
            driverows = SHORT (totaldrives / 2);
            if ((driverows * 2) < totaldrives) driverows++;
      }
      else
      {
            drivecolumns = 1;
            driverows = totaldrives;
      }
   }

   // Must also change the reserved parameters.

   pdbarinfo->reserverows = buttonrows;
   pdbarinfo->reservecolumns = buttoncolumns;

   // Check the reserved space for the tool and drive bar.  If it's changed it means it means
   // a major reconfiguration of the available space.  Otherwise, just shrink or expand the
   // first window as a ratio of the available space.

   ptooldim = FFtoolbar->rejig (buttonrows, buttoncolumns);
   pdrivedim = FFdrivebar->rejig (driverows, drivecolumns);

   FCwidth = ((orgswpcx - ptooldim->horz - pdrivedim->horz) * FCwidth)/ (FCwidth + SCwidth);
   FCheight = orgswpcy - ptooldim->vert - pdrivedim->vert;
   SCwidth = orgswpcx - FCwidth - ptooldim->horz - pdrivedim->horz;
   SCheight = FCheight;

   FirstChild->rejig (FCwidth, FCheight);
   SecondChild->rejig (SCwidth, SCheight);

   resize();
}
//-----------------------------------------------------------
// This routine toggles the toolbar visibility.  It is used on startup if the
// toolbar is set as invisible and also used with MID_TOOLBAR.
// This is done by giving the toolbar buttons zero dimensions and then resizing
// everything esle to fit.
//-----------------------------------------------------------
void ToggleToolbar ()
{
                  if (ptbarinfo->btncx < 3)
                  {
                     ptbarinfo->btncx = TBwidth;
                     ptbarinfo->btncy = TBheight;
                     pdbarinfo->reservecx = ptbarinfo->btncx;
                     pdbarinfo->reservecy = ptbarinfo->btncy;
                     WinSendMsg (hwndmenu, MM_SETITEMATTR, MPFROM2SHORT
                                                       (MID_TOOLBAR, TRUE), MPFROM2SHORT
                                                                    (MIA_CHECKED, MIA_CHECKED));
                     // Turn off the hidden bit.

                     toolbarorient &= 1;
                  }
                  else
                  {
                     if (buttonrows > buttoncolumns)
                     {
                     ptbarinfo->btncx = 0;
                     ptbarinfo->btncy = 0;
                     }
                     else
                     {
                     ptbarinfo->btncx = 0;
                     ptbarinfo->btncy = 0;
                     }
                     pdbarinfo->reservecx = ptbarinfo->btncx;
                     pdbarinfo->reservecy = ptbarinfo->btncy;
                     WinSendMsg (hwndmenu, MM_SETITEMATTR, MPFROM2SHORT
                                                       (MID_TOOLBAR, TRUE), MPFROM2SHORT
                                                                                (MIA_CHECKED, NULL));
                     // Turn on the hidden bit.

                     toolbarorient |= 2;
                  }
                  resize ();
                  WinQueryWindowPos (hwndframe, &frameswp);
                  WinSetWindowPos (hwndframe, HWND_TOP, frameswp.x, frameswp.y,
                                              frameswp.cx + 1, frameswp.cy, SWP_SIZE);
                  WinSetWindowPos (hwndframe, HWND_TOP, frameswp.x, frameswp.y,
                                              frameswp.cx -1, frameswp.cy, SWP_SIZE);
}
//-------------------------------------------------------------------
// This is a delete Tree routine.  It will find all decendant subdirectories and all files
// in the specified and subordinate subdirectories and delete them.
// The specified directory must be deleted by the caller (it will be empty).
//-------------------------------------------------------------------
void DeleteTree (PSZ pszdirname)
{
      // These buffers are used with the DosFindFirst etc. functions.  The large array
      // of buffers holds information on many files at a time.

      PFILEFINDBUF3 pNextbuf;
      PBYTE pbBuf;
      PFILEFINDBUF3 pFilebuf;
      pFilebuf = new FILEFINDBUF3 [100];
      pbBuf = (PBYTE) pFilebuf;
      pNextbuf = pFilebuf;

      // Create an array of strings and pointers to hold the directory names for each
      // successive level of depth.  If it goes greater than 30 levels deep (Egads!)
      // the routine is aborted (below).

      PSZ ppszParentdir [30];
      for (i = 0; i < 30; i++)
      ppszParentdir [i] = new CHAR [(CCHMAXPATH + 1)];

      ULONG maxfiles;
      maxfiles = 1;
      rc = 0;
      HDIR hdir;
      ULONG fileattrs;
      fileattrs =  FILE_ARCHIVED | FILE_SYSTEM | FILE_HIDDEN | FILE_READONLY;

      // Create our filespec

      strcpy (szpathbuffer, pszdirname);
      strcat (szpathbuffer, "\\*.*");
      strcpy (ppszParentdir [0], pszdirname);

      int iii;
      iii = 0;

      // This loop will make two passes for each directory that it must pass through.
      // 100000 will allow for 50,000 directories in the tree.  I hope this is sufficient.
      // There is no limit to the number of files it can handle.

      for (i = 0; i < 100000; i++)
      {
         hdir =HDIR_SYSTEM;
         rc = DosFindFirst ( szpathbuffer, &hdir, MUST_HAVE_DIRECTORY, pFilebuf, FF_BUFFSIZE,
                           &maxfiles, FIL_STANDARD);
         // If the directories are "." and "..", take another read and get a real dir name.

         if (pFilebuf->cchName == 1)
         {
            if (pFilebuf->achName [0] == '.')
            {
               rc = DosFindNext (hdir, pFilebuf, FF_BUFFSIZE, &maxfiles);
               if (pFilebuf->cchName == 2)
               {
                  if (pFilebuf->achName [0] == '.')
                  {
                     if (pFilebuf->achName [1] == '.')
                     rc = DosFindNext (hdir, pFilebuf, FF_BUFFSIZE, &maxfiles);
                  }
               }
            }
         }

         // If no more directories can be found below this level, delete the files (if any).

         if (rc)
         {
            // Get files from the directory in bunches of 100.

            maxfiles = 100;
            rc = DosFindFirst ( szpathbuffer, &hdir, fileattrs, pFilebuf, FF_BUFFSIZE,
                                &maxfiles, FIL_STANDARD);

            // Provide an escape if this was an empty directory.
            if (rc) maxfiles = 0;

            pbBuf = (PBYTE) pFilebuf;
            pNextbuf = pFilebuf;
            PSZ pszTail;
            APIRET rcode;
            strcpy (sznamebuffer, ppszParentdir [iii]);
            strcat (sznamebuffer, "\\");
            pszTail = &sznamebuffer [strlen (sznamebuffer)];
            for (ii = 0; ii < maxfiles; ii ++)
            {
               *pszTail = 0;
               strcat (pszTail, pNextbuf->achName);
               DosDelete (sznamebuffer);
               pbBuf += pNextbuf->oNextEntryOffset;
               pNextbuf = (PFILEFINDBUF3) pbBuf;
            }
            while (maxfiles == 100)
            {
               rc = DosFindNext (hdir, pFilebuf, FF_BUFFSIZE, &maxfiles);

               // Provide an escape if there were only 100 files exactly.

               if (rc) maxfiles = 0;

               pbBuf = (PBYTE) pFilebuf;
               pNextbuf = pFilebuf;
               strcpy (sznamebuffer, ppszParentdir [iii]);
               strcat (sznamebuffer, "\\");
               pszTail = &sznamebuffer [strlen (sznamebuffer)];
               for (ii = 0; ii < maxfiles; ii ++)
               {
                  *pszTail = 0;
                  strcat (pszTail, pNextbuf->achName);
                  DosDelete (sznamebuffer);
                  pbBuf += pNextbuf->oNextEntryOffset;
                  pNextbuf = (PFILEFINDBUF3) pbBuf;
               }
            }
            // The files have been deleted and if this is not the top level directory in
            // this tree, delete the empty directory.

            if (iii)
            {
               strcpy (sznamebuffer, ppszParentdir [iii]);

               // If a directory can't be deleted (may contain a system file etc.)
               // break out of the loop.  An error will be registered when the calling
               // routine tries to delete the last dir and can't.

               rcode = DosDeleteDir (sznamebuffer);
               if (rcode) i = 100000;

               iii--;
               strcpy (szpathbuffer, ppszParentdir [iii]);
               strcat (szpathbuffer, "\\*.*");
               maxfiles = 1;
            }
            // Except for the top level directory (which will be removed by the caller),
            // we are done so break out of the loop.

            else i =100000;
         }
         else
         {
            strcpy (szpathbuffer, ppszParentdir [iii]);
            strcat (szpathbuffer, "\\");
            strcat (szpathbuffer, pFilebuf->achName);
            iii++;

            // This is a trap for a directory tree structure that has greater than 30 levels.

            if (iii > 29) i = 100000;
            else
            {
               strcpy (ppszParentdir [iii], szpathbuffer);
               strcat (szpathbuffer, "\\*.*");
               maxfiles = 1;
            }
         }
      }
      for (i = 0; i < 30; i++)
      delete ppszParentdir [i];
      delete pFilebuf;
}
//-----------------------------------------------------------------------
MRESULT EXPENTRY ButtonDropProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   char szDir [CCHMAXPATH];
   PDRAGINFO pDragInfo;
   PDRAGITEM pDragItem;
   USHORT usOp;
   USHORT usIndicator, cItems, i;
   ULONG ulBytes, index;

   // Get the index into the array of Button procedure pointers so that this sub-classed procedure
   // can return to the right window procedure.

   index = WinQueryWindowULong (hwnd, QWL_USER);

   // Initialize the pointer for the DRAGINFO structure

   pDragInfo = (PDRAGINFO) mp1;

   switch (msg)
   {
      //-------------------------------------------------------
      //  Someone's dragging an object over us.
      //-------------------------------------------------------
      case DM_DRAGOVER:

         if (!fEmphasis)
         {
            fEmphasis = 1;
            DrawEmphasis (hwnd);
         }

         /* Get access to the DRAGINFO data structure */

         DrgAccessDraginfo (pDragInfo);

         // If this is an external packet (from another instance of FF or from some other
         // process) disallow dropping on the drive letter.  If its a packet from another FF
         // instance, it should use its own drive letters.  If it's a foreign process, to bad-
         // it can only drop on the directory tree window.

         if (pDragInfo->hwndSource != hwndfilescntr)
         {
            if (pDragInfo->hwndSource != hwnddircntr)
            {
               DrgFreeDraginfo (pDragInfo);
               return (MRFROM2SHORT (DOR_NEVERDROP, 0));
            }
         }

         /* Can we accept this drop? */
         switch (pDragInfo->usOperation)
         {
          /* Return DOR_NODROPOP if current operation */
          /* is link or unknown                       */

            case DO_UNKNOWN:
               DrgFreeDraginfo (pDragInfo);
               return (MRFROM2SHORT (DOR_NODROPOP, 0));

            /* Our default operation is Move */
            case DO_DEFAULT:
                 usOp =  DO_MOVE;
               break;

            /* Do the requested specific operation */
            case DO_MOVE:
            case DO_COPY:
               usOp = pDragInfo->usOperation;
               break;
         }

         usIndicator = DOR_DROP;
         cItems = (USHORT) DrgQueryDragitemCount (pDragInfo);

          /* Now, we need to look at each item in turn */

          for (i = 0; i < cItems; i++)
          {
             pDragItem = DrgQueryDragitemPtr (pDragInfo, i);

             /* Make sure we can move for a Move request */
             /* or copy for a Copy                       */

             if (((pDragItem->fsSupportedOps & DO_COPYABLE)   &&
                  (usOp == (USHORT) DO_COPY))                 ||
                 ((pDragItem->fsSupportedOps & DO_MOVEABLE)   &&
                  (usOp == (USHORT) DO_MOVE)))

             {
                /* Check the rendering format */
                if (DrgVerifyRMF (pDragItem, "DRM_OS2FILE", "DRF_UNKNOWN"))
                   usIndicator = DOR_DROP;
                else
                   usIndicator = DOR_NEVERDROP;
             }
             else
                usIndicator = DOR_NODROPOP;
          }

         /* Release the draginfo data structure */

         DrgFreeDraginfo (pDragInfo);

         return (MRFROM2SHORT (usIndicator, usOp));

         /* Dragged object just left */

      case DM_DRAGLEAVE:

         if (fEmphasis)
         {
            fEmphasis = 0;
            DrawEmphasis (hwnd);
         }
         return (MRESULT) FALSE;

      /* Drop the object on us (receive the object) */

      case DM_DROP:

         // Set the Destination drive variable here since it is needed in the DragDrop dialog
         // in order to fill the container window.  It will always be One more than the index.

         DropDrive = index + 1;

         // Clear the emphasis from the target

         if (fEmphasis)
         {
            fEmphasis = 0;
            DrawEmphasis (hwnd);
         }
         // Test for source drive ready (media inserted etc.).

         rc = IsDriveReady (CurrDrive);
         if (!rc)
            return (MRFROM2SHORT (DOR_NODROPOP, 0));

         // Test to see if the destination drive is ready (removable media drives).

         rc = IsDriveReady (DropDrive);
         if (!rc)
            return (MRFROM2SHORT (DOR_NODROPOP, 0));

         /* Get access to the DRAGINFO data structure */

         DrgAccessDraginfo (pDragInfo);

         // If this is an external packet (from another instance of FF or from some other
         // process) disallow dropping on the drive letter.  If its a packet from another FF
         // instance, it should use its own drive letters.  If it's a foreign process, to bad-
         // it can only drop on the directory tree window.

         if (pDragInfo->hwndSource != hwndfilescntr)
         {
            if (pDragInfo->hwndSource != hwnddircntr)
            {
               DrgFreeDraginfo (pDragInfo);
               return (MRFROM2SHORT (DOR_NEVERDROP, 0));
            }
         }

         /* Can we accept this drop? */
         switch (pDragInfo->usOperation)
         {
            /* Return DOR_NODROPOP if current operation */
            /* is link or unknown                       */
            case DO_UNKNOWN:
               DrgFreeDraginfo (pDragInfo);
               return (MRFROM2SHORT (DOR_NODROPOP, 0));

            // Our default operation is Copy
            case DO_DEFAULT:
               pDragItem = DrgQueryDragitemPtr (pDragInfo, 0);
               ulBytes  = DrgQueryStrName (pDragItem->hstrContainerName, sizeof (szDir), szDir);
               if (!ulBytes)
                 return (MRFROM2SHORT (DOR_NODROPOP, 0));
               usOp = (USHORT) DO_MOVE;
               DropDestOp.Operation |= 4;
               break;

            /* Do the requested specific operation */
            case DO_MOVE:
               usOp = pDragInfo->usOperation;
               DropDestOp.Operation |= 4;
               break;

            case DO_COPY:
               usOp = pDragInfo->usOperation;
               DropDestOp.Operation |= 2;
               break;
         }
         usIndicator = DOR_DROP;
         cItems = (USHORT) DrgQueryDragitemCount (pDragInfo);

         // Our involvement with the source can now be terminated since the target (here)
         // is going to handle the rendering.

         /* Now, we need to look at each item in turn */

         for (i = 0; i < cItems; i++)
         {
            pDragItem = DrgQueryDragitemPtr (pDragInfo, i);

//-----Debugging Code----------

//~ CHAR obj [101];
//~ CHAR ren [101];
//~ CHAR con [101];
//~ CHAR src [101];
//~ CHAR trg [101];
//~ CHAR msgg [1500];
//~ DrgQueryStrName (pDragItem->hstrType, 100, obj);
//~ DrgQueryStrName (pDragItem->hstrRMF, 100, ren);
//~ DrgQueryStrName (pDragItem->hstrContainerName, 100, con);
//~ DrgQueryStrName (pDragItem->hstrSourceName, 100, src);
//~ DrgQueryStrName (pDragItem->hstrTargetName, 100, trg);
//~ sprintf (msgg, "Hwnd:%d\nID:%d\nCX:%d\nCY:%d\nControl:%d\nOps:%d\nObj:%s\nRen:%s\nCon:%s\nSrc:%s\nTrg:%s\n",pDragItem->hwndItem, pDragItem->ulItemID, pDragItem->cxOffset, pDragItem->cyOffset, pDragItem->fsControl, pDragItem->fsSupportedOps, obj,ren,con,src,trg);
//~  WinMessageBox (HWND_DESKTOP, HWND_DESKTOP, msgg, "Dropped", 0, MB_OK);

//-----Debugging Code ends--------

            WinSendMsg (pDragInfo->hwndSource, DM_ENDCONVERSATION,
                                    MPFROMLONG (pDragItem->ulItemID),
                                    MPFROMLONG (DMFL_TARGETSUCCESSFUL));
         }

         // Deallocate the memory that was set up to hold the string handles for both
         // Draginfo and Dragitem structures associated with a specific DragInfo and
         // invalidate the handles.

         DrgDeleteDraginfoStrHandles (pDragInfo);

         /* Release the draginfo data structure */

         DrgFreeDraginfo (pDragInfo);

         // Restore the arrow pointer.
         WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP,
                                                                              SPTR_ARROW, FALSE));

         // Call up the DragDrop Dialog and get the necessary information about
         // destination and operation.
         // Note: The default operation going into this dialog is MOVE.  This can be
         // changed to COPY via the Control key.  Other key combinations could be
         // implemented to invoke ZIP and UNZIP operations, but presently these must
         // be changed manually in the dialog.

         WinDlgBox (HWND_DESKTOP, hwnd, DragDropDlgProc, NULLHANDLE, DDDEST_ID,
                                                                          (PVOID) &DropDestOp);

         // Read the replace files flag set in the DragDropDlgProc.

         USHORT usReplace;
         usReplace = USHORT (DropDestOp.Operation & 1);

         // Pick out the operation to perform on the files selected

         if (DropDestOp.Operation & 2)
         {
            // Call the copy routine that is shared with the standard CopyDlgProc.

            DoCopy (DropDestOp.szDropName, DropDestOp.szDropPath,
                                usReplace,  DropDestOp.ccPathname, COPY, pDragInfo->hwndSource);
         }
         else if (DropDestOp.Operation & 4)
         {
            // Call the MOVE routine that is shared with the MoveDlgProc

            DoCopy (DropDestOp.szDropName, DropDestOp.szDropPath,
                                  usReplace,  DropDestOp.ccPathname, MOVE, pDragInfo->hwndSource);
         }
         else if (DropDestOp.Operation & 8)
         {
            // First, change the destination to the dragdrop choice.

            strcpy (szDestPath, DropDestOp.szDropPath);

            // Call the ZipName dialog

            WinDlgBox (HWND_DESKTOP, hwnd, ZipnameDlgProc, NULLHANDLE,
                                                                              INPUTZIPNAMEID, NULL);
            fillStatusbar1 (DropDrive, (PSZ) DropDestOp.szDropPath);

            // Reset the default disk since it somehow gets lost in the dragdrop shuffle.

            DosSetDefaultDisk (CurrDrive);

            return (MRFROM2SHORT (usIndicator, usOp));
         }
         else if (DropDestOp.Operation & 16)
         {
            // First, change the destination to the dragdrop choice.

            fillStatusbar1 (DropDrive, (PSZ) DropDestOp.szDropPath);

            // Call the Unzip routine

            DoUnzip ((SHORT) MID_UNZIP, hwnd);

            // Reset the default disk since it somehow gets lost in the dragdrop shuffle.

            DosSetDefaultDisk (CurrDrive);

            return (MRFROM2SHORT (usIndicator, usOp));
         }

         else if (DropDestOp.Operation & 128)
         {
            // First, change the destination to the dragdrop choice.

            strcpy (szDestPath, DropDestOp.szDropPath);

            // Call the Append dialog

            WinDlgBox (HWND_DESKTOP, hwnd, AppendDlgProc, NULLHANDLE, APPENDID, NULL);

            fillStatusbar1 (DropDrive, (PSZ) DropDestOp.szDropPath);

            // Reset the default disk since it somehow gets lost in the dragdrop shuffle.

            DosSetDefaultDisk (CurrDrive);

            return (MRFROM2SHORT (usIndicator, usOp));
         }

         else if (DropDestOp.Operation & 32)
         {
            // The user has pressed the cancel button, so Cancel this call.
            // Reset the default disk since it somehow gets lost in the dragdrop shuffle.

            DosSetDefaultDisk (CurrDrive);

            return (MRFROM2SHORT (usIndicator, usOp));
         }

        // Now, change the destination drive/directory to the Drop Path.  This will
        // make it easy for Drag/Droppers to check the destination after doing a drop.

         fillStatusbar1 (DropDrive, (PSZ) DropDestOp.szDropPath);

         // Reset the default disk since it somehow gets lost in the dragdrop shuffle.

         DosSetDefaultDisk (CurrDrive);

         return (MRFROM2SHORT (usIndicator, usOp));

      /* Send the message to the usual WC_FRAME WndProc */
      default:
         return (*ButtonProc [index]) (hwnd, msg, mp1, mp2);
   }
}
//------------------------------------------------------------
//  DoDrag() - the actual drag function.
//------------------------------------------------------------
BOOL DoDrag (PRECORDCORE precordundermouse, HWND hwnddragsource)
{
   DRAGITEM Dragitem;
   HSTR hstrType, hstrRMF, hstrContainer;  // String handles
   CHAR szContainer [CCHMAXPATH + 1];
   CHAR szName [CCHMAXPATH + 1];
   PDRAGINFO pSourceDraginfo;
   DRAGIMAGE dimg;
   int useselected;
   USHORT len;

   // Clear the DROPDEST structure

   DropDestOp.szDropPath [0] = 0;
   DropDestOp.szDropName [0] = 0;
   DropDestOp.Operation = 0;
   DropDestOp.ccPathname = 0;
   DropDestOp.recundermouse = 0;
   DropDestOp.hwndCaller = NULL;

   // Clear the emphasis flag

   fEmphasis = 0;

   // Get the first selected file names from the files container.

    int cRecs = 0;

    useselected = FALSE;

    // If the record under the mouse is NULL, we must be over whitespace, in
    // which case we don't want to drag any records.

   if (!precordundermouse) return FALSE;
   else
   {
      // Count the records with 'selection' emphasis. These are the records
      // we want to drag, unless the container record under the mouse does
      // not have selection emphasis. If that is the case, we only want to
      // process that one.

      pcnrTemp = (PCNRITEM) CMA_FIRST;
      while (pcnrTemp)
      {
         pcnrTemp = (PCNRITEM) WinSendMsg (hwnddragsource, CM_QUERYRECORDEMPHASIS,
                                          MPFROMP (pcnrTemp), MPFROMSHORT (CRA_SELECTED));
         if (pcnrTemp)
         {
            if ((PRECORDCORE) &pcnrTemp->rc == precordundermouse)
            {
               // If the file under the mouse is selected, this means that all selected files
               // will be used.
               useselected = TRUE;
            }
            pWhatRec = pcnrTemp;
            cRecs++;
         }
      }

      if (!useselected)
      {
         cRecs = 1;
         pWhatRec = (PCNRITEM) precordundermouse;
         DropDestOp.recundermouse = 1;
         DropDestOp.pRec = pWhatRec;
      }

      szContainer [0] = CHAR (CurrDrive + 64);
      szContainer [1] = 58;
      szContainer [2] = 0;
      strcat (szContainer, pWhatRec->szDirPath);
      len = (USHORT) strlen (pWhatRec->szFileName);

      // If it is a subdir from the Files container, remove the trailing backslash.
      // The trailing backslash isn't used with the Tree container

      if  (pWhatRec->attrFile & 16 && hwnddragsource == hwndfilescntr)
      {
         len--;
      }
      szContainer [strlen (szContainer) - len] = 0;

      }
      //  Allocate the DRAGINFO data structure.

      pSourceDraginfo = DrgAllocDraginfo (cRecs);

      //  Define file type as unknown.

      hstrType  = DrgAddStrHandle (DRT_UNKNOWN);
      hstrRMF   = DrgAddStrHandle (szFormats);         /* OS2file unknown type*/

      // Set up the DRAGIMAGE icon.

      if (cRecs > 1) dimg.hImage = WinQuerySysPointer (HWND_DESKTOP, SPTR_MULTFILE, FALSE);
      else
      dimg.hImage = WinQuerySysPointer (HWND_DESKTOP, SPTR_FILE, FALSE);

      // Initialize the rest of the drag image.

      dimg.cb = sizeof (DRAGIMAGE);
      dimg.fl = DRG_ICON;// | DRG_TRANSPARENT;
      dimg.cxOffset = -14;  // These offsets seem to be required to get the icon under the
      dimg.cyOffset = -14;  // pointer.

      //  Get our current directory for the container name.

      hstrContainer = DrgAddStrHandle (szContainer);

      // Now loop through the selected records and gather the DRAGITEM information.

      pWhatRec = (PCNRITEM) CMA_FIRST;

      for (i = 0; i < cRecs; i++)
      {
         pWhatRec = (PCNRITEM) WinSendMsg (hwnddragsource, CM_QUERYRECORDEMPHASIS,
                                          MPFROMP (pWhatRec), MPFROMSHORT (CRA_SELECTED));

         if (!useselected)
         {
            // We have already determined that there is a file under the mouse.
            // If the pointer was not on the selected files, we will drag the file under the mouse.
            // The record pointer was saved above in the if (!useselected) routine.
            // The record count in this instance would be 1 so we only travel thru this loop once.

            pWhatRec = (PCNRITEM) precordundermouse;
         }
         // To get the required format, some stuff must be added and other stuff
         // subtracted.
         // The Container name should be in the form of "A:\Myapps\"
         // and the filename should be in the form of "filename".

         szContainer [0] = CHAR (CurrDrive + 64);
         szContainer [1] = 58;
         szContainer [2] = 0;
         strcat (szContainer, pWhatRec->szDirPath);
         len = (USHORT) strlen (pWhatRec->szFileName);
         strcpy (szName, pWhatRec->szFileName);

         // If it is a subdir from the Files container, remove the trailing backslash.
         // The trailing backslash isn't used with the Tree container

         if  (pWhatRec->attrFile & 16 && hwnddragsource == hwndfilescntr)
         {
            len--;
            szName [len] = 0;
         }
         szContainer [strlen (szContainer) - len] = 0;

         memset (&Dragitem, 0, sizeof (Dragitem));

         // DragItem.hwndItem is the window that will get the DM_RENDER and
         // DM_RENDERCOMPLETE messages.

         Dragitem.hwndItem = hwnddragsource;

         // ulItemID is used to store information that can be used at drop time.

         Dragitem.ulItemID = i;

         // hstrType identifies 'types' when it is necessary to differentiate. A
         // good example is if you are dragging file names (DRM_OS2FILE) and need to
         // pass the file type to the target (i.e. DRT_BITMAP would mean the file
         // contained a bitmap, DRT_TEXT is an ascii file, etc.).
         // It defaults to DRT_UNKNOWN. This means that the file type is not known.

         Dragitem.hstrType = hstrType;
         Dragitem.hstrContainerName = hstrContainer;
         Dragitem.fsControl = 0;
         Dragitem.fsSupportedOps = DO_COPYABLE | DO_MOVEABLE;
         Dragitem.hstrSourceName = DrgAddStrHandle (szName);
         Dragitem.hstrTargetName = Dragitem.hstrSourceName;
         Dragitem.ulItemID = i;

         Dragitem.hstrRMF = hstrRMF;

         // Set the DRAGITEM struct into the memory allocated by
         // DrgAllocDraginfo ()

         DrgSetDragitem (pSourceDraginfo, &Dragitem, sizeof (Dragitem), i);

         // Set source emphasis for this container record - The CRA_SOURCE definition is lost
         // but is known to be 16384.

         WinSendMsg (hwnddragsource, CM_SETRECORDEMPHASIS,
                            MPFROMP (pWhatRec), MPFROM2SHORT (TRUE, 16384));
      }

      //  Start drag operation.

      DrgDrag (hwnddragsource, pSourceDraginfo, &dimg, 1, VK_BUTTON2, NULL);

      DrgFreeDraginfo (pSourceDraginfo);

            // Restore the arrow pointer.
            WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP,
                                                                              SPTR_ARROW, FALSE));

      return TRUE;
}
//---------------------------------------------------------------------------
// This function is used to draw/remove Emphasis around a possible target in Drag & Drop
// DESCRIPTION:  Inverts a thin rectangle just inside the borders of the window.
//---------------------------------------------------------------------------
void DrawEmphasis (HWND hwnd)
{
   HPS      hps;
   RECTL    rclRect;

// Get a presentation space for the window

   hps = DrgGetPS (hwnd);
   if (hps != NULLHANDLE)
   {

      // Get the window rectangle and adjust for the borders and scroll bars

      WinQueryWindowRect (hwnd, &rclRect);
// --------------------------------------------------------------------------
// Invert a small border inside the normal border
// --------------------------------------------------------------------------
      WinDrawBorder (hps, &rclRect, 4, 4, 0, 0, DB_DESTINVERT | DB_STANDARD);

// --------------------------------------------------------------------------
// release the presentation space
// --------------------------------------------------------------------------
      DrgReleasePS (hps);
   }
}

//-------------------------------------------------------------------
// A function to handle a ENTER key event in the Directory Tree containers.  Used
// in the main Directory Tree window and in the DragDrop dialog window.
// The last parameter indicates the caller.  The Tree view window sends a NULL
// while the drag/drop container sends a pointer to it's tree object.
//-------------------------------------------------------------------
void DoTree (HWND hwnd, PCNRITEM pcr, roottree *ddt)
{
      // This part handles a problem that is caused because I have used the Tree
      // View container feature.  The Tree View container provides it's own
      // keyboard interface (by sub-classing this procedure I guess) and opens and closes
      // folders with a single mouse click.  The folders must have children to open.
      // If the folder is empty, however, the folder will not open and a
      // CM_EXPANDTREE message is not sent to this Main Procedure.
      // Since I wanted the folders left empty when closed, this aspect of the
      // 'feature set' has been worked around by refilling them each time they
      // are opened with a double-click or the enter key (both of which are
      // treated as enter keys and are channeled thru here) and emptying them
      // each time they are closed by whatever method.  The single-clicks to open
      // are ignored by the sub-classed procedure because the folder is initially
      // empty.  This actually is an advantage since the folders are always
      // up-to-date, being freshly refilled each time they are opened.
      // The next aspect of the problem arises when an attempt is made to
      // open a direct ancestor of the currently opened folder. (All direct ancestors
      // are also displayed as open.)  Remember that when the folders are
      // opened that the sub-classed single-click is ignored because the folder
      // is empty.  This single-click makes up part of the double-click captured
      // by this procedure.  Double-clicks on an open folder are interpreted as
      // a) a single-click to close a folder and b) a double-click to open a
      // folder.  When the ancestor is first closed, the screen (depending on
      // whether the tree display has been scrolled or not and the positioning
      // of the records in the window) is updated and sometimes the folder that
      // was closed is now located in a position below where it was previously.
      // When the number of folders displayed in a container window is reduced,
      // you would expect the whole display to be displaced upwards, but in some
      // instances this is the opposite of what happens and in these cases
      // a different folder is under the cursor when the second-click registers.
      // Remember that this only happens to folders that are already shown as open.
      // This is interpreted as a double click on a folder that was not selected.
      // This routine remedys this by searching for the peer folder (which will
      // be below the one now under the pointer) that actually has the cursor on it,
      // and opening that one instead.

      PCNRITEM pHold;

      if (!(pcr->rc.flRecordAttr & CRA_CURSORED))
      {
         while (pcr)
         {
            pHold = pcr;

            // NOTE:
            // The next record may be a peer folder, or it may be a child folder.  Look for either
            // if necessary.  If the next folder, whichever it is, doesn't have the cursor on it,
            // that means that the original folder still does.

            pcr = (PCNRITEM) WinSendMsg (hwnd, CM_QUERYRECORD,
                                                       MPFROMP (pcr),
                                       MPFROM2SHORT (CMA_NEXT, CMA_ITEMORDER));
            if (pcr)
            {
               if (pcr->rc.flRecordAttr & CRA_CURSORED)
                  break;
            }
            else
            {
               pcr = (PCNRITEM) WinSendMsg (hwnd, CM_QUERYRECORD,
                                                       MPFROMP (pHold),
                                       MPFROM2SHORT (CMA_FIRSTCHILD, CMA_ITEMORDER));

               if (pcr)
               {
                  if (pcr->rc.flRecordAttr & CRA_CURSORED)
                     break;
               }
            }
         }
      }

      // Fill the Tree container and open all folders in the path.
      // NOTE:
      // Two different calling routines are sent through here.
      // One routine is for the main Tree View Window while the other is for the
      // Drag/Drop dialog tree view.
      // They have different requirements insofar as the drag/drop routine does not need
      // to fill a files window and has a different tree container object to work with.
      // The different requirements are signalled by the last parameter. (see header above)
      // Skip filling it if it is the root folder with the 'IN USE' marking.

      if (!(pcr->rc.flRecordAttr & CRA_INUSE))
      {
         if (ddt == NULL)
         {
         // Update the title of the Files container with the new path

            szFilesTitle [0] = CHAR (CurrDrive + 64);
            szFilesTitle [1] = 58;
            szFilesTitle [2] = 0;
            if (!strlen (pcr->szDirPath))
            {
               szFilesTitle [2] = 92;
               szFilesTitle [3] = 0;
            }
            else strcat (szFilesTitle, pcr->szDirPath);

            //  Don't write the new title yet, since it doesn't look good when the title
            // changes before the container is filled.

            PSZ pSave;
            pSave = &szFilesTitle [2];

            // NOTE:
            // Refill the Tree before filling the files container.  If the files container is filled
            // first, the Tree routine does not scroll correctly to display the opened folders.
            // I don't know why this happens.

            // Send the DTree parameter for the object pointer.

            RefillTree (CurrDrive, USEPATH, hwnd, DTree, NULL);

            DFiles->PathToOrder (pSave, (SHORT) CurrDrive);

            // Fill the files container and update the source status bar.

            DFiles->FillContainer (filefilter);

            SecondChild->newtitle ((PSZ) szFilesTitle);

            // This statusbar information can't be gathered until the container is filled.

            fillStatusbar2 (NULL, NULL);
         }
         else
         {
            // Don't need a drive designator in this path since a discreet container is used
            // and has already been initialized to the drop drive.  The drive is sent as the
            // first parameter.

            RefillTree (DropDrive, USEPATH, hwnd, ddt, pcr->szDirPath);
         }
      }
}
//---------------------------------------------------------------
// This function handles part of the CN_EXPANDTREE code that is common
// between the DragDrop dialog and the main window proc.
//---------------------------------------------------------------
void ExpandTreeFix (PCNRITEM pcnr, HWND hwnd)
{
      // For Records Without Children Only:
      // This section substitutes the open folder icon for the collapsed folder
      // icon.  This is required because the folder must appear to be
      // open since the Files container will display the files inside regardless
      // of whether there are subdirectories or not.

      pcnrTemp = (PCNRITEM) WinSendMsg (hwnd, CM_QUERYRECORD,
                                                 MPFROMP (pcnr),
                            MPFROM2SHORT (CMA_FIRSTCHILD, CMA_ITEMORDER));
      if (!pcnrTemp)
      {
         pcnr->rc.hptrIcon = pcnr->rc.pTreeItemDesc->hptrExpanded;
         PCNRITEM *pptemp = &pcnr; // Need pointer to pointer.
         WinSendMsg (hwnd, CM_INVALIDATERECORD, MPFROMP (pptemp),
                                        MPFROM2SHORT (1, CMA_NOREPOSITION));
      }

      // This section closes all of the peer folders in the Directory container that
      // is being opened.  This keeps the display as small as possible.

      whatRec = CMA_NEXT;
      pcnrTemp = pcnr;
      pWhatRec = pcnr;
      while (pWhatRec)
      {
         pWhatRec = (PCNRITEM) WinSendMsg (hwnd, CM_QUERYRECORD,
                                                    MPFROMP (pWhatRec),
                                       MPFROM2SHORT (whatRec, CMA_ITEMORDER ));
         if (pWhatRec)
         {
            if (pWhatRec->rc.flRecordAttr & CRA_EXPANDED)
            WinSendMsg (hwnd, CM_COLLAPSETREE,
                                                       MPFROMP (pWhatRec), NULL);
         }
      }
      whatRec = CMA_PREV;
      while (pcnrTemp)
      {
         pcnrTemp = (PCNRITEM) WinSendMsg (hwnd, CM_QUERYRECORD,
                                              MPFROMP (pcnrTemp),
                                        MPFROM2SHORT (whatRec, CMA_ITEMORDER ));
         if (pcnrTemp)
         {
            if (pcnrTemp->rc.flRecordAttr & CRA_EXPANDED)
            WinSendMsg (hwnd, CM_COLLAPSETREE,
                                            MPFROMP (pcnrTemp), NULL);
         }
      }
}

//---------------------------------------------------------------
// This function handles part of the CN_COLLAPSETREE code that is common
// between the DragDrop dialog and the main window proc.
//---------------------------------------------------------------
void CollapseTreeFix (PCNRITEM pcnr, HWND hwnd, roottree * ddt)
{
      ddt->RemoveKids (pcnr);

      // For Records Without Children Only:
      // This section substitutes the collapsed folder icon for the open folder
      // icon.  This is required because of a previous opposite substitution
      // for folders that have no subdirectorys.  The folder must appear to be
      // open since the Files container will display the files inside regardless
      // of whether there are subdirectories or not.
      // One exception is made - the root directory folder must stay expanded.

      pcnrTemp = (PCNRITEM) WinSendMsg (hwnd, CM_QUERYRECORD,
                                                 MPFROMP (pcnr),
                            MPFROM2SHORT (CMA_FIRSTCHILD, CMA_ITEMORDER));
      if ((!pcnrTemp) && (!(pcnr->rc.flRecordAttr & CRA_INUSE)))
      {
         pcnr->rc.hptrIcon = pcnr->rc.pTreeItemDesc->hptrCollapsed;
         PCNRITEM *pptemp = &pcnr;
         WinSendMsg (hwnd, CM_INVALIDATERECORD, MPFROMP (pptemp),
                                            MPFROM2SHORT (1,CMA_NOREPOSITION));
      }
      // For all records:
      // This command makes sure the cursor ends up on the proper file since the container
      // goes through an uncontrollable repositioning that normally moves the cursor to the
      // current pointer position regardless of what file is underneath.
      // WinPostMsg must be used here.

      WinPostMsg (hwnd, CM_SETRECORDEMPHASIS,
                            MPFROMLONG (pcnr), MPFROM2SHORT (TRUE, CRA_CURSORED));
}
//--------------------------------------------------------------------
// This is a thread feeder for the Copy Files routine used by the CopyDlgProc and the
// DragDropDlgProc.
// This will prevent the the program from monopolizing the input queue
// and blocking the Desktop when dealing with slow peripherals.
// This would normally happen when it is dealing with a lengthy process
// that can't be done in a separate thread.
// This is avoided now because the procedure will be running
// in a separate thread while the main program checks for the mutex semaphore
// before allowing other procedures to begin.
//--------------------------------------------------------------------
void DoCopy (PSZ pszName, PSZ pszPath, USHORT replace, USHORT cPath, USHORT op,
                                                                                         HWND hSource)
{
               cparms.Hab = hab5;
               cparms.Hmq = hmq5;
               cparms.job = DIVERGE;
               cparms.pszName = pszName;
               cparms.pszPath = pszPath;
               cparms.cPath = cPath;
               cparms.usReplace = replace;
               cparms.usOp = op;
               cparms.hwndfocus = hSource;
               tid5 = _beginthread (threadall, NULL, 8192, &cparms);

               WinSetFocus (HWND_DESKTOP, hwndfocus);//filescntr);
}
//--------------------------------------------------------------------
// This is the Copy Files routine used by the CopyDlgProc and the DragDropDlgProc
// thread feeder.
//--------------------------------------------------------------------
BOOL DoCopyOp (PSZ pszName, PSZ pszPath, USHORT replace, USHORT cPath, USHORT op,
                                                                                       HWND hSource)
{
      CHAR szEditbuffer [CCHMAXPATH + 1];
      CHAR szSourcename [CCHMAXPATH + 1];
      CHAR szCopyname [CCHMAXPATH + 1];
      CHAR szCopypath [(CCHMAXPATH * 2) + 1];
      SHORT noUpdate, current;
      ULONG lentest;
      PSZ pDestination;
      HWND hwndsource;

      // It is possible to copy complete directory trees from either the Files window
      // or the Tree window.
      // Determine if the source window is the Files window or the Tree window
      // and set the container handle to suit.

      hwndsource = hSource;

      // Test for destination drive ready (media inserted etc.).
      // Derive the destination drive from the path since it may have been changed
      // from the current destination.

      int nofiles;
      ULONG dDrive;
      dDrive = pszPath [0];
      if (dDrive > 96) dDrive -= 96;
      else dDrive -= 64;
      rc = IsDriveReady (dDrive);
      if (!rc) return TRUE;

      // Test for source drive ready (media inserted etc.).

      rc = IsDriveReady (CurrDrive);
      if (!rc) return TRUE;

      strcpy (szCopyname, pszName);
      strcpy (szCopypath, pszPath);

      noUpdate = 1;
      current = 1;

      // Compare the source and the destination.  If they are the same, the
      // files container will have to be updated to show the new files.
      // This will only happen if the files in the source directory are being
      // copied to another filename in the same directory.
      // The 'current' flag will trigger the update if it is zero.
      // The update will take place at the end of this routine.

      // If the source container is the Tree container, compare the destination subdir and the
      // Files title.

      if (hwndsource == hwnddircntr)
      {
         strcpy (szSourcename, szCopypath);
      }
      else // It's the files container.
      {
         strcpy (szSourcename, szFilesTitle);
      }
      lentest = strlen (szSourcename);

      // No need to compare the source and destination if the path lengths are unequal.

      if (cPath == lentest)
      {
         current = (SHORT) strnicmp (szFilesTitle, szCopypath, cPath);
      }

      // A search will be made for selected records.  The search criteria for the first
      // record is CMA_FIRST, but for the following records, the criteria is the
      // record of the preceeding search.

      pWhatRec = (PCNRITEM) CMA_FIRST;

      // Set a trap to catch if no files were selected.

      nofiles = 1;

      while (pWhatRec)
      {
         // Test to see if a single file under the mouse is being used (that was not
         // selected) in a Drag&Drop manouver.  If so the selected files must be
         // ignored while this file is used.

         if (DropDestOp.recundermouse) pWhatRec = (PCNRITEM) DropDestOp.pRec;

         // Otherwise scan for selected files.

         else
         pWhatRec = (PCNRITEM) WinSendMsg (hwndsource,
                               CM_QUERYRECORDEMPHASIS, MPFROMP (pWhatRec),
                                                      MPFROMSHORT (CRA_SELECTED));
         if (pWhatRec)
         {
            // Put up the hourglass cursor if ours is the active program.

            if (WinQueryActiveWindow (HWND_DESKTOP) == hwndframe)
            {
               WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP,
                                                                        SPTR_WAIT, FALSE));
            }

            // If only a drive name eg. "X:\" is shown, the backslash will not have to
            // be added to the path before the filename is tacked on.

            nofiles = 0;
            szCopypath [cPath] = 0;
            if (cPath != 3) strcat (szCopypath, "\\");
            else if (szCopypath [1] != ':') strcat (szCopypath, "\\");

            // Filter the new and old names through the DosEditName filter to handle the
            // wildcards and produce a new filename, if this is a Copy operation.

            // First, do a little fix-up here in case this was a subdir name (it ends with a "\").
            // DosEditName seems to choke on this every once and a while, although the
            // documentation seems to indicate that it should be ok.  Therefore, I remove the
            // backslash. (DoCopyOp now will copy complete directory trees)
            // Cover the possibilty that someone has edited the name and also that the
            // possibility that the name string still carries an asterisk.

            USHORT lenCopyname;
            lenCopyname = (USHORT) strlen (szCopyname);
            if (szCopyname [lenCopyname - 1] == 92)
               szCopyname [lenCopyname -1] = 0;

            strcpy (szSourcename, pWhatRec->szFileName);
            lenCopyname = (USHORT) strlen (szSourcename);
            if (szSourcename [lenCopyname - 1] == 92)
               szSourcename [lenCopyname - 1] = 0;

            if (op == COPY)
               DosEditName (1, szSourcename, szCopyname, szEditbuffer,
                                                                              CCHMAXPATH +1);
            else strcpy (szEditbuffer, pWhatRec->szFileName);

            // Tack the new filename onto the destination path and the original
            // filename onto the source path.

            strcat (szCopypath, szEditbuffer);

            strcpy (szSourcename, szFilesTitle);
            if (szSourcename [strlen (szSourcename) - 1] != 92)
               strcat (szSourcename, "\\");
            strcat (szSourcename, pWhatRec->szFileName);

            pDestination = szCopypath;

            // If it is a directory that has been selected, prepare to copy the entire tree structure
            // from that node on down.

            if (pWhatRec->attrFile  & 16)
            {
               USHORT usResponse;
               if (op == MOVE)
               {
                  usResponse = (USHORT) WinMessageBox (HWND_DESKTOP, hwndframe,
                              "The MOVE operation is not available for directory trees.\n"
                              "To Move directory tree structures, first Copy the tree"
                              " and then delete the original.",
                              "Notice!", MESSAGEBOXID,
                              MB_OK | MB_WARNING | MB_DEFBUTTON2);
                  return TRUE;
               }
               else
               {
                  CHAR *pszrec;
                  pszrec = new CHAR [CCHMAXPATH * 2 + 2];

                  // Copy the filename into a buffer but trim off the trailing backslash
                  // and enclose it in quotes.

                  strcpy (pszrec, "\"");
                  strcat (pszrec, pWhatRec->szFileName);

                  // If the source container is the Files container, a backslash has been appended
                  // to all the subdir names for appearance sake in the files window.
                  // Trim it off.

                  if (hwndsource == hwndfilescntr)
                     pszrec [strlen (pWhatRec->szFileName)] = 0;
                  strcat (pszrec, "\"");
                  strcat (pszrec, "\nThis Directory may contain Files\n and/or other "
                              "Directories which may also contain Files.\n\n"
                              "Do you wish to recreate this directory and all of it's contents?");
                  usResponse = (USHORT) WinMessageBox (HWND_DESKTOP, hwndframe,
                              pszrec, "WARNING!", MESSAGEBOXID,
                              MB_OKCANCEL | MB_WARNING | MB_DEFBUTTON1);

                  delete pszrec;
                  if (usResponse == MBID_OK)
                  {
                     // Put up the hourglass cursor.

                     WinSetPointer (HWND_DESKTOP, WinQuerySysPointer
                                               (HWND_DESKTOP, SPTR_WAIT, FALSE));

                     rc = CopyTree (pWhatRec->szDirPath, szEditbuffer, szCopypath);

                     // If the copy was good, deselect the file.

                     if (!rc) WinSendMsg (hwndsource, CM_SETRECORDEMPHASIS,
                         MPFROMLONG (pWhatRec), MPFROM2SHORT (FALSE, CRA_SELECTED));

                     // The windows must both be rebuilt.

                     noUpdate = 2;

                     // First, clear the trap for "No Files Were Selected"

                     nofiles = 0;
                  }
                  else return TRUE;
               }
            }
            else
            {
               rc = DosCopy (szSourcename, pDestination, replace);

               // If this is a Move operation, AND the copy was successful,
               // delete the original file.

               if (op == MOVE)
                  if (!rc) DosDelete (szSourcename);

               // If the copy was good, deselect the file.

               if (!rc) WinSendMsg (hwndsource, CM_SETRECORDEMPHASIS,
                      MPFROMLONG (pWhatRec), MPFROM2SHORT (FALSE, CRA_SELECTED));

               // If copy failed, test for invalid path errror and if so, query user if the
               // subdirectory should be created.  All subdirectories between the Drive
               // letter and the target subdirectory must be valid or the creation will fail.
               // This circumstance can occur if the user types in a path into the entry field
               // for copying to a destination that doesn't currently exist.

               if (rc == ERROR_PATH_NOT_FOUND)
               {
                  USHORT usResponse;
                  usResponse = (USHORT) WinMessageBox (HWND_DESKTOP, hwndframe,
                                 "Path not found!\nCreate Target Directory?", "WARNING!",
                                              MESSAGEBOXID, MB_OKCANCEL | MB_QUERY);
                  if (usResponse == MBID_OK)
                  {
                     // Create the new directory.

                     szCopypath [cPath] = 0;
                     rc = DosCreateDir (pDestination, NULL);

                     // No need to return after an error since nothing has been done.

                     if (!rc)
                     {
                        // Try again to copy the files and report any errors.

                        strcat (szCopypath, "\\");
                        strcat (szCopypath, szEditbuffer);
                        rc = DosCopy (szSourcename, pDestination, replace);

                        // If this is a Move operation, AND the copy was successful,
                        // delete the original file.

                        if (op == MOVE)
                           if (!rc) rc = DosDelete (szSourcename);

                        // If the copy was good, deselect the file.

                        if (!rc) WinSendMsg (hwndsource, CM_SETRECORDEMPHASIS,
                                              MPFROMLONG (pWhatRec),
                                              MPFROM2SHORT (FALSE, CRA_SELECTED));

                        // Set the refresh flag to indicate that the Tree windows may
                        // need refreshed since a new directory has been created somewhere.

                        noUpdate = 2;

                        // If it was a Copy operation, and the Source drive is the same as the
                        // Destination drive,  set the flag that will refresh the files container
                        // just in case the directory created on the fly is in the current directory.

                        if (op == COPY)
                        {
                           if (CurrDrive == DestDrive) current = 0;
                        }
                     }
                     else
                     {
                        MessageBox (hwndframe, rc);
                        pWhatRec = NULL;
                     }
                  }
                  else pWhatRec = NULL; // break from loop.
               }
               // If the ACCESS DENIED error occurs, this could be because the attributes are
               // read-only, hidden or system, OR because the file already exists.
               // Here, this is determined and if the file already exists and is archived or normal,
               // the user is asked if it should be overwritten.  This is an additional feature
               // to supplement the "Replace Existing Files" checkbox.
               // If the file is Read-Only, Hidden or System, the usual error (ACCESS DENIED) is
               // displayed in a messagebox.

               if (rc == ERROR_ACCESS_DENIED)
               {
                  // First, get the destination file's attributes.

                  unsigned oldattributes;
                   _dos_getfileattr (pDestination, &oldattributes);

                  if ((oldattributes == _A_NORMAL) || (oldattributes == _A_ARCH))
                  {
                     CHAR *pszrec;
                     pszrec = new CHAR [CCHMAXPATH * 2 +2];

                     // Copy the filename into a buffer and enclose it in quotes.

                     strcpy (pszrec, "\"");
                     strcat (pszrec, pWhatRec->szFileName);

                     strcat (pszrec, "\"");
                     strcat (pszrec, "\nThis File already exists.\n\n"
                                 "Do you wish to overwrite it?");
                     USHORT usResponse;
                     usResponse = (USHORT) WinMessageBox (HWND_DESKTOP, hwndframe,
                                     pszrec, "WARNING!", MESSAGEBOXID,
                                  MB_OKCANCEL | MB_WARNING | MB_DEFBUTTON2);
                     delete pszrec;
                     if (usResponse == MBID_OK)
                     {
                        // Put up the hourglass cursor.

                        WinSetPointer (HWND_DESKTOP, WinQuerySysPointer
                                            (HWND_DESKTOP, SPTR_WAIT, FALSE));
                        rc = DosCopy (szSourcename, pDestination, TRUE);

                        // If the copy was good, deselect the file.

                        if (!rc)
                        {
                           WinSendMsg (hwndsource, CM_SETRECORDEMPHASIS,
                              MPFROMLONG (pWhatRec), MPFROM2SHORT (FALSE, CRA_SELECTED));

                           // If this is a Move operation, AND the copy was successful,
                           // delete the original file.

                           if (op == MOVE)
                              DosDelete (szSourcename);
                        }

                        if (rc)
                        {
                           MessageBox (hwndframe, rc);
                           pWhatRec = NULL;
                        }
                     }
                     // If the response was CANCEL
                     else
                     {
                        // Break out of the loop for this item only.
                        pWhatRec = NULL;
                     }
                  }
                  // If the target file's attributes were other than ARCHIVE, report an error
                  // (ACCESS DENIED).
                  else
                  {
                     MessageBox (hwndframe, rc);
                     pWhatRec = NULL;
                  }
               }
               // Take the error number to the MessageBox routine for display of the error.
               // After display, return to the routine.  This is done by setting pWhatRec to
               // NULL in order to break out of the loop.  Then if the files directory needs
               // refreshed, this can happen.

               else if (rc)
               {
                  MessageBox (hwndframe, rc);
                  pWhatRec = NULL;
               }
            }
            // If the record being used was a single record under the mouse (not one
            // of the selected records) we must break out of the loop now.

            if (DropDestOp.recundermouse)
            {
               pWhatRec = NULL;
               DropDestOp.recundermouse = 0;
            }
         }
      }
      // A message that notifies the user that 'No Files Were Selected!'.

      if (nofiles) MessageBox (hwndframe, 70000);

      // If a target directory was created on the fly, check whether it was on  the
      // current source drive.  If it was, refresh the display in case the directory
      // should be visible.

      if (noUpdate == 2)
      {
         if ((szCopypath [0] == CurrDrive + 64) || (szCopypath [0] == CurrDrive + 96))
         {
            // Rebuild the root tree.  This can be done without triggering a refilling
            // of the files container with the root files.

            RefillTree (CurrDrive, USEPATH, hwnddircntr, DTree, NULL);
         }
      }

      // Refill the files container if it was also the destination of the copy or
      // if it was a move operation since the container will repectively gain and lose records.

      if ((current == 0) || (op == MOVE))
      {
         // Find the record that has the cursor on it and copy the filename to a buffer.
         // It will be needed below in the returnto () function.

         pWhatRec = (PCNRITEM) WinSendMsg (hwndsource,
                               CM_QUERYRECORDEMPHASIS, MPFROMP (CMA_FIRST),
                                                      MPFROMSHORT (CRA_CURSORED));
         strcpy (tparms7.szSavename, pWhatRec->szFileName);

         // Refill the files container. Don't use a thread here because of the
         // returnto () function at the end of this section.

         DFiles->EmptyContainer ();

         // Fill the files container and update the source status bar.

         DFiles->FillContainer (filefilter);

         // This statusbar information can't be gathered until the container is filled.

         fillStatusbar2 (NULL, NULL);

         // Place the cursor back on the original cursored file.

                  tparms7.Hab = NULL;
                  tparms7.Hmq = NULL;
                  tparms7.hwnd = hwndsource;
                  tparms7.job = RETURNTO;
                  tid7 = _beginthread (threadall, NULL, 8192, &tparms7);
      }
      else
      {
         // Update the Destination status bar info with new bytes free etc.

         fillStatusbar1 (DestDrive, (PSZ) szDestPath);
      }

      // Update the root window titlebar byte count.  This would normally only be
      // required if the destination was on the same drive as the source, but it
      // is an inexpensive process so it is done all the time rather than use more
      // code to decide if it is necessary.

      getFSInfo (CurrDrive);

      // Put up the regular cursor.

      WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP,
                                                                        SPTR_ARROW, FALSE));

      // If this is bit is set, the origin of this process was another instance of FF.
      // This means that the caller's object window handle is stored and a message
      // must be sent to it telling it to refresh the displays due to a change in contents
      // and byte counts.

      if ((DropDestOp.Operation & 64) == 64)
      {
            WinPostMsg (DropDestOp.hwndCaller, OBJM_REFRESH, NULL, NULL);
      }

      // Clear this variable now.

      DropDestOp.recundermouse = 0;

      return FALSE;
}
//----------------------------------------------------------------
// This is an Unzip routine that is used by both the main window procedure
// unzip routine and also the Drag/Drop unzip procedure.
// The hwnd parameter has no use right now.  It was used earlier but now it's not.
//----------------------------------------------------------------
BOOL DoUnzip (SHORT tempmsg, HWND hwnd)
{
      // A search will be made for the first selected record.
      // If there is not a file highlighted, the process will pass.
      // This option is designed for use with InfoZips 'UNZIP' since the parameters
      // required for unzipping to a destination directory are unique to UNZIP
      // and are inserted here.  This routine also uses UNZIP to test a zip and
      // must distinguish which command was given.
      // If UNZIPTODIR was the command, it will be handled just before the
      // UNZIP routine.

      PCNRITEM pUnzipRec;

      // Test to see if a single file under the mouse is being used (that was not
      // selected) in a Drag&Drop manouver.  If so the selected files must be
      // ignored while this single file is used.

      if (DropDestOp.recundermouse)
      {
         // Note:
         //      DropDestOp.recundermouse is cleared in WM_APPTERMINATENOTIFY

         pUnzipRec = (PCNRITEM) DropDestOp.pRec;
      }

      // Otherwise scan for selected files.

      else
      pUnzipRec = (PCNRITEM) WinSendMsg (hwndfilescntr,
                               CM_QUERYRECORDEMPHASIS, MPFROMP (CMA_FIRST),
                                                      MPFROMSHORT (CRA_SELECTED));
      if (pUnzipRec)
      {
         CHAR sztemp [5];

         // Get the path and filename stub of the file selected.
         // Begin to surround the filepathname here in quotes so that filenames with
         // embedded spaces will not fail.

         sztemp [0] = 32;
         sztemp [1] = 34;
         sztemp [2] = CHAR (CurrDrive + 64);
         sztemp [3] = 58;
         sztemp [4] = 0;

         // Determine whether this is a zip test or an unzip call.  They require
         // different parameters which are inserted as needed.

         if (tempmsg != MID_ZIPTEST)      // This is an Unzip request
         {
            // This will be a request to create a directory on the fly with the same
            // name as the zip file.

            if (tempmsg == MID_UNZIPTODIR)
            {
               // Find where the zip file name terminates (before the extension)
               // and use this name as the new directory name.

               int namelen, stub;
               namelen = strlen (pUnzipRec->szFileName);
               stub = namelen;
               for (int finddot = 0; finddot < namelen; finddot ++)
               {
                  if (pUnzipRec->szFileName [finddot] == 46) stub = finddot;
               }
               PSZ pszNewdir;
               pszNewdir = new CHAR [namelen +1];
               strcpy (pszNewdir, pUnzipRec->szFileName);
               pszNewdir [stub] = 0;

               strcpy (sznamebuffer, szDestPath);
               if (strlen (szDestPath) != 3) strcat (sznamebuffer, "\\");
               else if (szDestPath [1] != ':') strcat (sznamebuffer, "\\");
               strcat (sznamebuffer, pszNewdir);

               // Note:  DosCreateDir does not need directory names surrounded in quotes in
               //        the case of embedded spaces etc.

               rc = DosCreateDir (sznamebuffer, NULL);
               if (rc)
               {
                  PSZ pszmessage;
                  pszmessage = new CHAR [namelen + 70];
                  strcpy (pszmessage, "Can't Create Directory for file name\n\"");
                  strcat (pszmessage, pszNewdir);
                  strcat (pszmessage, "\"\n");
                  strcat (pszmessage, "It may already exist!");
                  WinMessageBox (HWND_DESKTOP, hwnd, pszmessage,
                            "ERROR!", MESSAGEBOXID, MB_OK | MB_ERROR);
                  delete pszNewdir;
                  delete pszmessage;
                  return FALSE;
               }
               delete pszNewdir;
            }

            // Copy the parameters to the parameter buffer.
            // Test whether the user has used a '-' or not and compensate.
            // This makes the parameter foolproof.

            strcpy (szpathbuffer, "-");

            PSZ pszparm;
            if (szUnzipparms [0] == '-') pszparm = &szUnzipparms [1];
            else pszparm = szUnzipparms;
            strcat (szpathbuffer, pszparm);

            // The process of surrounding the filenamepath in quotes is finished here.

            strcat (szpathbuffer, sztemp);
            strcat (szpathbuffer, pUnzipRec->szDirPath);
            strcat (szpathbuffer, "\"");

            // The next parms are 'UNZIP's way of unzipping to a destination directory.
            // The parameter and destination follow all other parameters, creating a
            // sandwich.
            // The destination directory name is also surrounded in quotes in case it has
            // embedded spaces etc.

            strcat (szpathbuffer, " -d \"");
            if (tempmsg == MID_UNZIPTODIR)  strcat (szpathbuffer, sznamebuffer);
            else strcat (szpathbuffer, szDestPath);

            // Before the destination directory name is finally closed with quotes, make
            // sure that it isn't ended with a trailing backslash as in the case of a root
            // dir name eg. "J:\".  This won't be accepted and the unzip will choke.
            // Therefore add a single dot to indicate a root directory.  Otherwise,
            // if the destination drive is the home of FF, it will dump it in the home
            // directory (it's the default) instead of the root.

            INT parmslen;
            parmslen = strlen (szpathbuffer);
            if (szpathbuffer [parmslen -1] == 92) strcat (szpathbuffer, ".");
            strcat (szpathbuffer, "\"");

            // Fill in the PROGDETAILS structure for the WinStartApp function.
            // Using WinStartApp instead of DosStartSession here because of the
            // possible neccesity to update the source directory if the zipfile in the
            // source is unzipped into the source.  Since DosStartSession spins off
            // on it's own thread, there is no simple way to tell when it has finished
            // it's unzipping.  Code following the code to start these sessions
            // (including WinStartApp) is processed before the sessions have a chance
            // to complete the job, so the filescontainer would be updated before the
            // files are unzipped.  With WinStartApp this is circumvented since it
            // posts a WM_APPTERMINATENOTIFY message when it is finished,
            // at which time the filescontainer is refreshed.

            pDetails = &Details;
            pDetails->progt.progc     = PROG_WINDOWABLEVIO;
            pDetails->progt.fbVisible = SHE_VISIBLE;
            pDetails->pszTitle        = "UnArchiver";
            pDetails->pszStartupDir   = NULL;
            pDetails->pszExecutable   = (PSZ) szUnzipprog;

            // Change the current dir to the home dir so that the Info-zip utilities can be
            // found if they are there.

            DosSetDefaultDisk (Homedrive);
            DosSetCurrentDir (szHomepath);

            // Start Info-Zip.  When it is finished, a notification message will
            // be sent to WM_APPTERMINATENOTIFY.

            WinStartApp (hwndclient, pDetails, szpathbuffer, NULL, SAF_STARTCHILDAPP);

            // Change the drive back to where we were.

            DosSetDefaultDisk (CurrDrive);
         }
         else  // This is a Zip Test then.
         {
            // Copy the parameters to the parameter buffer.

            strcpy (szpathbuffer, "-tM");

            // The process of surrounding the filenamepath in quotes is finished here.

            strcat (szpathbuffer, sztemp);
            strcat (szpathbuffer, pUnzipRec->szDirPath);
            strcat (szpathbuffer, "\"");

            // Send the command string to DosStartSession.  DosStartSession is used
            // because it gives greater control over the way the process is presented,
            // in particular, it allows the window to persist until the user dismisses it.
            // Note: I just recently found out that I could do the same with WinStartApp
            // by using a SWP_NOAUTOCLOSE flag.

            //  Specify the various session start parameters yet to be filled in //

            StartData.Related = 1;  // Establish a Child session so focus returns at end//
            StartData.FgBg = SSF_FGBG_FORE; // Start session in FOREground //
            StartData.PgmName = (PSZ) szUnzipprog;  // Program path-name string //
            StartData.PgmInputs = szpathbuffer; // arguments passed to the program//
            StartData.SessionType = SSF_TYPE_WINDOWABLEVIO; // the session type //
            StartData.PgmControl = SSF_CONTROL_VISIBLE | SSF_CONTROL_NOAUTOCLOSE;
                                             // Start as visible and persistent //

            // Change the current dir to the home dir so that the Info-zip utilities can be
            // found if they are there.


            DosSetDefaultDisk (Homedrive);
            DosSetCurrentDir (szHomepath);

            // Start this mini session.  If there is an error, beep.

            rc = DosStartSession (&StartData, &SessID, &pid);

            // Change the drive back to where we were.

            DosSetDefaultDisk (CurrDrive);

            if (rc != 0) DosBeep (1000,100);
         }

         // NOTE: The file is deselected in the WM_APPTERMINATENOTIFY message
         //         response.

      }
      return TRUE;
}
//---------------------------------------------------------------
// A small routine to compare the source and destination directories and update
// the files container if they are the same.
//---------------------------------------------------------------
BOOL CompareandRefresh ()
{
      BOOL bStatus;
      bStatus = 0;

      // Compare the source and the destination.  If they are the same, the
      // files container will have to be updated to show the new file.
      // This will only happen if the files in the source directory are being
      // zipped/unzipped etc. to the same directory.
      // The 'test' flag will trigger the update if it is zero.

      test = 1;
      ltemp = strlen (szFilesTitle);
      ltemp1 = strlen (szDestPath);
      if (ltemp == ltemp1)
         test = (SHORT) strnicmp (szFilesTitle, szDestPath, ltemp);

      // Refill the files container if it was also the destination of the copy.

      if (!test)
      {
         bStatus = TRUE;
         DFiles->EmptyContainer ();

         // Fill the files container and update the source status bar.
         // Using a thread here can cause latency problems elsewhere.
         // (this is when records that have been removed but still not
         // replaced are being accessed with null pointers resulting).

         DFiles->FillContainer (filefilter);

         // This statusbar information can't be gathered until the container is filled so it must
         // follow it in this thread.  It can't be done outside of the thread unless the container
         // is filled outside of the thread.

         fillStatusbar2 (NULL, NULL);

         // Update the root window titlebar byte count.

         getFSInfo (CurrDrive);
      }
      return bStatus;
}
//-----------------------------------------------------------------------
// This is used in a couple of places to handle the menu View settings.  If one is
// checked, the others must be unchecked etc.
// It is used by both the main menu and the popup menu since they use the same template
// and control IDs.
//-----------------------------------------------------------------------
void SetView (ULONG View, HWND hMenu)
{
      // First, disable the headings item and uncheck all the others.

      WinSendMsg (hMenu, MM_SETITEMATTR, MPFROM2SHORT
                                      (MID_FILEHEADINGS, TRUE), MPFROM2SHORT
                                                      (MIA_CHECKED, NULL));
      WinSendMsg (hMenu, MM_SETITEMATTR, MPFROM2SHORT
                                      (MID_FILEHEADINGS, TRUE), MPFROM2SHORT
                                                      (MIA_DISABLED, MIA_DISABLED));

      WinSendMsg (hMenu, MM_SETITEMATTR, MPFROM2SHORT
                                      (MID_FILEFLOWED, TRUE), MPFROM2SHORT
                                                      (MIA_CHECKED, NULL));
      WinSendMsg (hMenu, MM_SETITEMATTR, MPFROM2SHORT
                                      (MID_FILEICON, TRUE), MPFROM2SHORT
                                                      (MIA_CHECKED, NULL));
      WinSendMsg (hMenu, MM_SETITEMATTR, MPFROM2SHORT
                                      (MID_FILEDETAIL, TRUE), MPFROM2SHORT
                                                      (MIA_CHECKED, NULL));
      WinSendMsg (hMenu, MM_SETITEMATTR, MPFROM2SHORT
                                         (MID_DEFAULTVIEW, TRUE), MPFROM2SHORT
                                                         (MIA_CHECKED, NULL));

      switch (View)
      {
         case DETAILVIEW:

            // A method to set the details view in the "View" menu.  It is also
            // responsible for enabling or disabling the 'Column Headings' menu item
            // which of course can't be enabled if the details view is disabled.

            WinSendMsg (hMenu, MM_SETITEMATTR, MPFROM2SHORT
                                            (MID_FILEHEADINGS, TRUE), MPFROM2SHORT
                                                            (MIA_DISABLED, NULL));
            WinSendMsg (hMenu, MM_SETITEMATTR, MPFROM2SHORT
                                            (MID_FILEDETAIL, TRUE), MPFROM2SHORT
                                                            (MIA_CHECKED, MIA_CHECKED));
            break;

         case FLOWVIEW:

            // The Text Flowed view is enabled.

            WinSendMsg (hMenu, MM_SETITEMATTR, MPFROM2SHORT
                                                (MID_FILEFLOWED, TRUE), MPFROM2SHORT
                                                             (MIA_CHECKED, MIA_CHECKED));
            break;

         case ICONVIEW:

            // The Name Flowed view is enabled.
            // The Files container must also be reloaded if this view is being enabled.
            // The records are stored with icon pointers only if the 'CV_NAME' attribute
            // is set.  This is to speed up loading if the icons are not required.

            WinSendMsg (hMenu, MM_SETITEMATTR, MPFROM2SHORT
                                               (MID_FILEICON, TRUE), MPFROM2SHORT
                                                               (MIA_CHECKED, MIA_CHECKED));
            break;

         case COLUMNHEADINGS:

            WinSendMsg (hMenu, MM_SETITEMATTR, MPFROM2SHORT
                                                  (MID_FILEHEADINGS, TRUE), MPFROM2SHORT
                                                                  (MIA_CHECKED, MIA_CHECKED));
            WinSendMsg (hMenu, MM_SETITEMATTR, MPFROM2SHORT
                                                  (MID_FILEDETAIL, TRUE), MPFROM2SHORT
                                                                  (MIA_CHECKED, MIA_CHECKED));
            break;

         case TEXTVIEW:

            // The Text view is enabled.

            WinSendMsg (hMenu, MM_SETITEMATTR, MPFROM2SHORT
                                               (MID_DEFAULTVIEW, TRUE), MPFROM2SHORT
                                                               (MIA_CHECKED, MIA_CHECKED));
      }
}
//-------------------------------------------------------------------
// This is a Copy Tree routine.  It will find all decendant subdirectories and all files
// in the specified and subordinate subdirectories and create the subdirectories in
// the destination and copy the source files to them.
// PSZ pszdirname is the source path in the form of "\sourcepath\dirname"
// PSZ pszName is the name of the directory only eg. "dirname"
// PSZ pszTargetPath is the name of the full target path eg. "H:\test\dirname"
//-------------------------------------------------------------------
BOOL CopyTree (PSZ pszdirname, PSZ pszName, PSZ pszTargetPath)
{
      // A structure that holds pointers to strings and other info needed in the
      // MakeDirs function.  It is defined in "cnrstuff.h"

      PCOPYTREEROOTS pCTroots;
      pCTroots = new COPYTREEROOTS;

      // Start to initialize the stable members of the COPYTREEROOTS structure here.

      pCTroots->pszOrgName = pszName;
      pCTroots->pszOrgTarget = pszTargetPath;

      // These buffers are used with the DosFindFirst etc. functions.  The large array
      // of buffers holds information on many files at a time.

      PFILEFINDBUF3 pNextbuf;
      PBYTE pbBuf;
      PFILEFINDBUF3 pFilebuf;
      pFilebuf = new FILEFINDBUF3 [100];
      pbBuf = (PBYTE) pFilebuf;
      pNextbuf = pFilebuf;
      int pathlen, stub, findbackslash;

      // Create an array of Handles to hold the directory handle for each
      // successive level of depth.  If it goes greater than 100 levels deep (Egads!)
      // the routine is aborted (below).

      HDIR hDir [100];
      INT hdirindex;
      PSZ pszsourcepath, pszdestpath;
      pszsourcepath = new CHAR [CCHMAXPATH + 1];
      pszdestpath = new CHAR [CCHMAXPATH + 1];

      // Create a flag to indicate if the target directory is a subordinate directory in
      // the same path.  This is important since if nothing is done about this, the
      // source directory will be replicated forever in each pass in a self-perpetuating
      // cycle.
      // For instance, if an attempt is made to copy "a:\thisdir\temp" to "a:\thisdir\temp\here"
      // the first pass will create "a:\thisdir\temp\here\temp" which then will include a copy
      // of "a:\thisdir\temp\here" which of course includes "a:\thisdir\temp\here\temp".

      PSZ ptemp;

      ptemp = pszTargetPath + 2;
      pCTroots->unrelated = strnicmp (ptemp, pszdirname, strlen (pszdirname));

      // Create a fully qualified source path.

      pszsourcepath [0] = CHAR (CurrDrive + 64);
      pszsourcepath [1] = 58;
      pszsourcepath [2] = 0;
      strcat (pszsourcepath, pszdirname);

      // The maximum number of files that are searched for at one time must match the
      // number of FILEFINDBUF3 buffers.

      ULONG maxfiles;
      maxfiles = 100;
      rc = 0;

      // Copy the new destination directory path.

      strcpy (pszdestpath, pszTargetPath);

      // Create the new directory.  This is needed to bootstrap the process.

      rc = DosCreateDir (pszdestpath, NULL);

      // Clean up and return after an error since nothing has been done.

      if (rc)
      {
         MessageBox (hwndframe, rc);
         delete pFilebuf;
         delete pszsourcepath;
         delete pszdestpath;
         delete pCTroots;
         return TRUE;
      }
      // The Copy operation will change (add to) our filespec so save the original length
      // so that it can be restored.  This first MakeCopies operation is still part of the
      // bootstrap process.

      pathlen = strlen (pszsourcepath);

      rc = MakeCopies (pszsourcepath, pszdestpath, pFilebuf);

      // Keep an index into the array of HDIR handles.  This will be needed later when
      // backtracking to a previous directory level and using DosFindNext.

      hdirindex = 0;

      // Make the decendent directories by pursuing the next single nested level until
      // there are no more nested levels.  The hdirindex will keep track of what level
      // is current.
      // Note:  By passing a the first member of the HDIR array as a pointer, it will
      //        allow the function to use it as if it were since it is an address reference
      //        to the whole array.
      // This is still part of the bootstrap process.  The last parameter passed indicates
      // that DosFindFirst will be used as the first call in the MakeDirs function.
      // These parameters are passed as part of the COPYTREEROOTS structure which
      // must have its dynamic members initialized before the MakeDirs call.

      pCTroots->pszCurDestPath = pszdestpath;
      pCTroots->pszCurDirPath = pszsourcepath;
      pCTroots->pathlen = pathlen;
      pCTroots->phdirbase = hDir;
      pCTroots->phdirindex = &hdirindex;
      pCTroots->findnextflag = 0;

      strcat (pszsourcepath, "\\*.*");
      rc = MakeDirs (pCTroots, pFilebuf);
      if (rc)
      {
         delete pFilebuf;
         delete pszsourcepath;
         delete pszdestpath;
         delete pCTroots;
         return TRUE;
      }

      // This is the main loop here.  It will cycle through this loop while there is still
      // and index level.  When the index level reaches 0, we have returned to the bootstrap
      // level which has already been created/copied and the process is complete.

      while (hdirindex)
      {
         hdirindex--;
         stub = 0;

         // Each pass through this loop must continually trim back the paths of the source and
         // destination directories since they are added to in the MakeDirs function below.
         // The path will match the index into the HDIR handles array so that further subdirs
         // may be located and copied with the DosFindNext api.

         pathlen = strlen (pszdestpath);
         for (findbackslash = 0; findbackslash < pathlen; findbackslash ++)
         {
            if (pszdestpath [findbackslash] == 92) stub = findbackslash;
         }
         pszdestpath [stub] = 0;

         // Make sure pathlen is set to length of pszsourcepath before MakeDir.

         pathlen = strlen (pszsourcepath);
         for (findbackslash = 0; findbackslash < pathlen; findbackslash ++)
         {
            if (pszsourcepath [findbackslash] == 92)
            {
               stub = findbackslash;
            }
         }
         pszsourcepath [stub] = 0;

         pCTroots->pszCurDestPath = pszdestpath;
         pCTroots->pszCurDirPath = pszsourcepath;
         pCTroots->pathlen = pathlen;
         pCTroots->phdirbase = hDir;
         pCTroots->phdirindex = &hdirindex;
         pCTroots->findnextflag = 1;

         rc = MakeDirs (pCTroots, pFilebuf);
         if (rc)
         {
            delete pFilebuf;
            delete pszsourcepath;
            delete pszdestpath;
            delete pCTroots;
            return TRUE;
         }
      }
      // The job is done so clean up.

      delete pFilebuf;
      delete pszsourcepath;
      delete pszdestpath;
      delete pCTroots;
      return FALSE;
}
//----------------------------------------------------------
// A function to make progressive directories in the CopyTree function.
// Each call will create only single subdirs in each level until no more levels
// can be found.  The caller must then retreat a level and try the next
// peer path.
// The last parameter passed (sub) indicates whether it is a DosFindFirst
// or a DosFindNext operation used in the first call.
// So far DosFindFirst is only used as the first call in the bootstrap part of the operation.
// This routine uses the strings and FILEFINDBUF3 array passed as parameters as if they
// were it's own.
//----------------------------------------------------------
BOOL MakeDirs (PCOPYTREEROOTS pRoots, PFILEFINDBUF3 pFfb)
{
      APIRET rc;
      ULONG maxfiles;
      PHDIR phD;
      INT pathlen;
      INT findnextflag;
      CHAR szinterim [CCHMAXPATH + 1];

      findnextflag = 0;

      // Set the HDIR array pointer to the first member.  The individual array members will
      // be accessed by adding the value of the index to the pointer.
      // If it is a DosFindFirst operation, the value will be set to allow DosFindFirst to assign
      // a unique id.

      phD = pRoots->phdirbase;
      if (pRoots->findnextflag)
      {
         phD = phD + *pRoots->phdirindex;
         findnextflag = 1;
      }
      else
      *phD = HDIR_CREATE;

      // Subdirs will only be retrieved one at a time.

      maxfiles = 1;
      pathlen = pRoots->pathlen;
      rc = 0;

      // Find the subdirs and create them in single peer fashion until a dead end is reached.
      // Each time a new subdir is created, the files in the source subdir will be copied to the
      // new dir.

      while (maxfiles)
      {
         if (findnextflag)
         {
            findnextflag = 0;
            rc = DosFindNext (*phD, pFfb, FF_BUFFSIZE, &maxfiles);
            if (rc == ERROR_NO_MORE_FILES)
            {
               pRoots->pszCurDirPath [pathlen] = 0;
               return FALSE;
            }
            else if (rc) return TRUE;
         }
         else
         rc = DosFindFirst (pRoots->pszCurDirPath, phD, MUST_HAVE_DIRECTORY, pFfb,
                                                           FF_BUFFSIZE, &maxfiles, FIL_STANDARD);
         if (rc) return TRUE;

         // Sluff of those pesky "." and ".." entries.

         if ((pFfb->cchName == 1) && (pFfb->achName [0] == '.'))
         {
            rc = DosFindNext (*phD, pFfb, FF_BUFFSIZE, &maxfiles);
            if (rc)
            {
               if (rc == ERROR_NO_MORE_FILES)
               {
                  pRoots->pszCurDirPath [pathlen] = 0;
                  return FALSE;
               }
               else if (rc) return TRUE;
            }
         }
         if ((pFfb->cchName == 2) && ((pFfb->achName [0] == '.') && (pFfb->achName [1] == '.')))
         {
            rc = DosFindNext (*phD, pFfb, FF_BUFFSIZE, &maxfiles);
            if (rc)
            {
               if (rc == ERROR_NO_MORE_FILES)
               {
                  pRoots->pszCurDirPath [pathlen] = 0;
                  return FALSE;
               }
               else if (rc) return TRUE;
            }
         }

         // Test for the source directory trying to copy itself in a decendant directory.
         // If it is, skip it.  This would repeat until the path overflowed.

         if (!pRoots->unrelated)
         {
            strcpy (szinterim, pRoots->pszCurDirPath);
            szinterim [pathlen] = 0;
            strcat (szinterim, "\\");
            strcat (szinterim, pFfb->achName);
            rc = stricmp (szinterim, pRoots->pszOrgTarget);
            if (!rc)
            {
               rc = DosFindNext (*phD, pFfb, FF_BUFFSIZE, &maxfiles);
               if (rc)
               {
                  if (rc == ERROR_NO_MORE_FILES)
                  {
                     pRoots->pszCurDirPath [pathlen] = 0;
                     return FALSE;
                  }
                  else if (rc) return TRUE;
               }
            }
         }

         // If a subdir was finally found, extend the source path.

         if (maxfiles)
         {
            pRoots->pszCurDirPath [pathlen] = 0;
            strcat (pRoots->pszCurDirPath, "\\");
            strcat (pRoots->pszCurDirPath, pFfb->achName);
            pathlen = strlen (pRoots->pszCurDirPath);
            strcat (pRoots->pszCurDirPath, "\\*.*");

            // Increment the index for the caller.

            (*pRoots->phdirindex)++;

            // Increment the pointer for the next HDIR handle and give it a value required for
            // a system assigned handle in DosFindFirst.

            phD++;
            *phD = HDIR_CREATE;

            // Create a new destination subdir.

            strcat (pRoots->pszCurDestPath, "\\");
            strcat (pRoots->pszCurDestPath, pFfb->achName);
            rc = DosCreateDir (pRoots->pszCurDestPath, NULL);

            // Clean up and return after an error since nothing has been done.

            if (rc) return TRUE;

            // Otherwise, copy all the files from the source subdir to the new subdir.

            else
            {
               pRoots->pszCurDirPath [pathlen] = 0;
               rc = MakeCopies (pRoots->pszCurDirPath, pRoots->pszCurDestPath, pFfb);
               strcat (pRoots->pszCurDirPath, "\\*.*");
            }
         }
      }
      return FALSE;
}
//----------------------------------------------------------
// A function to copy files for the CopyTree and MakeDirs function.
// The source and destination paths must be provided as well as access
// to the FINDFILEBUF3 array that was created by the CopyTree caller.
//----------------------------------------------------------
BOOL MakeCopies (PSZ pszsource, PSZ pszdest, PFILEFINDBUF3 pFfb)
{
      HDIR hDir;
      ULONG fileattrs, maxfiles;
      PSZ pszsourcebuf, pszdestbuf, pszsourcetail, pszdesttail;
      PFILEFINDBUF3 pNextbuf;
      PBYTE pbNext;
      INT findbackslash, sourcelen, stub;
//..

// This is some debugging code.

//      USHORT usResponse;
//      usResponse = (USHORT) WinMessageBox (HWND_DESKTOP, hwndframe,
//                      pszsource, pszdest, MESSAGEBOXID,
//                      MB_OK);

//..
      // Create some new buffers here so there is a bit of redundancy.  There could be many
      // files here and a local buffer is probably safer.  This also allows some greater freedom
      // in the way paths are manipulated.
      // They will be deleted at the end of this routine.

      pszsourcebuf = new CHAR [(CCHMAXPATH * 2) +1];
      pszdestbuf = new CHAR [(CCHMAXPATH *2) + 1];
      strcpy (pszsourcebuf, pszsource);
      strcat (pszsourcebuf, "\\*.*");
      strcpy (pszdestbuf, pszdest);

      hDir = HDIR_SYSTEM;
      fileattrs =  FILE_ARCHIVED | FILE_SYSTEM | FILE_HIDDEN | FILE_READONLY;

      // Save the length of the source path string with a trailing "\".

      sourcelen = strlen (pszsourcebuf);
      for (findbackslash = 0; findbackslash < sourcelen; findbackslash ++)
      {
         if (pszsourcebuf [findbackslash] == 92) stub = findbackslash;
      }
      pszsourcetail = &pszsourcebuf [stub + 1];

      //  Set up the destination path to accept a filename.

      strcat (pszdestbuf, "\\");
      pszdesttail = &pszdestbuf [strlen (pszdestbuf)];

      // Set up the FINDFILEBUF3 pointers.

      pNextbuf = pFfb;
      pbNext = (PBYTE) pFfb;

      // Get files from the directory in bunches of 100.

      maxfiles = 100;
      rc = DosFindFirst (pszsourcebuf, &hDir, fileattrs, pFfb, FF_BUFFSIZE,
                          &maxfiles, FIL_STANDARD);

      // Provide an escape if this was an empty directory.

      if (rc) maxfiles = 0;

      // Process the first batch of files.  If there were more than the original order (100),
      // a new batch will be picked up by DosFindNext below.

      while (maxfiles)
      {
         // Put up the hourglass cursor.

         WinSetPointer (HWND_DESKTOP,
                                        WinQuerySysPointer (HWND_DESKTOP, SPTR_WAIT, FALSE));

         for (ii = 0; ii < maxfiles; ii ++)
         {
            *pszsourcetail = 0;
            *pszdesttail = 0;
            strcat (pszsourcetail, pNextbuf->achName);
            strcat (pszdesttail, pNextbuf->achName);
            rc = DosCopy (pszsourcebuf, pszdestbuf, NULL);
            pbNext += pNextbuf->oNextEntryOffset;
            pNextbuf = (PFILEFINDBUF3) pbNext;
         }
         if (maxfiles == 100)
         {
            rc = DosFindNext (hDir, pFfb, FF_BUFFSIZE, &maxfiles);

            // Reset the FINDFILEBUF3 pointers.

            pNextbuf = pFfb;
            pbNext = (PBYTE) pFfb;

            if (rc) maxfiles = 0;
         }
         else maxfiles = 0;
      }
      // The job is done.

      delete pszsourcebuf;
      delete pszdestbuf;
      return FALSE;
}
//----------------------------------------------------------
// A function to display a message when an operation is chosen that
// would interfere with a copy/move operation currently in progress.
//----------------------------------------------------------
void NoCanDo ()
{
   WinMessageBox (HWND_DESKTOP, hwndframe, "Copy/Move Operation currently in progress!\n",
                                                       "Please Wait!", NULL, MB_OK | MB_ERROR);
}
//----------------------------------------------------------
// A function to handle the SORT routines for the FILES container.
// This also sets the menu item's checked or unchecked status.  This is
// required for both the popup and main menu.
//----------------------------------------------------------
void SortFiles (int sorttype, HWND hOrgmenu)
{
      // Save the current sort method - it's used in the ini file.
      sortmethod = sorttype;

      // Clear all the menu items first.  The individual item will be checked
      // below in the case: statement.

      WinSendMsg (hOrgmenu, MM_SETITEMATTR, MPFROM2SHORT
                                         (MID_SORTBYNAME, TRUE), MPFROM2SHORT
                                                         (MIA_CHECKED, NULL));
      WinSendMsg (hOrgmenu, MM_SETITEMATTR, MPFROM2SHORT
                                         (MID_SORTBYDATE, TRUE), MPFROM2SHORT
                                                         (MIA_CHECKED, NULL));
      WinSendMsg (hOrgmenu, MM_SETITEMATTR, MPFROM2SHORT
                                         (MID_SORTBYSIZE, TRUE), MPFROM2SHORT
                                                         (MIA_CHECKED, NULL));
      WinSendMsg (hOrgmenu, MM_SETITEMATTR, MPFROM2SHORT
                                         (MID_SORTBYEXT, TRUE), MPFROM2SHORT
                                                         (MIA_CHECKED, NULL));
      WinSendMsg (hOrgmenu, MM_SETITEMATTR, MPFROM2SHORT
                                         (MID_REVSORTBYNAME, TRUE), MPFROM2SHORT
                                                         (MIA_CHECKED, NULL));
      WinSendMsg (hOrgmenu, MM_SETITEMATTR, MPFROM2SHORT
                                         (MID_REVSORTBYDATE, TRUE), MPFROM2SHORT
                                                         (MIA_CHECKED, NULL));
      WinSendMsg (hOrgmenu, MM_SETITEMATTR, MPFROM2SHORT
                                         (MID_REVSORTBYSIZE, TRUE), MPFROM2SHORT
                                                         (MIA_CHECKED, NULL));
      WinSendMsg (hOrgmenu, MM_SETITEMATTR, MPFROM2SHORT
                                         (MID_REVSORTBYEXT, TRUE), MPFROM2SHORT
                                                         (MIA_CHECKED, NULL));

      // The originating menu is of importance here because of the way the Popup menu
      // shares the main menu control IDs.  When a 'sort' menu item is chosen from either menu,
      // it is channelled through here via the MID_SORTBY.... commands.  The 'hOrgmenu'
      // parameter in either case is always set to "hwndmenu" because of the routing.
      // This is fine since the sorting is done and menu attributes for the main menu are
      // changed here.  However, when the pop-up menu is activated, it has not received any
      // attribute change notifications (checked or unchecked) since, although they are copies,
      // they maintain separate lives.  This is remedied by sending a new "Sortfiles" command
      // as soon as the WinPopupMenu order has been given, and this time passing the 'hOrgmenu'
      // parameter as being "hwndpopup".  Because the case routines check for the originating
      // menu handle, the sorting process is ignored for this iteration and only the menu attributes
      // are updated.
      // The purpose of all of this is to avoid having to include double the code to send the
      // MM_SETITEMATTR message to both menu window handles each time.  It also makes
      // it easier to keep the popmenu updated since it can avoid having to do a set up on boot-up.


      switch (sorttype)
      {
         case SORTBYNAME:

            if (hOrgmenu == hwndmenu) DFiles->NameSort (NULL);

            WinSendMsg (hOrgmenu, MM_SETITEMATTR, MPFROM2SHORT
                                               (MID_SORTBYNAME, TRUE), MPFROM2SHORT
                                                               (MIA_CHECKED, MIA_CHECKED));
            break;

         case SORTBYDATE:

            if (hOrgmenu == hwndmenu) DFiles->DateSort (NULL);

            WinSendMsg (hOrgmenu, MM_SETITEMATTR, MPFROM2SHORT
                                               (MID_SORTBYDATE, TRUE), MPFROM2SHORT
                                                               (MIA_CHECKED, MIA_CHECKED));
            break;

         case SORTBYSIZE:

            if (hOrgmenu == hwndmenu) DFiles->SizeSort (NULL);

            WinSendMsg (hOrgmenu, MM_SETITEMATTR, MPFROM2SHORT
                                               (MID_SORTBYSIZE, TRUE), MPFROM2SHORT
                                                               (MIA_CHECKED, MIA_CHECKED));
            break;

         case SORTBYEXT:

            if (hOrgmenu == hwndmenu) DFiles->ExtensionSort (NULL);

            WinSendMsg (hOrgmenu, MM_SETITEMATTR, MPFROM2SHORT
                                               (MID_SORTBYEXT, TRUE), MPFROM2SHORT
                                                               (MIA_CHECKED, MIA_CHECKED));
            break;

         case REVSORTBYNAME:

            if (hOrgmenu == hwndmenu) DFiles->NameSort (TRUE);

            WinSendMsg (hOrgmenu, MM_SETITEMATTR, MPFROM2SHORT
                                               (MID_REVSORTBYNAME, TRUE), MPFROM2SHORT
                                                               (MIA_CHECKED, MIA_CHECKED));
            break;

         case REVSORTBYDATE:

            if (hOrgmenu == hwndmenu) DFiles->DateSort (TRUE);

            WinSendMsg (hOrgmenu, MM_SETITEMATTR, MPFROM2SHORT
                                               (MID_REVSORTBYDATE, TRUE), MPFROM2SHORT
                                                               (MIA_CHECKED, MIA_CHECKED));
            break;

         case REVSORTBYSIZE:

            if (hOrgmenu == hwndmenu) DFiles->SizeSort (TRUE);

            WinSendMsg (hOrgmenu, MM_SETITEMATTR, MPFROM2SHORT
                                               (MID_REVSORTBYSIZE, TRUE), MPFROM2SHORT
                                                               (MIA_CHECKED, MIA_CHECKED));
            break;

         case REVSORTBYEXT:

            if (hOrgmenu == hwndmenu) DFiles->ExtensionSort (TRUE);

            WinSendMsg (hOrgmenu, MM_SETITEMATTR, MPFROM2SHORT
                                               (MID_REVSORTBYEXT, TRUE), MPFROM2SHORT
                                                               (MIA_CHECKED, MIA_CHECKED));
            break;
      }
}
//-------------------------------------------------------------------
// This is a routine to add up the bytes used by all the files in a tree structure.
// A directory name must be handed to this function as a starting point in the
// directory hierarchy.
//-------------------------------------------------------------------
void CountBytes (PSZ pszdirname)
{
      // Put up the hourglass pointer.

      WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP,
                                                                         SPTR_WAIT, FALSE));

      // This unsigned long holds the count of bytes.  It can hold up to 4,294,967,295 bytes.

      ULONG total;
      total = 0;

      // These buffers are used with the DosFindFirst etc. functions.  The large array
      // of buffers holds information on many files at a time.

      PFILEFINDBUF3 pNextbuf;
      PBYTE pbBuf;
      PFILEFINDBUF3 pFilebuf;
      pFilebuf = new FILEFINDBUF3 [100];
      pbBuf = (PBYTE) pFilebuf;
      pNextbuf = pFilebuf;

      // Create an array of Handles to hold the directory handle for each
      // successive level of depth.  If it goes greater than 100 levels deep (Egads!)
      // the routine is aborted (below).

      HDIR hDir [100];
      INT hdirindex;
      PSZ pszsourcepath;
      pszsourcepath = new CHAR [CCHMAXPATH + 1];

      // Create a fully qualified source path.

      pszsourcepath [0] = CHAR (CurrDrive + 64);
      pszsourcepath [1] = 58;
      pszsourcepath [2] = 0;
      strcat (pszsourcepath, pszdirname);

      // The maximum number of files that are searched for at one time must match the
      // number of FILEFINDBUF3 buffers.

      ULONG maxfiles;
      maxfiles = 100;
      rc = 0;

      // Find where the source directory name begins (foreshorten the path)
      // and use this name as the new directory name (pDirname).

      int pathlen, stub, findbackslash;
      pathlen = strlen (pszdirname);
      stub = 0;
      for (findbackslash = 0; findbackslash < pathlen; findbackslash ++)
      {
         if (pszdirname [findbackslash] == 92) stub = findbackslash;
      }

      PSZ pDirname;
      if (stub) pDirname = &pszdirname [stub + 1];
      else pDirname = &pszdirname [1];
      if ((pathlen == 2) && (pszdirname [1] == 58)) pDirname = &pszdirname [pathlen];

      // The dotally operation will change (add to) our filespec so save the original length
      // so that it can be restored.

      pathlen = strlen (pszsourcepath);

      total += DoTally (pszsourcepath, pFilebuf);

      // Keep an index into the array of HDIR handles.  This will be needed later when
      // backtracking to a previous directory level and using DosFindNext.

      hdirindex = 0;

      // Make the decendent directories by pursuing the next single nested level until
      // there are no more nested levels.  The hdirindex will keep track of what level
      // is current.
      // Note:  By passing a the first member of the HDIR array as a pointer, it will
      //        allow the function to use it as if it were since it is an address reference
      //        to the whole array.
      // This is still part of the bootstrap process.  The last parameter passed indicates
      // that DosFindFirst will be used as the first call in the MakeDirs function.

      strcat (pszsourcepath, "\\*.*");

       total += drillfortally (pszsourcepath, hDir, &hdirindex, pFilebuf, pathlen, 0);

      // This is the main loop here.  It will cycle through this loop while there is still
      // and index level.  When the index level reaches 0, we have returned to the bootstrap
      // level which has already been created/copied and the process is complete.

      while (hdirindex)
      {
         hdirindex--;
         stub = 0;

         // Each pass through this loop must continually trim back the paths of the source and
         // destination directories since they are added to in the MakeDirs function below.
         // The path will match the index into the HDIR handles array so that further subdirs
         // may be located and copied with the DosFindNext api.

         pathlen = strlen (pszsourcepath);
         for (findbackslash = 0; findbackslash < pathlen; findbackslash ++)
         {
            if (pszsourcepath [findbackslash] == 92)
            {
               stub = findbackslash;
            }
         }
         pszsourcepath [stub] = 0;

         total += drillfortally (pszsourcepath, hDir, &hdirindex, pFilebuf, pathlen, 1);
      }
      // The job is done so clean up.

      delete pFilebuf;
      delete pszsourcepath;

      static CHAR totalstring [17] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
      CHAR *cString = totalstring;

      if (!total)
      {
         totalstring [0] = 48;
         totalstring [1] = 0;
      }
      else
      InsertCommas (total, cString);

      CHAR *psztreesize;
      psztreesize = new CHAR [CCHMAXPATH * 2 +2];

      // Copy the filename into a buffer but trim off the trailing backslash
      // and enclose it in quotes.

      strcpy (psztreesize, "\"");
      psztreesize [0] = 34;
      psztreesize [1] = CHAR (CurrDrive + 64);
      psztreesize [2] = 58;
      psztreesize [3] = 0;
      strcat (psztreesize, pszdirname);
      psztreesize [strlen (pszdirname) + 3] = 0;
      strcat (psztreesize, "\"");
      strcat (psztreesize, "\nThis Directory structure contains\n ");
      strcat (psztreesize, cString);
      strcat (psztreesize, " bytes.");
      USHORT usResponse;
      usResponse = (USHORT) WinMessageBox (HWND_DESKTOP, hwndframe,
                      psztreesize, "Tree Size", MESSAGEBOXID,
                      MB_OK);

      delete psztreesize;

      // Restore the arrow pointer.

      WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP,
                                                                        SPTR_ARROW, FALSE));
}
//-------------------------------------------------------------
// A helper function to count the bytes when doing a tree byte total.
//-------------------------------------------------------------
ULONG DoTally (PSZ pszsource, PFILEFINDBUF3 pFilebuf)
{
      HDIR hDiralt;
      ULONG fileattrs, mostfiles;
      PSZ pszsourcebuf;
      PFILEFINDBUF3 pOtherbuf;
      PBYTE pbOther;
      ULONG tally;
      APIRET rcode;

      tally = 0;

      // Create some new buffers here so there is a bit of redundancy.  There could be many
      // files here and a local buffer is probably safer.  This also allows some greater freedom
      // in the way paths are manipulated.
      // They will be deleted at the end of this routine.

      pszsourcebuf = new CHAR [CCHMAXPATH +1];
      strcpy (pszsourcebuf, pszsource);
      strcat (pszsourcebuf, "\\*.*");

      hDiralt = HDIR_SYSTEM;
      fileattrs =  FILE_ARCHIVED | FILE_SYSTEM | FILE_HIDDEN | FILE_READONLY;

      // Set up the FINDFILEBUF3 pointers.

      pOtherbuf = pFilebuf;
      pbOther = (PBYTE) pFilebuf;

      // Get files from the directory in bunches of 100.

      mostfiles = 100;
      rcode = DosFindFirst (pszsourcebuf, &hDiralt, fileattrs, pFilebuf, FF_BUFFSIZE,
                          &mostfiles, FIL_STANDARD);

      // Provide an escape if this was an empty directory.

      if (rcode) mostfiles = 0;

      // Process the first batch of files.  If there were more than the original order (100),
      // a new batch will be picked up by DosFindNext below.

      while (mostfiles)
      {
         for (ii = 0; ii < mostfiles; ii ++)
         {
            tally += pOtherbuf->cbFile;
            pbOther += pOtherbuf->oNextEntryOffset;
            pOtherbuf = (PFILEFINDBUF3) pbOther;
         }
         if (mostfiles == 100)
         {
            rcode = DosFindNext (hDiralt, pFilebuf, FF_BUFFSIZE, &mostfiles);

            // Reset the FINDFILEBUF3 pointers.

            pOtherbuf = pFilebuf;
            pbOther = (PBYTE) pFilebuf;

            if (rcode) mostfiles = 0;
         }
         else mostfiles = 0;
      }
      // The job is done.

      delete pszsourcebuf;
      return tally;
}
//-------------------------------------------------------------
// Another helper function to count the bytes when doing a tree byte total.
//-------------------------------------------------------------
ULONG drillfortally (PSZ pszsourcepath, PHDIR phRef, INT *phDindex,
                                      PFILEFINDBUF3 pFfb, INT len, INT findnext)
{
      APIRET rc;
      ULONG maxfiles;
      PHDIR phD;
      INT pathlen;
      INT findnextflag;
      ULONG tally;

      tally = 0;
      findnextflag = 0;

      // Set the HDIR array pointer to the first member.  The individual array members will
      // be accessed by adding the value of the index to the pointer.
      // If it is a DosFindFirst operation, the value will be set to allow DosFindFirst to assign
      // a unique id.

      phD = phRef;
      if (findnext)
      {
         phD = phD + *phDindex;
         findnextflag = 1;
      }
      else
      *phD = HDIR_CREATE;

      // Subdirs will only be retrieved one at a time.

      maxfiles = 1;
      pathlen = len;
      rc = 0;

      // Find the subdirs and create them in single peer fashion until a dead end is reached.
      // Each time a new subdir is created, the files in the source subdir will be copied to the
      // new dir.

      while (maxfiles)
      {
         if (findnextflag)
         {
            findnextflag = 0;
            rc = DosFindNext (*phD, pFfb, FF_BUFFSIZE, &maxfiles);
            if (rc == ERROR_NO_MORE_FILES)
            {
               pszsourcepath [pathlen] = 0;
               return tally;
            }
            else if (rc) return 0;
         }
         else
         rc = DosFindFirst (pszsourcepath, phD, MUST_HAVE_DIRECTORY, pFfb,
                                                           FF_BUFFSIZE, &maxfiles, FIL_STANDARD);
         if (rc) return 0;

         // Sluff of those pesky "." and ".." entries.

         if ((pFfb->cchName == 1) && (pFfb->achName [0] == '.'))
         {
            rc = DosFindNext (*phD, pFfb, FF_BUFFSIZE, &maxfiles);
            if (rc)
            {
               if (rc == ERROR_NO_MORE_FILES)
               {
                  pszsourcepath [pathlen] = 0;
                  return tally;
               }
               else if (rc) return 0;
            }
         }
         if ((pFfb->cchName == 2) && ((pFfb->achName [0] == '.') && (pFfb->achName [1] == '.')))
         {
            rc = DosFindNext (*phD, pFfb, FF_BUFFSIZE, &maxfiles);
            if (rc)
            {
               if (rc == ERROR_NO_MORE_FILES)
               {
                  pszsourcepath [pathlen] = 0;
                  return tally;
               }
               else if (rc) return 0;
            }
         }
         // If a subdir was finally found, extend the source path.

         if (maxfiles)
         {
            pszsourcepath [pathlen] = 0;
            strcat (pszsourcepath, "\\");
            strcat (pszsourcepath, pFfb->achName);
            pathlen = strlen (pszsourcepath);
            strcat (pszsourcepath, "\\*.*");

            // Increment the index for the caller.

            (*phDindex)++;

            // Increment the pointer for the next HDIR handle and give it a value required for
            // a system assigned handle in DosFindFirst.

            phD++;
            *phD = HDIR_CREATE;

               pszsourcepath [pathlen] = 0;
               tally += DoTally (pszsourcepath, pFfb);
               strcat (pszsourcepath, "\\*.*");
         }
      }
      return tally;
}
//-------------------------------------------------------------------------
// A helper function to test whether a drive is ready or not.  This is needed
// because of removeable media drives which may have had the media removed
// at any time, even while the directory is displayed.
//-------------------------------------------------------------------------
BOOL IsDriveReady (ULONG drive)
{
      // Change to the clock pointer while this test is run.

      WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP,
                                                               SPTR_WAIT, FALSE));
      // Test to see if the destination drive is ready (removable media drives).
      // This has to be done here rather than in the OBJM_DROP area since the
      // DosQueryFSInfo api causes an access violation there.  This must have to
      // do with the fact that it takes place in a separate thread.

      PFSALLOCATE pFS;
      pFS = new FSALLOCATE;
      rc = DosQueryFSInfo (drive, 1, pFS, sizeof (FSALLOCATE));
      delete pFS;

      // Reset the pointer to an arrow.

      WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP,
                                                               SPTR_ARROW, FALSE));
      if (rc)
         return FALSE;
      else
         return TRUE;
}
//---------------------------------------------------------------
// A function used about 8 times to write the menu names for the user defined
// programs.
//---------------------------------------------------------------
void WriteMenuItem (PSZ pszString1, PSZ pszString2, const ULONG id, ULONG sLen)
{
      // Note:  The actual name can be 13 Characters plus a terminating zero.

      PSZ pMenuname;
      pMenuname = new CHAR [25];
      strcpy (pMenuname, pszString1);
      i = strlen (pMenuname);
      for (ii = i; ii < 16; ii++)
      {
         pMenuname [ii] = 32;
      }
      pMenuname [16] = 0;
      strcat (pMenuname, pszString2);

      // Rewrite the menu entry to reflect the new user program name.

      WinSendMsg (hwndmenu, MM_SETITEMTEXT, MPFROMSHORT
                                                             (id), MPFROMP (pMenuname));
      WinSendMsg (hwndpopup, MM_SETITEMTEXT, MPFROMSHORT
                                                             (id), MPFROMP (pMenuname));

      // The sLen parameter indicates whether or not there is a program issued for
      // this user defined link.  If not, the menu item is to be disabled.

      if (!sLen)
      {
         WinSendMsg (hwndmenu, MM_SETITEMATTR, MPFROM2SHORT
                                      (id, TRUE), MPFROM2SHORT
                                                      (MIA_DISABLED, MIA_DISABLED));
         WinSendMsg (hwndpopup, MM_SETITEMATTR, MPFROM2SHORT
                                      (id, TRUE), MPFROM2SHORT
                                                      (MIA_DISABLED, MIA_DISABLED));
      }
      else
      {
         WinSendMsg (hwndmenu, MM_SETITEMATTR, MPFROM2SHORT
                                      (id, TRUE), MPFROM2SHORT
                                                      (MIA_DISABLED, NULL));
         WinSendMsg (hwndpopup, MM_SETITEMATTR, MPFROM2SHORT
                                      (id, TRUE), MPFROM2SHORT
                                                      (MIA_DISABLED, NULL));
      }
      delete pMenuname;
}
//-------------------------------------------------------------------------
// A function to refresh the strings that store the volume labels for all known drives.
// The 'firstdrive' variable allows the refresh operation to start after the floppys if desired.
// The 'single' variable, indicates whether a single drive only is to be refreshed.
//-------------------------------------------------------------------------
void RefreshVolLabels (ULONG firstdrive, BOOL single)
{
      // Read the volume labels of all the located drives.
      // NOTE: for reference, when 120 MB floppies become standard, I am checking
      // for them here by checking the Device Type and number of cylinders etc.
      // returned in ucDataArea [31], [32], and [33] (below).
      // A high density (5 1/4") floppy is returned as device type (ucDataArea [33]) 1 and
      // should be ignored since these devices are obsolete.   A 1.44 floppy is returned as
      // device type 7, but so is a CD-Rom drive.  The new high capacity drives will also
      // probably be returned as type 7.  Since all are listed as removeable media
      // (ucDataArea [34] bit 0 = 0), they must be distinguished by the number of tracks
      // (ucDataArea [31] low byte [32] high byte).  The 1.44 is always 80 tracks in
      // the low byte so anything other than 80 tracks is probably a CD-Rom or High
      // Capacity drive.

      ULONG   DriveNumber;   /* Drive number */
      ULONG   limit;  // Number of drives to check
      ULONG   FSInfoBufSize; /* File system info buffer size */
      APIRET  rcLabel;

      FSInfoBufSize = 17;

      // DosQueryFSInfo uses drive numbers starting at 1.
      // 0 is used to denote the current drive.
      // The variable 'firstdrive is used so that some routines calling this function
      // can specify that the floppy drives can be skipped.  This is mainly for the
      // Volume label refresh after a timed refresh of the drive table.
      // The variable "limit" is used to limit the number of drives being checked.
      // This alows for a single drive to be refreshed which will be the case if
      // a floppy drive is opened.  If the second parameter passed to this routine
      // is TRUE as opposed to FALSE, limit is set so that only a single drive
      // will be read.

      if (single) limit = firstdrive + 1;
      else limit = 27;
      for (DriveNumber = firstdrive; DriveNumber < limit; DriveNumber++)
      {
         ulParmlen = 2;
         ulDatalen = 0;
         ucParms [0] = 0;
         ucParms [1] = UCHAR (DriveNumber -1);

         DosError (FERR_DISABLEHARDERR);

         // Clip the current entry just in case.

         ucVoldat [DriveNumber - 1][0] = 0;

         // Find out the volume label if there is one and store it in the ucVoldat array.

         rcLabel = DosQueryFSInfo (DriveNumber, FSIL_VOLSER, ucVoldat [DriveNumber - 1],
                                                                                 FSInfoBufSize);
         DosError (FERR_ENABLEHARDERR);

         if (rcLabel)
         {
            rc = DosDevIOCtl (-1, 0x8, 0x63,  ucParms, 2, &ulParmlen, ucDataArea, 40, &ulDatalen);
            if (!rc)
            {
               if (!(ucDataArea [34] & 1))
               {

                  // If there was an error reading the label, and it is a drive with removeable
                  // media, put a question mark there.

                  ucVoldat [DriveNumber - 1][5] = 63;
                  ucVoldat [DriveNumber - 1][6] = 0;
               }
            }
         }
      }
}
//-------------------------------------------------------------------------
// A function to set or stop the timers for the Directory and Drive refresh options
// The handle indicates which checkbox is being modified and the ULONG indicates
// which timer this uses.
// Since named timers are associated with windows, I think it is ok to use named
// timers without worrying about conflicts with other program's named timers.
//-------------------------------------------------------------------------
void SetTimers (HWND hwndbox, HWND hwndspinbtn, ULONG timer, PULONG pinterval)
{
      // Find out if the checkbox in question is checked or blank.

      ULONG interval;
      USHORT replacestate;
      replacestate = (USHORT) WinSendMsg (hwndbox, BM_QUERYCHECK, NULL, NULL);

      // If it's checked, the timer can be reset by starting it again with a new value.

      if (replacestate)
      {
         // Get the value of the spin button.
         // Note:
         // This spinbutton will support values of up to 600.  Translated into seconds,
         // that is 10 minutes max.  Warp 2.1, however, only supports timer intervals
         // of up to 65 seconds, so therefore this feature will not work with any version
         // below Warp 3.0.

         USHORT value;
         WinSendMsg (hwndspinbtn, SPBM_QUERYVALUE, MPFROMP (&value),
                                             MPFROM2SHORT (NULL, NULL));
         // The spin buttons range between 1 and 600 therefore always have a value.

         *pinterval = value;

         // Be sure to multiply the seconds by 1000 since the timers need to be programmed
         // in milliseconds.

         interval = value * 1000;
         WinStartTimer (hab1, hwndclient, timer, interval);
      }
      else
      {
         // If the checkbox for the timer was unchecked, stop the timer
         // and set the interval to zero.  This is the value that will
         // be written to the ini file.

         WinStopTimer (hab1, hwndclient, timer);
         *pinterval = 0;
      }
}
//-------------------------------------------------------------------------
// A function to count and set up the Drive Bar for the Drive bar initialization and the
// Drive bar refresh options.
//-------------------------------------------------------------------------
void GetAvailableDrives (BOOL renew)
{
//Fill up the Logical Drive Map so we know how many drives we have and which ones

      // The 'drive' variable is not used here but is required.

      ULONG drive;
      DosQueryCurrentDisk (&drive, &DrvMap);
      int x;
      totaldrives = 0;
      for (int i = 0; i < 26; i++)
      {
         x = DrvMap >> i;
         if (x & 1) totaldrives++;
         drivearray [i] = x & 1;
      }
      // Add another drive button which will have an "*" label to signify a list of
      // most recent used drives/directories.

      totaldrives++;

// Write some labels for the Drive Pushbuttons.
// Care is taken here since the drives may not always be sequential. The labels
// will state the proper drive designation even if there are gaps in the order.

      // If this is a refresh of the drive count, start over with the array.
      if (renew) delete pLabelarray;

      USHORT cchLabel = 4;
      pLabelarray = new CHAR [totaldrives * cchLabel];
      int ii = 0;
      for (i = 0; i < 26; i++)
      {
         if (drivearray [i])
         {
            pLabelarray [ii * cchLabel +1] = 58;
            pLabelarray [ii * cchLabel +2] = 0;
            pLabelarray [ii * cchLabel] = (CHAR) (i + 65);
            ii++;
         }
      }
      // Add a "*" label for the "last used directories" button (it will be the last one).

      pLabelarray [ii * cchLabel] = (CHAR) 42;
      pLabelarray [ii * cchLabel + 1] = 0;

      WinQueryWindowPos (hwndclient, (PSWP)&swp);

      // For a horizontal bar do this

      if ((toolbarorient & 1) == 0)
      {
         if (swp.cx < (totaldrives * (DBwidth + 2)))
         {
            driverows = 2;
            drivecolumns = SHORT (totaldrives / 2);
            if ((drivecolumns * 2) < totaldrives) drivecolumns++;
         }
         else
         {
            driverows = 1;
            drivecolumns = totaldrives;
         }
      }
      else
      // For a vertical bar do this.  Also switch the drive buttonbar rows and columns
      // here since it must move with the toolbar.
      // Note that the drive buttonbar will only be a single column if vertical or a
      // single row if horizontal.

      {
         if (swp.cy < (totaldrives * (DBheight + 2)))
         {
            driverows = SHORT (totaldrives / 2);
            if ((driverows * 2) < totaldrives) driverows++;
            drivecolumns = 2;
         }
         else
         {
            driverows = totaldrives;
            drivecolumns = 1;
         }
      }

//  Create a drive bar on the main window

      if (renew) delete pdbarinfo;

      pdbarinfo = new BTNINFO;
      pdbarinfo->btncx = DBwidth;
      pdbarinfo->btncy = DBheight;
      pdbarinfo->flBstyle = BS_PUSHBUTTON | BS_NOPOINTERFOCUS;
      pdbarinfo->brows = driverows;
      pdbarinfo->bcolumns = drivecolumns;
      pdbarinfo->totalbuttons = totaldrives;
      pdbarinfo->reservecx = ptbarinfo->btncx;
      pdbarinfo->reservecy = ptbarinfo->btncy;
      pdbarinfo->reservecolumns = buttoncolumns;
      pdbarinfo->reserverows = buttonrows;
      pdbarinfo->label = pLabelarray;
      pdbarinfo->labelsize = cchLabel;

      if (renew) delete FFdrivebar;

      FFdrivebar = new toolbar(hwndclient, DRIVEBARID, pdbarinfo);
      pdrivedim = FFdrivebar->rejig( driverows, drivecolumns);
}
//-------------------------------------------------------------------------
// A function to handle the rebuilding of the Directory Tree container window.
// This will be used only when the tree is being completely rebuilt and is not used if
// child records are just being added as is done with the TreetoOrder method.
// Its main purpose is to reopen the folders that were open before the rebuild and to
// reposition the display so that the last folder opened is visible.
//-------------------------------------------------------------------------
void RefillTree (ULONG drive, ULONG ulPath, HWND hwnd, roottree *ptree, PSZ pPath)
{
      PCNRITEM pOpen, pTest, pLast;
      SHORT sTest;
      int pathlen, child, findbackslash;
      PSZ psznamebuffer, pszpathbuffer;

      pszpathbuffer = new CHAR [CCHMAXPATH *2];
      psznamebuffer = new CHAR [CCHMAXPATH *2];

      // Get the record pointer of the folder shown to be open farthest down in the hierarchy.

      pOpen = 0;
      pTest = 0;
      pLast = 0;
      child = 0;

      // A test is done here to determine the origin of the call.

      PSZ  pszbmpath;

      if (ulPath)
      {
         // If it's a Bookmark or a Source window calling, the new path has already been
         // written to the FilesTitle.
         // If it's the drag/drop dialog calling, a pointer to the path has been sent.
         // Otherwise the path pointer is null.

         // Strip the drive designator from the path pointer.

         if (pPath)
            pszbmpath = pPath;
         else
            pszbmpath = &szFilesTitle [2];
      }
      // If it isn't a Bookmark, Source or drag/drop for which a path is available, then treat all
      // paths as Destination paths being swapped.

      else
      {
         // Use a new string here because other dependant routines may be using
         // the szpathbuffer and sznamebuffer strings.

         // Strip the drive designator from the path pointer.

         pszbmpath = &szDestPath [2];
      }
      strcpy (psznamebuffer, pszbmpath);
      strcpy (pszpathbuffer, pszbmpath);

      // Make the window invisible so that the scrolling and repositioning below won't be
      // a distraction.

      WinSetWindowPos (hwnd, HWND_TOP, 0, 0, 0, 0, SWP_HIDE);

      // Fill the container with the root subdirs.

      ptree->RenewCont ((USHORT) drive);
      ptree->FillContainer ();

      // Find where the directory names begin and foreshorten the path
      // as neccessary for each subdir in the path.

      pathlen = strlen (pszpathbuffer);
      for (findbackslash = 1; findbackslash < pathlen; findbackslash ++)
      {
         if ((pszpathbuffer [findbackslash] == 92) || (findbackslash == (pathlen-1)))
         {
            if (findbackslash != (pathlen -1))
               pszpathbuffer [findbackslash] = 0;

            // The first folder filled will not be a decendant of any other.
            // All following folders will be decendants however and so must
            // be searched for using the CMA_FIRSTCHILD parameter.
            // For example, in the pathname "\temp\trash\trash1", "\temp" will not be
            // a child whereas "\temp\trash" and "temp\trash\trash1" will be.

            if (child)
            {
               pTest = (PCNRITEM) WinSendMsg (hwnd, CM_QUERYRECORD,
                        MPFROMP (pOpen), MPFROM2SHORT (CMA_FIRSTCHILD, CMA_ITEMORDER ));
            }
            else
            {
               pTest = (PCNRITEM) WinSendMsg (hwnd, CM_QUERYRECORD,
                               MPFROMP (pTest), MPFROM2SHORT (CMA_FIRST, CMA_ITEMORDER ));
               child++;
            }
            // Find the record that matchs the name.

            while (pTest)
            {
               // Use the case insensitive 'stricmp' here
               // Also lock in the last record name matched in case the last record open
               // was just deleted in a delete operation that is now calling this routine.
               // This will allow a recovery of the last ancestor's open folder.

               sTest = (SHORT) stricmp (pszpathbuffer, pTest->szDirPath);
               if (!sTest)
               {
                  pOpen = pTest;
                  pLast = pTest;
                  break;
               }
               pTest = (PCNRITEM) WinSendMsg (hwnd, CM_QUERYRECORD,
                         MPFROMP( pTest), MPFROM2SHORT (CMA_NEXT, CMA_ITEMORDER ));
            }
            // Trap here in case of a NULL pointer.

            if (pTest)
            {
               // Use TreetoOrder here since it only adds contents to an existing folder

               ptree->TreetoOrder (pTest);
               ptree->FillContainer();

               // Set this flag here.  It is only used to stop the CN_EXPANDTREE routine
               // from filling the Files container each time a folder is opened in the Tree
               // container.

               expandflag = 1;
               WinSendMsg (hwnd, CM_EXPANDTREE, MPFROMP (pTest), NULL);

               // Reload the saved pathname.

               strcpy (pszpathbuffer, psznamebuffer);
            }
         }
      }

      if (!pTest)
      {
         if (pLast) pTest = pLast;
      }
      // Trap again in case of a NULL pointer.

      if (pTest)
      {
         // Make sure the cursor is on the last folder to be filled.

         WinSendMsg (hwnd, CM_SETRECORDEMPHASIS,
                            MPFROMLONG (pTest), MPFROM2SHORT (TRUE, CRA_CURSORED));

         // Now, scroll down about 10 lines to bring the open folder to about the centre of the window.
         // Use WinSendMsg here.  If they are posted, the movement comes after the window is
         // visible and is too distracting.

         // Get the size of the container window rectangle.  For a guesstimate line height,
         // use 16 pels, then double it to 32 so that the window will scroll halfway.

         RECTL contrectl;
         WinSendMsg (hwnd, CM_QUERYVIEWPORTRECT, MPFROMP (&contrectl),
                                               MPFROM2SHORT (CMA_WINDOW, NULL));
         ii = (contrectl.yTop - contrectl.yBottom) / 32;

         for (i = 0; i <ii; i++)
         {
            WinSendMsg (hwnd, WM_CHAR, MPFROMSH2CH
                                          (KC_VIRTUALKEY, 0, 0), MPFROM2SHORT (0, VK_DOWN));
         }
         // This bit here deals with a bug in this API.  It should be possible to have the
         // container automatically scroll to the selected folder, but this only seems to work
         // in CV_DETAILS view.
         // These next two messages are posted, and as such, will not be acted on until after
         // the WinSetWindowPos message below.  This is necessary because otherwise the
         // display will not scroll to the proper position.  The CM_SETRECORDEMPHASIS
         // message is then necessary to return the cursor to the proper folder.

         WinPostMsg (hwnd, WM_CHAR, MPFROMSH2CH
                                          (KC_VIRTUALKEY, 0, 0), MPFROM2SHORT (0, VK_DOWN));
         WinPostMsg (hwnd, CM_SETRECORDEMPHASIS,
                            MPFROMLONG (pTest), MPFROM2SHORT (TRUE, CRA_CURSORED));

      }
      // Make the window visible again.

      WinSetWindowPos (hwnd, HWND_TOP, 0, 0, 0, 0, SWP_SHOW);

      delete pszpathbuffer;
      delete psznamebuffer;
}
//***********************The End ********************************************/
