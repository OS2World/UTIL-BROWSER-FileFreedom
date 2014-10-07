/*---------------------------------------------------------
       message.h -- a window to display a scrollable message
                                (c) Bruce Henry 1996

  -----------------------------------------------------------*/
#ifndef __messagewindow__
#define __messagewindow__

#define ID_WINDOW   100

typedef struct _MESSAGEDATA
         {
            HAB  hab;				
            CHAR szString[256];		
            PSZ  pszErrMsg;
            HWND hwndClient;
            HWND hwndFrame;
            HWND hwndMLE;
            SWP swp;
            HPS    hps;			
            RECTL  rcl;			
            QMSG qmsg;				
            ULONG flCreate;			
         } MESSAGEDATA, *PMESSAGEDATA;


class message
{
   protected:

      MESSAGEDATA md;
      PMESSAGEDATA pmd;

      /****Protected Methods***************************/

      //------------------------------------------------------------------
      // A main window procedure
      //------------------------------------------------------------------
      static MRESULT EXPENTRY MainProc (HWND hwnd, ULONG msg, MPARAM mp1,
                                                                                        MPARAM mp2);


   public:

      /******Public Methods ***************************/

      /* First, a constructor to build the window */

      message (PSZ pszMessage, PSZ pszTitle, HWND owner);

      //------------------------
      // The required destructor
      //------------------------
      ~message ();

      //--------------------------------------------------
      // A Method to return the HWND of the window frame.
      //--------------------------------------------------
      HWND GetHwnd ();

     };
#endif


