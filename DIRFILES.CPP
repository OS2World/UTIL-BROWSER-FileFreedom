/*---------------------------------------------------------------*/
/*                       Methods for the dirfiles class                              */
/*---------------------------------------------------------------*/

#define INCL_DOSFILEMGR
#define INCL_WINFRAMEMGR
#define INCL_WINSTDCNR
#define INCL_WINWINDOWMGR
#define INCL_DOSPROCESS
#define INCL_WINPOINTERS
#define INCL_WININPUT
#define INCL_DOSNLS

#include <os2.h>
#include <string.h>
#include "dirfiles.h"

   dirfiles::dirfiles (HWND container, SHORT drive)
   {
      // Get the Date format for the country of the user.
      // The date format will be returned in the CtryInfo.fsDateFmt member of the CtryInfo
      // structure.  0=mm/dd/yy   1=dd/mm/yy    2=yy/mm/dd

      Country.country = 0;
      Country.codepage = 0;
      rc = DosQueryCtryInfo (sizeof (CtryInfo), &Country, &CtryInfo, &ulInfoLen);

      // If the query failed, use the North American format.

      if (rc) CtryInfo.fsDateFmt = 0;

      // Set the default sorting method to 'SortByName'.
      // This sortflag is sticky and is used in all subsequent FillContainer methods.

      sortflag = 1;

      // Set the drive passed as the new default.

      DosSetDefaultDisk ( drive);

      hwndcontainer = container;

      // Allocate a PFILESANDBYTES structure

      pFandB = new FILESANDBYTES;

      // Determine the size of the extra storage needed for the CNRITEM structure.

      extrabytes = sizeof (CNRITEM)-sizeof (RECORDCORE);

      pszFilespec = new char[CCHMAXPATH +1];
      pszDirspec = new char[CCHMAXPATH +1];
      pszFilespec[0] = 0;
      pszDirspec[0] = 0;

      // These buffers are used with the DosFindFirst etc. functions.  The large array
      // of buffers holds information on many files at a time.

      pffb = new FILEFINDBUF3[FILES_TO_GET];

      // Memory allocation for these pointers is handled by the CM_ALLOCRECORD message

      pcnrItem = NULL;
      pcnrFirst = NULL;
      pcnrParent = NULL;

      // The CNRINFO structure contains information about the container.  It should be
      // cleared first and then the required fields reset.  It is updated via a CM_SETCNRINFO
      // message which also indicates which fields to change with the CMA_* attributes.

      memset (&cnri, 0, sizeof (CNRINFO));

      cnri.cb = sizeof (CNRINFO);
      cnri.pSortRecord = NULL;
      cnri.flWindowAttr = CV_TEXT;

      cnri.cyLineSpacing = 0;
      cnri.cFields = 5;

      WinSendMsg (hwndcontainer, CM_SETCNRINFO, MPFROMP (&cnri),
                      MPFROMLONG ( CMA_FLWINDOWATTR
                      | CMA_LINESPACING ));

      // Set up the static members of the RECORDINSERT structure

      (void) memset( &ri, 0, sizeof( RECORDINSERT ) );

      ri.cb                 = sizeof( RECORDINSERT );
      ri.pRecordOrder       = (PRECORDCORE) CMA_END;
      ri.zOrder             = (USHORT) CMA_TOP;
      ri.fInvalidateRecord  = FALSE;

   }

   // The memory that has been allocated must be freed up.  This is critical.

   dirfiles::~dirfiles()
   {
      WinSendMsg (hwndcontainer, CM_REMOVEDETAILFIELDINFO, NULL,
                                                            MPFROM2SHORT( 0, CMA_FREE ) );
      WinSendMsg (hwndcontainer, CM_REMOVERECORD, NULL, MPFROM2SHORT( 0, CMA_FREE ) );
      delete pffb;
      delete pszFilespec;
      delete pszDirspec;
      delete pFandB;
   }

   // This method reads the files in the directories and sends out to have the files inserted in
   // the container.  It also sends out to have the columns arranged.

   dirfiles::FillContainer (ULONG fileattributes)
   {
     // HDIR_CREATE = System assigns handle for standard output (always available).
     // A new handle will be stored here on return from the function DosFindFirst.

      hdir = HDIR_SYSTEM;
      ok = TRUE;

      SetColumns ();

      maxfiles = FILES_TO_GET;
      rc = 0;

      // Create our filespec and mark the end of the pathname so that the wildcard pattern
      // can be trimmed.

      strcat (pszFilespec, pszDirspec);
      pchEndname = pszFilespec + strlen (pszFilespec);  // Pointer to end of pathname string.
      strcat (pszFilespec, "\\*.*");

      rc = DosFindFirst ( pszFilespec, &hdir, fileattributes, pffb, FF_BUFFSIZE,
                           &maxfiles, FIL_STANDARD);

      // Strip the  trailing "\\*.*" from the pszFilespec.
      // pszFilespec will now hold the complete path and is ready for the next pass.
      // This happens only if TraverseSubDirs() is called below.

      *pchEndname = 0;

      // This loop will send out to insert the subdirectories in the container and it also will
      // keep reading in subdirectories if there were more than could be read on the
      // DosFindFirst pass.  When thru, it releases the handle.

      while ( !rc)
      {
         InsertRecords ();
         if ( maxfiles == FILES_TO_GET)
         {
            rc = DosFindNext (hdir, pffb, FF_BUFFSIZE, &maxfiles);
         }
         else
         {
            rc = 1;
            DosFindClose (hdir);
         }
      }

      // Now that the containers are inserted, sort them according to the 'sortflag' setting.

      switch (sortflag)
      {
         // Ascending and Decending sorts are indicated by the last parameter.
         // NULL = Ascending
         // PVOID TRUE = Decending

         case 1:
            WinSendMsg (hwndcontainer, CM_SORTRECORD, MPFROMP (SortByName) , NULL);
            break;
         case 2:
            WinSendMsg (hwndcontainer, CM_SORTRECORD, MPFROMP (SortByDate) , NULL);
            break;
         case 3:
            WinSendMsg (hwndcontainer, CM_SORTRECORD, MPFROMP (SortBySize) , NULL);
            break;
         case 4:
            WinSendMsg (hwndcontainer, CM_SORTRECORD, MPFROMP (SortByExtension) , NULL);
            break;
         case 5:
            WinSendMsg (hwndcontainer, CM_SORTRECORD, MPFROMP (SortByName) , (PVOID) TRUE);
            break;
         case 6:
            WinSendMsg (hwndcontainer, CM_SORTRECORD, MPFROMP (SortByDate) , (PVOID) TRUE);
            break;
         case 7:
            WinSendMsg (hwndcontainer, CM_SORTRECORD, MPFROMP (SortBySize) , (PVOID) TRUE);
            break;
         case 8:
            WinSendMsg (hwndcontainer, CM_SORTRECORD, MPFROMP (SortByExtension) , (PVOID) TRUE);
            break;
      }
      // Scroll to the head of the file.  With the newly sorted directories on fat partitions,
      // the cursor will rest on the oldest file in the directory otherwise.  This may be
      // nowhere near the head of the directory.

      WinSendMsg (hwndcontainer, WM_CHAR, MPFROMSH2CH (KC_VIRTUALKEY, 0, 0),
                            MPFROM2SHORT (0, VK_HOME));
   }

   // The setting up of the Details field info via the FIELDINFO and FIELDINFOINSERT structures
   // need only be done once since the information in them is not file dependent.

   void dirfiles::SetColumns ()
   {

    // Allocate storage for container column data

    pfinfo = (PFIELDINFO) WinSendMsg( hwndcontainer, CM_ALLOCDETAILFIELDINFO,
                                                   MPFROMLONG (CCOLUMNS ), NULL );

    if (pfinfo )
    {
        PFIELDINFO      pfinfoFirst;

        // Store original value of pfinfo so we won't lose it when it changes.
        // This will be needed on the CM_INSERTDETAILFIELDINFO message.

        pfinfoFirst = pfinfo;

        // Memory allocation for this structure is handled by the CM_ALLOCDETAILFIELDINFO
        // message.  The .cb is also probable filled in.

        //      pfinfo->cb = sizeof (FIELDINFO);              /* Structure size  */
        //      pfinfo->flData = ;          /* Data attributes  */
        //      pfinfo->flTitle = ;         /* Attributes of column headings  */
        //      pfinfo->pTitleData = ;      /* Column heading data  */
        //      pfinfo->offStruct = ;       /* Structure offset  */
        //      pfinfo->pUserData = ;       /* Pointer  */
        //      pfinfo->pNextFieldInfo = ;  /* Pointer  */
        //      pfinfo->cxWidth = ;         /* Column width  */

        // Fill in column information for the file name. Note that we are
        // using the pszFileName variable rather than szFileName. We do this
        // because the container needs a pointer to the file name. If we used
        // szFileName (a character array, not a pointer), the container would
        // take the first 4 bytes of szFileName and think it was a pointer,
        // which of course it is not. Later in the FillInRecord function we set
        // pszFileName to point to szFileName.

        // Fill in column information for the File Name column

        pfinfo->flData     = CFA_STRING | CFA_LEFT | CFA_HORZSEPARATOR | CFA_OWNER;
        pfinfo->flTitle    = CFA_LEFT | CFA_FITITLEREADONLY;
        pfinfo->pTitleData = "File Name";
        pfinfo->offStruct  = FIELDOFFSET( CNRITEM, pszFileName );

         // NOTE: The FIELDOFFSET (type, field) is defined in OS2DEF.H and can be used
         //         with any structure.

        // Fill in column information for the file size

        pfinfo             = pfinfo->pNextFieldInfo;
        pfinfo->flData     = CFA_ULONG | CFA_RIGHT | CFA_HORZSEPARATOR ;
        pfinfo->flTitle    = CFA_CENTER | CFA_FITITLEREADONLY;
        pfinfo->pTitleData = "File Size";
        pfinfo->offStruct  = FIELDOFFSET( CNRITEM, cbFile );
        pfinfo->cxWidth = 0;

        // Fill in the File Attributes column

        pfinfo             = pfinfo->pNextFieldInfo;
        pfinfo->flData     = CFA_STRING | CFA_HORZSEPARATOR | CFA_LEFT;
        pfinfo->flTitle    = CFA_LEFT | CFA_FITITLEREADONLY;
        pfinfo->pTitleData = "Attrib    Date     Time";
        pfinfo->offStruct  = FIELDOFFSET( CNRITEM, pszAttrString );

        // Set up the static members of the FIELDINFOINSERT structure.
        // No need to zero out since they are all used each insert.

        finfoinsert.cb = sizeof (FIELDINFOINSERT);     /* Structure size  */
        finfoinsert.pFieldInfoOrder = (PFIELDINFO) CMA_FIRST;       /* Column order  */
        finfoinsert.fInvalidateFieldInfo = TRUE;         /* Update flag  */
        finfoinsert.cFieldInfoInsert = CCOLUMNS;       /* Number of columns  */


        // Use the CM_INSERTDETAILFIELDINFO message to tell the container
        // all the column information it needs to function properly. Place
        // this column info first in the column list and update the display
        // after they are inserted (fInvalidateFieldInfo = TRUE)

        WinSendMsg( hwndcontainer, CM_INSERTDETAILFIELDINFO, MPFROMP( pfinfoFirst ),
                         MPFROMP( &finfoinsert ));
    }
   }

   BOOL dirfiles::InsertRecords ()
   {
      BOOL success = TRUE;
      PBYTE pbBuf = (PBYTE) pffb;

      pcnrItem = (PCNRITEM) WinSendMsg (hwndcontainer, CM_ALLOCRECORD,
                                   MPFROMSHORT (extrabytes), MPFROMSHORT ((USHORT) maxfiles));

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

        ri.pRecordParent      = (PRECORDCORE) pcnrParent;
        ri.cRecordsInsert     = cFilesInserted;

        success =  (BOOL) WinSendMsg (hwndcontainer, CM_INSERTRECORD, MPFROMP (pcnrFirst),
                        MPFROMP (&ri ));
      }
      return success;
   }

   BOOL dirfiles::FillInRecord ()
   {
      // The CNRITEM structure is a devised structure to hold extra information that
      // supplements the info in the RECORDCORE or MINIRECORDCORE structures.
      // It must also contain the RECORDCORE or MINIRECORDCORE structure itself.
      // The structure is up to the programmer to design with fields as required.
      // Fill in the values of CNRITEM and RECORDCORE. Note that the .cb
      // field of RECORDCORE was filled in by CM_ALLOCRECORD.

      // First, if it's not the root directory (pcnrParent will be equal to 0),
      // Also sluff off any listing of a single "." directory.

      if ((pffbFile->cchName ==1) && (pffbFile->achName[0] == '.')) return FALSE;

      // Copy over the filename and if this happens to be a subdirectory (it should be)
      // other than '..', add a '\' to the string.  If it is '..', trim back the
      // path one step.

      memset (pcnrItem->szFileName, 0, sizeof (pcnrItem->szFileName));
      memcpy (pcnrItem->szFileName, pffbFile->achName, pffbFile->cchName);

      if (pffbFile->cchName == 2)
      {
         if ((pffbFile->achName[0] == '.') && (pffbFile->achName[1] == '.'))
         {
            strcpy (pcnrItem->szDirPath, pszDirspec);
            int plength = strlen (pcnrItem->szDirPath);
            int backslash = 0;

            for (int q = plength; q > 0; q--)
            {
               if (pcnrItem->szDirPath [q] == 92) break;
            }
            pcnrItem->szDirPath [q] = 0;
         }
         else
         {
            strcpy (pcnrItem->szDirPath, pszDirspec);
            strcat (pcnrItem->szDirPath, "\\");
            strcat (pcnrItem->szDirPath, pcnrItem->szFileName);
         }
      }
      else
      {
         strcpy (pcnrItem->szDirPath, pszDirspec);
         strcat (pcnrItem->szDirPath, "\\");
         strcat (pcnrItem->szDirPath, pcnrItem->szFileName);
      }

      pcnrItem->cbFile = pffbFile->cbFile;
      if (pffbFile->attrFile & 16) strcat (pcnrItem->szFileName, "\\");

      // Set up the file name and attribute string pointers to point to the strings themselves
      // since in this case a pointer is required.

      pcnrItem->pszFileName = pcnrItem->szFileName;
      pcnrItem->pszAttrString = pcnrItem->szAttrString;

      // This string needs a null terminator.

      pcnrItem->szAttrString[4] = 0;

      // Fill in all fields of the container record.

      pcnrItem->date.day       = (UCHAR) pffbFile->fdateLastWrite.day;
      pcnrItem->date.month     = (UCHAR) pffbFile->fdateLastWrite.month;
      pcnrItem->date.year      = (USHORT) (pffbFile->fdateLastWrite.year + 80);
      if (pcnrItem->date.year > 99) pcnrItem->date.year -= 100;
      pcnrItem->time.seconds  = (UCHAR) pffbFile->ftimeLastWrite.twosecs;
      pcnrItem->time.minutes   = (UCHAR) pffbFile->ftimeLastWrite.minutes;
      pcnrItem->time.hours     = (UCHAR) pffbFile->ftimeLastWrite.hours;
      pcnrItem->attrFile       = pffbFile->attrFile;

      // This next section will fill in an ascii string that is comprised of the file date,
      // the file time and the file attributes.  This will appear as three separate columns
      // in the container, but is formatted here as one column since formatting three
      // columns with the fieldinfo structure puts in too many spaces and wastes space.
      // This ascii string will be stored in the CNRITEM structure as szAttrString.

      pcnrItem->attrFile & 1 ? pcnrItem->szAttrString[0] = 114 : pcnrItem->szAttrString[0] = 45;
      pcnrItem->attrFile & 2 ? pcnrItem->szAttrString[1] = 104 : pcnrItem->szAttrString[1] = 45;
      pcnrItem->attrFile & 4 ? pcnrItem->szAttrString[2] = 115 : pcnrItem->szAttrString[2] = 45;
      pcnrItem->attrFile & 32 ? pcnrItem->szAttrString[3] = 97 : pcnrItem->szAttrString[3] = 45;
      pcnrItem->szAttrString[4] = 32;

      pcnrItem->szAttrString[5] = 32;

      // The date will be inserted according to the Date Format used by the Country of the user.

      if (CtryInfo.fsDateFmt == 0)
      {
      	if (pcnrItem->date.month > 9) pcnrItem->szAttrString [6] = 49;
         // If the leading character is a zero, make it a blank.
      	if (pcnrItem->date.month < 10) pcnrItem->szAttrString [6] = 32;
      	pcnrItem->szAttrString [7] = CHAR (pcnrItem->date.month % 10 + 48);
      	pcnrItem->szAttrString [8] = 45;
      	pcnrItem->szAttrString [9] = CHAR (pcnrItem->date.day / 10 + 48);
      	pcnrItem->szAttrString [10] = CHAR (pcnrItem->date.day % 10 + 48);
      	pcnrItem->szAttrString [11] = 45;
      	pcnrItem->szAttrString [12] = CHAR (pcnrItem->date.year / 10 + 48);
      	pcnrItem->szAttrString [13] = CHAR (pcnrItem->date.year % 10 + 48);
      }
      else if (CtryInfo.fsDateFmt ==1)
      {
      	pcnrItem->szAttrString [6] = CHAR (pcnrItem->date.day / 10 + 48);
         // If the leading character is a zero, make it a blank.
         if (pcnrItem->szAttrString [6] == 48) pcnrItem->szAttrString [6] = 32;
      	pcnrItem->szAttrString [7] = CHAR (pcnrItem->date.day % 10 + 48);
      	pcnrItem->szAttrString [8] = 45;
      	if (pcnrItem->date.month > 9) pcnrItem->szAttrString [9] = 49;
      	if (pcnrItem->date.month < 10) pcnrItem->szAttrString [9] = 48;
      	pcnrItem->szAttrString [10] = CHAR (pcnrItem->date.month % 10 + 48);
      	pcnrItem->szAttrString [11] = 45;
      	pcnrItem->szAttrString [12] = CHAR (pcnrItem->date.year / 10 + 48);
      	pcnrItem->szAttrString [13] = CHAR (pcnrItem->date.year % 10 + 48);
      }
      else if (CtryInfo.fsDateFmt == 2)
      {
         // If the leading character is a zero, because this is the year, we will leave it a zero.
      	pcnrItem->szAttrString [6] = CHAR (pcnrItem->date.year / 10 + 48);
      	pcnrItem->szAttrString [7] = CHAR (pcnrItem->date.year % 10 + 48);
        	pcnrItem->szAttrString [8] = 45;
      	if (pcnrItem->date.month > 9) pcnrItem->szAttrString [9] = 49;
      	if (pcnrItem->date.month < 10) pcnrItem->szAttrString [9] = 48;
      	pcnrItem->szAttrString [10] = CHAR (pcnrItem->date.month % 10 + 48);
      	pcnrItem->szAttrString [11] = 45;
      	pcnrItem->szAttrString [12] = CHAR (pcnrItem->date.day / 10 + 48);
      	pcnrItem->szAttrString [13] = CHAR (pcnrItem->date.day % 10 + 48);
      }
      // There are no formats listed for other numbers but there is another date format described
      // which would be yy/dd/mm.  Therefore if there is a different number listed, we will
      // assume that it is the designator for this format.
      // This also insures that any other number returned will at least be formatted.

      else
      {
         // If the leading character is a zero, because this is the year, we will leave it a zero.
      	pcnrItem->szAttrString [6] = CHAR (pcnrItem->date.year / 10 + 48);
      	pcnrItem->szAttrString [7] = CHAR (pcnrItem->date.year % 10 + 48);
        	pcnrItem->szAttrString [8] = 45;
      	pcnrItem->szAttrString [9] = CHAR (pcnrItem->date.day / 10 + 48);
      	pcnrItem->szAttrString [10] = CHAR (pcnrItem->date.day % 10 + 48);
      	pcnrItem->szAttrString [11] = 45;
      	if (pcnrItem->date.month > 9) pcnrItem->szAttrString [12] = 49;
      	if (pcnrItem->date.month < 10) pcnrItem->szAttrString [12] = 48;
      	pcnrItem->szAttrString [13] = CHAR (pcnrItem->date.month % 10 + 48);
      }
   	pcnrItem->szAttrString[14]=32;
   	pcnrItem->szAttrString[15]=32;

   	int pm=0;
   	if (pcnrItem->time.hours >= 12 && pcnrItem->time.minutes >= 0) pm = 1;
   	if (pcnrItem->time.hours == 0 && pm == 0)
      {
         pcnrItem->szAttrString[16] = 49;
         pcnrItem->szAttrString[17] = 50;
      	pcnrItem->szAttrString[21] = 97;
      }
      else if (pcnrItem->time.hours > 0 && pm ==0)
   	{
   		pcnrItem->szAttrString[16] = CHAR (pcnrItem->time.hours / 10 + 48);
   		if (pcnrItem->time.hours / 10 == 0) pcnrItem->szAttrString[16] = 32;
   		pcnrItem->szAttrString[17] = CHAR (pcnrItem->time.hours % 10 + 48);
   		pcnrItem->szAttrString[21] = 97;
   	}
   	else if (pcnrItem->time.hours > 21 && pm == 1)
   	{
   		pcnrItem->szAttrString[16] = 49;
   		pcnrItem->szAttrString[17] = CHAR (pcnrItem->time.hours + 26);
   		pcnrItem->szAttrString[21] = 112;
   	}
   	else if (pcnrItem->time.hours >= 13 && pm == 1)
   	{
   		pcnrItem->szAttrString[16] = 32;
   		pcnrItem->szAttrString[17] = CHAR (pcnrItem->time.hours + 36);
   		pcnrItem->szAttrString[21] = 112;
   	}
   	else if (pcnrItem->time.hours >= 12 && pm == 1)
   	{
   		pcnrItem->szAttrString[16] = 49;
   		pcnrItem->szAttrString[17] = 50;
   		pcnrItem->szAttrString[21] = 112;
   	}

   	pcnrItem->szAttrString[18] = 58;
   	pcnrItem->szAttrString[19] = CHAR (pcnrItem->time.minutes / 10 + 48);
   	pcnrItem->szAttrString[20] = CHAR (pcnrItem->time.minutes % 10 + 48);

   	pcnrItem->szAttrString[22] = 32;
   	pcnrItem->szAttrString[23] = 32;
   	pcnrItem->szAttrString[24] = 0;
      // Fill in all fields of the RECORDCORE structure. Note that the .cb
      // field of the RECORDCORE struct was filled in by CM_ALLOCRECORD.
      // Only load the file icons if the 'CV_NAME' attribute is set.  It is the
      // responsibility of the calling program to reload the files container with
      // that attribute set to see the icons.  This saves loading time for all
      // the other views.

      if (cnri.flWindowAttr & CV_NAME)
      pcnrItem->rc.hptrIcon  = WinLoadFileIcon (pcnrItem->szDirPath, FALSE);
      pcnrItem->rc.hptrMiniIcon = pcnrItem->rc.hptrIcon;
      pcnrItem->rc.flRecordAttr = CRA_RECORDREADONLY | CRA_COLLAPSED;

      pcnrItem->rc.pszTree = pcnrItem->pszFileName;
      pcnrItem->rc.pszName = pcnrItem->pszFileName;
      pcnrItem->rc.pszText = pcnrItem->pszFileName;

     return TRUE;
   }
