/******       ffIDs.h    -      File Freedom IDs   ************/

#define OBJECTWINDOWID 50 // An Object window
#define OBJM_DROP 8192  // a User-defined message for the object window
#define OBJM_REFRESH 8193 // a User-defined message for the object window
#define HELPTABLEID 250  // Helptable id
#define DIRWINDOWID  200 // The next two numbers in this series are used automatically
                               //for sub-windows
#define FILESWINDOWID 300 // The next two numbers in this series are used automatically
                                // for the sub-windows (Static title and Container).
#define TOOLBARID 100
#define PBone  101  // Tool bar flip
#define PBtwo  102  // delete
#define PBthree   103  // copy
#define PBfour    104  // move
#define PBfive   105  // rename
#define PBsix   106  // attributes
#define PBseven  107  // mark all files
#define PBeight  108  // unmark all files
#define PBnine 109  // compress files
#define PBten 110  // uncompress files
#define PBeleven    111  // set destination
#define PBtwelve   112  // flip source and destination
#define PBthirteen    113  // edit file
#define PBfourteen   114  // append file
#define PBfifteen  115  // Call PMSEEK.EXE
#define PBsixteen  116 // Refresh files and tree displays
#define PBseventeen  117 // move separator right
#define PBeighteen  118 //move separator left
// The rest of the '100' series are reserved for icons and pushbuttons.

#define  DRIVEBARID 400
// All possible 26 drive IDs must be defined because it is unknown how many will exist.
#define pbA  401
#define pbB  402
#define pbC  403
#define pbD  404
#define pbE  405
#define pbF  406
#define pbG  407
#define pbH  408
#define pbI  409
#define pbJ  410
#define pbK  411
#define pbL  412
#define pbM  413
#define pbN  414
#define pbO  415
#define pbP  416
#define pbQ  417
#define pbR  418
#define pbS  419
#define pbT  420
#define pbU  421
#define pbV  422
#define pbW  423
#define pbX  424
#define pbY  425
#define pbZ  426

// The rest of the '400' series are reserved for icons and pushbuttons.

#define STATUSBAR1ID 598        // textstrip object id.
#define STATUSBAR2ID 599        // These are hard-coded to the object and are only
                                       // entered here for reference.
#define STDWINDOWID 1000     // This id identifies the main window which is referenced
                                    // by various other resources, eg. the menu, accelerator table etc.

#define  MID_FILE            4000
#define  MID_FILECOPY      4001
#define  MID_APPEND        4011
#define  MID_FILEMOVE      4002
#define  MID_FILEDELETE    4003
#define  MID_FILERENAME    4004
#define  MID_FILEATTR       4006
#define  MID_MARKALL       4007
#define  MID_UNMARKALL    4008
#define  MID_REFRESH       4009
#define  MID_QUIT            4010
#define  MID_DRIVES         4100
#define  MID_DRIVENEXT     4102
#define  MID_DRIVEPREV     4103
#define  MID_REFRESHLABELS     4104
#define  MID_VOLUMELABEL       4105
#define  MID_VIEW           4200
#define  MID_FILEDETAIL    4201
#define  MID_FILEHEADINGS  4202
#define  MID_FILEICON       4203
#define  MID_FILEFLOWED   4204
#define  MID_DEFAULTVIEW  4205
#define  MID_SORTBYNAME  4206
#define  MID_SORTBYSIZE  4207
#define  MID_SORTBYDATE  4208
#define  MID_SORTBYEXT   4209
#define  MID_REVSORTBYNAME  4214
#define  MID_REVSORTBYSIZE  4215
#define  MID_REVSORTBYDATE  4216
#define  MID_REVSORTBYEXT   4217
#define  MID_SHOWSYSTEM 4210
#define  MID_SHOWHIDDEN  4211
#define  MID_DIVIDERRIGHT 4212
#define  MID_DIVIDERLEFT  4213
#define  MID_OPTIONS       4300
#define  MID_PRINT        4301
#define  MID_EDIT          4302
#define  MID_ZIPTEST      4303
#define  MID_ZIP           4304
#define  MID_UNZIP        4305
#define  MID_ZIPVIEWER   4306
#define  MID_USER1       4307
#define  MID_USER2       4308
#define  MID_USER3       4310
#define  MID_USER4       4311
#define  MID_USER5       4314
#define  MID_USER6       4315
#define  MID_USER7       4316
#define  MID_USER8       4317
#define  MID_SEEK         4309
#define  MID_CREATEOBJ  4312
#define  MID_UNZIPTODIR   4313
#define  MID_SETTINGSMENU  4400
#define  MID_SETTINGS       4401
#define  MID_TOOLBAR       4402
#define  MID_VIEWERS       4403
#define  MID_HELPMENU      4500
#define  MID_ABOUT         4501
#define  MID_HELP           4502
#define  MID_USINGHELP     4503
#define  MID_KEYSHELP      4504
#define  MID_REGISTER       4505
#define  MID_DIRECTORIES  4600
#define  MID_CREATEDIR    4601
#define  MID_DESTINATION  4602
#define  MID_SWAPSandD     4603
#define  MID_TREESIZE       4604
#define  MID_BOOKMARKS    4605

#define FILECOPYID                  500
#define COPYOKID                    501
#define COPYTOPATHID               502
#define COPYTOFILENAMEID          503
#define COPYCANCELID                504
#define COPYHELPID                  505
#define COPYDEFAULTID               506
#define COPYREPLACEID               507

#define MESSAGEBOXID                508

