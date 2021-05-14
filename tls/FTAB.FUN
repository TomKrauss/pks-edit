
/*

# include	"edierror.h"

Function Parameter-List:	( Returnwert - P1 - P2 - .... )
with:
	bool	= bool
	i	= short integer
	I	= long Integer
	S	= string
	E 	= STRING
	N	= force 0-pointer
	W	= window-handle
	number prefix: default value

ExampleFunc(b1i1iS) <==> bool ExampleFunc(int i=1, int j=1, char *S)

*/

# define	Defun(f,flgs,comment,name)	name { f <flgs> comment }

Defun(EdFileAbandon(bool,ABANDONOPT),C,
	 "Liest die aktuelle Datei neu und ignoriert alle ƒnderungen",
	 "abandon-file")
# if defined(_Windows)
Defun(EdAbout(bool),-,
	"Gibt das Copyright aus",
	"about")
# endif
Defun(EdBlockCopy(bool),*CBUm,
	"Kopiert den markierten Textblock zur Cursor-Position",
	"copy-block")
Defun(EdBlockDelete(bool,"int:flag:1 -> sichern auf der Papierkorb - Ablage"),CBUm,
	"Lˆscht den markierten Textblock",
	"delete-block")
Defun(EdBlockFindEnd(bool),CB,
	"Setzt den Cursor auf das Ende des markierten Textblocks",
	"find-block-end")
Defun(EdBlockFindStart(bool),CB,
	"Setzt den Cursor auf den Anfang des markierten Textblocks",
	"find-block-start")
Defun(EdBlockMove(bool),*CBUm,
	"Verschiebt den markierten Textblock zur Cursor-Position",
	"move-block")
Defun(EdBlockRead(bool),CUm,
	"F¸gt eine Datei an der Cursor-Position ein",
	"read-block")
Defun(EdBlockWrite(bool),CB,
	"Sichert den markierten Textblock in Datei..",
	"write-block")
Defun(EdBlockCut(bool,YANKOPT,DIALOGOPT,"int:buffer:Name des Speichers"),CB,
	"Bringt den markierten Textblock zu einer Ablage oder einem Textablagespeicher",
	"yank-block")
Defun(EdBlockPaste(bool,PASTEOPT,DIALOGOPT,"int:buffer:Name des Speichers"),CUm,
	"F¸gt einen Textablagespeicher an der Cursor-Position ein",
	"paste-block")
Defun(EdBlockHide(bool),C,
	"Versteckt den Textblock",
	"hide-block")
Defun(EdBlockMark(bool,"MARKMODE:mode"),C,
	"Setzt Textblock Markierungen",
	"set-block-mark")
Defun(EdLinesYank(bool),CM,
	"Bringt Zeilen zur Ablage",
	"yank-lines")
Defun(EdLineDelete(bool,"LINEDELETEMODE:mode"),CUm,
	"Lˆscht die aktuelle Zeile",
	"delete-lines")
Defun(EdBufferFree(bool),-,
	"Gibt alle Textablagespeicher frei",
	"free-buffer")
# if defined(_Windows)
Defun(EdKeycodeInsert(bool,DIALOGOPT,"int:keycode:als String einzuf¸gender Tastencode"),*CUm,
	"F¸gt einen Tasten-Code als Text an der Cursor-Position ein",
	"insert-keycode")
# endif
Defun(EdCharInsert(bool,"int:character:einzuf¸gendes Zeichen"),*Ccm,
	"F¸gt ein Zeichen an der Cursor-Position ein",
	"insert-char")
Defun(EdFormatText(bool,DIALOGOPT,"FORMATTYP:typ",FORMATOPT,"RANGETYP:range"),CUm,
	"Formatiert einen Textbereich",
	"format-text")
Defun(EdLineInsert(bool,"LINEINSERTMODE:mode"),CUm,
	"f¸gt eine Zeile ein",
	"insert-line")
Defun(EdMarkedLineOp(bool,"MLNTYPE:typ"),CUm,
	"F¸hrt .. auf allen allen markierten Zeilen aus",
	"mark-line-op")
Defun(EdSearchListRead(bool,DIALOGOPT,"string:filename:Name der zu lesenden Datei"),F,
	"Liest eine Suchliste",
	"read-search-list")
Defun(EdErrorListRead(bool,"string:compiler:Compilertyp",DIALOGOPT,"string:filename:Name der zu lesenden Datei"),-,
	"Liest eine Fehlerliste eines Compiler f¸r die Fehlersuche",
	"read-error-list")