//----------------------------------------------------------------------
// This sort function is necessary even with HPFS (which is pre-sorted in the directory
// structure itself) in order to put the subdirectories first in the listing and also for
// FAT directory structures.  This function sorts by name.  If the PVOID parameter is true,
// the sort is done in reverse order.
//----------------------------------------------------------------------

   static SHORT __syscall dirfiles::SortByName (PCNRITEM p1, PCNRITEM p2, PVOID reverse)
   {
      SHORT Sresult;

      // Special handling is required to get the sub-directories at the head of the list.

      if ( p1->attrFile & 16)
      {
         // Use the case insensitive 'stricmp' here.
         // If the 'reverse' flag is set, the results are multiplied by minus 1 to achieve
         // opposite results.

         if (!(p2->attrFile &16)) Sresult = -1;
         else
         {
            Sresult = (SHORT) stricmp (p1->pszFileName, p2->pszFileName);
            if (reverse && Sresult) Sresult = SHORT (Sresult * -1);
         }
      }
      else if (!(p1->attrFile & 16))
      {
         // Use the case insensitive 'stricmp' here

         if (p2->attrFile & 16) Sresult = 1;
         else
         {
            Sresult = (SHORT) stricmp (p1->pszFileName, p2->pszFileName);
            if (reverse && Sresult) Sresult = SHORT (Sresult * -1);
         }
      }
      return Sresult;
   }

