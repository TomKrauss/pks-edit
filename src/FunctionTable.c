/*
 * FunctionTable.c
 *
 * Project: PKS Edit for Windows
 *
 * purpose:
 * definition of all functions and commands callable in PKS Edit,
 * which can be used either in macros or bound to menus / keyboard etc...
 *
 * 										created: 20.02.87
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <windows.h>
#include "pksmacro.h"
#include "pksmacrocvm.h"
#include "funcdef.h"
#include "pksedit.h"
#include "edierror.h"
#include "editorconfiguration.h"
#include "findandreplace.h"
#include "iccall.h"
#include "winfo.h"
#include "publicapi.h"
#include "stringutil.h"
#include "caretmovement.h"
#include "pksrc.h"
#include "menu.h"
#include "pksmacro.h"
#include "markpositions.h"
#include "windowselector.h"
#include "edierror.h"
#include "editoperations.h"
#include "mainframe.h"
#include "comparefiles.h"
#include "evaluator.h"
#include "edexec.h"
#include "findandreplace.h"
#include "printing.h"

extern long long
EdAbout(long ), EdBlockCopy(long ), EdBlockDelete(),
EdBlockFindEnd(long ), EdBlockFindStart(long ), EdBlockMove(long ), EdBlockRead(long ),
EdBlockWrite(long ), bl_cutOrCopy(long ), EdBlockPaste(long ), bl_hideSelectionInCurrentWindow(),
EdSyncSelectionWithCaret(long ), bl_cutLines(long ), EdLineDelete(long ), bl_destroyAll(long ),
EdKeycodeInsert(long ), EdCharInsert(long ), EdFormatText(long ),
EdSearchListRead(long ), EdErrorListRead(long ),
EdMacrosEdit(long ), doctypes_saveToFile(long ),
EdTagfileRead(long ), EdSetMultiplier(long ), EdGetMultiplier(long), EdReplaceTabs(long ),
EdParaGotoBegin(long ), edit_shiftSelection(int aDirection),
EdParaGotoEnd(long ), EdCharDelete(long ), codecomplete_showSuggestionWindow(long ),
EdMacroRecord(long ), EdFindInFileList(long ), EdFind(long ),
EdReplace(long ), EdFindAgain(long ), EdReplaceAgain(long ), EdCharControlInsert(long ),
EdGotoLine(long ), EdMarkSet(long ), EdMarkGoto(long ),
EdLinesJoin(long ), EdOptionSet(long ),
EdExitAndSave(long ), EdExit(long ), 
EdCommandExecute(long ), EdExecute(long ),
EdShiftBetweenBrackets(long ), EdSort(long ), EdLinesShift(long ), EdInfoFiles(long ),
EdShowMatch(long ), dlg_configureEditorModes(long ),
EdFindTag(long ), EdFindFileCursor(long ), xref_navigateSearchErrorList(long ), EdFindTagCursor(long ),
EdFindWordCursor(long ), EdRangeShift(long ),
EdUndo(long ), EdRedo(long), EdFilesCompare(long ), EdScrollScreen(long ), EdScrollCursor(long ),
EdAlignText(long ), 
EdMouseMarkParts(long ), EdMouseMoveText(long ), EdMouseSelectLines(long ), EdMousePositionUngrabbed(long ),
EdAlert(long ), error_displayAlertBoxWithOptions(long ), EdPromptAssign(long ), interpreter_sprintf(long ),
macro_getSelectedText(long ), EdHideLines(long ), EdUnHideLine(long ), macroc_substituteStringWith(long ),
EdExpandAbbreviation(long ), EdConfigureIcons(long ), EdHelpContext(long ), EdListBindings(long ),
EdCompileMacros(long ), EdDocTypes(long ), EdIsDefined(long ), ft_cloneWindow(),
bl_moveSelectionUpDown(long),
EdShowClipboard(long ), EdSaveAllFiles(), EdSetExtendSelectionMode(long ), EdFindOnInternet(), macroc_print(const char*), macroc_println(const char*), macroc_clearConsole(),
interpreter_typeOf(), interpreter_foreach(), interpreter_size(), interpreter_createArray(), interpreter_createMap(), interpreter_registerType(), macroc_toupper(), macroc_fileOpen(), macroc_fileClose(), macroc_fileReadLine(), macroc_fileWriteLine(), macroc_indexOf(), macroc_stringTokenize(), macroc_tolower(), macro_getFunctionNamesMatching(), macroc_fileTest(),
macroc_fileListFiles(), edit_replaceText(), edit_getAllEditors(), macroc_pathCreateFromSegments(), edit_getSelectedLineRange(), edit_getLineLen(), edit_getLineText(),
macroc_findPattern(), edit_replaceLines(), edit_replaceSpacesWithTabs(), edit_replaceTabsWithSpaces(), op_changeEditorOption();

static const char* ww_getFilename(WINFO* wp) {
    FTABLE* fp = wp->fp;
    return fp->fname;
}

static WINFO* ww_openFile(const char* pszFilename, int nOptions) {
    FTABLE* fp = ft_openFileWithoutFileselector(pszFilename, 0, &(FT_OPEN_OPTIONS) {.fo_isNewFile = 0});
    if (!fp) {
        return 0;
    }
    return WIPOI(fp);
}

// Describes functions and their respective names, options and parameter type descriptions for being used e.g. from within PKSMacroC.
NATIVE_FUNCTION _functionTable[MAX_NATIVE_FUNCTIONS] = {
{/*0*/   EdFileAbandon , -1,EW_NEEDSCURRF,                                                         "AbandonFile",                NULL,  "iW"                                           },
{/*1*/   EdAbout       , -1,0,                                                                         "About",                      NULL,  "i"                                           },
{/*2*/   EdBlockCopy   , -1,EW_MODIFY | EW_NEEDSCURRF | EW_UNDOFLSH | EW_NEEDSBLK,                 "CopyBlock",                  NULL,  "iW"                                           },
{/*3*/   EdBlockDelete , -1,EW_MODIFY | EW_NEEDSCURRF | EW_UNDOFLSH | EW_NEEDSBLK,                 "DeleteBlock",                NULL,  "iWi"                                          },
{/*4*/   EdBlockFindEnd, -1,EW_NEEDSCURRF | EW_NEEDSBLK,                                           "FindBlockEnd",               NULL,  "iW"                                           },
{/*5*/   EdBlockFindStart, -1,  EW_NEEDSCURRF | EW_NEEDSBLK,                                       "FindBlockStart",             NULL,  "iW"                                           },
{/*6*/   EdBlockMove   , -1,EW_MODIFY | EW_NEEDSCURRF | EW_UNDOFLSH | EW_NEEDSBLK,                 "MoveBlock",                  NULL,  "iW"                                           },
{/*7*/   EdBlockRead   , -1,EW_MODIFY | EW_NEEDSCURRF | EW_UNDOFLSH,                               "ReadBlock",                  NULL,  "iW"                                           },
{/*8*/   EdBlockWrite  , -1,EW_NEEDSCURRF | EW_NEEDSBLK,                                           "WriteBlock",                 NULL,  "iW"                                           },
{/*9*/   bl_cutOrCopy    , -1,EW_UNDOFLSH|EW_NEEDSCURRF | EW_NEEDSBLK,                             "CutBlock",                   NULL,  "iWeCUT_"                                },
{/*10*/  EdBlockPaste  , -1,EW_MODIFY | EW_FINDCURS | EW_NEEDSCURRF | EW_UNDOFLSH,                 "PasteBlock",                 NULL,  "iWbPASTE_bFORM_i"                                },
{/*11*/  bl_hideSelectionInCurrentWindow   , -1,EW_NEEDSCURRF,                                     "HideBlock",                  NULL,  "iW"                                           },
{/*12*/  EdSyncSelectionWithCaret   , -1,   EW_NEEDSCURRF,                                         "SetBlockMark",               NULL,  "iWeMARK_"                                      },
{/*13*/  bl_cutLines   , -1,EW_NEEDSCURRF | EW_MULTI,                                              "CutLines",                   NULL,  "iW"                                           },
{/*14*/  EdLineDelete  , -1,EW_MODIFY | EW_NEEDSCURRF | EW_UNDOFLSH,                               "DeleteLines",                NULL,  "iWbELO_"                                      },
{/*15*/  bl_destroyAll  , -1,0,                                                                        "FreeBuffer",                 NULL,  "i"                                           },
{/*16*/  EdKeycodeInsert, -1,   EW_MODIFY | EW_NEEDSCURRF | EW_UNDOFLSH,                           "InsertKeycode",              NULL,  "ibFORM_i"                                     },
{/*17*/  EdCharInsert  , -1,EW_MODIFY | EW_NEEDSCURRF | EW_CCASH,                                  "InsertChar",                 NULL,  "iWi"                                          },
{/*18*/  EdFormatText  , -1,EW_MODIFY | EW_NEEDSCURRF | EW_UNDOFLSH | EW_CUSTOM_ENABLEMENT,            "FormatText",ft_supportsFormatting,  "ibFORM_eAL_eRNG_"                       },
{/*19*/  edit_insertLine  , -1, EW_MODIFY | EW_NEEDSCURRF | EW_UNDOFLSH,                           "InsertLine",                 NULL,  "ieWELO_"                                      },
{/*20*/  edit_performLineFlagOperation, -1, EW_MODIFY | EW_NEEDSCURRF | EW_UNDOFLSH,               "MarkLineOp",                 NULL,  "iWeMLN_"                                      },
{/*21*/  EdSearchListRead, -1, EW_HASFORM,                                                         "ReadSearchList",             NULL,  "ibFORM_s"                                     },
{/*22*/  EdErrorListRead, -1, 0,                                                                       "ReadErrorList",              NULL,  "isbFORM_s"                                    },
{/*23*/  ww_zoomWindow, -1, EW_NEEDSCURRF,                                                         "ZoomWindow",                 NULL,  "iWi"                                           },
{/*24*/  EdMacrosEdit, -1, EW_MULTI,                                                               "EditMacros",                 NULL,  "i"                                           },
{/*25*/  (long long (*)())string_getVariableWithDefaults, -1, 0,                                       "GetVariable",               NULL,  "ss"                                           },
{/*26*/  doctypes_saveToFile, -1, EW_HASFORM,                                                      "SaveDocumentSettings",      NULL,  "ibFORM_s"                                     },
{/*27*/  macro_readWriteWithFileSelection, -1, EW_HASFORM,                                         "MacrosReadWrite",            NULL,  "iibFORM_s"                                },
{/*28*/  EdTagfileRead, -1, EW_HASFORM,                                                            "ReadTagfile",                NULL,  "ibFORM_s"                                     },
{/*29*/  (long long (*)())config_saveConfiguration, -1, 0,                                             "SaveConfiguration",          NULL,  "iibFORM_s"                                     },
{/*30*/  EdSetMultiplier, -1, 0,                                                                       "SetMultiplier",              NULL,  "ibFORM_i"                                     },
{/*31*/  EdReplaceTabs, -1, EW_MODIFY | EW_NEEDSCURRF | EW_UNDOFLSH,                               "ReplaceTabs",                NULL,  "ieCT_bFORM_eRNG_"                            },
{/*32*/  EdLineSplit, -1, EW_MODIFY | EW_NEEDSCURRF | EW_UNDOFLSH,                                 "SplitLine",                  NULL,  "iWeRET_"                                      },
{/*33*/  EdChapterGotoBegin, -1, EW_NEEDSCURRF | EW_MULTI,                                         "GotoChapterStart",           NULL,  "iWeDIR_"                                      },
{/*34*/  EdChapterGotoEnd, -1, EW_NEEDSCURRF | EW_MULTI,                                           "GotoChapterEnd",             NULL,  "iWeMOT_"                                      },
{/*35*/  EdParaGotoBegin, -1, EW_NEEDSCURRF | EW_MULTI,                                            "GotoPgrphStart",             NULL,  "iWeMOT_"                                      },
{/*36*/  EdParaGotoEnd, -1, EW_NEEDSCURRF | EW_MULTI,                                              "GotoPgrphEnd",               NULL,  "iWeMOT_"                                      },
{/*37*/  EdCursorRight, -1, EW_NEEDSCURRF,                                                         "CursorRight",                NULL,  "iWeMOT_"                                      },
{/*38*/  EdCursorLeft, -1, EW_NEEDSCURRF,                                                          "CursorLeft",                 NULL,  "iWeMOT_"                                      },
{/*39*/  EdCursorUp, -1, EW_NEEDSCURRF | EW_MULTI,                                                 "CursorUp",                   NULL,  "iWeMOT_"                                      },
{/*40*/  EdCursorDown, -1, EW_NEEDSCURRF | EW_MULTI,                                               "CursorDown",                 NULL,  "iWeMOT_"                                      },
{/*41*/  EdCharDelete, -1, EW_MODIFY | EW_NEEDSCURRF | EW_UNDOFLSH,                                "Delete",                     NULL,  "iWeMOT_"                                      },
{/*42*/  fkey_keyModifierStateChanged, -1, 0,                                                          "SwitchFkeys",                NULL,  "i"                                      },
{/*43*/  codecomplete_showSuggestionWindow, -1, EW_MODIFY | EW_NEEDSCURRF | EW_UNDOFLSH | EW_HASFORM , "CodeCompleteSuggest",        NULL,  "ibFORM_i"                                     },
{/*44*/  macro_executeMacroByIndex, -1, EW_NOCASH | EW_CCASH,                                      "PlayMacro",                  NULL,  "ieMAC_"                                      },
{/*45*/  EdMacroRecord, -1, EW_NOCASH,                                                             "RecordMacro",                NULL,  "i"                                           },
{/*46*/  EdFindInFileList, -1, EW_HASFORM,                                                         "FindInFileList",             NULL,  "ibFORM_ssibRE_"                             },
{/*47*/  EdFind, -1, EW_NEEDSCURRF | EW_HASFORM,                                                   "Find",                       NULL,  "ibFORM_seDIR_eRNG_bRE_"                      },
{/*48*/  EdReplace, -1, EW_MODIFY | EW_NEEDSCURRF | EW_HASFORM,                                    "Replace",                    NULL,  "ibFORM_sseREP_eRNG_bRE_"                    },
{/*49*/  EdFindAgain, -1, EW_NEEDSCURRF,                                                           "FindAgain",                  NULL,  "iWeDIR_"                                      },
{/*50*/  EdReplaceAgain, -1, EW_MODIFY | EW_NEEDSCURRF,                                            "ReplaceAgain",               NULL,  "iW"                                           },
{/*51*/  EdCharControlInsert, -1, EW_MODIFY | EW_NEEDSCURRF | EW_HASFORM | EW_CCASH,               "InsertControl",              NULL,  "ibFORM_i"                                     },
{/*52*/  fm_gotoNextPosition, -1, EW_CUSTOM_ENABLEMENT,                                                "GotoLastpos",      fm_canGotoNext,  "ieMTE_"                                      },
{/*53*/  EdGotoLine, -1, EW_NEEDSCURRF,                                                            "GotoLine",                   NULL,  "ibFORM_i"                                     },
{/*54*/  EdMarkSet, -1, EW_NEEDSCURRF | EW_HASFORM,                                                "SetMark",                    NULL,  "ibFORM_i"                                     },
{/*55*/  EdMarkGoto, -1, EW_NEEDSCURRF | EW_HASFORM,                                               "GotoMark",                   NULL,  "ibFORM_i"                                     },
{/*56*/  menu_openContextMenu, -1, 0,                                                                  "OpenContextMenu",            NULL,  "ii"                                          },
{/*57*/  EdLinesJoin, -1, EW_MODIFY | EW_NEEDSCURRF | EW_UNDOFLSH,                                 "JoinLines",                  NULL,  "iW"                                           },
{/*58*/  EdEditFile, -1, EW_HASFORM,                                                               "EditFile",                   NULL,  "ibOPEN_bFORM_s"                                },
{/*59*/  EdOptionSet, -1, EW_HASFORM,                                                              "SetOptions",                 NULL,  "ibFORM_eO_"                            },
{/*60*/  (long long (*)())EdPrint, -1, 0,                                                              "Print",                      NULL,  "iePRT_s"                                     },
{/*61*/  EdExitAndSave, -1, 0,                                                                         "ExitEdit",                   NULL,  "ii"                                          },
{/*62*/  EdExit, -1, 0,                                                                                "QuitEdit",                   NULL,  "ii"                                          },
{/*63*/  mainframe_closeChildWindows, -1, 0,                                                           "CloseWindows",              NULL,   "ibCWF_"  },
{/*64*/  EdSaveFile, -1, EW_NEEDSCURRF,                                                            "SaveFile",                   NULL,  "ibSAV_bFORM_s"                                },
{/*65*/  (long long (*)())ft_selectWindowWithId, -1, 0,                                                "SelectWindow",               NULL,  "ii"                                      },
{/*66*/  EdCommandExecute, -1, EW_HASFORM,                                                         "ExecuteCommandDialog",       NULL,  "ibFORM_ssseEX_"                              },
{/*67*/  EdExecute, -1, EW_HASFORM,                                                                "ExecuteCommand",             NULL,  "ieEX_sss"                              },
{/*68*/  EdShiftBetweenBrackets, -1, EW_MODIFY | EW_NEEDSCURRF | EW_UNDOFLSH,                      "ShiftBetweenBrackets",       NULL,  "iWeDIR_"                                      },
{/*69*/  EdSort, -1, EW_MODIFY | EW_NEEDSCURRF,                                                    "Sort",                       NULL,  "ibFORM_ssseRNG_eSO_"                         },
{/*70*/  EdLinesShift, -1, EW_MODIFY | EW_NEEDSCURRF | EW_UNDOFLSH | EW_MULTI,                     "ShiftLines",                 NULL,  "iWeDIR_"                                      },
{/*71*/  EdInfoFiles, -1, 0,                                                                           "ShowInfo",                   NULL,  "i"                                           },
{/*72*/  EdShowMatch, -1, EW_NEEDSCURRF,                                                           "CheckBrackets",              NULL,  "iW"                                           },
{/*73*/  edit_convertCharacterCase, -1, EW_MODIFY | EW_FINDCURS | EW_NEEDSCURRF | EW_UNDOFLSH,     "UpToLow",                    NULL,  "iWeCC_"                                           },
{/*74*/  evaluator_evaluateCurrentSelection, -1, EW_NEEDSCURRF | EW_FINDCURS,                      "EvaluateSelection",          NULL,  "iW"                                        },
{/*75*/  dlg_configureEditorModes, -1, EW_NEEDSCURRF,                                              "SetEditorSettings",                NULL, "ibFORM_siiiieSHOW_"                            },
{/*76*/  mainframe_toggleFullScreen, -1, 0,                                                            "ToggleFullScreen",           NULL,  "i"                                           },
{/*77*/  EdFindOnInternet, -1, EW_NEEDSCURRF | EW_FINDCURS,                                        "FindOnInternet",             NULL,  "iWbFORM_siii"                                  },
{/*78*/  (long long (*)())EdOptionToggle, -1, EW_NEEDSCURRF,                                       "ToggleEditorOption",             NULL,  "iWeWM_"                                      },
{/*79*/  EdGetMultiplier, -1, 0,                                                                       "GetMultiplier",              NULL,  "i"                                     },
{/*80*/  EdFindTag, -1, EW_HASFORM,                                                                "FindTag",                    NULL,  "iWibFORM_s"                                    },
{/*81*/  EdFindFileCursor, -1, EW_NEEDSCURRF,                                                      "CursorFindFile",             NULL,  "iW"                                           },
{/*82*/  xref_navigateSearchErrorList, -1, 0,                                                          "NextResultOrError",          NULL,  "ieLIST_"                                      },
{/*83*/  EdFindTagCursor, -1, EW_NEEDSCURRF | EW_FINDCURS| 0,                                          "CursorFindTag",              NULL,  "iW"                                           },
{/*84*/  EdFindWordCursor, -1, EW_NEEDSCURRF | EW_FINDCURS,                                        "CursorFindWord",             NULL,  "iWeDIR_"                                      },
{/*85*/mainframe_manageDocks, -1, 0,                                                                   "ManageDocks",                NULL,  "ieMD_"                                      },
{/*86*/xref_navigateSearchErrorList, -1, EW_NEEDSCURRF | EW_FINDCURS,                                  "FindErrorMouseClick",        NULL,  "ii"                                          },
{/*87*/EdRangeShift, -1, EW_MODIFY | EW_NEEDSCURRF | EW_FINDCURS | EW_UNDOFLSH,                    "ShiftText",                  NULL,  "ieDIR_bFORM_eRNG_"                            },
{/*88*/EdUndo, -1, EW_MODIFY | EW_NEEDSCURRF | EW_UNDO_AVAILABLE,                                      "Undo",                       NULL,  "iW"                                           },
{/*89*/EdFilesCompare, -1, EW_NEEDSCURRF,                                                          "CompareWindows",             NULL,  "ieDIR_"                                      },
{/*90*/EdScrollScreen, -1, EW_NEEDSCURRF,                                                          "ScrollScreen",               NULL,  "iWeMOT_"                                      },
{/*91*/EdScrollCursor, -1, EW_NEEDSCURRF,                                                          "ScrollCursor",               NULL,  "iWeMOT_"                                      },
{/*92*/fm_gotoLastPosition, -1, EW_CUSTOM_ENABLEMENT,                                                  "GotoLastPosition", fm_canGotoLast,  "i"                                           },
{/*93*/EdAlignText, -1, EW_MODIFY | EW_NEEDSCURRF | EW_UNDOFLSH,                                   "AlignText",                  NULL,  "ibFORM_sieRNG_bRE_eAL_"                     },
{/*94*/error_setShowMessages, -1, 0,                                                                   "ShowMessages",               NULL,  "ii"                                          },
{/*95*/interpreter_createArray, -1, EW_NEEDSCURRF,                                                 "CreateArray",                NULL,  "P"                                      },
{/*96*/EdMouseMarkParts, -1, EW_NEEDSCURRF,                                                        "MouseMarkParts",             NULL,  "iWeMOT_"                                      },
{/*97*/EdMouseMoveText, -1, EW_NEEDSCURRF,                                                         "MouseMoveText",              NULL,  "iWeMM_"                                      },
{/*98*/EdMouseSelectLines, -1, EW_NEEDSCURRF | EW_FINDCURS,                                        "MouseSelectLines",           NULL,  "iWi"                                      },
{/*99*/EdMousePositionUngrabbed, -1, EW_NEEDSCURRF,                                                "MousePosition",              NULL,  "iWi"                                          },
{/*100*/EdAlert, -1, 0,                                                                                "Alert",                      NULL,  "is"                                          },
{/*101*/error_displayAlertBoxWithOptions, -1, 0,                                                       "MessageBox",                 NULL,  "ieMB_s"                                        },
{/*102*/EdPromptAssign, -1, 0,                                                                         "PromptAssign",               NULL,  "sss"                                         },
{/*103*/ interpreter_sprintf, -1, 0,                                                                   "sprintf",                    NULL,  "P"                                         },
{/*104*/macro_getSelectedText, -1, 0,                                                                  "GetSelected",                NULL,  "sW"                                           },
{/*105*/EdHideLines, -1, EW_MODIFY | EW_NEEDSCURRF | EW_UNDOFLSH,                                  "HideLines",                  NULL,  "iW"                                           },
{/*106*/EdUnHideLine, -1, EW_MODIFY | EW_NEEDSCURRF | EW_UNDOFLSH,                                 "UnHideLine",                 NULL,  "iW"                                           },
{/*107*/macroc_substituteStringWith, -1, 0,                                                            "StringReplace",              NULL,  "ssssbRE_i"                                  },
{/*108*/EdExpandAbbreviation, -1, 0,                                                                   "ExpandAbbreviation",         NULL,  "i"                                           },
{/*109*/caret_addSecondaryWithMouse, -1, EW_NEEDSCURRF| EW_CCASH ,                                     "AddSecondaryCaretMouse",     NULL,  "ibFORM_i"                                     },
{/*110*/EdHelpContext, -1, 0,                                                                          "ShowHelp",                   NULL,  "ii"                                          },
{/*111*/EdListBindings, -1, 0,                                                                         "ListBindings",               NULL,  "ieLISTB_"                                    },
{/*112*/EdCompileMacros, -1, EW_NEEDSCURRF,                                                        "CompileMacros",              NULL,  "ii"                                          },
{/*113*/EdDocTypes, -1, 0,                                                                             "DefineDocTypes",             NULL,  "i"                                           },
{/*114*/find_initiateIncrementalSearch, -1, EW_NEEDSCURRF,                                             "StartIncrementalSearch",     NULL,   "i" },
{/*115*/EdIsDefined, -1, 0,                                                                            "QueryStatus",                NULL,  "ieQUERY_"                                    },
{/*116*/caret_removeSecondaryCarets, -1, EW_NEEDSCURRF,                                                "RemoveSecondaryCarets",      NULL,  "isii"                                        },
{/*117*/EdShowClipboard, -1, 0,                                                                        "ShowClipboard",              NULL,  "ii"                                          },
{/*118*/caret_moveAndAddSecondary, -1, EW_CCASH | EW_NEEDSCURRF,                                   "MoveAndAddSecondaryCaret",   NULL,  "iiis"                                        },
{/*119*/EdSetExtendSelectionMode, -1, EW_NEEDSCURRF,                                               "SetSelectionExtensionMode",  NULL,  "iWi"                                          },
{/*120*/EdRedo, -1, EW_MODIFY | EW_NEEDSCURRF | EW_REDO_AVAILABLE,                                     "Redo",                       NULL,  "iW"                                          },
{/*121*/EdSaveAllFiles, -1, EW_NEEDSCURRF,                                                         "SaveAll",                    NULL,  "i"                                          },
{/*122*/ft_cloneWindow, -1, EW_NEEDSCURRF,                                                             "CloneWindow",                NULL,  "iW"                                          },
{/*123*/edit_shiftSelection, -1, EW_MODIFY | EW_NEEDSCURRF | EW_UNDOFLSH,                              "ShiftSelection",             NULL,  "ii"                                          },
{/*124*/edit_toggleComment, -1, EW_MODIFY | EW_NEEDSCURRF | EW_UNDOFLSH,                               "ToggleComment",              NULL,  "iW"                                          },
{/*125*/windowselector_showWindowList,-1,0,                                                            "ShowWindowSelector",         NULL,  "i"                                          },
{/*126*/compare_navigate,-1, EW_NEEDSCURRF| EW_COMPARISON_MODE,                                        "CompareNavigate",            NULL,  "ii"                                          },
{/*127*/compare_clear, -1, EW_NEEDSCURRF | EW_COMPARISON_MODE,                                         "CompareClear",               NULL,  "i"                                          },
{/*128*/bl_moveSelectionUpDown, -1, EW_NEEDSCURRF | EW_UNDOFLSH,                                       "MoveSelection",              NULL,  "ii"                                          },
{/*129*/edit_insertString, -1, EW_NEEDSCURRF | EW_UNDOFLSH,                                            "EditorInsertString",         NULL,  "iWs"                                          },
{/*130*/interpreter_size, -1, 0,                                                                       "size",                     NULL,  "P" },
{/*131*/(long long (*)())strstr, -1, 0,                                                                 "strstr",                    NULL, "iss"},
{/*132*/macroc_print, -1, 0, "print", NULL, "is"},
{/*133*/macroc_println, -1, 0, "println", NULL, "is" },
{/*134*/macroc_clearConsole, -1, 0, "clear", NULL, "i"},
{/*135*/interpreter_typeOf, -1, 0, "typeof", NULL, "P"},
{/*136*/macroc_toupper, -1, 0, "toupper", NULL, "ss" },
{/*137*/macroc_tolower, -1, 0, "tolower", NULL, "ss" },
{/*138*/macroc_stringTokenize, -1, 0, "StringTokenize", NULL, "ass" },
{/*139*/interpreter_foreach, -1, 0, "foreach", NULL, "P"},
{/*140*/macroc_indexOf, -1, 0, "IndexOf", NULL, "P" },
{/*141*/macro_getFunctionNamesMatching, -1, 0, "FunctionNamesMatching", NULL, "asbLMT" },
{/*142*/macroc_fileOpen, -1, 0, "FileOpen", NULL, "P" },
{/*143*/macroc_fileClose, -1, 0, "FileClose", NULL, "P" },
{/*144*/macroc_fileReadLine, -1, 0, "FileReadLine", NULL, "P" },
{/*145*/macroc_fileWriteLine, -1, 0, "FileWriteLine", NULL, "P" },
{/*146*/macroc_fileTest, -1, 0, "FileTest", NULL, "iss" },
{/*147*/macroc_fileListFiles, -1, 0, "FileListFiles", NULL, "ass" },
{/*148*/(long long (*)())memory_mapKeys, -1, 0, "MapKeys", NULL, "P" },
{/*149*/(long long (*)())memory_mapValues, -1, 0, "MapValues", NULL, "P" },
{/*150*/(long long (*)())memory_mapEntries, -1, 0, "MapEntries", NULL, "P" },
{/*151*/(long long (*)())edit_replaceText, -1, EW_NEEDSCURRF, "EditorReplaceText", NULL, "iWssbRE_eRNG_eREP_" },
{/*152*/(long long (*)())ww_getCurrentEditorWindow, -1, 0, "EditorGetCurrent", NULL, "W" },
{/*153*/(long long (*)())ww_selectWindow, -1, EW_NEEDSCURRF, "EditorSetCurrent", NULL, "iW" },
{/*154*/(long long (*)())edit_getAllEditors, -1, 0, "EditorGetAll", NULL, "P" },
{/*155*/(long long (*)())ww_getFilename, -1, EW_NEEDSCURRF, "EditorGetFilename", NULL, "sW" },
{/*156*/(long long (*)())ww_openFile, -1, 0, "EditorOpenFile", NULL, "Wsi" },
{/*157*/macroc_pathCreateFromSegments, -1, 0, "PathCreateFromSegments", NULL, "P"},
{/*158*/(long long (*)())function_registerNativeFunction, -1, 0, "RegisterNative", NULL, "issssss" },
{/*159*/(long long (*)())edit_getSelectedLineRange, -1, 0, "EditorSelectedLineRange", NULL, "P" },
{/*160*/(long long (*)())edit_getLineText, -1, 0, "EditorGetLineText", NULL, "sWi" },
{/*161*/(long long (*)())edit_getLineLen, -1, 0, "EditorGetLineLen", NULL, "iWi" },
{/*162*/macroc_findPattern, -1, 0, "StringFindPattern", NULL, "issbRE_" },
{/*163*/caret_bufferOffset2Screen, -1, 0, "EditorBufferOffset2Screen", NULL, "iWsi" },
{/*164*/caret_screenOffset2Buffer, -1, 0, "EditorScreenOffset2Buffer", NULL, "iWsii" },
{/*165*/edit_replaceLines, -1, 0, "EditorReplaceLines", NULL, "P" },
{/*166*/edit_replaceSpacesWithTabs, -1, 0, "EditorReplaceSpacesWithTabs", NULL, "sWs" },
{/*167*/edit_replaceTabsWithSpaces, -1, 0, "EditorReplaceTabsWithSpaces", NULL, "sWs" },
{/*168*/interpreter_createMap, -1, 0, "CreateMap", NULL, "P" },
{/*170*/(long long (*)())interpreter_registerType, -1, 0, "RegisterType", NULL, "P" },
{/*171*/(long long (*)())op_changeEditorOption, -1, 0, "EditorChangeOption", NULL, "iWeOP_ii" },
0
};

