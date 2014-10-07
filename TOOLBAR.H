/*---------------------------------------------------------
       toolbar.h -- a Toolbar window implementation
                                (c) Bruce Henry 1996

 NOTE: The pushbuttons in this object will automatically use the next
         49 numbers in the series of ID numbers assigned to this Frame
         window. eg. If this object is assigned ID#400, it will also use 401
         and 402 and so on as ID#s for the pushbuttons.  It will also use the
         next 50 ID numbers following 449 if Icons are used on the pushbuttons.
         If Icons are not used, those IDs will be free.
  ---------------------------------------------------------*/
#ifndef __toolbar__
#define __toolbar__

     //----------------------------------------------------------------------
     //A pointer to this structure is returned by the resize() method so that windows with two
     //dynamic dimension can get the static dimension of a window with only one dynamic
     //dimension and one static one like this toolbar window.  This can also be used when the
     //orientation of the window is changed from a vertical toolbar (horizontal dimension is static)
     //to a horizontal toolbar (vertical dimension is static).
     //----------------------------------------------------------------------
typedef struct _STATICDIM
     {
        INT vert;       // Vertical static size - 0=vertical dimension is dynamic.
        INT horz;      // Horizontal static size - 0=horizontal dimension is dynamic.
      } STATICDIM, *PSTATICDIM;

     //---------------------------------------------------------------------
     // The 'BTNCDATA' structure declated in pmwin.h (INCL_WINBUTTONS) would not work
     // here.  I kept getting a 'requires declaration specifiers' (error #336) error from the
     // compiler so I cloned it here.  It is used with the 'WinCreateWindow' function to
     // create a pushbutton control with an icon.
     //---------------------------------------------------------------------
typedef struct _MYBTNCDATA    /* bdata */
     {
        USHORT  cb;
        USHORT  fsCheckState;
        USHORT  fsHiliteState;
        LHANDLE hImage;
     } MYBTNCDATA, *PMYBTNCDATA;

      //-----------------------------------------------------------------------
      //This structure consolidates some of the info needed for the constructor.  These could all
      //be passed as separate parameters, but this will make it a little more organized.
      //-----------------------------------------------------------------------
typedef struct _BTNINFO   /* binfo */
      {
         USHORT btncx;         // The button dimension cx (width)
         USHORT btncy;         // The button dimension cy (height)
         ULONG flBstyle;        // A style flag
         SHORT brows;          // Rows of buttons
         SHORT bcolumns;       // Columns of buttons
         SHORT totalbuttons;    // in case there is an odd number.
         LONG reservecx;
         LONG reservecy;
         USHORT reserverows;
         USHORT reservecolumns;
         PCHAR label;            // Pointer to an array of labels for pushbutton (if text buttons)
                                   // Can be NULL if icons are used.
         USHORT labelsize;      // Number of chars in each label including zero terminator
                                   // Can be NULL if icons are used.
      } BTNINFO, *PBTNINFO;   // The number of labels will be brows x bcolumns


class toolbar
{
   protected:

      HWND        hwndChildWindow;      // This will hold the handle to the container parent
      HWND        hwndPushB[48];        // An array of handles for the pushbuttons (possible 48)
      SHORT        buttonrows;           // A count of rows of buttons
      SHORT        buttoncols;              // A count of the columns of buttons
      LONG        totalbuttons;           // The product of buttoncols*buttonrows
      HWND        hwndClient ;            //Handle to Client window
      LONG         width;                 // this will hold the initial width of the window
      LONG         height;                // this will hold the initial height of the window
      LONG         holdcx;               // An amount of space to reserve for another toolbar etc.
      LONG         holdcy;               // An amount of space to reserve for anothe toolbar etc.
      SHORT      buttoncx;            // Dimensions of the buttons in pixels
      SHORT      buttoncy;
      SWP         dirswp;               // Create SWP structure for window position info
      FRAMECDATA  fcdata;         // Frame control data structure
      STATICDIM    sdim;           // A structure to hold the static dimension of the toolbar
      HPOINTER  hicon;             // A handle for icons
      MYBTNCDATA   bdata;        // A structure to hold pushbutton control data
      PBTNINFO  pInfo;             // reference to the structure passed in the constructor.

      /****Protected Methods***************************/

      //--------------------------------------------------------------------
      // This method to destroy the window will be used by the destructor.
      //--------------------------------------------------------------------
      void DestroyWindow () ;


   public:
      /******Public Methods ***************************/

      /* First, a constructor to Register the Class and build the window */
          //----------------------------------------------------------------
          //-The first param holds the handle of the client window
          //-The second param holds the unique id.  The sub windows in this class (the
          // Pushbuttons) will automatically use the next numbers in the series so keep
          // them reserved.
          //-The third param contains a BTNINFO pointer that contains the following:
          //              -The size of the buttons
          //              -The style flags
          //              -The number of columns and rows of buttons
          //              -A pointer to labels
          //              -The length of each zero terminated segment in the label array
          //    Note:  The rows x columns will reveal the number of buttons.
          //
          //----------------------------------------------------------------
      toolbar (HWND hwndCLIENT, ULONG idCHILD, PBTNINFO pbinfo);

      /* A method to get the handle of the Frame */
      HWND getToolbarHWND();

      /* A method to return the static dimensions of the toolbar. */
      //----------------------------------------------------------------------
      //The static dimension will always be the smallest dimension.  In the other dimension
      //the toolbar will stretch with the window.  Therefore this method will return '0' for
      //the dynamic dimension and a number for the static dimension.
      //The first parameter is the number of button rows.
      //The second parameter is the number of button columns.
      //----------------------------------------------------------------------
      PSTATICDIM rejig( SHORT brows, SHORT bcolumns);

      /* A method to resize the window parts to fit a new client size */
      //-----------------------------------------------------------------
      //This method returns the same pointer to vertical and horizontal static dimensions
      //as the above method.  One of the dimensions will always zero.
      //It also resizes the toolbar window to fit the new shape when the client window is
      //resized.
      //-----------------------------------------------------------------
      PSTATICDIM resizewin();

//-------------
//void beep();
//-------------

      /* The required destructor */
     ~toolbar();
     };
#endif