//-------------------------------------------------
// This function sorts files by Date.
//If the PVOID parameter is true, the sort is done in reverse order.
//-------------------------------------------------
   static SHORT __syscall dirfiles::SortByDate (PCNRITEM p1, PCNRITEM p2, PVOID reverse)
   {
      SHORT Sresult;

      // Special handling is required to get the sub-directories at the head of the list.

      if ( p1->attrFile & 16)
      {
         if (!(p2->attrFile &16)) Sresult = -1;
         else
         {
            // If the file IS a subdirectory, prepare to sort it and allow for
            // files that are dated year 2000+ to maintain a Y2K compatible sort order.
            // This fix-up will actually work up to 2049!!

            if  (!(p1->date.year == p2->date.year))
            {
                int zz =0;
                if (p1->date.year < 50 && p2->date.year > 49)
                {
                        Sresult = 1;
                        zz=1;
                }

                if (p2->date.year < 50 && p1->date.year > 49)
                {
                        Sresult = -1;
                        zz=1;
                }

               if (p2->date.year > p1->date.year && zz==0) Sresult = -1;
               if (p1->date.year > p2->date.year && zz==0) Sresult = 1;
            }
            else
            {
               if (p1->date.month > p2->date.month) Sresult = 1;
               else if (p1->date.month < p2->date.month) Sresult = -1;
               else
               {
                  if (p1->date.day > p2->date.day) Sresult = 1;
                  else if (p1->date.day < p2->date.day) Sresult = -1;
                  else
                  {
                     if (p1->time.hours > p2->time.hours ) Sresult = 1;
                     else if (p1->time.hours < p2->time.hours) Sresult = -1;
                     else
                     {
                        if (p1->time.minutes > p2->time.minutes) Sresult = 1;
                        else if (p1->time.minutes < p2->time.minutes) Sresult = -1;
                        else
                        {
                           Sresult = 0;
                        }
                     }
                  }
               }
            }
            // If a reverse sort is called for, reverse the result here.

            if (reverse && Sresult) Sresult = SHORT (Sresult * -1);
         }
      }
      else if (!(p1->attrFile & 16))
      {
         if (p2->attrFile & 16) Sresult = 1;
         else
         {
            // If the file is NOT a subdirectory, prepare to sort it and allow for
            // files that are dated year 2000+ to maintain a Y2K compatible sort order.

            if  (!(p1->date.year == p2->date.year))
            {
                int zz =0;
                if (p1->date.year < 50 && p2->date.year > 49)
                {
                        Sresult = 1;
                        zz=1;
                }

                if (p2->date.year < 50 && p1->date.year > 49)
                {
                        Sresult = -1;
                        zz=1;
                }

               if (p2->date.year > p1->date.year && zz==0) Sresult = -1;
               if (p1->date.year > p2->date.year && zz==0) Sresult = 1;
            }
            else
            {
               if (p1->date.month > p2->date.month) Sresult = 1;
               else if (p1->date.month < p2->date.month) Sresult = -1;
               else
               {
                  if (p1->date.day > p2->date.day) Sresult = 1;
                  else if (p1->date.day < p2->date.day) Sresult = -1;
                  else
                  {
                     if (p1->time.hours > p2->time.hours) Sresult = 1;
                     else if (p1->time.hours < p2->time.hours) Sresult = -1;
                     else
                     {
                        if (p1->time.minutes > p2->time.minutes) Sresult = 1;
                        else if (p1->time.minutes < p2->time.minutes) Sresult = -1;
                        else
                        {
                           Sresult = 0;
                        }
                     }
                  }
               }
            }
            // If a reverse sort is called for, reverse the result here.
            if (reverse && Sresult) Sresult = SHORT (Sresult * -1);
         }
      }
      return Sresult;
   }