static int _staticallyDefinedFunctions;
int _functionTableSize;

void function_destroyRegisteredNative(NATIVE_FUNCTION* pFunc) {
    free((char*)pFunc->nf_description);
    free((char*)pFunc->nf_name);
    free((char*)pFunc->nf_paramTypes);
    free((char*)pFunc->nf_parameters);
    pFunc->nf_description = 0;
    pFunc->nf_name = 0;
    pFunc->nf_paramTypes = 0;
    pFunc->nf_parameters = 0;
}

/*
 * Returns the number of statically defined functions.
 */
int function_getNumberOfStaticallyDefinedFunctions() {
    if (_staticallyDefinedFunctions == 0) {
        while (_functionTable[_staticallyDefinedFunctions].nf_name) {
            _staticallyDefinedFunctions++;
        }
        _functionTableSize = _staticallyDefinedFunctions;
    }
    return _staticallyDefinedFunctions;
}

/*
 * Release all dynamically defined functions.
 */
void function_destroy() {
    // For statically defined functions we allow to define the documentation
    // using MacroC code.
    function_getNumberOfStaticallyDefinedFunctions();
    for (int i = 0; i < _staticallyDefinedFunctions; i++) {
        free((char*)_functionTable[i].nf_description);
        free((char*)_functionTable[i].nf_parameters);
        _functionTable[i].nf_description = 0;
        _functionTable[i].nf_parameters = 0;
    }
    for (int i = _staticallyDefinedFunctions; i < _functionTableSize; i++) {
        function_destroyRegisteredNative(& _functionTable[i]);
    }
    _functionTableSize = 0;
}