Defun(EdLineal(bool,READ_OR_WRITE,DIALOGOPT,"string:filename:Name der Datei"),CF,
	"Sichert/Liest eine Modus - Datei",
	"rw-modus")
Defun(EdMacrosEdit(bool),M,
	"Erlaubt das Starten und Bearbeiten von Makros und Tastenanbindungen",
	"edit-macros")
Defun(EdDocMacrosEdit(bool),-,
	"Editiert weitere Dokumenteinstellungen f¸r den aktuellen Typ",
	"edit-doc-macros")
Defun(EdDocMacrosAdd(bool,DIALOGOPT,"string:filename:Name der Datei"),F,
	"F¸gt weitere Dokumenteinstellungen einer Modusdatei hinzu",
	"merge-doc-macros-with")
Defun(EdMacrosReadWrite(bool,READ_OR_WRITE,DIALOGOPT,"string:filename:Name der Datei"),F,
	"Sichert/liest eine Makro-Datei",
	"rw-macros")
Defun(EdTagfileRead(bool,DIALOGOPT,"string:filename:Name der Datei"),F,
	"Liest die Quer-Verweis-Datei",
	"read-tagfile")
#if defined(_Windows)
Defun(EdSetup(bool,DIALOGOPT,"string:filename:Name der Datei"),-,
	"Sichert/liest sonstige Einstellungen",
	"rw-setup")
#else
Defun(EdSetup(bool,READ_OR_WRITE,DIALOGOPT,"string:filename:Name der Datei"),-,
	"Sichert/liest sonstige Einstellungen",
	"rw-setup")
#endif
Defun(EdSetMultiplier(bool,DIALOGOPT,"int:mult:Multiplikatorwert"),-,
	"Setzt den Multiplikator f¸r die Ausf¸hrung der n‰chsten Funktion",
	"set-multiplier")
Defun(EdReplaceTabs(bool,"CONVTABSACTION:typ",DIALOGOPT,"RANGETYP:range"),CUm,
	"Wandelt alle Tabulatoren in Leerzeichen oder vc. vs.",
	"replace-tabs")
Defun(EdLineSplit(bool,LINESPLITOPT),CUm,
	"Spaltet die Zeile an der Cursor-Position in 2 Zeilen auf",
	"split-line")
Defun(EdChapterGotoBegin(bool,DIRECTION),CM,
	"Bewegt den Cursor zum Absatzanfang",
	"goto-chapter-start")
Defun(EdChapterGotoEnd(bool,DIRECTION),CM,
	"Bewegt den Cursor zum Absatzende",
	"goto-chapter-end")
Defun(EdParaGotoBegin(bool,DIRECTION),CM,
	"Bewegt den Cursor zum Paragraphenanfang",
	"goto-pgrph-start")
Defun(EdParaGotoEnd(bool,DIRECTION),CM,
	"Bewegt den Cursor zum Paragraphenende",
	"goto-pgrph-end")
Defun(EdCursorRight(bool,CURSORMOTION),C,
	"Bewegt den Cursor nach rechts",
	"cursor-right")
Defun(EdCursorLeft(bool,CURSORMOTION),C,
	"Bewegt den Cursor nach links",
	"cursor-left")
Defun(EdCursorUp(bool,CURSORMOTION),CM,
	"Bewegt den Cursor nach oben",
	"cursor-up")
Defun(EdCursorDown(bool,CURSORMOTION),CM,
	"Bewegt den Cursor nach unten",
	"cursor-down")
Defun(EdCharDelete(bool,CURSORMOTION),CUm,
	"Lˆscht .. Zeichen an der Cursor-Position",
	"delete")
Defun(EdFkeysSwitch(bool,FKEYSPARAM),-,
	"Steuert die Funktionstastenanzeige",
	"switch-fkeys")
Defun(EdMacroEscape(bool,DIALOGOPT,"int:id:Kennung des Bausteins"),FCUm,
	"F¸gt einen Textbaustein ein",
	"insert-text")
Defun(EdMacroPlay(bool,MACROPLAYTYP),Nc,
	"F¸hrt ein Makro aus",
	"play-macro")
Defun(EdMacroRecord(bool),N,
	"Startet eine Makro-Aufnahme",
	"record-macro")
Defun(EdFindInFileList(bool,DIALOGOPT,
	"string:finds:Suchstring",
	"string:plist:Pfadlist f¸r die Suche",
	"int:once:einmal suchen ?",
	"int:depth:Suchtiefe in Ordnern",
	FINDOPT),F,
	"Sucht nach Suchmustern in Ordnern",
	"find-in-file-list")
