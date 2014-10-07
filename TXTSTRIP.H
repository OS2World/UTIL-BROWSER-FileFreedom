/*----------------------------------------------------------------------
       txtstrip.h -- a Static window implementation to provide an info text strip
                                (c) Bruce Henry 1996

       Note:  This object will provide one or two text strips depending on the constructor
               value sent.  If two are used, the next id in the series is used as the id for
               the second window. This allows both windows to be written to separately.
               The ids used are 598 and possibly 599.  These are hard-coded in and
               must be reserved.
  ------------------------------------------------------------------------*/
#ifndef __textstrip__
#define __textstrip__

//------------------------------------------------------------------
// A structure to hold information passed to the subclassed Frame procedure via the
// Window Words QWL_USER offset.
//------------------------------------------------------------------
typedef struct _STATICPARAMS
{
   PFNWP OldFrameProc;          // Pointer to the old procedure
   USHORT cWindows;             // Number of windows ( must be 1 or 2 only)

} STATICPARAMS, *PSTATICPARAMS;



class textstrip
{
   protected:

     HWND        hwndTextstrip1;          // Static window that holds title of child window
     HWND        hwndTextstrip2;
     HWND        hwndFrame ;               //Handle to Client window
     PSZ         pszfontnamesize;           // Pointer to string to hold PresParm font
     STATICPARAMS staticparams;

      /****Protected Methods***************************/

      //--------------------------------------------------------------------
      // This method to destroy the window will be used by the destructor.
      //--------------------------------------------------------------------
      void DestroyWindow () ;


   public:
      /******Public Methods ***************************/

          /* First, a constructor to build the window */
          //----------------------------------------------------------------
          //-The first param holds the handle of the client window
          //-The second param holds the owner of the window, usually the client window.
          //-The final param is the number of bars ( 1 or 2 ).
          //----------------------------------------------------------------
      textstrip (HWND hwndframe, HWND hwndOwner, USHORT nBars);

      //-------------------------------------------------------------------
      // It is necessary to subclass the Frame window procedure so that the status bar
      // can be made to be visible when the client area changes size.
      //-------------------------------------------------------------------
      static MRESULT EXPENTRY NewFrameProc (HWND hwnd, ULONG msg, MPARAM mp1,
                                                      MPARAM mp2);


      //----------------------------------------------------------
      // A method that will allow sticking in new text into the first status bar
      //----------------------------------------------------------
      void newtitle1 (PSZ pNewtext);

      //------------------------------------------------------------
      // A method that will allow sticking in new text into the second status bar
      //------------------------------------------------------------
      void newtitle2 (PSZ pNewtext);

      /* The required destructor */
      ~textstrip ();
     };
#endif

