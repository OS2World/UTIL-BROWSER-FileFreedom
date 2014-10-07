 /*---------------------------------------------------------------------
                    ROOTS.H   -  A class to create a root tree in a container window.

                    The container handle and the drive must be handed to this object.
                    IDs 298 and 299 are reserved for this class for the expanded and
                    collapsed folder icons.

                                    (C) Bruce Henry  1996
------------------------------------------------------------------------*/

#ifndef _roottree_
#define _roottree_

#include "cnrstuff.h"

class roottree
{
protected:

   CNRINFO cnri;
   RECORDINSERT ri;
   HWND hwndcontainer;
   HAB   hab;
   HMQ   hmq;
   TID   tid;
   FILEFINDBUF3  *pffb;
   PFILEFINDBUF3  pffbFile;
   PCNRITEM    pcnrParent;    // Pointers for CNRITEM created above with a typedef.
   PCNRITEM    pcnrGrandParent;
   PCNRITEM    pcnrItem;
   PCNRITEM    pcnrFirst;
   SHORT extrabytes;
   PSZ pszFilespec;
   PSZ pszDirspec;
   HDIR hdir;
   PCH pchEndpath;
   PCH pchEndname;
   ULONG maxfiles;
   APIRET rc;
   HPOINTER hicon, hExIcon, hCollIcon;
   SIZEL  sl;
   TREEITEMDESC *ptidesc;
   BOOL ok;
   USHORT newdriveflag;

   // The bitmap and pointer handles are used in the TREEITEMDESC structure which is required
   // for the tree view.

//------------------
//protected methods
//------------------

   BOOL InsertRecords ();

   BOOL FillInRecord ();

   //---------------------------------------------------------------------------
   // This sort routine needs both a 'static' and a __syscall modifier.  Without the static modifier
   // it will not compile and without the __syscall modifier, it will not return from the routine.
   // As far as I can tell, the STATIC modifier will cause this method to be shared among all
   // objects of this class.  I can't see this being a problem at this time.
   //---------------------------------------------------------------------------
   static SHORT __syscall SortByName (PCNRITEM p1, PCNRITEM p2);//, PVOID pStorage);


//----------------
//public methods
//----------------

public:

   roottree (HWND container, SHORT drive);

   ~roottree();

   //A method to populate the container
   FillContainer ();

   // A method to pass a record pointer to enable a new FillContaineer to be run
   // for a subdirectory
   TreetoOrder (PCNRITEM pRecord);

   // A method for removing records that are children of the record passed.
   // Normally used when a folder is collapsed so the records can be purged.
   // This will insure that the records are always up to date when a folder is opened.
   RemoveKids (PCNRITEM pRecord);

   // ------------------------------------------------------------
   // A method to remove all old records and refill from a new drive
   //-------------------------------------------------------------
   RenewCont ( SHORT drive);

};
#endif