Defun(EdFind(bool,DIALOGOPT,string,"DIRECTION:dir","RANGETYP:range",FINDOPT),CF,
	"Sucht nach Suchmustern",
	"find")
Defun(EdReplace(bool,DIALOGOPT,string,string,REPLACTION,RANGETYP,FINDOPT,REPLOPT),FCm,
	"Ersetzt Text",
	"replace")
Defun(EdFindAgain(bool,DIRECTION),C,
	"Wiederholt die letzte Suche",
	"find-again")
Defun(EdReplaceAgain(bool),Cm,
	"F¸hrt die letzte Ersetzung erneut durch",
	"replace-again")
Defun(EdCharControlInsert(bool,DIALOGOPT,"int:character:einzuf¸gendes Zeichen"),FCcm,
	"F¸gt ein Controlzeichen mit Fomular ein",
	"insert-control")
Defun(EdGotoLastPos(bool,"GOTOTEXTMARKACTION:typ"),C,
	"Geht zur letzten .. Position des Cursors",
	"goto-lastpos")
Defun(EdGotoLine(bool,DIALOGOPT,"int:number:Zeile"),C,
	"Geht zu Zeile..",
	"goto-line")
Defun(EdMarkSet(bool,DIALOGOPT,"int:mark:Kennbuchstabe der Marke"),FC,
	"Setze eine Text-Marke",
	"set-mark")
Defun(EdMarkGoto(bool,DIALOGOPT,"int:mark:Kennbuchstabe der Marke"),FC,
	"Geht zu einer Text-Marke",
	"goto-mark")
Defun(EdHelp(bool,"int:which:Hilfen-Eintragsnummer"),-,
	"Hilfen zu ...",
	"internal-help")
Defun(EdLinesJoin(bool),CUm,
	"F¸gt Zeilen zusammen",
	"join-lines")
Defun(EdEditFile(bool,OPENOPTION,DIALOGOPT,"string:filename:Dateiname"),F,
	"÷ffnet eine Datei zur Bearbeitung",
	"edit-file")
Defun(EdOptionSet(bool,DIALOGOPT,"OPTIONS:options","LAYOUTOPTIONS:layout"),F,
	"Stellt Optionen ein",
	"set-options")
Defun(EdPrint(bool,PRINTOPTION,"string:filename:Name der Datei"),-,
	"Druckt...",
	"print")
Defun(EdExitAndSave(bool,int),-,
	"Beendet PKS-EDIT mit Sichern aller geˆffneten Dateien",
	"exit-edit")
Defun(EdExit(bool,int),-,
	"Beendet PKS-EDIT ohne Sicherung aller geˆffneten Dateien",
	"quit-edit")
# if defined(_Windows)
Defun(EdCloseAll(bool,"int:flag:Icons schlieﬂen?"),-,
	"Schlieﬂt alle Fenster",
	"close-all-windows")
# endif
Defun(EdSaveFile(bool,SAVEOPTION,DIALOGOPT,"string:filename:Dateiname f¸r Sichern als.."),C,
	"Sichert die aktuelle Datei und/oder schlieﬂt das Bearbeitungs-Fenster",
	"save-file")
Defun(EdSelectWindow(bool,SELWINDOWACTION),-,
	"Wechselt zu Fenster ...",
	"select-window")
Defun(EdCommandExecute(bool,DIALOGOPT,
	"string:prog:auszuf¸hrendes Programm",
	"string:params:Parameter",
	"string:errfile:Fehlerliste die nach Programmende gelesen wird",
	 EXECOPT),F,
	"F¸hrt ein Kommando aus",
	"execute-command")
# if defined(_Windows)
Defun(EdExecute(bool, DIALOGOPT, "string:cmd", "string:dir", "string:errlist", 
	EXECOPT),F,
	"Ausf¸hren Programm",
	"exec")
# endif
Defun(EdShiftBetweenBrackets(bool,DIRECTION),CUm,
	"Verschiebt einen Klammernblock",
	"shift-between-brackets")
Defun(EdSort(bool,DIALOGOPT,
	"string:sel:Suchmuster fÅr Selektion",
	"string:fs:Feld-Trenner",
	"string:fields:SchlÅsselfelder","RANGETYP:typ",SORTOPT),Cm,
	"Sortiert einen Tetxbereich",
	"sort")
Defun(EdLinesShift(bool,"DIRECTION:dir"),CMUm,
	"Shiftet (n) Zeilen",
	"shift-lines")
Defun(EdInfoFiles(bool),-,
	"Gibt Infos zur aktuellen Datei",
	"show-info")
