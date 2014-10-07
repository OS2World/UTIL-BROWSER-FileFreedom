/*---------------------------------------------------------------*/
/*                       Methods for the zipcntr class                              */
/*---------------------------------------------------------------*/

#define INCL_DOSSESMGR
#define INCL_WINFRAMEMGR
#define INCL_WINSTDCNR
#define INCL_WINWINDOWMGR
#define INCL_DOSPROCESS
#define INCL_WINPOINTERS
#define INCL_WININPUT
#define INCL_DOSFILEMGR
#define INCL_WINPROGRAMLIST

#include <os2.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "message.h"
#include "zipcntr.h"

   zipcntr::zipcntr (HWND notify, HWND container, HWND statusbar, PSZ tempdir, PSZ unzip)
   {
      // Put up the hourglass cursor.

      WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP, SPTR_WAIT, FALSE));

      hwndcontainer = container;
      hwndnotify = notify;
      hwndstatusbar = statusbar;
      pszTempdir = tempdir;
      pszUnzip = unzip;
      lenZipcomment = 0;

      // Determine the size of the extra storage needed for the CNRUNIT structure.

      extrabytes = sizeof (CNRUNIT)-sizeof (RECORDCORE);

      // Memory allocation for these pointers is handled by the CM_ALLOCRECORD message

      pcnrItem = NULL;
      pcnrFirst = NULL;
      pcnrtemp = NULL;

      // The CNRINFO structure contains information about the container.  It should be
      // cleared first and then the required fields reset.  It is updated via a CM_SETCNRINFO
      // message which also indicates which fields to change with the CMA_* attributes.

      memset (&cnri, 0, sizeof (CNRINFO));

      cnri.cb = sizeof (CNRINFO);
      cnri.pSortRecord = NULL;
      cnri.flWindowAttr = CV_DETAIL | CA_DETAILSVIEWTITLES;

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

   zipcntr::~zipcntr()
   {
      // Don't really need to delete the records here because the container is only used once, then
      // it is deleted so the records will go with it.
      //      WinSendMsg (hwndcontainer, CM_REMOVEDETAILFIELDINFO, NULL,
      //                                                            MPFROM2SHORT( 0, CMA_FREE ) );
      //      WinSendMsg (hwndcontainer, CM_REMOVERECORD, NULL, MPFROM2SHORT( 0, CMA_FREE ) );

      if (lenZipcomment)
      {
         delete pszZipcomment;
      }

      //This delete is redundant but should help to keep the file removed in case of
      // operator error eg. quitting File Freedom without closing viewer.

      strcpy (szParms, pszTempdir);
      strcat (szParms, "\\ziptemp");
      rc=DosDelete (szParms);
   }

   //-------------------------------------------------------------------------
   // This method makes a verbose unzip of the archive into a temporary file.
   // Since WinStartApp is used to do this, no further processing can be done until a notification
   // (WM_APPTERMINATENOTIFY) is sent to the owner window telling it that the WinStartApp
   // process is complete.
   // The owner procedure must then call 'FillContainer ()' (below) twice to complete the parsing
   // of file names and comments and stuffing them into the container as records.
   //-------------------------------------------------------------------------

   BOOL zipcntr::Unziptofile (PSZ pszFilename)
   {
      // Put up the hourglass cursor.

      WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP, SPTR_WAIT, FALSE));

      strcpy (szFilename, pszFilename);

      // Use WinStartApp to unzip the zip file to a file (use redirection).
      // This file will then be parsed to extract the file names and column headers
      // etc. which will then be inserted into the container.

      pDetails = new PROGDETAILS;

      pDetails->Length          = sizeof (PROGDETAILS);
      pDetails->progt.progc     = PROG_DEFAULT;
      pDetails->progt.fbVisible = SHE_INVISIBLE;
      pDetails->pszTitle        = NULL;
      pDetails->pszExecutable   = "cmd.exe";
      pDetails->pszParameters   = NULL;
      pDetails->pszStartupDir   = NULL;
      pDetails->pszIcon         = NULL;
      pDetails->pszEnvironment  = NULL;
      pDetails->swpInitial.fl   = SWP_HIDE;
      pDetails->swpInitial.cy   = 0;    /* width of window */
      pDetails->swpInitial.cx   = 0;    /* height of window */
      pDetails->swpInitial.y    = 0;    /* lower edge of window */
      pDetails->swpInitial.x    = 0;    /* left edge of window */
      pDetails->swpInitial.hwndInsertBehind = NULL;
      pDetails->swpInitial.hwnd             = hwndnotify;
      pDetails->swpInitial.ulReserved1      = 0;
      pDetails->swpInitial.ulReserved2      = 0;

      // When the parameter string is complete, it will look like
      //        "-vq "zipname""
      // and the program name is "unzip.exe".

      strcpy (szParms, pszTempdir);
      strcat (szParms, "\\ziptemp");

      rc = DosOpen (szParms, &hFile, &Action, NULL, NULL, OPEN_ACTION_CREATE_IF_NEW |
                  OPEN_ACTION_REPLACE_IF_EXISTS,
                  OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE, NULL);

      // If error opening "ziptemp" file, put up message.

      if (rc != 0)
      {
         WinMessageBox (HWND_DESKTOP, hwndcontainer, "Can't open temporary file\n",
                                           "ERROR!", NULL, MB_OK | MB_ERROR);
         return TRUE;
      }
      // Set up to redirect stdout to the temporary file "\path\ziptemp".
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

      hStdout = 1;
      hTemp = -1;
      DosDupHandle (hStdout, &hTemp);
      hStdout = 1;
      DosDupHandle (hFile, &hStdout);
      DosClose (hFile);

      strcpy (szParms, " -vq \"");
      strcat (szParms, szFilename);
      strcat (szParms, "\"");
      pDetails->pszExecutable  = pszUnzip;
      stage = 1;

      // Do the first write of the zip verbose listing to a file.  'Stage' is used to
      // distinguish whether we have reached the first or the second writing of the
      // ziptemp file (needed in the FillContainer() method below).

      errorflag = 0;
      happ = WinStartApp (hwndnotify, pDetails, szParms, NULL, SAF_STARTCHILDAPP);

      hStdout = 1;
      DosDupHandle (hTemp, &hStdout);
      if (!happ) return TRUE;

      return FALSE;
   }

  //-----------------------------------------------------------------------
  // This method must be called twice in sucession after the 'Unziptofile' method is complete
  // (signalled by a WM_APPTERMINATENOTIFY message sent to the owner window procedure).
  // After the first stage is complete another WM_APPTERMINATENOTIFY message is sent so
  // that the same procedure that triggers stage one can also trigger stage two.
  // The 'stage' variable allows this method to determine which part of the process is current.
  //-----------------------------------------------------------------------

   BOOL zipcntr::FillContainer ()
   {
      // The first part of this routine is common to both the first and second stage.
      // The rest of the routine is divided into two stages.  Each of these stages creates
      // a temporary file which is a verbose listing of the archive.  The difference is that
      // in the first stage, the comments are excluded from the file, while in the second stage,
      // the archive comment plus comments for each individual file (if present) are included.
      // This is not really necessary, but it makes it easier to extract the file names etc from
      // the file.  This is because the comments are always an unknown quantity - they may
      // or may not be present - and the first verbose listing without comments can be used
      // as a baseline to distinguish comments from other information in the second listing.

      // Now that a "ziptemp" file has been created, 1) open the file 2) read it into a buffer
      // 3) parse it for header names, file names, file count etc.

      // Put up the hourglass pointer.

      WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP,
                                                                                 SPTR_WAIT, FALSE));

      Newpointer = 0;
      tempsize = 0;

      strcpy (szParms, pszTempdir);
      strcat (szParms, "\\ziptemp");

      rc = DosOpen (szParms, &hFile, &Action, NULL, NULL, OPEN_ACTION_FAIL_IF_NEW |
                  OPEN_ACTION_OPEN_IF_EXISTS,
                  OPEN_SHARE_DENYNONE | OPEN_ACCESS_READONLY, NULL);

      // If error opening "ziptemp" file, put up message.

      if (rc != 0)
      {
         WinMessageBox (HWND_DESKTOP, hwndcontainer, "Can't open temporary file\n",
                                           "ERROR!", NULL, MB_OK | MB_ERROR);
         return TRUE;
      }

      // If "ziptemp" opened okay, get the file size, create a large enough buffer and then
      // read the file into the buffer.

      else
      {
         DosSetFilePtr (hFile, 0, FILE_END, &Newpointer);
         pszFilebuffer = new CHAR [Newpointer + 1];
         tempsize = Newpointer;
         DosSetFilePtr (hFile, 0, FILE_BEGIN, &Newpointer);
         DosRead (hFile, pszFilebuffer, tempsize, &Newpointer);
         DosClose (hFile);

         strcpy (szParms, pszTempdir);
         strcat (szParms, "\\ziptemp");
         rc=DosDelete (szParms);
      }

      // In this first stage, do this.

      if (stage == 1)
      {
         // Since the first verbose listing used 'q' (-vq) there will be no file title or comments.
         // Scan for the first Carriage Return/Linefeed character set to indicate the end of the
         // Line that holds the column headers.
         // After this line comes a line of dashes which can be thrown away.  This line is then
         // followed by the file data as illustrated below.  The columns do line up if viewed
         // with a monospaced font - particularily, the file names begin in the same column
         // as the column heading "Name".

         //
         // Length  Method   Size  Ratio   Date    Time   CRC-32     Name
         // ------  ------   ----  -----   ----    ----   ------     ----
         //   9142  Defl:N    3646  60%  07-31-95  18:36  77d51bd4   COPYING
         //    327  Defl:N     230  30%  05-01-96  22:07  ffcb488e   file_id.diz
         //   3742  Defl:N    1587  58%  04-29-96  16:51  d6badc05   funzip.doc
         //   2590  Defl:N     942  64%  06-14-96  10:31  8f94113b   install.cmd
         //1608751  Defl:N  650327  60%  12-21-92  18:57  343ee517   CORELDRW.EXE
         // ------          ------  ---                              -------
         //1624552          656732  60%                              5 files
         //
         //1 File has 300 bytes of OS/2 extended attributes attached.

         // First, capture the column headings in a string for use later (in SetColumns).
         // Trim off the last part of the header that says 'Name'.

         // Note: If there is an error in the zipfile, one of two things can happen.
         //    1) An error message is generated that only goes to the screen and a 'ziptemp'
         //        file of '0' bytes is created.
         //     2) An error message is generated that only goes to the screen but a 'ziptemp'
         //        file is also created that has the same information in it that would be found
         //        should the zip file be error free.

         if (tempsize > 4)
         {
            for (i = 0; i < tempsize; i++)
            {
               if (pszFilebuffer [i] == 13)
                  if (pszFilebuffer [i + 1] == 10)
                  {
                     memcpy (szHeadings, pszFilebuffer, i - 4);     // Trim back four characters.
                     szHeadings [i - 4] = 0;
                     ii = i + 2;
                     i = tempsize;
                  }
            }
         }

         // Save the column number of the beginning of "Name" for determining the start of
         // filenames.

         namecolumn = USHORT (ii - 6);

         // Set up to test for unzip error (not a zip, bad zip etc.).

         errorflag = 0;
         if (!tempsize) errorflag++;

         // Skip over the line of dashes unless the file has ended prematurely ie. there
         // was a write or zip file error.

         if (!errorflag)
         {
            for (i = ii; i < tempsize; i++)
            {
               // If there is not a line of intermittent dashes under the first line (which should
               // of been a line of headers) it is probably an error message.

               if (pszFilebuffer [i] != 32 && pszFilebuffer [i] != 45)
               {
                  if (pszFilebuffer [i] != 13 && pszFilebuffer [i] != 10) errorflag++;
               }
               if (pszFilebuffer [i] == 13)
                  if (pszFilebuffer [i + 1] == 10)
                  {
                     ii = i + 2;
                     placeholder = ii;
                     i = tempsize;
                  }
            }
         }

         // If it was a verbose error message, save it as a comment and display it.

         if (errorflag)
         {
            WinMessageBox (HWND_DESKTOP, hwndcontainer, "Not a proper zip file or error"
                                          "\nin zip file.  Advise testing zip file.",
                                           "Zip File Error!", NULL, MB_OK | MB_NOICON);
            return 1;
         }


         // Count the number of files.  This is determined by counting the number of cr/lf pairs
         // remaining in the file before the last line of dashes is reached.

         filecount = 0;
         int columnx = 0;
         for (i = ii; i < tempsize; i++)
         {
            // The last line of dashes is identified by the second column being a dash (if it
            // were a file line, it would have to start with a number).

            if (pszFilebuffer [i] == 45)
               if (columnx == 1) i = tempsize;   // breakout of loop.

            columnx++;

            // If it's the end of a line, increment the file counter.

            if (pszFilebuffer [i] == 13)
               if (pszFilebuffer [i + 1] == 10)
               {
                  filecount++;
                  i++;
                  columnx = 0;
               }
         }

         // OK now to set up the columns for the detail display.

         SetColumns ();

         // OK now to fill in the records as far as possible before the buffer is refilled with
         // the verbose listing from which the comments will be extracted.

         ok = TRUE;

         pcnrItem = (PCNRUNIT) WinSendMsg (hwndcontainer, CM_ALLOCRECORD,
                                 MPFROMSHORT (extrabytes), MPFROMSHORT ((USHORT) filecount));

         if ( pcnrItem )
         {
            pcnrFirst = pcnrItem;
            FillInRecord ();
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
         // TRUE supplies the user with visual feedback if a large amount of
         // records are inserted.

         ri.pRecordParent = NULL;
         ri.cRecordsInsert = filecount;

         // This is a good place to save the information from the 'totals' line at the end
         // of the filebuffer.
         // First, skip over another line of dashes and measure the following line.

         int nextline = 0;
         for (ii = placeholder; ii < tempsize; ii++)
         {
            if (pszFilebuffer [ii] == 13)
            {
               if (pszFilebuffer [ii + 1] == 10)
               {
                  nextline++;
                  if (nextline == 2)
                  {
                     iii = ii;
                     ii = tempsize;
                  }
                  else placeholder = ii + 2;
               }
            }
         }

         // Copy and format the line following the last line of dashes to the footer string.
         // This is the line that contains the byte totals, compressed total, %, and files count.

         int index;
         index = 0;
         CHAR szFiles [12];

         // First, use our own files count for a files total and format it.

         szFooter [0] = 32;
         szFooter [1] = 0;
         itoa (filecount, szFiles, 10);
         strcat (szFooter, szFiles);
         strcat (szFooter, " Files with ");

         // Now, move the beginning of the total byte count number and copy it to the string.

         for (i = placeholder; i < iii; i++)
         {
            if (pszFilebuffer [i] != 32)
            {
               ii = i;
               i = iii;
            }
         }
         for (i = ii; i < iii; i++)
         {
            if (pszFilebuffer [i] != 32)
            {
               szFiles [index] = pszFilebuffer [i];
               index++;
            }
            else
            {
               ii = i;
               i = iii;
            }
         }
         szFiles [index] = 0;
         strcat (szFooter, szFiles);
         strcat (szFooter, " Bytes Compressed to ");
         index = 0;

         // Next, move to the compressed bytes total number and copy it to the string.

         for (i = ii; i < iii; i++)
         {
            if (pszFilebuffer [i] != 32)
            {
               ii = i;
               i = iii;
            }
         }
         for (i = ii; i < iii; i++)
         {
            if (pszFilebuffer [i] != 32)
            {
               szFiles [index] = pszFilebuffer [i];
               index++;
            }
            else
            {
               ii = i;
               i = iii;
            }
         }
         szFiles [index] = 0;
         strcat (szFooter, szFiles);
         strcat (szFooter, " Bytes (");
         index = 0;

         // Now, move to the next number which is the compression % and copy it to the string.

         for (i = ii; i < iii; i++)
         {
            if (pszFilebuffer [i] != 32)
            {
               ii = i;
               i = iii;
            }
         }
         for (i = ii; i < iii; i++)
         {
            if (pszFilebuffer [i] != 37)
            {
               szFiles [index] = pszFilebuffer [i];
               index++;
            }
            else
            {
               ii = i;
               i = iii;
            }
         }
         szFiles [index] = 0;
         strcat (szFooter, szFiles);
         strcat (szFooter, "%)");

         delete pszFilebuffer;

         // Now to refill the filebuffer with a verbose listing and extract the comments.  This is
         // tedious because there are two kinds of comments found in the zip.  There is a
         // possible multi-line comment for the complete archive and there are also possible
         // single line comments for each individual file in the archive.  The way that they
         // are interspersed between the file listings make them difficult to extract.  This will
         // be done in stage two.

         strcpy (szParms, pszTempdir);
         strcat (szParms, "\\ziptemp");

         rc = DosOpen (szParms, &hFile, &Action, NULL, NULL, OPEN_ACTION_CREATE_IF_NEW |
                     OPEN_ACTION_REPLACE_IF_EXISTS,
                     OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE, NULL);

         // If error opening "ziptemp" file, put up message.

         if (rc != 0)
         {
            WinMessageBox (HWND_DESKTOP, hwndcontainer, "Can't open temporary file\n",
                                              "ERROR!", NULL, MB_OK | MB_ERROR);
            return TRUE;
         }
         // Set up to redirect stdout to the temporary file "\path\ziptemp".
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

         hStdout = 1;
         hTemp = -1;
         DosDupHandle (hStdout, &hTemp);
         hStdout = 1;
         DosDupHandle (hFile, &hStdout);
         DosClose (hFile);

         strcpy (szParms, " -v \"");
         strcat (szParms, szFilename);
         strcat (szParms, "\"");
         pDetails->pszExecutable  = pszUnzip;
         stage = 2;

         happ = WinStartApp (hwndnotify, pDetails, szParms, NULL, SAF_STARTCHILDAPP);

         hStdout = 1;
         DosDupHandle (hTemp, &hStdout);
         if (!happ) return TRUE;

         return 0;
      }
      else if (stage == 2 && errorflag == 0)
      {

         // This is what the new buffer looks like.

         //Archive:  test.zip
         //This would be a multi-line comment which could go on and on for as many lines
         //of whatever length the user wanted.
         // Length  Method   Size  Ratio   Date    Time   CRC-32     Name
         // ------  ------   ----  -----   ----    ----   ------     ----
         //   9142  Defl:N    3646  60%  07-31-95  18:36  77d51bd4   COPYING
         //This would be a single line comment for the file COPYING.
         //    327  Defl:N     230  30%  05-01-96  22:07  ffcb488e   file_id.diz
         //This would be a single line comment for the file file_id.diz.
         //   3742  Defl:N    1587  58%  04-29-96  16:51  d6badc05   funzip.doc
         //This would be a single line comment for the file funzip.doc
         //   2590  Defl:N     942  64%  06-14-96  10:31  8f94113b   install.cmd
         //This would be a single line comment for the file install.cmd.
         //1608751  Defl:N  650327  60%  12-21-92  18:57  343ee517   CORELDRW.EXE
         //This would be a single line comment for the file CORELDRW.EXE
         // ------          ------  ---                              -------
         //1624552          656732  60%                              5 files
         //
         //1 File has 300 bytes of OS/2 extended attributes attached.

         // Start again by throwing away the first line - it's just a file title we don't need.

         for (i = 0; i < tempsize; i++)
         {
            if (pszFilebuffer [i] == 13)
               if (pszFilebuffer [i + 1] == 10)
               {
                  placeholder = i + 2;
                  i = tempsize;
               }
         }

         // Now, parse for the headings string.  Anything between the placeholder and the
         // headings string is a multi-line (possible) zipfile comment.

         int savepos;
         savepos = placeholder;
         psztemp = &pszFilebuffer [placeholder];
         for (i = placeholder; i < tempsize; i++)
         {
            if (pszFilebuffer [i] == 13)
               if (pszFilebuffer [i + 1] == 10)
               {
                  memcpy (szParms, psztemp, i - placeholder);
                  szParms [i - placeholder - 4] = 0;  // Trim back string 4 chars to match header.
                  result = (SHORT) stricmp (szParms, szHeadings);
                  if (!result)
                  {
                     // The headings string has been located so the comment (if it exists)
                     // is delineated by 'savepos' at the beginning and 'placeholder' at the end.

                     ii = i;
                     i = tempsize;
                  }
                  else
                  {
                     placeholder = i + 2;
                     psztemp = &pszFilebuffer [placeholder];
                  }
               }
         }

         // If a zipfile comment was found, create a string array and save it.

         lenZipcomment = placeholder - savepos;
         if (lenZipcomment)
         {
            psztemp = &pszFilebuffer [savepos];
            pszZipcomment = new CHAR [lenZipcomment + 1];
            memcpy (pszZipcomment, psztemp, lenZipcomment);
            pszZipcomment [lenZipcomment] = 0;

         }
         placeholder = ii + 2;

         // Continue again by throwing away the line that follows the headings string.
         // It's just another line of dashes.

         for (i = placeholder; i < tempsize; i++)
         {
            if (pszFilebuffer [i] == 13)
               if (pszFilebuffer [i + 1] == 10)
               {
                  placeholder = i + 2;
                  i = tempsize;
               }
         }

         // Now we begin to search for individual file (single line) comments.
         // Since we know how many file names there are in the buffer, we can use a loop
         // to scan for the file names until all possible file comments have been found.

         int notlineofdashes;
         pcnrItem = pcnrFirst;
         pcnrtemp = pcnrFirst;
         psztemp = &pszFilebuffer [placeholder + namecolumn];

         for (i = 0; i < filecount; i++)
         {
            for (ii = placeholder; ii < tempsize; ii++)
            {
               if (pszFilebuffer [ii] == 13)
                  if (pszFilebuffer [ii + 1] == 10)
                  {
                     // This is an exclusion for the last record (pcnrItem = Null).  It must be
                     // handled differently since the string following the file data will never
                     // match a following record (there isn't one) so we don't do a compare here
                     // but will check it for dashes and spaces below.

                     if (!pcnrItem)
                     {
                        psztemp = &pszFilebuffer [placeholder];
                        memcpy (szParms, psztemp, ii - placeholder);
                        szParms [ii - placeholder] = 0;
                        result = 1;
                     }

                     // If the line is shorter than the column that a name would start in, it
                     // must be a comment so no need to compare the string to a name.

                     else if (ii - placeholder > namecolumn - 1)
                     {
                        memcpy (szParms, psztemp, ii - placeholder - namecolumn);
                        szParms [ii - placeholder - namecolumn] = 0;
                        result = (SHORT) stricmp (szParms, pcnrItem->szName);
                     }
                     else result = 1;

                     // If a match is not found, that means it's a comment for the previous file.

                     if (result != 0)
                     {
                        // Copy the comment into the appropriate record structure.

                        notlineofdashes = 1;
                        if (!pcnrItem)
                        {
                           notlineofdashes = 0;
                           int len = strlen (szParms);
                           for (i = 0; i < len; i++)
                              if (szParms [i] != 32 && szParms [i] != 45) notlineofdashes++;
                           i = filecount;  // Set these two variables to bread out of the loop.
                           if (notlineofdashes == 0) ii = tempsize;
                        }
                        if (notlineofdashes)
                        {
                           psztemp = &pszFilebuffer [placeholder];
                           memcpy (pcnrtemp->szComment, psztemp, ii - placeholder);
                           pcnrtemp->szComment [ii - placeholder] = 0;
                           placeholder = ii + 2;
                           if (!pcnrItem) ii = tempsize;  // Set variable to break out of loop;
                        }
                     }
                     else
                     {
                        placeholder = ii + 2;
                        pcnrtemp = pcnrItem;
                        pcnrItem = (PCNRUNIT) pcnrItem->rc.preccNextRecord;

                        // This line will allow the loop to continue one more time if there are
                        // no more records (i will be equal to filecount) in order to check
                        // the possibility of a comment for the last record.

                        if (pcnrItem) ii = tempsize;
                     }
                     psztemp = &pszFilebuffer [placeholder + namecolumn];
                  }
            }
         }

         // All of the fields of the CNRUNIT structure should be filled in now and the
         // records can now be inserted.

         ok =  (BOOL) WinSendMsg (hwndcontainer, CM_INSERTRECORD, MPFROMP (pcnrFirst),
                           MPFROMP (&ri ));

         // Clean up the allocated memory.

         delete pDetails;
         delete pszFilebuffer;

         // Now that the records are inserted, sort them by name.

         WinSendMsg (hwndcontainer, CM_SORTRECORD, MPFROMP (SortByName) , NULL);

         WinSendMsg (hwndcontainer, CM_INVALIDATERECORD, NULL ,
                                                           MPFROM2SHORT (NULL, CMA_REPOSITION));

         // Scroll to the head of the file.  With the newly sorted directories on fat partitions,
         // the cursor will rest on the oldest file in the directory otherwise.  This may be
         // nowhere near the head of the directory.

         WinSendMsg (hwndcontainer, WM_CHAR, MPFROMSH2CH (KC_VIRTUALKEY, 0, 0),
                               MPFROM2SHORT (0, VK_HOME));

         // Fill in the status bar info

         WinSetWindowText (hwndstatusbar, szFooter);

         // Display the comment if there is one.

         if (lenZipcomment)
         {
            zipmessage = new message (pszZipcomment, "Zip File Comment", hwndnotify);
            delete zipmessage;
         }
      }
      return 0;
   }
   // The setting up of the Details field info via the FIELDINFO and FIELDINFOINSERT structures
   // need only be done once since the information in them is not file dependent.

   void zipcntr::SetColumns ()
   {
      // Put up the hourglass cursor.

      WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP, SPTR_WAIT, FALSE));

      // Allocate storage for container column data using three columns

      pfinfo = (PFIELDINFO) WinSendMsg( hwndcontainer, CM_ALLOCDETAILFIELDINFO,
                                                      MPFROMLONG (3), NULL );
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
         // using the pszField, pszName and pszComments variable rather than the 'szXxxx'
         // form.  We do this because the container needs a pointer to the file name. If we used
         // szName (a character array, not a pointer), the container would
         // take the first 4 bytes of szName and think it was a pointer,
         // which of course it is not. Later in the FillInRecord function we set
         // pszName to point to szName.

         // Fill in column information for the Headings column.

         pfinfo->flData     = CFA_STRING | CFA_LEFT | CFA_HORZSEPARATOR;
         pfinfo->flTitle    = CFA_LEFT | CFA_FITITLEREADONLY;
         pfinfo->pTitleData = "Name";
         pfinfo->offStruct  = FIELDOFFSET (CNRUNIT, pszName);
         pfinfo->cxWidth = 0;

         // NOTE: The FIELDOFFSET (type, field) is defined in OS2DEF.H and can be used
         //         with any structure.

         // Fill in column information for the file Name.

         pfinfo = pfinfo->pNextFieldInfo;
         pfinfo->flData     = CFA_STRING | CFA_LEFT | CFA_HORZSEPARATOR;
         pfinfo->flTitle    = CFA_LEFT | CFA_FITITLEREADONLY;
         pfinfo->pTitleData = szHeadings ;
         pfinfo->offStruct  = FIELDOFFSET (CNRUNIT, pszField);
         pfinfo->cxWidth = 0;

         // Fill in column information for the file Comments.

         pfinfo = pfinfo->pNextFieldInfo;
         pfinfo->flData     = CFA_STRING | CFA_LEFT | CFA_HORZSEPARATOR;
         pfinfo->flTitle    = CFA_LEFT | CFA_FITITLEREADONLY;
         pfinfo->pTitleData = "Comments";
         pfinfo->offStruct  = FIELDOFFSET (CNRUNIT, pszComment);
         pfinfo->cxWidth = 0;


         // Set up the (all) static members of the FIELDINFOINSERT structure.
         // No need to zero out since they are all used each insert.

         finfoinsert.cb = sizeof (FIELDINFOINSERT);     /* Structure size  */
         finfoinsert.pFieldInfoOrder = (PFIELDINFO) CMA_FIRST;       /* Column order  */
         finfoinsert.fInvalidateFieldInfo = TRUE;         /* Update flag  */
         finfoinsert.cFieldInfoInsert = 3;       /* Number of columns  */


         // Use the CM_INSERTDETAILFIELDINFO message to tell the container
         // all the column information it needs to function properly. Place
         // this column info first in the column list and update the display
         // after they are inserted (fInvalidateFieldInfo = TRUE)

         WinSendMsg( hwndcontainer, CM_INSERTDETAILFIELDINFO, MPFROMP( pfinfoFirst ),
                          MPFROMP( &finfoinsert ));
      }
   }


   BOOL zipcntr::FillInRecord ()
   {
      // Put up the hourglass cursor.

      WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP, SPTR_WAIT, FALSE));

      // The CNRUNIT structure is a devised structure to hold extra information that
      // supplements the info in the RECORDCORE or MINIRECORDCORE structures.
      // It must also contain the RECORDCORE or MINIRECORDCORE structure itself.
      // The structure is up to the programmer to design with fields as required.
      // Fill in the values of CNRUNIT and RECORDCORE except for the pcnrItem->szComment.
      // field which will be filled in after the second verbose listing is parsed.

      for (i = 0; i < filecount; i++)
      {
         if (pcnrItem)
         {
            pcnrItem->pszField = pcnrItem->szField;
            pcnrItem->pszName = pcnrItem->szName;
            pcnrItem->pszComment = pcnrItem->szComment;

            psztemp = &pszFilebuffer [placeholder];
            memcpy (pcnrItem->szField, psztemp, namecolumn);
            pcnrItem->szField [namecolumn] = 0;

            psztemp = &pszFilebuffer [placeholder + namecolumn];
            for (ii = placeholder; ii < placeholder + 256; ii++)
            {
               if (pszFilebuffer [ii] == 13)
               {
                  if (pszFilebuffer [ii + 1] == 10)
                  {
                     iii = ii;
                     ii = placeholder + 256;     // breakout of loop
                  }
               }
            }
            memcpy (pcnrItem->szName, psztemp, iii - placeholder - namecolumn);
            pcnrItem->szName [iii - placeholder - namecolumn] = 0;
            placeholder = iii + 2;

            // Fill in all the neccessary fields of the RECORDCORE structure. Note that the .cb
            // field of the RECORDCORE struct was filled in by CM_ALLOCRECORD.

            pcnrItem->rc.flRecordAttr = CRA_RECORDREADONLY | CRA_COLLAPSED;

            // Set up the address of the next record structure.

            pcnrItem = (PCNRUNIT) pcnrItem->rc.preccNextRecord;
         }
      }
      return TRUE;
   }
