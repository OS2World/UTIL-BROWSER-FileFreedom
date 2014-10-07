/*---------------------------------------------------------
       stdwind.h -- a standard window implementation
         (c) Bruce Henry 1996

  ---------------------------------------------------------*/

#ifndef __stdwindow__
#define __stdwindow__


class stdwindow
{
   protected:

      //--------------------------------------------------------------------
      // HAB is a pointer (Anchor Block Handle) of typedef LHANDLE HAB
      //         LHandle is a typedef ULONG *LHANDLE
      //--------------------------------------------------------------------
     HAB          hab ;                      //Anchor Block Handle
     HWND        hwndFrame;      // This will hold the handle to the frame
     HWND        hwndClient ;               //Handle to Client window

      /****Protected Methods***************************/

      //--------------------------------------------------------------------
      // This method to destroy the window will be used by the destructor.
      //--------------------------------------------------------------------
      void DestroyStdWin () ;

   public:
      /******Public Methods ***************************/

       //-----------------------------------------------------------------
       //First, a constructor to Register the Class and build the window
       //-----------------------------------------------------------------
      stdwindow(HAB habmain, ULONG flFrameFlags, PSZ szClassName,
                              PFNWP ClientWndProc, PSZ szWindowTitle, USHORT id);

       //----------------------------------------------------
       //A method to return the handle to the Client window
       //----------------------------------------------------
      HWND getClientHWND();
       //-------------------------------------------
       //A method to get the handle of the Frame
       //-------------------------------------------
      HWND getFrameHWND();

       //---------------------------
       //The required destructor
       //---------------------------
     ~stdwindow();

     };
#endif