Defun(EdShowMatch(bool),C,
	"Pr¸ft Klammernhierarchieen",
	"check-brackets")
Defun(EdCharUpToLow(bool),CUm,
	"Wandelt Groﬂ- in Kleinschreibung und vc. vs.",
	"up-to-low")
# if defined(_Windows)
Defun(EdDlgWorkMode(bool,DIALOGOPT,string,"WORKMODE:mode"),C,
	"Setzt den Arbeits-Modus",
	"set-work-mode")
Defun(EdDlgDispMode(bool,DIALOGOPT,"string:alt_statusline",
	"int:tabstops","int:rmargin","int:tabfiller","int:bg_color", 
	"SHO_MODE:mode"),C,
	"Setzt den Anzeige-Modus",
	"set-disp-mode")
Defun(EdDlgCursTabs(bool,DIALOGOPT,"int:scrollmin","int:scrolldelta",
	"CPS_AFTER:position","SCROLLOPT:flags"),C,
	"Bestimmt das Cursor-Verhalten",
	"set-cursor-mode")
Defun(EdDlgModeVals(bool,DIALOGOPT,"string:bak_extension",
	"int:nl1","int:nl2","int:cr"),C,
	"Stellt das Dateiformat ein",
	"set-file-format")
# else
Defun(EdLinealEdit(bool),CF,
	"Textmodi...",
	"set-text-mode")
# endif
Defun(EdOptionToggle(bool,TOGGLE),*C,
	"Schaltet eine Option (ein/aus)",
	"toggle-text-mode")
Defun(EdPasteString(bool,"string:text:einzufÅgender Text"),C,
	"FÅgt einen String mit gegebener LÑnge ein",
	"insert-string")
Defun(EdFindTag(bool,"int:helpflg:1 fÅr Hilfen-Verweis",DIALOGOPT,"string:search:Verweisname"),F,
	"Geht zu Quer-Verweis ..",
	"find-tag")
Defun(EdFindFileCursor(bool),C,
	"Sucht die Datei unter dem Cursor",
	"cursor-find-file")
Defun(EdErrorNext(bool,LISTGOTOMODE),-,
	"Geht zu Compiler-Fehler..",
	"find-error")
Defun(EdFindTagCursor(bool),C,
	"Sucht den Quer-Verweis unter dem Cursor",
	"cursor-find-tag")
Defun(EdFindWordCursor(bool,DIRECTION),C,
	"Sucht ein Wort unter dem Cursor",
	"cursor-find-word")
# if defined(_Windows)
Defun(EdWinArrange(bool,WINARRANGEACTION),-,
	"Stellt Fenster ein ...",
	"window-arrange")
# else
Defun(EdWinArrange(bool,WINARRANGEACTION),C,
	"Stellt Fenster ein ...",
	"window-arrange")
# endif
Defun(EdWindowRegSet(bool,int),C,
	"Definiert ein Fenstergîûen-register...",
	"set-window-register")
Defun(EdRangeShift(bool,"DIRECTION:dir",DIALOGOPT,"RANGETYP:range"),CUm,
	"Verschiebt einen Textbereich",
	"shift-text")
Defun(EdUndo(bool),Cm,
	"Macht die letzte ƒnderung r¸ckg‰ngig",
	"undo")
Defun(EdFilesCompare(bool,DIRECTION),C,
	"Vergleicht die beiden obersten Bearbeitungsfenster",
	"compare-windows")
Defun(EdScrollScreen(bool,CURSORMOTION),C,
	"Rollt den Bildschirm",
	"scroll-screen")
Defun(EdScrollCursor(bool,CURSORMOTION),C,
	"Rollt den Bildschirm, der Cursor folgt",
	"scroll-cursor")
Defun(EdGotoPreviousTag(bool),-,
	"Geht zum letzten Verweis",
	"goto-lasttag")
Defun(EdAlignText(bool,DIALOGOPT,
	"string:pattern:auszurichtendes Muster als RE",
	"int:filler:FÅllzeichen",
	"RANGETYP:range:auszurichtender Bereich",
	"FINDOPT:foptions:Such-Optionen",
	"ALIGNOPT:options:Optionen fÅr die Ausrichtung"),CUm,
	"Richtet Texte aus",
	"align-text")
#if !defined(_Windows)
Defun(EdModuleExec(bool,MODULTYP),-,
	"Aufruf externe Module",
	"exec-module")
# endif
Defun(EdIconsQuit(int),-,
	"Schlieﬂt Sinnbilder",
	"quit-icons")