//-------------------------------------------------------
// This function sorts files by size in bytes.
//If the PVOID parameter is true, the sort is done in reverse order.
//-------------------------------------------------------
   static SHORT __syscall dirfiles::SortBySize (PCNRITEM p1, PCNRITEM p2, PVOID reverse)
   {
      SHORT Sresult;

      // Special handling is required to get the sub-directories at the head of the list.

      if ( p1->attrFile & 16)
      {
         // The subdirectories will not be sorted by size but by name.

         if (!(p2->attrFile &16)) Sresult = -1;
         else Sresult = (SHORT) stricmp (p1->pszFileName, p2->pszFileName);
      }
      else if (!(p1->attrFile & 16))
      {
         // Use the case insensitive 'stricmp' here

         if (p2->attrFile & 16) Sresult = 1;
         else
         {
            if (p1->cbFile > p2->cbFile) Sresult = 1;
            else if (p1->cbFile < p2->cbFile) Sresult = -1;
            else Sresult = 0;

            // If a reverse sort is called for, reverse the result here.
            if (reverse && Sresult) Sresult = SHORT (Sresult * -1);
         }
      }
      return Sresult;
   }

//------------------------------------------------------------
// This function sorts files by the last extension it finds.
//If the PVOID parameter is true, the sort is done in reverse order.
//------------------------------------------------------------
   static SHORT __syscall dirfiles::SortByExtension (PCNRITEM p1, PCNRITEM p2, PVOID reverse)
   {
   // Special Note for SortByExtension:
   // The sort order of the extensions is only part of the display sort order .
   // Since there can be many files with the same extension, the sort order of the files
   // within a group of similar extensions will be determined by the previous sort.
   // Eg.  if the last sort method was RevSortByName, all files with similar extensions will
   // be sorted in the alpahabetically decending order.

      SHORT Sresult;

      // Special handling is required to get the sub-directories at the head of the list.

      if ( p1->attrFile & 16)
      {
         // Use the case insensitive 'stricmp' here

         if (!(p2->attrFile &16)) Sresult = -1;
         else Sresult = (SHORT) stricmp (p1->pszFileName, p2->pszFileName);
      }
      else if (!(p1->attrFile & 16))
      {
         // The subdirectories will not be sorted by extension, but by name.

         if (p2->attrFile & 16) Sresult = 1;
         else
         {
            USHORT len;
            len = USHORT (strlen (p1->szFileName));
            USHORT dot, i;
            dot = 0;
            for (i = 0; i < len; i++)
            {
               if (p1->szFileName [i] == 46) dot = i;
            }
            if (!dot) dot = len;
            PSZ pp1;
            pp1 = &p1->szFileName [dot + 1];
            dot = 0;
            len = USHORT (strlen (p2->szFileName));
            for (i = 0; i < len; i++)
            {
               if (p2->szFileName [i] == 46) dot = i;
            }
            if (!dot) dot = len;
            PSZ pp2;
            pp2 = &p2->szFileName [dot + 1];
            Sresult = (SHORT) stricmp (pp1, pp2);

            // If a reverse sort is called for, reverse the result here.
            if (reverse && Sresult) Sresult = SHORT (Sresult * -1);
         }
      }
      return Sresult;
   }