COMMAND _commandTable[] = {
0, C_1FUNC, 58 /* EdEditFile */, 							1, 0 , "open-file",
1, C_1FUNC, 58 /* EdEditFile */, 							1, OPEN_NOFN, "open-new-file",
2, C_1FUNC, 58 /* EdEditFile */, 							1, OPEN_DIRGIVEN, "open-file-dir-given",
3, C_1FUNC, 58 /* EdEditFile */, 							1, OPEN_NOFN|OPEN_LINEAL, "open-special",
4, C_1FUNC, 58 /* EdEditFile */, 							1, OPEN_NOFN|OPEN_HISTORY|0x0000, "open-history",
5, C_1FUNC, 58 /* EdEditFile */, 							1, OPEN_NOFN|OPEN_HISTORY|0x1000, "open-history1",
6, C_1FUNC, 58 /* EdEditFile */, 							1, OPEN_NOFN|OPEN_HISTORY|0x2000, "open-history2",
7, C_1FUNC, 58 /* EdEditFile */, 							1, OPEN_NOFN|OPEN_HISTORY|0x3000, "open-history3",
8, C_1FUNC, 58 /* EdEditFile */, 							1, OPEN_NOFN|OPEN_HISTORY|0x4000, "open-history4",
9, C_1FUNC, 64 /* EdSaveFile */, 							1, SAV_QUIT , "quit-file",
10, C_1FUNC, 64 /* EdSaveFile */, 							1, SAV_QUIT|SAV_SAVE, "save-file-and-close",
11, C_1FUNC, 64 /* EdSaveFile */, 							1, SAV_SAVE|SAV_AS, "save-file-as",
12, C_1FUNC, 64 /* EdSaveFile */, 							1, SAV_SAVE , "save-file",
13, C_0FUNC, 7 /* EdBlockRead */, 							1, 0 , "read-block",
14, C_0FUNC, 8 /* EdBlockWrite */, 							1, 0 , "save-selection",
15, C_1FUNC, 61 /* EdExitAndSave */,						1, 0 , "exit-edit",
16, C_1FUNC, 62 /* EdExit */, 								1, 0 , "quit-edit",
17, C_0FUNC, 47 /* EdFind */, 								1, 0 , "find-string",
18, C_0FUNC, 48 /* EdReplace */, 							1, 0 , "replace-string",
19, C_1FUNC, 49 /* EdFindAgain */, 							1, 0 , "find-again",
20, C_1FUNC, 49 /* EdFindAgain */, 							1, 1 , "find-again-forward",
21, C_1FUNC, 49 /* EdFindAgain */, 							1, -1 , "find-again-backward",
22, C_1FUNC, 84 /* EdFindWordCursor */, 					1, 1 , "find-word-forward",
23, C_0FUNC, 83 /* EdFindTagCursor */, 						1, 0 , "find-tag-under-cursor",
24, C_1FUNC, 84 /* EdFindWordCursor */, 					1, -1 , "find-word-back",
25, C_0FUNC, 81 /* EdFindFileCursor */, 					1, 0 , "find-file-under-cursor",
26, C_0FUNC, 46 /* EdFindInFileList */, 					1, 0 , "find-in-filelist",
27, C_1FUNC, 80 /* EdFindTag */, 							1, 0 , "find-tag",
28, C_0FUNC, 54 /* EdMarkSet */, 							1, 0 , "set-mark",
29, C_0FUNC, 55 /* EdMarkGoto */, 							1, 0 , "goto-mark",
30, C_0FUNC, 53 /* EdGotoLine */, 							1, 0 , "goto-line",
31, C_1FUNC, 92 /* fm_gotoLastPosition */, 					1, MTE_AUTO_LAST_SEARCH, "goto-last-pos",
32, C_1FUNC, 92 /* fm_gotoLastPosition */, 					1, MTE_AUTO_LAST_INSERT , "goto-last-insertion-pos",
33, C_1FUNC, 12 /* EdSyncSelectionWithCaret */, 			1, MARK_START, "start-selection",
34, C_1FUNC, 12 /* EdSyncSelectionWithCaret */, 			1, MARK_END , "end-selection",
35, C_1FUNC, 12 /* EdSyncSelectionWithCaret */, 			1, MARK_ALL , "select-all",
36, C_0FUNC, 11 /* bl_hideSelectionInCurrentWindow */, 		1, 0 , "remove-selection",
37, C_0FUNC, 5 /* EdBlockFindStart */, 						1, 0, "find-selection-start",
38, C_0FUNC, 4 /* EdBlockFindEnd */, 						1, 0 , "find-selection-end",
39, C_1FUNC, 9 /* bl_cutOrCopy */, 							1, CUT_DELETE, "cut-selection",
40, C_1FUNC, 9 /* bl_cutOrCopy */, 							1, CUT_QUERYID|CUT_DELETE, "cut-to-named-buffer",
41, C_1FUNC, 9 /* bl_cutOrCopy */, 							1, CUT_QUERYID, "copy-to-named-buffer",
42, C_1FUNC, 9 /* bl_cutOrCopy */, 							1, 0, "copy-to-clipboard",
43, C_1FUNC, 10 /* EdBlockPaste */, 						1, PASTE_QUERYID, "paste-named-buffer",
44, C_1FUNC, 10 /* EdBlockPaste */, 						1, PASTE_CLIP, "paste-clipboard",
45, C_1FUNC, 10 /* EdBlockPaste */, 						1, PASTE_UNDO, "paste-trash",
46, C_1FUNC, 82 /* xref_navigateSearchErrorList */, 		1, LIST_CURR, "errorlist-current-line",
47, C_1FUNC, 3 /* EdBlockDelete */, 						1, 1 , "delete-selection",
48, C_0FUNC, 2 /* EdBlockCopy */, 							1, 0 , "copy-selection",
49, C_0FUNC, 6 /* EdBlockMove */, 							1, 0 , "move-selection",
50, C_0FUNC, 13 /* bl_cutLines */, 							1, 0 , "cut-lines",
51, C_1FUNC, 78 /* EdOptionToggle */, 						1, OPT_WMODE|WM_COLUMN_SELECTION, "toggle-column-selection",
52, C_1FUNC, 78 /* EdOptionToggle */, 						1, OPT_DMODE|SHOW_CONTROL_CHARS, "toggle-show-control",
53, C_1FUNC, 78 /* EdOptionToggle */, 						1, OPT_DMODE|SHOW_RULER, "toggle-show-ruler",
54, C_0FUNC, 114 /* EdPrinterLayout */, 					1, 0 , "dialog-printer-layout",
55, C_1FUNC, 78 /* EdOptionToggle */, 						1, OPT_DMODE|SHOW_HEX_DISPLAY, "toggle-hex-modus",
56, C_1FUNC, 78 /* EdOptionToggle */, 						1, OPT_WMODE|WM_DELETE_MULTIPLE_SPACES, "delete-multiple-spaces",
57, C_1FUNC, 78 /* EdOptionToggle */, 						1, OPT_WMODE|WM_INSERT, "toggle-insert-mode",
58, C_0FUNC, 59 /* EdOptionSet */, 							1, 0 , "set-options",
59, C_0FUNC, 18 /* EdFormatText */, 						1, 0 , "format-text",
60, C_1FUNC, 31 /* EdReplaceTabs */, 						1, 0 , "compress-tabs",
61, C_1FUNC, 31 /* EdReplaceTabs */, 						1, 1 , "expand-tabs",
62, C_0FUNC, 93 /* EdAlignText */, 							1, 0 , "align-text",
63, C_1FUNC, 87 /* EdRangeShift */, 						1, 1 , "shift-range-right",
64, C_1FUNC, 87 /* EdRangeShift */, 						1, -1 , "shift-range-left",
65, C_0FUNC, 69 /* EdSort */, 								1, 0 , "sort-text",
66, C_1FUNC, 89 /* EdFilesCompare */, 						1, LIST_NEXT , "next-difference",
67, C_1FUNC, 60 /* EdPrint */, 								1, PRT_CURRWI, "print-window",
68, C_1FUNC, 60 /* EdPrint */, 								1, PRT_CURRBLK, "print-block",
69, C_1FUNC, 60 /* EdPrint */, 								1, PRT_FILE , "print-file",
70, C_0FUNC, 66 /* EdCommandExecute */, 					1, 0, "execute-command",
71, C_0FUNC, 28 /* EdTagfileRead */, 						1, 0, "read-tagfile",
72, C_0FUNC, 21 /* EdSearchListRead */, 					1, 0, "read-search-list",
73, C_1FUNC, 22 /* EdErrorListRead */, 						1, 0, "read-compiler-errors",
74, C_0FUNC, 26 /* doctypes_saveToFile */, 					1, 0,  "save-document-settings",
75, C_0FUNC, 45 /* EdMacroRecord */, 						1, 0 , "record-macro",
76, C_0FUNC, 24 /* EdMacrosEdit */, 						1, 0 , "dialog-macros",
77, C_1FUNC, 27 /* macro_readWriteWithFileSelection */, 	1, 1 , "write-macros",
78, C_1FUNC, 27 /* macro_readWriteWithFileSelection */, 	1, 0 , "read-macros",
79, C_1FUNC, 29 /* EdSetup */, 								1, 1,  "save-configuration",
80, C_0FUNC, 109 /* EdConfigureIcons */, 					1, 0 , "configure-icons",
81, C_0FUNC, 75 /* dlg_configureEditorModes */, 			1, 0 , "dialog-display-mode",
82, C_0FUNC, 74 /* EdDlgWorkMode */, 						1, 0 , "dialog-work-mode",
83, C_0FUNC, 76 /* EdDlgCursTabs */, 						1, 0 , "dialog-text-cursor",
84, C_0FUNC, 77 /* EdDlgModeVals */, 						1, 0 , "dialog-macro-bindings",
85, C_1FUNC, 23 /* doctypes_readWriteDocumentDescriptor */, 1, 1 , "save-ruler",
86, C_1FUNC, 23 /* doctypes_readWriteDocumentDescriptor */, 1, 0 , "read-ruler",
87, C_0FUNC, 71 /* EdInfoFiles */, 							1, 0 , "info-to-file",
88, C_1FUNC, 63 /* mainframe_closeChildWindows */, 			1, CWF_ALL, "close-all-windows",
89, C_1FUNC, 85 /* EdWinArrange */, 						1, WIN_ICONARRANGE, "arrange-icons",
90, C_1FUNC, 78 /* EdOptionToggle */, 						1, OPT_WMODE | WM_PINNED, "toggle-pin-window",
91, C_1FUNC, 85 /* EdWinArrange */, 						1, WIN_TILE , "tile-windows",
CMD_HELP_README, C_1FUNC, 110 /* EdHelpContext */, 			1, IDM_HLPREADME, "help-readme",
CMD_HELP_OVERVIEW, C_1FUNC, 110 /* EdHelpContext */, 		1, IDM_HLPOVERVIEW, "help-overview",
CMD_HELP_ON_KEYS, C_1FUNC, 110 /* EdHelpContext */, 		1, IDM_HLPONKEYS, "help-on-keys",
CMD_HELP_RELEASE_NOTES, C_1FUNC, 110 /* EdHelpContext */, 	1, IDM_HLPRELEASENOTES, "help-release-notes",
CMD_HELP_MANUAL_TOC, C_1FUNC, 110 /* EdHelpContext */, 		1, IDM_HLP_MANUAL_TOC, "help-manual-toc",
CMD_SHOW_COPYRIGHT, C_0FUNC, 1 /* EdAbout */, 				1, 0 , "show-copyright",
CMD_KEYCODE_INSERT, C_0FUNC, 16 /* EdKeycodeInsert */, 		1, 0 , "keycode-insert",
99, C_0FUNC, 74 /* evaluate_evaluateSelection */, 			1, 0 , "evaluate-selection",
100, C_0FUNC, 76 /* mainframe_toggleFullScreen */, 			1, 0, "toggle-full-screen",
101, C_1FUNC, 78 /* EdOptionToggle */, 						1, OPT_DMODE | SHOW_WYSIWYG_DISPLAY, "toggle-show-wysiwyg",
102, C_0FUNC, 109 /* caret_addSecondaryWithMouse */, 		1, 3 , "add-secondary-caret-mouse",
103, C_0FUNC, 116 /* caret_removeSecondaryCarets */, 		1, 0, "remove-secondary-carets",
104, C_1FUNC, 118 /* caret_moveAndAddSecondary*/, 			1, MSM_UP, "secondary-cursor-up-line",
105, C_1FUNC, 85 /* EdWinArrange */, 						1, WIN_FULL , "set-win-zoomed",
106, C_1FUNC, 85 /* ManageDocks */,  						1, MD_ADD_HORIZONTAL, "tile-docks-horizontally",
107, C_1FUNC, 85 /* ManageDocks */,  						1, MD_ADD_VERTICAL, "tile-docks-vertically",
108, C_1FUNC, 86 /* xref_navigateSearchErrorList */, 		1, LIST_CURR, "errorlist-click-line",
109, C_1FUNC, 56 /* menu_openContextMenu */, 				1, 0, "open-context-menu",
110, C_1FUNC, 77 /* EdFindOnInternet */, 					1, 0, "search-on-internet",
111, C_1FUNC, 118 /* caret_moveAndAddSecondary*/, 			1, MSM_DOWN, "secondary-cursor-down-line",
112, C_1FUNC, 65 /* ft_selectWindowWithId */, 						1, 1 , "select-window-1",
113, C_1FUNC, 65 /* ft_selectWindowWithId */, 						1, 2 , "select-window-2",
114, C_1FUNC, 65 /* ft_selectWindowWithId */, 						1, 3 , "select-window-3",
115, C_1FUNC, 65 /* ft_selectWindowWithId */, 						1, 4 , "select-window-4",
116, C_1FUNC, 65 /* ft_selectWindowWithId */, 						1, 5 , "select-window-5",
117, C_1FUNC, 65 /* ft_selectWindowWithId */, 						1, 6 , "select-window-6",
118, C_1FUNC, 65 /* ft_selectWindowWithId */, 						1, -1 , "select-previous-window",
119, C_1FUNC, 125 /* windowselector_showWindowList */, 		1, 0, "cycle-window",
120, C_1FUNC, 73 /* edit_convertCharacterCase */, 			1, CC_TOGGLE, "char-toggle-upper-lower",
121, C_1FUNC, 70 /* EdLinesShift */,   						1, -1 , "shift-line-left",
122, C_1FUNC, 70 /* EdLinesShift */,   						1, 1 , "shift-line-right",
123, C_0FUNC, 50 /* EdReplaceAgain */, 						1, 0 , "replace-again",
124, C_1FUNC, 82 /* xref_navigateSearchErrorList */, 		1, LIST_NEXT , "errorlist-next",
125, C_1FUNC, 82 /* xref_navigateSearchErrorList */, 		1, LIST_END , "errorlist-end",
126, C_1FUNC, 82 /* xref_navigateSearchErrorList */, 		1, LIST_PREV , "errorlist-previous",
127, C_1FUNC, 82 /* xref_navigateSearchErrorList */, 		1, LIST_START, "errorlist-start",
128, C_0FUNC, 72 /* EdShowMatch */, 						1, 0 , "check-brackets",
129, C_0FUNC, 51 /* EdCharControlInsert */,    				1, 0 , "insert-char-with-dialog",
130, C_1FUNC, 68 /* EdShiftBetweenBrackets */, 				1, -1 , "shift-between-brackets-left",
131, C_1FUNC, 68 /* EdShiftBetweenBrackets */, 				1, 1 , "shift-between-brackets-right",
132, C_1FUNC, 17 /* EdCharInsert */, 						1, 127 , "delete-char",
133, C_1FUNC, 41 /* EdCharDelete */, 						1, MOT_TOEND , "delete-to-eol",
134, C_1FUNC, 41 /* EdCharDelete */, 						1, MOT_SPACE, "delete-word",
135, C_1FUNC, 41 /* EdCharDelete */, 						1, MOT_UNTILC, "delete-to-next-char",
136, C_1FUNC, 41 /* EdCharDelete */, 						1, -MOT_SPACE, "delete-back-white",
137, C_1FUNC, 41 /* EdCharDelete */, 						1, MOT_WORD, "delete-to-white",
138, C_1FUNC, 41 /* EdCharDelete */, 						1, -MOT_TOEND, "delete-to-sol",
139, C_1FUNC, 41 /* EdCharDelete */, 						1, -MOT_SPACE, "delete-back-word",
140, C_1FUNC, 41 /* EdCharDelete */, 						1, -MOT_UNTILC, "delete-until-previous-char",
141, C_1FUNC, 17 /* EdCharInsert */, 						1, 13 , "split-line",
142, C_1FUNC, 32 /* EdLineSplit */, 						1, RET_PLUS , "split-line-indent-plus",
143, C_1FUNC, 32 /* EdLineSplit */, 						1, RET_MINUS , "split-line-indent-minus",
144, C_0FUNC, 57 /* EdLinesJoin */, 						1, 0 , "join-lines",
145, C_1FUNC, 39 /* EdCursorUp */, 							1, MOT_SINGLE, "cursor-up-line",
146, C_1FUNC, 39 /* EdCursorUp */, 							1, MOT_TOEND , "cursor-top-of-page",
147, C_1FUNC, 39 /* EdCursorUp */, 							1, MOT_WORD , "cursor-back-page",
148, C_1FUNC, 40 /* EdCursorDown */, 						1, MOT_SINGLE, "cursor-down-line",
149, C_1FUNC, 40 /* EdCursorDown */, 						1, MOT_TOEND , "cursor-bottom-of-page",
150, C_1FUNC, 40 /* EdCursorDown */, 						1, MOT_WORD , "cursor-down-page",
151, C_1FUNC, 37 /* EdCursorRight */, 						1, MOT_SINGLE, "cursor-right-char",
152, C_1FUNC, 37 /* EdCursorRight */, 						1, MOT_TOEND , "cursor-eol",
153, C_1FUNC, 37 /* EdCursorRight */, 						1, MOT_SPACE, "cursor-right-word",
154, C_1FUNC, 37 /* EdCursorRight */, 						1, MOT_UNTILC, "cursor-until-char",
155, C_1FUNC, 38 /* EdCursorLeft */, 						1, MOT_SINGLE, "cursor-left-char",
156, C_1FUNC, 38 /* EdCursorLeft */, 						1, MOT_TOEND , "cursor-sol",
157, C_1FUNC, 38 /* EdCursorLeft */, 						1, MOT_SPACE, "cursor-left-word",
158, C_1FUNC, 38 /* EdCursorLeft */, 						1, MOT_UNTILC, "cursor-back-to-char",
159, C_1FUNC, 37 /* EdCursorRight */,						1, MOT_SPACE , "cursor-to-space",
160, C_1FUNC, 38 /* EdCursorLeft */, 						1, MOT_SPACE , "cursor-back-to-space",
161, C_1FUNC, 39 /* EdCursorUp */, 							1, MOT_FILE , "cursor-home",
162, C_1FUNC, 40 /* EdCursorDown */, 						1, MOT_FILE , "cursor-eof",
163, C_1FUNC, 19 /* edit_insertLine */, 					1, ELO_CARET_FOLLOWS, "insert-line-follow",
164, C_1FUNC, 14 /* EdLineDelete */, 						1, ELO_CARET_STAYS, "delete-line-stay",
165, C_1FUNC, 14 /* EdLineDelete */, 						1, ELO_APPEND , "delete-line-append",
166, C_1FUNC, 14 /* EdLineDelete */, 						1, ELO_CARET_FOLLOWS, "delete-line-follow",
167, C_0FUNC, 0 /* EdFileAbandon */, 						1, 0 , "discard-changes-in-file",
168, C_1FUNC, 64 /* EdSaveFile */, 							1, SAV_SAVE|SAV_QUIT, "save-and-close",
169, C_1FUNC, 19 /* edit_insertLine */, 					1, 1, "insert-line-stay",
170, C_1FUNC, 111 /* EdListBindings */,                     1, LISTB_MACROC_API, "macroc-api-list",
171, C_0FUNC, 15 /* bl_destroyAll */, 						1, 0 , "delete-named-buffers",
172, C_1FUNC, 20 /* edit_performLineFlagOperation */, 		1, MLN_DELETE, "delete-marked-lines",
173, C_0FUNC, 30 /* EdSetMultiplier */, 					1, 0 , "set-multiplikator",
174, C_0FUNC, 43 /* codecomplete_showSuggestionWindow */, 	1, 0 , "escape-makro",
175, C_0FUNC, 42 /* fkey_keyModifierStateChanged */, 		1, 0, "switch-fkeys",
176, C_0FUNC, 113 /* EdDocTypes */, 						1, 0, "dialog-doc-types",
177, C_1FUNC, 63 /* mainframe_closeChildWindows */, 1,      CWF_EXCLUDE_CURRENT, "close-all-but-current-window",
178, C_1FUNC, 44 /* macro_executeMacroByIndex */, 			1, MAC_AUTO , "re-insert",
179, C_1FUNC, 118 /* caret_moveAndAddSecondary*/, 			1, MSM_WORDS, "secondary-cursor-mark-words",
180, C_0FUNC, 88 /* EdUndo */, 								1, 0, "undo",
181, C_1FUNC, 20 /* edit_performLineFlagOperation */, 		1, MLN_TOGGLE, "toggle-line-marks",
182, C_1FUNC, 85 /* EdWinArrange */, 						1, WIN_DESKICON, "iconize-edit",
183, C_1FUNC, 85 /* EdWinArrange */, 						1, WIN_ICONIZED, "iconize-window",
184, C_1FUNC, 89 /* EdFilesCompare */, 						1, LIST_PREV , "last-difference",
185, C_1FUNC, 90 /* EdScrollScreen */, 						1, MOT_SINGLE, "scroll-up-line",
186, C_1FUNC, 90 /* EdScrollScreen */, 						1, -MOT_SINGLE, "scroll-down-line",
187, C_1FUNC, 90 /* EdScrollScreen */, 						1, MOT_PAGE , "scroll-up-page",
188, C_1FUNC, 90 /* EdScrollScreen */, 						1, -MOT_PAGE , "scroll-down-page",
189, C_1FUNC, 40 /* EdCursorDown */,   						1, MOT_CENTER, "cursor-center-page",
190, C_1FUNC, 91 /* EdScrollCursor */, 						1, MOT_SINGLE, "scroll2-up-line",
191, C_1FUNC, 91 /* EdScrollCursor */, 						1, -MOT_SINGLE, "scroll2-down-line",
192, C_1FUNC, 91 /* EdScrollCursor */, 						1, MOT_CENTER, "scroll-center-page",
193, C_1FUNC, 20 /* edit_performLineFlagOperation */, 		1, MLN_JOIN , "join-marked-lines",
194, C_1FUNC, 63 /* mainframe_closeChildWindows */,         1, CWF_EXCLUDE_PINNED, "close-unpinned-windows",
195, C_1FUNC, 52 /* fm_gotoNextPosition */, 				1, MTE_AUTO_LAST_INSERT, "goto-next-insertion-position",
196, C_1FUNC, 60 /* EdPrint */, 							1, PRT_CLIP , "print-clipboard",
197, C_1FUNC, 60 /* EdPrint */, 							1, PRT_TRASH , "print-trash",
198, C_1FUNC, 10 /* EdBlockPaste */, 						1, PASTE_ICON|PASTE_UNDO, "paste-undo",
199, C_1FUNC, 10 /* EdBlockPaste */, 						1, PASTE_ICON|PASTE_CLIP, "paste-clip-icon",
200, C_0FUNC, 67 /* EdExecute */, 							1, 0 , "icon-execute",
201, C_1FUNC, 63 /* mainframe_closeChildWindows */, 1,      CWF_EXCLUDE_SELECTED, "close-all-but-selected-window",
202, C_1FUNC, 99 /* EdMousePositionUngrabbed */, 			1, 1, "cursor-mouse-goto",
203, C_1FUNC, 97 /* EdMouseMoveText */, 					1, 0 , "drag-copy-text",
204, C_1FUNC, 97 /* EdMouseMoveText */, 					1, 1 , "drag-move-text",
205, C_1FUNC, 98 /* EdMouseSelectLines */, 					1, 1 , "mouse-select-lines",
206, C_1FUNC, 98 /* EdMouseSelectLines */, 					1, 0 , "mouse-unselect-lines",
207, C_1FUNC, 44 /* macro_executeMacroByIndex */, 			1, MAC_LASTREC, "play-last-macro",
208, C_1FUNC, 95 /* EdBlockMouseMark */, 					1, 0 , "mouse-select",
209, C_1FUNC, 96 /* EdMouseMarkParts */, 					1, MOT_TOEND , "mouse-mark-line",
210, C_1FUNC, 96 /* EdMouseMarkParts */, 					1, MOT_WORD , "mouse-mark-word",
211, C_1FUNC, 96 /* EdMouseMarkParts */, 					1, MOT_SPACE , "mouse-mark-to-space",
212, C_1FUNC, 96 /* EdMouseMarkParts */, 					1, MOT_PGRPH , "mouse-mark-paragraph",
213, C_1FUNC, 95 /* EdBlockMouseMark */, 					1, 0, "mouse-update-selection",
214, C_1FUNC, 111 /* EdListBindings */,  					1, LISTB_MACROS, "macro-list",
215, C_1FUNC, 111 /* EdListBindings */,  					1, LISTB_KEYS , "key-list",
216, C_1FUNC, 111 /* EdListBindings */,  					1, LISTB_MICE , "mouse-list",
217, C_1FUNC, 112 /* EdCompileMacros */, 					1, 1, "compile-macros",
218, C_1FUNC, 17 /* EdCharInsert */, 	 					1, 10, "split-line-soft",
219, C_1FUNC, 117 /* EdShowClipboard */, 					1, 0 , "show-main-clipboard",
220, C_1FUNC, 3 /* EdBlockDelete */, 						1, 0, "erase-selection",
221, C_1FUNC, 111 /* EdListBindings */, 					1, LISTB_MENUS, "menu-list",
222, C_0FUNC, 120 /* EdRedo */, 							1, 0, "redo",
223, C_1FUNC, 78 /* EdOptionToggle */, 						1, OPT_DMODE | SHOW_LINENUMBERS, "toggle-show-linenumbers",
224, C_1FUNC, 38 /* EdCursorLeft */, 						1, MOT_SINGLE|MOT_EXTEND_SELECTION, "select-left",
225, C_1FUNC, 38 /* EdCursorLeft */, 						1, MOT_TOEND | MOT_EXTEND_SELECTION, "select-left-to-start",
226, C_1FUNC, 38 /* EdCursorLeft */, 						1, MOT_SPACE | MOT_EXTEND_SELECTION, "select-left-word",
227, C_1FUNC, 37 /* EdCursorRight */, 						1, MOT_SINGLE | MOT_EXTEND_SELECTION, "select-right",
228, C_1FUNC, 37 /* EdCursorRight */, 						1, MOT_SPACE | MOT_EXTEND_SELECTION, "select-right-word",
229, C_1FUNC, 37 /* EdCursorRight */, 						1, MOT_TOEND | MOT_EXTEND_SELECTION, "select-right-to-end",
230, C_1FUNC, 39 /* EdCursorUp */, 							1, MOT_SINGLE | MOT_EXTEND_SELECTION, "select-up",
231, C_1FUNC, 40 /* EdCursorDown */, 						1, MOT_SINGLE | MOT_EXTEND_SELECTION, "select-down",
232, C_1FUNC, 39 /* EdCursorUp */, 							1, MOT_PAGE | MOT_EXTEND_SELECTION, "select-page-up",
233, C_1FUNC, 40 /* EdCursorDown */, 						1, MOT_PAGE | MOT_EXTEND_SELECTION, "select-page-down",
234, C_1FUNC, 121 /* EdSaveAllFiles*/, 						1, 0, "save-all-files",
235, C_1FUNC, 114 /* find_initiateIncrementalSearch */, 	1, 0, "search-incrementally",
236, C_0FUNC, 122 /* ft_cloneWindow */, 					1, 0, "clone-window",
237, C_1FUNC, 123 /* edit_shiftSelection */, 				1, 1, "tab-shift-right",
238, C_1FUNC, 123 /* edit_shiftSelection */, 				1, -1, "tab-shift-left",
239, C_1FUNC, 23 /* ww_zoomWindow */, 						1, 1, "zoom-increase",
240, C_1FUNC, 23 /* ww_zoomWindow */, 						1, 0, "zoom-decrease",
241, C_0FUNC, 124 /* edit_toggle_comment */, 				1, -1, "toggle-comment",
242, C_1FUNC, 52 /* fm_gotoNextPosition */, 				1, MTE_AUTO_LAST_SEARCH, "goto-next-pos",
243, C_1FUNC, 52 /* fm_gotoNextPosition */, 				1, MTE_AUTO_LAST_INSERT, "goto-next-insertion-pos",
244, C_1FUNC, 20 /* edit_performLineFlagOperation */, 		1, MLN_MAKESOFT, "use-linux-lineends",
245, C_1FUNC, 20 /* edit_performLineFlagOperation */, 		1, MLN_MAKEHARD, "use-windows-lineends",
246, C_1FUNC, 73 /* edit_convertCharacterCase */, 			1, CC_UPPER, "char-to-upper",
247, C_1FUNC, 73 /* edit_convertCharacterCase */, 			1, CC_LOWER, "char-to-lower",
248, C_1FUNC, 126 /* compare_navigate*/, 					1, 1, "compare-navigate-next",
249, C_1FUNC, 126 /* compare_navigate*/, 					1, -1, "compare-navigate-previous",
250, C_0FUNC, 127 /* compare_clear*/,    					1, -1, "compare-clear",
251, C_1FUNC, 78 /* EdOptionToggle */, 						1, OPT_WMODE | WM_LINE_SELECTION, "toggle-line-selection",
252, C_1FUNC, 78 /* EdOptionToggle */, 						1, OPT_DMODE | SHOW_SYNTAX_HIGHLIGHT, "toggle-syntax-highlighting",
253, C_1FUNC, 128 /* bl_moveSelectionUpDown */, 			1, -1, "move-selection-up",
254, C_1FUNC, 128 /* bl_moveSelectionUpDown */, 			1, 1, "move-selection-down"
// no more commands can be added: maximum is currently 255.
};

int _commandTableSize = sizeof(_commandTable)/sizeof(_commandTable[0]);

