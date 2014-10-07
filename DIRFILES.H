 /*---------------------------------------------------------------------
                    DIRFILES.H   -  A class to create a directory listing of files
                                        in a container window.

                                    (C) Bruce Henry  1996
------------------------------------------------------------------------*/

#ifndef _dirfiles_
#define _dirfiles_

#include "cnrstuff.h"

class dirfiles
{
protected:

   CNRINFO cnri;
   RECORDINSERT ri;
   PFIELDINFO pfinfo;
   FIELDINFOINSERT finfoinsert;
   HWND hwndcontainer;
   HAB   hab;
   HMQ   hmq;
   TID   tid;
   FILEFINDBUF3  *pffb;
   PFILEFINDBUF3  pffbFile;
   PCNRITEM    pcnrItem;       // These structures contain the RECORDCORE structure
   PCNRITEM    pcnrFirst;
   PCNRITEM    pcnrParent;
   PFILESANDBYTES pFandB;     // PFILESANDBYTES typedef'd in cnrstuff.h.
   SHORT extrabytes;
   PSZ pszFilespec;
   PSZ pszDirspec;
   HDIR hdir;
   PCH pchEndpath;
   PCH pchEndname;
   ULONG maxfiles;
   APIRET rc;
   BOOL ok;
   USHORT sortflag;
   COUNTRYCODE Country;
   COUNTRYINFO CtryInfo;
   ULONG ulInfoLen;

   // The bitmap and pointer handles are used in the TREEITEMDESC structure which is required
   // for the tree view.

//------------------
//protected methods
//------------------
   void SetColumns ();

   BOOL InsertRecords ();

   BOOL FillInRecord ();

   //---------------------------------------------------------------------------
   // These sort routines needs both a 'static' and a __syscall modifier.  Without the static modifier
   // it will not compile and without the __syscall modifier, it will not return from the routine.
   // As far as I can tell, the STATIC modifier will cause this method to be shared among all
   // objects of this class.  I can't see this being a problem at this time.  I'm keeping them
   // private to minimize problems.
   //---------------------------------------------------------------------------
   static SHORT __syscall SortByName (PCNRITEM, PCNRITEM, PVOID);

   static SHORT __syscall SortByDate (PCNRITEM, PCNRITEM, PVOID);

   static SHORT __syscall SortBySize (PCNRITEM, PCNRITEM, PVOID);

   static SHORT __syscall SortByExtension (PCNRITEM, PCNRITEM, PVOID);

//----------------
//public methods
//----------------

public:

   dirfiles (HWND container, SHORT drive);

   ~dirfiles();

   //-----------------------------------------------------
   //A method to populate the container with the default drive files.
   //-----------------------------------------------------
   FillContainer (ULONG fileattributes);

   //-------------------------------------------------------------------------
   // A method to provide a new pathname for populating the container with files in a directory
   // path passed via a PSZ pointer.  The actual pathname is in a
   // PCNRITEM->szDirPath member.  The new default drive is also set.
   //-------------------------------------------------------------------------
   PathToOrder (PSZ pszPath, SHORT drive);

   //----------------------------------------------------------
   // A method to remove the records from the container before it is refilled.
   //----------------------------------------------------------
   EmptyContainer ();

   //-------------------------------------------------------------------
   // A method to return the cummulative count of the bytes and files in the container.
   //-------------------------------------------------------------------
   PFILESANDBYTES Getfilesandbytes ();

   //------------------------------------------------------------
   // A method to set the view of the container to the Name Flowed view.
   //------------------------------------------------------------
   SetNameView ();

   //------------------------------------------------------------
   // A method to set the view of the container to the Detail view.
   //------------------------------------------------------------
   SetDetailView ();

   //------------------------------------------------------------
   // A method to set the view of the container to the Text Flowed view.
   //------------------------------------------------------------
   SetFlowView ();

   //------------------------------------------------------------
   // A method to set the view of the container to the Text view.
   //------------------------------------------------------------
   SetTextView ();

   //------------------------------------------------------------
   // A method to enable the column headings in the detail view of the container.
   //------------------------------------------------------------
   SetColumnHeadings ();

   //-------------------------------------------------------------
   // Some methods to call the protected sort methods.
   //-------------------------------------------------------------
   NameSort (ULONG);

   DateSort (ULONG);

   SizeSort (ULONG);

   ExtensionSort (ULONG);
};
#endif
