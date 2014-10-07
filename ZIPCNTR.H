 /*---------------------------------------------------------------------
                    ZIPCNTR.H   -  A class to create a listing of files in a zip archive
                                        in a container window.

This will be done by using Unzip to create a verbose listing of a zip file which is captured
by a file tied to stdout.  This verbose listing is then parsed to extract the names of the
files in the archive which are inserted as records into the container handed to this object.

                                    (C) Bruce Henry  1996
------------------------------------------------------------------------*/

#ifndef _zipcontainer_
#define _zipcontainer_

typedef struct _CNRUNIT               // CONTAINER RECORD STRUCTURE
{
  RECORDCORE  rc;                  // Base information
  PSZ pszField;
  PSZ pszName;
  PSZ pszComment;
  CHAR szField     [100];
  CHAR szName     [CCHMAXPATH + 1];
  CHAR szComment [128];
} CNRUNIT, *PCNRUNIT;

//-----------------------------------------------------//

class zipcntr
{
protected:

   CNRINFO cnri;
   RECORDINSERT ri;
   PFIELDINFO pfinfo;
   FIELDINFOINSERT finfoinsert;
   HWND hwndcontainer;
   HWND hwndnotify;
   HWND hwndstatusbar;
   HAB   hab;
   HMQ   hmq;
   TID   tid;
   PCNRUNIT    pcnrItem;       // These structures contain the RECORDCORE structure
   PCNRUNIT    pcnrFirst;
   PCNRUNIT    pcnrtemp;
   SHORT extrabytes;
   HDIR hdir;
   ULONG filecount;
   APIRET rc;
   BOOL ok;
   CHAR szParms [CCHMAXPATH * 2];
   CHAR szFilename [CCHMAXPATH + 1];
   PSZ pszFilebuffer;
   PSZ pszTempdir;
   PSZ pszUnzip;             // Pointer to unzip.exe path/name
   PPROGDETAILS pDetails;
   HAPP happ;
   FILE *fp;
   HFILE hFile, hTemp, hStdout;
   ULONG Action;
   ULONG Newpointer;
   ULONG tempsize;
   SHORT result;
   CHAR szHeadings [100];
   USHORT stage;
   USHORT namecolumn;
   CHAR szFooter [100];
   ULONG lenZipcomment;
   PSZ pszZipcomment;
   ULONG placeholder;
   ULONG errorflag;
   message *zipmessage;
   HWND hwndmessage;
   PSZ psztemp;
   int i, ii, iii;


   // The bitmap and pointer handles are used in the TREEITEMDESC structure which is required
   // for the tree view.

//------------------
//protected methods
//------------------

   //-------------------------------------
   // A method to set up the details view
   //-------------------------------------
   void SetColumns ();

   //-------------------------------------------------------------------
   // A method used to assign values to the CNRUNIT structure members for each record.
   //-------------------------------------------------------------------
   BOOL FillInRecord ();

   //---------------------------------------------------------------------------
   // These sort routines needs both a 'static' and a __syscall modifier.  Without the static modifier
   // it will not compile and without the __syscall modifier, it will not return from the routine.
   // As far as I can tell, the STATIC modifier will cause this method to be shared among all
   // objects of this class.  I can't see this being a problem at this time.  I'm keeping them
   // private to minimize problems.
   //---------------------------------------------------------------------------
   static SHORT __syscall SortByName (PCNRUNIT p1, PCNRUNIT p2);


//----------------
//public methods
//----------------

public:

   // The constructor gets handles for the container and the frame.
   // The frame handle is needed so that when "unzip.exe" is loaded with "WinStartApp"
   // the window procedure can be sent a WM_APPTERMINATENOTIFY message.
   // Many of the processes must wait until this message is received.

   zipcntr (HWND notify, HWND container, HWND statusbar, PSZ tempdir, PSZ unzip);

   ~zipcntr ();

   //----------------------------------------------------
   // A method to get a verbose unzip into a temporary file.
   //----------------------------------------------------

   BOOL Unziptofile (PSZ pszFilename);

   //-----------------------------------------------------
   //A method to populate the container with the default drive files.
   //-----------------------------------------------------
   BOOL FillContainer ();

   //----------------------------------------------------------
   // A method to remove the records from the container before it is refilled.
   //----------------------------------------------------------
   EmptyContainer ();

   //-------------------------------------------------------------
   // Some methods to call the protected sort methods.
   //-------------------------------------------------------------
   NameSort ();

   //---------------------------------------------
   // A method to redisplay a zip file message
   //---------------------------------------------
   RedisplayMessage ();

};
#endif