#define FILEDELETEID                600
#define DELETELISTID                601
#define DELETEID                    602
#define CANCELDELETEID              603
#define CREATEDIRID                 700
#define NEWDIRNAMEID                701
#define CANCELNEWDIRID              702
#define CREATEID                    703
#define ACCEPTNAMEID                704
#define RENAMEITEMID                800
#define ENTERRENAMEID               801
#define ACCEPTRENAMEID              802
#define RENAMEOKID                  803
#define RENAMECANCELID              804
#define OLDNAMEID                   805
#define MOVEFILESID                 900
#define MOVELISTID                  902
#define CANCELMOVEID                903
#define MOVEID                      904
#define FORCEDMOVEID                905
#define MOVEDESTID                  906
#define ATTRIBHID                   1102
#define ATTRIBSID                   1103
#define ATTRIBAID                   1104
#define ATTRIBROID                  1101
#define ATTRIBID                    1100
#define ATTRIBFILEID                1105
#define MONTHID                     1106
#define DAYID                       1107
#define YEARID                      1108
#define HOURID                      1109
#define MINUTEID                    1110
#define AMPMID                      1111
#define ATTRCHANGEID                1112
#define DAYCHANGEID                 1113
#define BOTHCHANGEID                1114
#define ATTRCANCELID                1115
#define VIEWERCFGID                 1200
#define VIEWERCANCELID              1204
#define VIEWERTYPEID                1201
#define VIEWEREDITID                1202
#define VIEWERDEFAULTSID         1203
#define VIEWERDELETEID              1207
#define VIEWERACCEPTID              1208
#define VIEWERADDEXTID              1205
#define SETTINGSID                  1300
#define NOTEBOOKID                  1301
#define SETTINGSHELPID              1302
#define SETTINGSDONEID            1303
#define PATHSID                     1400
#define PATHSOURCEID                1401
#define PATHDESTINATIONID           1402
#define EDITORID                    1500
#define EDITORNAMEID                1501
#define EDITORPARMSID               1502
#define ZIPID                       1601
#define ARCHIVERID                  1600
#define ZIPPARMSID                  1602
#define UNZIPID                     1603
#define UNZIPPARMSID                1604
#define USER1ID                     1700
#define USER2ID                     2000
#define USER3ID                     2100
#define USER4ID                     2200
#define USER5ID                     2001
#define USER6ID                     2002
#define USER7ID                     2003
#define USER8ID                     2004
#define USERONEID                   1701
#define USERONEPARMSID              1702
#define USERONEDIRID                1705
#define USERTWOID                   1703
#define USERTWOPARMSID              1704
#define USERTWODIRID                1706
#define USERTHREEID                 1707
#define USERTHREEPARMSID            1708
#define USERTHREEDIRID              1709
#define USERFOURID                  1710
#define USERFOURPARMSID             1711
#define USERFOURDIRID               1712
#define USERONEMENUNAMEID           1713
#define USERTWOMENUNAMEID           1714
#define USERTHREEMENUNAMEID           1715
#define USERFOURMENUNAMEID           1716
#define USERFIVEMENUNAMEID           1729
#define USERSIXMENUNAMEID           1730
#define USERSEVENMENUNAMEID           1731
#define USEREIGHTMENUNAMEID           1732
#define USERFIVEID                   1717
#define USERFIVEPARMSID              1718
#define USERFIVEDIRID                1719
#define USERSIXID                   1720
#define USERSIXPARMSID              1721
#define USERSIXDIRID                1722
#define USERSEVENID                 1723
#define USERSEVENPARMSID            1724
#define USERSEVENDIRID              1725
#define USEREIGHTID                  1726
#define USEREIGHTPARMSID             1727
#define USEREIGHTDIRID               1728
#define INPUTZIPNAMEID              1800
#define ZIPNAMEID                   1801
#define ZIPNAMECANCELID             1802
#define ZIPNAMEACCEPTID             1803
#define ABOUTID                     1900
#define ABOUT1TEXTID                1901
#define ABOUT2TEXTID                1902
#define DDDEST_ID                   2300
#define DDCOPY_ID                   2301
#define DDCONT_ID                   2302
#define DDMOVE_ID                   2303
#define DDHELP_ID                   2304
#define DDCANCEL_ID                 2305
#define DDOK_ID                     2306
#define DDREPLACE_ID                2307
#define DDENTRY_ID                  2308
#define DDZIP_ID                    2309
#define DDUNZIP_ID                  2310
#define DDNAME_ID                   2311
#define DDNAMETITLE_ID              2312
#define DDAPPEND_ID                 2313
#define ZIPLISTID                   1804
#define DIRHISTORYID                2400
#define HISTORYLISTID               2401
#define CANCELHISTORYID             2402
#define ADDBOOKMARKID               2403
#define BOOKMARKHELPID              2404
#define VOLUMELABELID               2500
#define DRIVELISTID                 2501
#define LABELFIELDID                2502
#define ACCEPTLABELID               2503
#define CANCELLABELID               2504
#define REFRESHID                   2600
#define FILESLISTINGID              2601
#define DRIVELISTINGID              2603
#define DIRECTORYINTERVALID         2604
#define DRIVEINTERVALID             2605
#define SEARCHID                    2700
#define SEARCHTOOLNAMEID        2701
#define STPARMSID                  2702
#define APPENDID                    2800
#define APPENDNAMEID                2801
#define APPENDHELPID                2802
#define APPENDLISTID                2803
#define APPENDCANCELID              2805
#define APPENDOKID                  2806
#define APPENDDELETEID              2804