//----------------------------------------------------
// A method to call the protected 'SortByxxxx' method.
// If the 'reversort' parameter is true, the sort is done in reverse order.
//----------------------------------------------------
   dirfiles::NameSort (ULONG reversesort)
   {
      WinSendMsg (hwndcontainer, CM_SORTRECORD, MPFROMP (SortByName) , (PVOID) reversesort);
      if (reversesort) sortflag = 5;
      else
         sortflag = 1;
   }
//----------------------------------------------------
// A method to call the protected 'SortByxxxx' method.
// If the 'reversort' parameter is true, the sort is done in reverse order.
//----------------------------------------------------
   dirfiles::DateSort (ULONG reversesort)
   {
      WinSendMsg (hwndcontainer, CM_SORTRECORD, MPFROMP (SortByDate) , (PVOID) reversesort);
      if (reversesort) sortflag = 6;
      else
         sortflag = 2;
   }
//----------------------------------------------------
// A method to call the protected 'SortByxxxx' method.
// If the 'reversort' parameter is true, the sort is done in reverse order.
//----------------------------------------------------
   dirfiles::SizeSort (ULONG reversesort)
   {
      WinSendMsg (hwndcontainer, CM_SORTRECORD, MPFROMP (SortBySize) , (PVOID) reversesort);
      if (reversesort) sortflag = 7;
      else
         sortflag = 3;
   }
