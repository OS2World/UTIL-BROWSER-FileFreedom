#define INCL_DOSFILEMGR
#define INCL_WINFRAMEMGR
#define INCL_WINSTDCNR
#define INCL_WINWINDOWMGR
#define INCL_DOSPROCESS
#define INCL_WINPOINTERS
#define INCL_WININPUT
#define INCL_WINSCROLLBARS

#include <os2.h>
#include <string.h>
#include "roots.h"

APIRET rcx;

   roottree::roottree(HWND container, SHORT drive)
   {
      // Set the drive passed as the new default.

      DosSetDefaultDisk (drive);
      hwndcontainer = container;

      // Set up a flag to enable a keyboard 'Home' message when a new drive tree is loaded.

      newdriveflag = 1;

      // Determine the size of the extra storage needed for the CNRITEM structure.

      extrabytes = sizeof (CNRITEM)-sizeof (RECORDCORE);

      pszFilespec = new char[CCHMAXPATH +1];
      pszDirspec = new char[CCHMAXPATH +1];
      pszFilespec[0] = 0;
      pszDirspec[0] = 0;

      // These buffers are used with the DosFindFirst etc. functions.  The large array
      // of buffers holds information on many files at a time.

      pffb = new FILEFINDBUF3[FILES_TO_GET];

      // This structure is used to hold pointers to the Icons used for expanded and collapsed
      // file folders and is referenced by the RECORDCORE structure.  It is used only in the
      // Tree-Name view.

      ptidesc = new TREEITEMDESC;
      hExIcon = WinLoadPointer (HWND_DESKTOP, 0, 298);

      // hicon shares the icon representing the collapsed folder. It is used as the default folder view.

      hicon = WinLoadPointer (HWND_DESKTOP, 0, 299);
      hCollIcon = hicon;

      pcnrParent = NULL;
      pcnrItem = NULL;
      pcnrFirst = NULL;

      // Initialize SIZEL structure used to set icon sizes.

      sl.cx = 18;
      sl.cy = 12;

      // The CNRINFO structure contains information about the container.  It should be
      // cleared first and then the required fields reset.  It is updated via a CM_SETCNRINFO
      // message which also indicates which fields to change with the CMA_* attributes.

      memset (&cnri, 0, sizeof (CNRINFO));

      cnri.cb = sizeof (CNRINFO);
      cnri.flWindowAttr = (CV_TREE | CV_NAME ) | CA_DRAWICON | CA_TREELINE;

      cnri.cyLineSpacing = 0;
      cnri.cxTreeIndent = 20;
      cnri.cxTreeLine = 1;
      cnri.slBitmapOrIcon = sl;
      cnri.slTreeBitmapOrIcon = sl;

      WinSendMsg (hwndcontainer, CM_SETCNRINFO, MPFROMP (&cnri),
                      MPFROMLONG ( CMA_FLWINDOWATTR | CMA_CXTREELINE
                      | CMA_SLTREEBITMAPORICON | CMA_TREEICON | CMA_LINESPACING
                      | CMA_SLBITMAPORICON | CMA_CXTREEINDENT ));

      // Begin to set up the RECORDINSERT structure here.  Other members are changed as
      // each record is inserted.  This is done in the InsertRecord method.

      (void) memset( &ri, 0, sizeof( RECORDINSERT ) );

      ri.cb                 = sizeof( RECORDINSERT );
      ri.pRecordOrder       = (PRECORDCORE) CMA_END;
      ri.zOrder             = (USHORT) CMA_TOP;
      ri.fInvalidateRecord  = TRUE;

   }

   // The memory that has been allocated must be freed up.  This is critical.

   roottree::~roottree()
   {
      WinSendMsg (hwndcontainer, CM_REMOVERECORD, NULL, MPFROM2SHORT( 0, CMA_FREE ) );
      delete ptidesc;
      delete pffb;
      delete pszFilespec;
      delete pszDirspec;
   }

   // This method reads the files in the directories and sends out to have the files inserted in
   // the container.  It also sends out to have the drive searched for subdirectorties.

   roottree::FillContainer ()
   {
     // HDIR_CREATE = System assigns a new handle.

     // A new handle will be stored here on return from the function DosFindFirst.

      hdir = HDIR_CREATE;

      // This first loop depends on the results from TraverseSubDirs().  If no more subdirectories
      // can be found, TraverseSubDirs returns FALSE and this loop ends.

      maxfiles = FILES_TO_GET;
      rcx = 0;

      // Create our filespec and mark the end of the pathname so that the wildcard pattern
      // can be trimmed.

      strcat (pszFilespec, pszDirspec);
      pchEndname = pszFilespec + strlen (pszFilespec);  // Pointer to end of pathname string.
      strcat (pszFilespec, "\\*.*");

      rcx = DosFindFirst (pszFilespec, &hdir, MUST_HAVE_DIRECTORY | FILE_HIDDEN,
                            pffb, FF_BUFFSIZE, &maxfiles, FIL_STANDARD);

      // Strip the  trailing "\\*.*" from the pszFilespec.
      // pszFilespec will now hold the complete path and is ready for the next pass.
      // This happens only if TraverseSubDirs() is called below.

      *pchEndname = 0;

      // This loop will send out to insert the subdirectories in the container and it also will
      // keep reading in subdirectories if there were more than could be read on the
      // DosFindFirst pass.  When thru, it releases the handle.

      while ( !rcx)
      {
         InsertRecords ();
         if ( maxfiles == FILES_TO_GET)
         {
            rcx = DosFindNext (hdir, pffb, FF_BUFFSIZE, &maxfiles);
         }
         else
         {
            rcx = 1;
            DosFindClose (hdir);
         }
      }

      // Blank the filespec string and send out for a sort job.

      *pszFilespec = 0;
      WinSendMsg (hwndcontainer, CM_SORTRECORD, MPFROMP (SortByName) , NULL);

      // Scroll to the head of the file.  With the newly sorted directories on fat partitions,
      // the cursor will rest on the oldest file in the directory otherwise.  This may be
      // nowhere near the head of the directory.
      // Only do this if it is a new root tree being loaded, otherwise everytime a folder
      // is opened, the cursor will move from the selected folder.

      if (newdriveflag)
      {
         WinSendMsg (hwndcontainer, WM_CHAR, MPFROMSH2CH
                                       (KC_VIRTUALKEY, 0, 0), MPFROM2SHORT (0, VK_HOME));

         // Use a WinPostMsg here since Dialog boxes block some of these messages
         // until after they are visible.

         WinPostMsg (hwndcontainer, CM_SCROLLWINDOW, MPFROMSHORT (CMA_VERTICAL),
                                                           MPFROMLONG (-66000));

      }
      newdriveflag = 0;
   }

   BOOL roottree::InsertRecords ()
   {
      BOOL success = TRUE;
      PBYTE pbBuf = (PBYTE) pffb;

      pcnrItem = (PCNRITEM) WinSendMsg (hwndcontainer, CM_ALLOCRECORD,
                                   MPFROMSHORT (extrabytes), MPFROMSHORT ((USHORT) maxfiles));

      if (!pcnrItem) return FALSE;
      if ( pcnrItem )
      {
        INT i;
        pcnrFirst = pcnrItem;
        ULONG  cFilesInserted = maxfiles;

        // Insert all files into the container in one shot by filling in each
        // linked list node that the container allocated for us.

        for( i = 0; i < maxfiles; i++ )
        {
           // Get next FILEFINDBUF3 structure that points to a found file.

           pffbFile = (PFILEFINDBUF3) pbBuf;

           // Fill in the container record with the file info.

           if (FillInRecord () )

                   // Get the next container record in the linked list that the
                   // container allocated for us.
                   pcnrItem = (PCNRITEM) pcnrItem->rc.preccNextRecord;
           else
                  cFilesInserted--;

           // Point to the next file in the buffer. This is done by adding
           // an offset value to the current location in the buffer. Since
           // the file name is variable length, this offset points to the
           // end of the current file name and the beginning of the next
           // one.

           pbBuf += pffbFile->oNextEntryOffset;
        }

           // The RECORDINSERT structure contains information that tells the
           // container how to insert the records. Here we ask to insert the
           // records at the end of the linked list. The parent record indicates
           // who to stick this batch of records under (if pciParent is NULL, the
           // records are at the top level). (Child records are only displayed in
           // Tree view). The zOrder is used for icon view only.
           // If fInvalidateRecord is TRUE, that will cause the records to be painted
           // as they are inserted. In a container with a small amount of records
           // you probably want to set this to FALSE and do a CM_INVALIDATERECORD
           // (using 0 for cNumRecord) after all records have been inserted. But
           // here the user needs visual feedback if a large amount of
           // subdirectories are found.
           // Most of the setup has already been done.  These items are dependant on
           // the records being inserted now.

           ri.pRecordParent      = (PRECORDCORE) pcnrParent;
           ri.cRecordsInsert     = cFilesInserted;

           success =  (BOOL) WinSendMsg (hwndcontainer, CM_INSERTRECORD,
                                                         MPFROMP (pcnrFirst), MPFROMP (&ri ));
      }
      return success;
   }

   BOOL roottree::FillInRecord ()
   {
      // The CNRITEM structure is a devised structure to hold extra information that
      // supplements the info in the RECORDCORE or MINIRECORDCORE structures.
      // It must also contain the RECORDCORE or MINIRECORDCORE structure itself.
      // The structure is up to the programmer to design with fields as required.
      // Fill in the values of CNRITEM and RECORDCORE. Note that the .cb
      // field of RECORDCORE was filled in by CM_ALLOCRECORD.

      // Sluff off any listing of a single "." directory.

      if ((pffbFile->cchName ==1) && (pffbFile->achName[0] == '.')) return FALSE;

      // Set up the Icon pointers so that a 'expanded' icon is shown for the root
      // directory and it also is given an 'INUSE' attribute.

      // Copy the file name and the Directory path to the CNRITEM.

      memset (pcnrItem->szFileName, 0, sizeof (pcnrItem->szFileName));
      memcpy (pcnrItem->szFileName, pffbFile->achName, pffbFile->cchName);
      strcpy (pcnrItem->szDirPath, pszDirspec);
      strcat (pcnrItem->szDirPath, "\\");
      strcat (pcnrItem->szDirPath, pcnrItem->szFileName);

      pcnrItem->rc.flRecordAttr = CRA_RECORDREADONLY | CRA_COLLAPSED;
      pcnrItem->rc.hptrIcon  = hicon;
      ptidesc->hptrExpanded = hExIcon;
      ptidesc->hptrCollapsed = hCollIcon;

      // First, if it's not the root directory (pcnrParent will be equal to 0),
      // sluff off the ".." subdirectory listings except for the root directory.
      // If it's the root directory, mark the root folder as 'in use' and remove
      // the '..' from the path.  Also fix the folder icon to be shown as expanded.

      if (!pcnrParent)
      {
         if (pffbFile->cchName == 2)
         {
            if ((pffbFile->achName[0] == '.') && (pffbFile->achName[1] == '.'))
            {
               pcnrItem->rc.hptrIcon  = hExIcon;
               ptidesc->hptrCollapsed = hExIcon;
               pcnrItem->rc.flRecordAttr = CRA_RECORDREADONLY | CRA_COLLAPSED | CRA_INUSE;
               pcnrItem->szDirPath [0] = 0;
            }
         }
      }
      else if (pffbFile->cchName == 2)
      {
         if ((pffbFile->achName[0] == '.') && (pffbFile->achName[1] == '.')) return FALSE;
      }



      // Set up the file name pointer to point to the filename itself
      // since in this case a pointer is required.

      pcnrItem->pszFileName  =  pcnrItem->szFileName;

      // Fill in all fields of the container record.

      pcnrItem->date.day       = (UCHAR) pffbFile->fdateLastWrite.day;
      pcnrItem->date.month     = (UCHAR) pffbFile->fdateLastWrite.month;
      pcnrItem->date.year      = pffbFile->fdateLastWrite.year;
      pcnrItem->time.seconds   = (UCHAR) pffbFile->ftimeLastWrite.twosecs;
      pcnrItem->time.minutes   = (UCHAR) pffbFile->ftimeLastWrite.minutes;
      pcnrItem->time.hours     = (UCHAR) pffbFile->ftimeLastWrite.hours;
      pcnrItem->cbFile         = pffbFile->cbFile;
      pcnrItem->attrFile       = pffbFile->attrFile;

      // Fill in the remaining fields of the RECORDCORE structure. Note that the .cb
      // field of the RECORDCORE struct was filled in by CM_ALLOCRECORD.

      pcnrItem->rc.pszTree = pcnrItem->pszFileName;
      pcnrItem->rc.pTreeItemDesc = ptidesc;
      pcnrItem->rc.pszName = pcnrItem->pszFileName;
      pcnrItem->rc.pszText = pcnrItem->pszFileName;

      return TRUE;
   }

   // This method will set up a path to add more records to.

   roottree::TreetoOrder (PCNRITEM pRecord)
   {
      pszFilespec[0] = 0;
      strcpy (pszDirspec, pRecord->szDirPath);
      pcnrParent = pRecord;
   }

   // A method to remove records that are children of the passed record.
   // The container screen must also be repainted since the container is collapsed
   // before the folders are removed.

   roottree::RemoveKids (PCNRITEM pRecord)
   {
      PCNRITEM pcnrChild;
      pcnrChild = (PCNRITEM) WinSendMsg (hwndcontainer, CM_QUERYRECORD,
                                           MPFROMP (pRecord),
                                           MPFROM2SHORT( CMA_FIRSTCHILD, CMA_ITEMORDER ));
      pcnrItem = pcnrChild;

      ULONG *pcnrArray;
      pcnrArray = new ULONG [100];
      SHORT cRecords =0;

      while (pcnrChild)
      {
         while (pcnrChild)
         {
            pcnrArray [cRecords] = (ULONG) pcnrChild;
            pcnrChild = (PCNRITEM) WinSendMsg (hwndcontainer, CM_QUERYRECORD,
                                           MPFROMP (pcnrChild),
                                           MPFROM2SHORT( CMA_NEXT, CMA_ITEMORDER ));

            cRecords++;
            if (cRecords > 99) break;
         }
         WinSendMsg (hwndcontainer, CM_REMOVERECORD, MPFROMP (pcnrArray),
                                 MPFROM2SHORT( cRecords, CMA_FREE ));
         cRecords = 0;
      }
      WinSendMsg (hwndcontainer, CM_INVALIDATERECORD, NULL,
                                  MPFROM2SHORT( 0,CMA_REPOSITION ));
      delete pcnrArray;
   }