//----------------------------------------------------------------------
// This sort function is necessary even with HPFS (which is pre-sorted in the directory
// structure itself) in order to put the subdirectories first in the listing and also for
// FAT directory structures.  This function sorts by name.
//----------------------------------------------------------------------

   static SHORT __syscall zipcntr::SortByName (PCNRUNIT p1, PCNRUNIT p2)
   {
      SHORT Sresult;

      // Use the case insensitive 'stricmp' here

      Sresult = (SHORT) stricmp (p1->szName, p2->szName);

      return Sresult;
   }

//----------------------------------------------------
// A method to call the protected 'SortByxxxx' method.
//----------------------------------------------------
   zipcntr::NameSort ()
   {
      WinSendMsg (hwndcontainer, CM_SORTRECORD, MPFROMP (SortByName) , NULL);
   }
//-----------------------------------------------------------------
// This method empties the container.
// It removes the records and removes the fieldinfo structures tied to each record.
//-----------------------------------------------------------------
   zipcntr::EmptyContainer ()
   {
      WinSendMsg (hwndcontainer, CM_REMOVEDETAILFIELDINFO, NULL,
                                                            MPFROM2SHORT( 0, CMA_FREE ) );
      WinSendMsg (hwndcontainer, CM_REMOVERECORD, NULL, MPFROM2SHORT( 0, CMA_FREE ) );
   }

//---------------------------------------------------------------
// This method will redisplay the zip file message if there was one.
//---------------------------------------------------------------
   zipcntr::RedisplayMessage ()
   {
      if (lenZipcomment)
      {
         zipmessage = new message (pszZipcomment, "Zip File Comment", hwndnotify);
         delete zipmessage;
      }
      else
      {
         zipmessage = new message ("There is no comment for this zip file!", "Zip File Comment",
                                  hwndnotify);
         delete zipmessage;
      }
   }
