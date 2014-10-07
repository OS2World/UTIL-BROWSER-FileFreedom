
#ifndef _cnrstuff_
#define _cnrstuff_

// Some constants are defined here

const USHORT FILES_TO_GET = 100;
const ULONG FF_BUFFSIZE = sizeof (FILEFINDBUF3) * FILES_TO_GET;
const ULONG CCOLUMNS = 3;

//---------------------------------------------------------------------------
// The size and members of this structure are completely up to the programmer, in fact it is not
// required if the RECORDCORE or MINIRECORDCORE structures are sufficient to store the
// required detail.  It must contain the RECORDCORE or MINIRECORDCORE structure as it's
// first member.
//---------------------------------------------------------------------------

typedef struct _CNRITEM               // CONTAINER RECORD STRUCTURE
{
  RECORDCORE  rc;                  // Base information
  PSZ            pszFileName;         // Pointer to File name
  HPOINTER     hptrIcon;            // Icon handle
  CDATE          date;                // Date of file
  CTIME          time;                // Time of file
  ULONG          cbFile;              // File size
  CHAR           szFileName[CCHMAXPATH + 1]; // File name without a leading backslash
  CHAR           szDirPath[CCHMAXPATH +1]; // Full path (less drive indicator) including filename.
                                                   // eg. "\path\filename"
  ULONG          attrFile;            // Attributes of this file
  PSZ             pszAttrString;      // Pointer to the attribute string.
  CHAR           szAttrString[25];     // Attribute string in the form of "rhsa" or "----"
                                          // as well as date and time strings.

} CNRITEM, *PCNRITEM;

//---------------------------------------------------------------------
// This structure is used in the main program to keep track of directory counts of files
//and bytes for use in the status source status bar.
//---------------------------------------------------------------------
typedef struct _FILESANDBYTES   // Structure to hold cummulative counts
{
   USHORT cFiles;             // Count of files
   ULONG cBytes;              // Count of bytes
} FILESANDBYTES, *PFILESANDBYTES;

//---------------------------------------------------------------------
// This structure is used in the DragDropDlgProc to store information about the
// operation to be preformed on the dropped objects and the pathname string of
// the destination.
//---------------------------------------------------------------------

typedef struct _DROPDEST
{
   USHORT cbDD;                           // Size of structure (required for WinDlgBox)
   CHAR szDropPath [CCHMAXPATH + 1];  // Path name of the chosen destination
   CHAR szDropName [CCHMAXPATH + 1]; // Filename mask for Copy operation
   USHORT ccPathname;                    // A count of the bytes in the dest path string
   USHORT recundermouse;                 // A flag to indicate that the record under the mouse
                                              // only is to be used.
   PVOID pRec;                             // A pointer to the record under the mouse.
   HWND hwndCaller;                       // Usually used to identify the target in a DM_RENDER
   USHORT Operation;                      // An operation mask encoded like this:
                                             //  Bit 0 = if set overwrite files with the same name
                                             //  Bit 1 = if set, operation is COPY
                                             //  Bit 2 = if set, operation is MOVE
                                             //  Bit 3 = if set, operation is ZIP
                                             //  Bit 4 = if set, operation is UNZIP
                                             //  Bit 5 = if set means CANCEL all operations
                                             //  Bit 6 = if set notify caller (hwndCaller) to refresh
                                             //  Bit 7 = if set operation is APPEND
} DROPDEST, *PDROPDEST;

//---------------------------------------------------------------------
// This structure is used in the CopyTree function
//---------------------------------------------------------------------

typedef struct _COPYTREEROOTS
{
   // The original paths are different from the current paths in that the current paths
   // are being changed dynamically as the operation progresses.  They are extended
   // and truncated as required.

   PSZ pszCurDestPath;  // is the current destination directory path in the form of
                           // "H:\test\dirname"
   PSZ pszCurDirPath;   // is the current directory source path in the form of
                          //   "C:\sourcepath\dirname"
   INT pathlen;          // stores current length of sourcepath
   PSZ pszOrgName;     // is the name only of the original source directory eg. "dirname"
   PSZ pszOrgTarget;    // is the name of the original target path eg. "H:\test\dirname"
   PHDIR phdirbase;        // a pointer to the base of an array of HDIRs
   INT * phdirindex;      // a pointer to the index into the array
   INT findnextflag;      // indicates whether or not the next operation should be a DosFindFirst
                           // or a DosFindNext.
   INT unrelated;        // indicates whether or not the destination directory is a decendant of
                          // the source (needed to stop self-replication)
} COPYTREEROOTS, *PCOPYTREEROOTS;

#endif