//----------------------------------------------------
// A method to call the protected 'SortByxxxx' method.
// If the 'reversort' parameter is true, the sort is done in reverse order.
//----------------------------------------------------
   dirfiles::ExtensionSort (ULONG reversesort)
   {
      WinSendMsg (hwndcontainer, CM_SORTRECORD, MPFROMP (SortByExtension) , (PVOID) reversesort);
      if (reversesort) sortflag = 8;
      else
         sortflag = 4;
   }
//---------------------------------------------------------------------
// This method will pull the directory path from a passed PCNRITEM pointer and call
// FillContainer() to fill the container with the files found in the passed path.
// If the passed pointer is NULL, it is the root of the current drive and doesn't need
// a path copied.
//---------------------------------------------------------------------
   dirfiles::PathToOrder (PSZ pszPath, SHORT drive)
   {
      // Set the drive passed as the new default.

      DosSetDefaultDisk ( drive);

      if (pszPath)
      {
         strcpy (pszDirspec, pszPath);
         if (strlen (pszPath) == 3)
         {
            if ((pszPath[1] == '.')
                         && (pszPath[2] == '.')) pszDirspec [0]  = 0;
         }
      }
      else pszDirspec[0] = 0;
      EmptyContainer ();
   }

//-----------------------------------------------------------------
// This method empties the container.
// It removes the records and removes the fieldinfo structures tied to each record.
//-----------------------------------------------------------------
   dirfiles::EmptyContainer ()
   {
      pszFilespec[0] = 0;

      WinSendMsg (hwndcontainer, CM_REMOVEDETAILFIELDINFO, NULL,
                                                            MPFROM2SHORT( 0, CMA_FREE ) );
      WinSendMsg (hwndcontainer, CM_REMOVERECORD, NULL, MPFROM2SHORT( 0, CMA_FREE ) );
   }

