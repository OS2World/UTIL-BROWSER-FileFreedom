/*---------------------------------------------------------
       contwind.h -- a Container window implementation
                                (c) Bruce Henry 1996

 NOTE: The two sub-windows in this object will automatically use the next
         two numbers in the series of ID numbers assigned to this Frame
         window. eg. If this object is assigned ID#400, it will also use 401
         and 402 as ID#s for the sub windows.  This may not be necessary
         since I haven't yet needed to use these IDs.
  -----------------------------------------------------------*/
#ifndef __contwindow__
#define __contwindow__

class contwindow
{
   protected:

     HWND        hwndChildWindow;      // This will hold the handle to the container parent
     HWND        hwndChildTitle;          // Static window that holds title of child window
     HWND         hwndContainer;       // This will hold the handle to the container.
     HWND        hwndClient ;               //Handle to Client window
     LONG         xorigin;               // x origin for frame
     LONG         yorigin;               // y origin for frame
     LONG         width;                 // this will hold the initial width of the window
     LONG         height;                // this will hold the initial height of the window
     LONG         startcx;               // this will hold the original width of the client window
     LONG         startcy;               // this will hold the original height of the client window
     SWP         dirswp;               // Create SWP structure for window position info
     PSZ         pszfontnamesize;  // Pointer to string to hold PresParm font

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
          //-The second param holds the unique id.  The sub windows in this class (the title,
          // the container) will automatically use the next two numbers in the series so keep
          // them reserved.
          //-The third param holds the 'x' origin for the new window.
          //-The fourth param holds the 'y' origin for the new window.
          //-The fifth param holds the required width of the new window.
          //-The six param holds the required height of the new window.
          //-The seventh param contains the original 'cx' width of the client window.
          //-The eighth param contains the original 'cy' height of the client window.
          //-The final param is the title that should appear along the top of the window.
          //----------------------------------------------------------------
      contwindow(HWND hwndCLIENT, ULONG idCHILD, LONG xorg, LONG yorg,
                    LONG xsize, LONG ysize, LONG cx, LONG cy, PSZ title);

      /* A method to get the handle of the Frame */
      HWND getContainerHWND();

      /* A method to get the handle of the Titlebar */
      HWND getTitlebarHWND();

      /* A method to reconfigure the window in a client that has remained static. */
      //---------------------------------------------------------------------
      //This is used when other windows within the client are either removed or have
      //changed their orientation.  This necessitates a change in the container's dimensions.
      //The parameters are the new width and height respectively.
      //---------------------------------------------------------------------
      VOID rejig( LONG xsize, LONG ysize);

      /* A method to resize the window parts to fit a new client size */
      //------------------------------------------------------------------------
      //The params passed are dimensions to reserve for horizontal or vertical static windows.
      //Those types of windows will only stretch in one dimension unlike this window which is
      //dynamic (resizeable) in both dimensions.
      //------------------------------------------------------------------------
      void resizewin (INT reservhorz, INT reservvert);

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


      /* The required destructor */
     ~contwindow();
     };
#endif





