Defun(EdBlockMouseMark(bool,MARKMODE),C,
	"Markiert den Textblock mit der Maus",
	"mouse-mark-block")
Defun(EdMouseMarkParts(bool,CURSORMOTION),C,
	"Markiert Textteile mit der Maus",
	"mouse-mark-parts")
Defun(EdMouseMoveText(bool,MOUSEMOVEACTION),C,
	"Bewegt Text mit der Maus",
	"mouse-move-text")
Defun(EdMouseSelectLines(bool,SELMODE),C,
	"Selektiert Zeilen mit der Maus",
	"mouse-select-lines")
Defun(EdMousePositionUngrabbed(bool,"int:grab:Mousezeiger grab aktiv"),C,
	"Positioniert den Cursor mit der Maus",
	"mouse-position")
Defun(EdAlert(bool,"string:text:Hinweis-Text"),-,
	"Gibt eine Warnmeldung aus",
	"alert")
#if defined(_Windows)
Defun(Form_Alert(int,"int:style:Stil gem‰ﬂ SDK - API","int:unused",
		"string:text:Meldungstex wird umgebrochen"),-,
	"Erzeugt eine Windows - MessageBox - Titel nicht einstellbar",
	"message-box")
#else
Defun(Form_Alert(int,"int:def_but:Default-Button",
		FAOPT,"string:text:Text im GEM-form_alert-Format"),-,
	"Erzeugt ein Formular mit maximal 3 Knîpfen zur Auswahl von Alernativen",
	"form-alert")
#endif
Defun(EdPromptAssign(string,"string:prompt","string:init:Default-Belegung"),-,
	"Erzeugt ein Formular zur Eingabe eines Wertes",
	"prompt-assign")
Defun(EdFormatPrint(string,"string:format","string:param:String-Parameter!"),-,
	"Formatiert maximal einen Wert (a la Statuszeile)",
	"format-print")
Defun(EdGetSelectedText(string),-,
	"Liefert den aktuell selektierten Text",
	"get-selected")
Defun(EdHideLines(bool),CUm,
	"Versteckt die Zeilen des aktuellen Textblocks",
	"hide-lines")
Defun(EdUnHideLine(bool),CUm,
	"Macht die versteckten Zeilen unter dem Cursor wieder sichtbar",
	"un-hide-line")
Defun(EdStringSubstitute(string,"int:max:Maximale Anzahl",
	"FINDOPT:foptions:Such-Optionen","string:what:zu ersetzender String",
	"string:pattern:Suchmuster","string:with:Ersatz"),-,
	"Ersetzt innerhalb von Strings",
	"replace-in-string")
Defun(EdExpandAbbreviation(bool),-,
	"Expandiert die links vom Cursor stehende Abk¸rzung",
	"expand-abbreviation")
# if defined(_Windows)
Defun(EdConfigureIcons(bool,DIALOGOPT,int),-,
	"Installiert Desktop Icons",
	"configure-icons")
Defun(EdHelpContext(bool,int),-,
	"Gibt Hilfe zu Arbeitssituation...",
	"internal-help-context")
Defun(EdListBindings(bool,"LISTOPT:which:Art der Liste"),-,
	"Erzeugt eine Liste aller Tastenanbindungen",
	"list-bindings")
Defun(EdCompileMacros(bool, "int:flags:1 = Fensterliste anzeigen"), C,
	"Compiliert den Text des aktuellen Fensters",
	"compile-macros")
Defun(EdDocTypes(bool),-,
	"Definiert Dokument Typen",
	"define-doc-types")
Defun(EdPrinterLayout(bool),-,
	"Stellt Optionen und Layout f¸rs Drucken ein",
	"set-printer-layout")
Defun(EdIsDefined(bool, "QUERY:what"),-,
	"Bestimmt, ob bestimmte Einstellungen/Zust‰nde aktiv sind",
	"query-status")
Defun(EdCallWinHelp(bool, string, int, int),-,
	"Ruft die Windows Hilfe Funktion",
	"windows-help")
Defun(EdShowClipboard(bool, "int:Id der anzuzeigenden Ablage"),-,
	"Ruft CLIPBRD.EXE zur Anzeige der Ablage oder eines Text-Buffers",
	"show-clipboard")
Defun(EdMenuTrackPopup(bool, "int:unused", "int:unused", 
	"string:Name des Popup Menus"),C,
	"÷ffnet ein Popup Menu",
	"track-popup")
Defun(EdBlockXtndMode(bool, "int:typ:Erweiterungsflag"),C,
	"Schaltet den Block-Erweiterungs Modus an oder aus",
	"set-block-extension-mode")
# endif