//-------------------------------------------------------------------------
// This method returns a structure that contains a count of the files and total bytes in
// those files.
//-------------------------------------------------------------------------
   PFILESANDBYTES dirfiles::Getfilesandbytes ()
   {
      SHORT whatRec = CMA_FIRST;

      // Give a value to the pointer so that the 'while' loop can get started.

      PCNRITEM pRecord = (PCNRITEM) 1;

      pFandB->cFiles = 0;
      pFandB->cBytes = 0;

      while (pRecord)
      {
         pRecord = (PCNRITEM) WinSendMsg (hwndcontainer, CM_QUERYRECORD,
                                                    MPFROMP( pRecord ),
                                       MPFROM2SHORT( whatRec, CMA_ITEMORDER ));
         whatRec = CMA_NEXT;
         if (pRecord)
         {
            pFandB->cFiles ++;
            pFandB->cBytes += pRecord->cbFile;
         }
      }
      return pFandB;
   }

//-------------------------------------------------------------
// Methods to set the current view.
//-------------------------------------------------------------
   dirfiles::SetNameView ()
   {
      // Want to use mini icons, but they come out mostly unrecognizable.
//      cnri.flWindowAttr = CV_NAME | CA_DRAWICON | CV_FLOW | CV_MINI;

      // CA_OWNERDRAW is used to colour the subdirecory names.

      cnri.flWindowAttr = CV_NAME | CA_DRAWICON | CV_FLOW | CA_OWNERDRAW;
      WinSendMsg (hwndcontainer, CM_SETCNRINFO, MPFROMP (&cnri),
                      MPFROMLONG ( CMA_FLWINDOWATTR));
   }