// ------------------------------------------------------------
// A method to remove all old records and refill from a new drive
//-------------------------------------------------------------
   roottree::RenewCont ( SHORT drive)
   {
      // Set the drive passed as the new default.

      rcx = 0;
      rcx = DosSetDefaultDisk (drive);

      // Set up a flag to enable a keyboard 'Home' message when a new drive tree is loaded.

      newdriveflag = 1;

      // Determine the size of the extra storage needed for the CNRITEM structure.

      pszFilespec[0] = 0;
      pszDirspec[0] = 0;

      pcnrParent = NULL;
      pcnrItem = NULL;
      pcnrFirst = NULL;

      WinSendMsg (hwndcontainer, CM_REMOVERECORD, NULL,
                                 MPFROM2SHORT( NULL, CMA_FREE ));
      WinSendMsg (hwndcontainer, CM_INVALIDATERECORD, NULL,
                                  MPFROM2SHORT( 0,CMA_REPOSITION ));
   }
//-----------------------------------------------------------------------
// This sort function is necessary even with HPFS (which is pre-sorted in the directory
// structure itself) in order to put the subdirectories first in the listing and also for
// FAT directory structures.
//-----------------------------------------------------------------------
   static SHORT __syscall roottree::SortByName (PCNRITEM p1, PCNRITEM p2)
   {
      SHORT Sresult;

      // Use the case insensitive 'stricmp' here

      Sresult = (SHORT) stricmp (p1->pszFileName, p2->pszFileName);
      return Sresult;
   }


//-----------------------------------------------------------------------