//-------------------------------------------------------------
// Methods to set the current view.
//-------------------------------------------------------------
   dirfiles::SetDetailView ()
   {
      // Don't use CA_OWNERDRAW here because it will destroy the columns.
      // Instead, use CFA_OWNER in the FIELDINFO.flData field.
      cnri.flWindowAttr = CV_DETAIL;
      WinSendMsg (hwndcontainer, CM_SETCNRINFO, MPFROMP (&cnri),
                      MPFROMLONG ( CMA_FLWINDOWATTR));
   }

//-------------------------------------------------------------
// Methods to set the current view.
//-------------------------------------------------------------
   dirfiles::SetFlowView ()
   {
      // CA_OWNERDRAW is used to colour the subdirecory names.

      cnri.flWindowAttr = CV_TEXT | CV_FLOW | CA_OWNERDRAW;
      WinSendMsg (hwndcontainer, CM_SETCNRINFO, MPFROMP (&cnri),
                      MPFROMLONG ( CMA_FLWINDOWATTR));
   }

//-------------------------------------------------------------
// Methods to set the current view.
//-------------------------------------------------------------
   dirfiles::SetTextView ()
   {
      // CA_OWNERDRAW is used to colour the subdirecory names.
      cnri.flWindowAttr = CV_TEXT | CA_OWNERDRAW;
      WinSendMsg (hwndcontainer, CM_SETCNRINFO, MPFROMP (&cnri),
                      MPFROMLONG ( CMA_FLWINDOWATTR));

   }
//-------------------------------------------------------------
// Methods to set the current view.
//-------------------------------------------------------------
   dirfiles::SetColumnHeadings ()
   {
      cnri.flWindowAttr = CV_DETAIL | CA_DETAILSVIEWTITLES;
      WinSendMsg (hwndcontainer, CM_SETCNRINFO, MPFROMP (&cnri),
                      MPFROMLONG ( CMA_FLWINDOWATTR));
   }
//-----------------------------------------------------------------------
