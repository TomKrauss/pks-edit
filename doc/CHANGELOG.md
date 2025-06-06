# Changelog

All notable changes to PKS Edit will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [2.11.0] - unreleased

### Improved

### Fixed
- Window Activation when restoring history file was sometimes wrong in combination with Wysiwyg windows.

## [2.10.1] - 2025-03-30

Release 2.10.1 is the huge file release fixing some bugs and some performance bottlenecks when working
with huge files (> 1 Mio lines).

### Improved
- We greatly optimized caret navigation and scrolling performance in big,big files (more than 100000 lines).
  Navigating to a new line in a file with 4.000.000 lines of code took about 400ms on a good machine before
  and is now as fast as navigating in a small file.
- PKS Edit will show a progress dialog now, when large files are being read from disc.

### Fixed
- A bug was fixed, which could cause "Reload File" to crash, when there is a non empty selection in a file.
- A bug was fixed causing PKS Edit to crash on the first macro execution after a big, big file had been
  loaded causing an internal pointer overflow in the macro execution engine.
- Fixed an endless loop, when one attempted to add secondary carets to empty match positions (e.g. pressing
  Chift+Ctrl+L on an empty line).

## [2.10.0] - 2024-10-30

### Added
- The new PKS Edit MacroC function `EditorSetCaret` can be used to set the input caret in a document.
  Some additional macros available by default to position to the first non whitespace character or after
  the last non whitespace character in the current line were added.
- The new PKS Edit MacroC type `CARET` allows to work with caret positions in PKS MacroC. 
- Added simple support for Wysiwyg rendering of documents formatted using [AsciiDoc](https://docs.asciidoctor.org) .

### Improved
- Searching in Files is much faster now, when the option `ignore binary files` is selected. PKS-Edit will
  skip in this case .git - folders and build folders resulting in some cases in a performance improvement
  by a factor of more than 100.
- PKS MacroC `for to loops` have been improved and allow now for using previously defined variable references.
  This allows for expressions as in the following example:
```c
int i = 0;
int length = EditorGetLineLen(-1);
for (i : 0..length) {
	c = text[i];
...
```

### Fixed
- A bug in the PKS Edit MacroC interpreter was fixed, causing branch instructions to become corrupted,
  when the MacroC bytecode was optimized. 
- A bug was fixed causing text copied to the clipboard not being preserved, when closing PKS Edit.
- The line number area did not correctly resize / redraw, when the number of lines in a file changed,
  so that resizing of the area was necessary.
- Windows were not correctly activated from a saved PKS Edit session, when more than one window dock were
  being displayed at the same time.
- Copying text to the system clipboard did not work, if a piece of text was copied from the last line of a file,
  when this line was not line terminated.

## [2.9.0] - 2024-09-02

### Changed
- PKS Edit will now render Cr+LF lineends as two characters, when the `showcontrol` option is selected.
- PKS Edit will display also control chars (end of line) selected in case a selection is present to better 
  provide selection feedback on empty lines.

### Fixed
- Leaving the incremental search field by pressing the `ESCAPE` key did not correctly focus the editor
  if the incremental search field had been activated by a mouse click rather than by pressing `Ctrl+Alt+S`.
  As a consequence one could not start typing right away but had to click into the editor again.
- Replace with option "Confirm Replacement" sometimes used the wrong replacement text. This is fixed.
- A performance problem during code completion caused by an inefficient implementation of a Levenshtein
  comparison, which could cause PKS Edit appear to hang for certain completions was fixed.
- Code completion has been improved in ignoring tag sources for languages not defining a tag source.

## [2.8.1] - 2024-09-02

### Fixed
- A bug in the user mode install routine was fixed, causing an error message during the attempt to create some
  registry keys.
- The background of lists in non dark mode was corrected.

## [2.8.0] - 2024-09-01

### Changed
- The Option _Save Settings on Exit_ was eliminated, as it was useless and even dangerous: settings are already saved 
  as soon as they are changed in the configuration dialog. Saving them again on exit could clobber information manually 
  added to `pkseditini.json`.

### Added
- JSON Code Completion support was added in particular for PKS Edit configuration files.
- The Code Completion window shows now supporting graphical information like the color of an inserted color name or
  the emoji to insert into a markdown document or the FontAweSome icon, when inserting an icon name into a configuration
  file.
- The new function `reload-themes` (by default bound to `Ctrl+F10`) makes theme updates and editing easier.
- File Templates: when creating files with a grammar defining a `file` - template will have the template being inserted, when the new file is created.
- The Markdown Wysiwyg mode supports now caret line highlighting.
- Support for inserting `copyright` templates was added. One may define copyright templates in a file named `copyright_profiles.json`
  along with options, how they should be formatted (using single line or multiple line comments). Copyrights are
  currently only supported in files having a grammar defining a language supporting comments.
- A generic exception handler will now catch errors, which occur in editing functions and allow to more gracefully
  exit PKS Edit in case of an internal error.
- Document Types may be hidden now in the file selector. This will cause the extension of the corresponding document
  type not being added to the list of selectable file extensions in file dialogs (to keep this list clear).
- Grammars for Docker (Dockerfile) and the languages Assembly, Julia, Rust, Cobol, Kotlin, Fortran, PHP and Swift were added.

### Improved
- The Align Text function is more intuitive to use now.
- Most dialogs support now context sensitive help. The PKS Edit manual was improved to be able to support this.
- The Code Evaluation menu is now only enabled, if code evaluation is really supported. Code evaluation of PKSMacroC
  code handles now more szenarios of selected text.
- When opening compilation or build output files, PKS Edit will now auto-detect the format to extract the information to navigate
  the problems found in the build output. Additional well known build output formats (Java, Typescript, Dart) have been
  added to the list of formats supported out of the box.
- XML code completion was improved: entries are sorted alphabetically, attribute suggestions are provided based on
  the current entity.
- The new function "New File - Select Name..." allows to select the file name when creating a file. 
- PKS Edit better supports now to work with multiple `tags` files (generated by [CTAGS](https://github.com/universal-ctags/ctags)).
  It will automatically scan the parent directories of a file in which cross references are searched for an existing tag
  file and will maintain tag indices per root directory. Also error handling (like out of date detection and illegal format)
  have been improved.
### Fixed
- A potential crash was fixed, which occurred, when opening a dialog displaying the *currently selected text* (e.g.
  the File & Replace dialog), when multiple lines of text were selected and the column of the selection end was less
  than the column of the selection start.
- A potential crash in the MacroC compiler was fixed. Some memory leaks were fixed as well, that occurred if macro
  compilation resulted in one or more errors.
- Opening build results **and** a PKS Edit search result list at the same time will now work correctly correctly.
  Pressing `Shift+Ctrl+N` will then navigate the list of results of the corresponding active search
  list. Search Lists and Build Log Windows are restored, when PKS Edit will be opened again.
- #21: wrong markdown rendering for `#HEADER` fixed.
- PKS Edit stored the _autosave temp file path_ in `pkseditini.json`, even if that was the default path from the
  user home directory, which could cause various problems like the failure to edit files
  without annoying error messages, when different users are working with PKS Edit.
- A potential crash was fixed, when trying to move a selected piece of text to a target position
  being inside the selected text to move.
- Files with very long file names (excluding directory name) could not be opened / displayed correctly and could
  cause the GIT integration to crash. This is fixed.

## [2.7.0] - 2024-07-05

### Added
- A Grammar for `Terraform Config Files` has been added.
- A Grammar for `Maven Files` with some explicit Maven support (custom templates etc...) has been added.
- The new action `Open Wysiwyg Window` allows for opening a window in wysiwyg mode, which will follow your
  caret in the editing window allowing for a better preview experience during editing text with a wysiwyg
  preview (e.g. markdown).

### Improved
- HEX editing has been improved. Caret movement works now as expected. There are still some remaining open issues
  regarding modification of the underlying data.
- PKS Edit correctly enables now menu items and option buttons depending on whether a display or edit mode
  is supported.
- Additional File types and grammars (e.g. explicit support for Maven pom files or Terraform config files) were added.

### Fixed
- We fixed the missing repaint of the line number area, when lines were modified.
- The Open File dialog is now correctly opened as child of the PKS Edit main window.
- Pressing backspace does not trigger template expansion any more.
- The active tab was sometimes not correctly visible - in particular after opening PKS-Edit
  from the command line passing the name of a file to be edited and having multiple windows
  open already.
- We fixed garbled messages displayed in confirmation boxes for messages referring to file names containing 
  umlaut characters.
- Changing the display/edit settings of a file results in saving `pkseditconfig.json`. When one
  defined a **line separator** or **cr** being 0 in `pkseditconfig.json`, this setting was lost during save.
- Using `${caret}` in a template sometimes resulted in a wrong caret position, when the corresponding
  template was inserted.
- A possible crash was fixed, when trying to format a file with a single line. In that case, the EOF-marker
  was removed and further processing could cause the crash.

## [2.6.0] - 2024-06-06

### Added
- PKS Edit was integrated with libgit2 and provides now some basic GIT information about the files edited.
- The new command line argument -v can be used to turn on verbose logging to a log file named `PKS_SYS\pksedit.log`.
- Change Document Type... allows now to correctly apply a selected document type to an arbitrary file selected
  allowing to e.g. change the Grammar used etc...

### Improved
- One may now place config files into a `.pksedit\config` directory located in `$HOME` and extend the installed configuration
  files (e.g. `pksactionbindings.json`). This will allow to override or extent key bindings, add new menu entries
  etc. If you place the same files into `.pksedit\pks_sys`, they will completely override the default files provided in the PKS Edit
  installation. Note, that in the later case, you should review overridden files, when installing a new version of PKS Edit.
- Find & replace will now always be initialized with patterns searched in previous PKS Edit sessions. One
  can for instance use find again without opening the find dialog.
- Some language related inconsistencies were corrected (sometimes the wrong language was used in some places)
  and changing the language works now without restarting PKS Edit.
- captures are now supported in nested patterns in a grammar leading to improved syntax highlighting support.
- Statusline Configuration is now more flexible and more intuitive. The individual segments can be configured individually
  may also contain icons (with an optiona color) and may tagged with a locale for I18N.
- Repaint during resize of mainframe areas has been removed and does not cause flicker any more.
- Text output consumed and inserted into PKS Edit from programs started with _Execute Program..._ will now use the
  correct character set for insertion. Futhermore PKS Edit will now detect, whether a program must be started using
- powershell or using the Windows cmd shell.

### Changed
- The action `delete-selection` was renamed to `cut-to-clipnoard`.

### Fixed
- Scrollbars were sometimes not correctly update in markdown wysiwyg editor windows.
- The Open Dialogs were using the wrong code page resulting in bad display of Umlauts (e.g. Datei �ffnen)
  in the dialog title area and othr places.
- Editor Tabs were displaying filenames containing Umlauts incorrectly.
- markdown rendering: space handling at end of line corrected.
- Print File did crash (while print current window worked) - this is fixed.
- A possible crash in Format File was fixed, when a range of global had been specified.

## [2.5.0] - 2024-05-25

### Added
- A grammar definition for `.arb`-files (Flutter Application Resource Bundles) was added.
- An image wysiwyg renderer mode was added allowing to display an image as such (currently only implemented for SVG).
- InnoSetup Script Grammar has been added.
- BOM Analysis to detect code pages added.

### Improved
- A more modern version of the Windows open and save dialog was integrated into PKS Edit.
- Markdown Wysiwyg rendering supports now:
	- [reference-style links](https://daringfireball.net/projects/markdown/syntax#link).
	- Images with hyperlinks
	- Rendering emojis. Only a small sub-set of Emojs is supported for now.
	- Image references using http or https URLs.
	- Simple SVG and more image formats. Note that we use D2D for SVG rendering which has a very 
	  limited feature set implemented (no CSS styling, no text elements, etc...).
	- Caching loaded images, in particular useful when viewing markdown documents with many online image references.
- We added markdown cheatsheet templates, which can now be accessed via code completion (Ctrl+SPACE by default).
- Templates in grammars support now an optional "help" section allowing to add a help text for the template to insert.
- Code completion works better now when matching "non-word" patterns like ":+xxx". Before, the ":+" was not
  considered in calculation of matching completion candidates and was not correctly replaced, when inserting
  the template / completion result.
- One may define the default display mode, in which a file is opened (currently hex, default or wysiwyg)
  in the grammar for the file now.

### Fixed
- The links to release notes from the tool bar and menu bar had been broken.
- The main windows resize cursors did not work correctly. There was no top window border resize possible 
  neither a right bottom diagonal windows resize mouse pointer shown.
- Converting line ends to Linux (\n) or Windows (\r\n) style did not cause the screen being correctly
  repainted.
- Cancelling out the of the process of quitting PKS Edit works now correctly, when multiple open files
  are changed. Before pressing the "Cancel" button caused other confirmation dialogs popping up for sub-sequent
  changed windows.
- The stack size of PKS Edit configured by default was too low causing in particular the open dialog
  to die on some machines. **kudos** and a big thank you to [RBri](https://github.com/rbri) for helping to debug and finding the solution!
- Context menus on tab buttons did refer to the "wrong" tab button / sometimes it was not able to open a context
  menu on a tab button (tabs displaying the individual editors), as the X coordinates of the events had been interpreted
  in a wrong way.

## [2.4.0] - 2023-07-09

### Added
- Markdown rendering supports now syntax highlighting in fenced code blocks.
- Grammar files for editing yaml format were added.
- A Grammar may define now multiple alternative language names for languages to be matched by the grammar. This is in particular
  useful when referring to a language from a fenced code block. The alternate languages are specified on the corresponding document
  types.
- Zoom plus and zoom minus icons have been added to the toolbar. Action Bindings to Ctrl++ and Ctrl+- were also added. Before
  one needed to use the num block for zooming.
- Icons were updated to FontAwesome Version 6 introducing new icons. The outline version of an icon can now be used as well (if available in the
  free FontAwesome font) by using a suffix of `-o` - e.g. `fa-file-o` for the outline version of the file icon.
- XML code completion was improved.
- _Groovy_ and _Gradle_ syntax have been added.
- _Sysconfig_ syntax has been added to better support hosts, protocol, network ... configuration files.
- Grammar files have a more meaningful description now. The grammar description of a file is displayed now in the window list.
- New option `Silently reload files changed externally`, which will avoid the confirmation dialog being displayed before
  reloading a file changed externally (but of course not in state modified).

### Changed
- The "captures" syntax in the syntax highlighting grammar was changed to use "name" instead of "pattern" as a name
  of the matched pattern for better compatibility with TextMate grammars.
- Various commands have been renamed to enforce a better and more common vocabular. Instead of using `block` to refer to
  the current selection we use `selection` now for instance.

### Fixed
- Wrong toolbar size after maximizing / restoring top level window of PKS Edit was fixed.
- If PKS Edit was called from the command line passing a file name as an argument, the corresponding file could not be found,
  if the file name specified container a "/" or a "\" character (e.g. `..\test.c`). This if fixed.
- Printing did not use the correct print mode (markdown format / color coding / ...) of the active window, if multiple windows were opened for
  a single file.
- Markdown rendering of images will now load absolute image pathes as well. Also URL encoded URLs will work in Markdown code.
- When opening multiple files so that not all file tabs are shown having a tab being selected, which needs to be scrolled into view, this
  tab was not initially visible on HiDPI devices. This is fixed.
- Fixed selection not being properly extended sometimes, when `Shift+PageUp` / `Shift+PageDown` is pressed.
- A bug was fixed causing the toolbar to show a strange layout after restoring PKS Edit from icon size.
- A repaint bug was fixed causing a strange window layout after the toolbar icon size was changed through the 
  settings dialog.
- PKS Edit detects an illegal configuration now (directory PKS_SYS and contained files not found) and aids the
  user in fixing up the configuration.
- Backup Files are created by copying the source file rather than moving the original file to the backup location to avoid potential data loss when creating
  new files will finally fail.
- When opening a file with PKS Edit using "Open with..." from the Windows menu, PKS Edit did not "guess" the code page to use.
  So opening an UTF-8-File did not work "out of the box" (while opening it using drag&drop into the editor had worked). This
- inconsistency is fixed.
- Creating new files will now default to current directory (rather than to directory `PKS_SYS`).

## [2.3.0] - 2023-03-23

### Added
- The new `editing mode` __correct capitals__ allows to auto-correct lowercase / uppercase spelling as you type.
  It will automatically correct words not written in upper case at the beginning of sentences and words containing
  multiple upper case characters at the beginning (such as CApital) to be spelled correctly with respect to upper
  case lower case spelling. The editing mode is enabled by default for all text document types (readme, .txt, .md, ... files).
  Of course this can be changed in the configuration.
- The new option `Reuse running PKS Edit instance` allows to force a single running PKS Edit instance editing multiple
  files opened from e.g. the Windows Explorer. 
- PKS Edit supports high dpi devices now and will scale appropriately. It will also dynamically adapt to dpi changes.
- PKSEDIT-15: When opening a file from the history list, which has been deleted or moved since the last PKS Edit session,
  only a small statusbar message is being displayed rather than annoying blocking dialogs.
- PKSEDIT-13: When opening a file, the file selector will by default open in the directory of the currently active
  editor. No pathes are stored any more in the history file, but pathes are derived also from the list of open files 
  (e.g. to be selected in Find in Files).
- When searching simple regular expressions PKS Edit uses the [Boyer Moore algorithm](https://de.wikipedia.org/wiki/Boyer-Moore-Algorithmus) 
  now internally speeding up the performance of simple searches considerably.
- The tag search list ui was overhauled and uses a Windows list view now.

### Fixed
- A repaint bug in main frame tabs fixed causing repaint artifacts being visible after dragging the splitter.
- causing artifacts in tab bar
- Scrolling and line number display in hex editor mode were corrected. The caret did move outside the visible area before
  and line numbers were not displayed up to the last visible line in hex editor mode.
- Convert spaces to tabs had been broken and is fixed.
- A crash in the markdown renderer was fixed, that occurred when using \[](imagename) as the syntax for including
  images (empty image description).
- Various markdown parsing and rendering issues affecting the display of `fenced code blocks` were fixed.
- A crash which could happen upong the attempt to follow a link in large markdown documents with links in tables was fixed.
- An issue was fixed which had lead to corrupted macros. PKS Edit had crashed when playing the corrupted macros.
- Strings containing some special characters not supported by Windows (characters 129, 141, 143, 144, and 157), were
  painted _wrong_ leading to misplaced carets making editing impossible. As a work around PKS Edit will now display a '?'
  to represent these characters.
- Several issues with long file names were fixed: opening files from the command line or by drag&drop onto PKS Edit with
  path names longer than 256 characters was not possible in elder releases.
- Some dark theme related issues were fixed. List Views for instance are now correctly displayed also in dark mode.
- Fullscreenmode did produce a strange screen layout, if more than 1 `mainframe dock` was open (one dock was using 
  fullscreen layout, the others not). Now fullscreen mode will only maximize one tab hiding the tabstrip, if 
  only one dock is being displayed.

## [2.2.0] - 2022-06-11

This release features __multi-caret__ editing and a major update of the PKSMacroC language and related tools.
Wysiwyg rendering of markdown documents and navigating in wysiwyg windows has been improved. Printing of
formatted content (Wysiwyg markdown and HTML documents) and in Hex mode is now supported.

#### Added
- PKS Edit supports now __multi-caret__ editing. Multi-caret editing is started by placing additional carets
  with the mouse (`Alt+Click`) or by inserting a template which uses multi-caret input. In multi-caret mode all character inserts
  and character deletes (DEL + Backspace) are concurrently applied to multiple places in the code concurrently.
- Errors in macroc code can now be debugged in a simple debugger allowing to inspect the stack frame, global and local 
  variables of each function executed. The debugger supports to step, continue and abort the current macroC execution.
- Sorting text is more easy to use now and supports guessing of field separators and a variety of new formats and options 
  (e.g. sorting of floating numbers). When sorting by *key fields*, one may now define key fields using field seperators
  with CSV quoting or with backslash quoting support. There is no limit any more regarding the number of lines / blocks,
  which can be sorted.
- The PKS Edit tabstrip supports now context menus (to close multiple editor windows and to *pin* / *unpin* windows). It 
  also supports scrolling through many tabs using the mousewheel.
- PKS Edit supports now multi-selection in the open file dialog and supports passing command line arguments containing
  wildcards to open multiple files.
- The code completion window displays now additional information about completions in a secondary window in situations
where additional information is available. An example are templates, which show a preview of the text to insert or PKSMacroC
functions and macros which show the description of the respective function or macro. The code completion help window supports
simple HTML formatting including hyperlink to more documentation.

#### PKSMacroC
- Expressions with strings such as "x"+1 are automatically coerced to string. One does not need to write any more "x"+(string)1.
- PKSMacroC supports now postfix `++` and `--` operators and shorthand assignment operators (e.g. `*=`, `+=`, ...).
- New datatypes `float`, `boolean`, `range`, `map`, array types (`string[]`, `int[]`, ...) and some additional special types like `FILE`
  and `EDITOR` were introduced.
- PKSMacroC supports now foreach loops using the following syntax: `for (type variable : expression) {...}`. One can currently iterate
  over ranges, strings and arrays.
- Macro comments in PKSMacroC support now a 'Java' - like syntax for formatting comments. In particular one may use 
  @param or @return tags to describe return values or parameters.
- PKSMacroC supports now regular C style for loops with the following syntax `for (initializer; condition; increment) {...}`.
- PKSMacroC supports now switch expressions.
- PKSMacroC allows for simple extension using native (WIN32 API) methods.
- Many utility methods in particular for working with arrays, files and strings were added to PKSMacroC.
- One may define a macroC function parameter with a type of `auto` to allow for arbitrary values being passed to a PKSMacroC function
- One may use the new syntax `*function(params...)` now to execute a function (macro or native) **by name**.
- Expressions can now be correctly passed as function arguments.
- PKSMacroC supports now [] operators to extract single characters or a range of characters from a string or one or more words from a string array.
  or in assigments to change elements of an array or to access elements of a map.
- PKSMacroC integer literals were extended to also support `_` and other representations (binary, octal, ...).
- PKSMacroC supports now global (namespace local / static) variables. Namespaces are currently limited however.
- Long running macroC macros can now be interrupted - a progress bar pops up after a while to cancel them (e.g. if the macro code contains
  an error such as an endless loop).
- One can now debug print the low level instructions of PKSMacroC macros (from the macro dialog).
- /* */ comments in macros are now supported and automatically attached to a macro as a comment (e.g. used as help in code completion), 
  if placed in front of a macro definition.
- Decompilation of macros will better reverse engineer control flow expressions (if, while, else....).
- PKSMacroC compiler error diagnostics was improved. Error messages will now more likely point you to the actual problem. Special
  error reporting for common programming errors (missing closing brackets, use of undeclared vars etc...) was added.
- Internal objects allocated by the MacroC VM are dynamically garbagge collected. No memory leaks any more like before, when executing
  PKSMacroC code.
- One can open, print to and clear a console from macroC code for logging or debugging purpose.
- Error diagnostics of the PKSMacroC compiler and error handling in PKSMacroC has been improved (there is still room for improvement). 
- One can create a PKSMacroC native method API documentation now from within PKS Edit using the new menu point "Macro->View Bindings->MacroC API Documentation".

#### Markdown wysiwyg mode
- One can search now and find matches in markdown documents rendered in Wysiwyg format.
- More HTML formatting is now supported in wysiwyg rendering
- A simple wysiwxg mode was added for HTML documents (do not expect to much :-)).
- One can navigate to and activate hyperlinks Wysiwyg mode using the keyboard (default binding is 
  `UP`, `DOWN`, `LEFT`, `RIGHT` keys to navigate to and `SPACE` to activate a hyperlink).
- One can print now Wysiwyg display documents (markdown and HTML).

#### Hex mode
- One can search now and find matches in hex mode (fixes issue PKSEDIT-3).
- One can print files in hex mode now.

### Changed
- all files previously stored in the old Windows INI-file format (`pksedit.his`, `pksedit.ini`) were replaced by JSON formatted files:
  `pkseditini.json` captures the standard PKS Edit configuration, file `pkssession.json` contains _session_ information like the list
  of previously opened files. The later file is generated by PKS Edit and should not be edited manually, the `pkseditini.json` file is
  written, when print or configuration settings are changed, but is also intended for manual editing.
- PKSMacroC syntax has changed on some places to make it more compatible with simple "C" code. Macros are not any more marked with a special
  keyword "macro", but are proceeded by either "void" or by specifying the return type (e.g. `string concatStrings(string s1, string 2)`).
- The format of the internal printf facility format specifiers (as being unsed in PKSMacroC macros or to format statusline and printed
  headers and footers) is now more compatible with standard C-printf-format and uses %d, %o, %x to format integer numbers (as opposed to %j, ... as before).

### Fixed
- A potential buffer overflow, when defining syntax error formats (navigation patterns) with long associated compiler names has been fixed.
- A potential crash when closing a comparison window was fixed.
- Save actions are now really executed just before a file is saved.
- Find and replace options were not persisted correctly in the PKS Editor session.
- A regression when compiling PKS-MacroC files containing strings, which cause a compilation error was fixed.
- A potential exception occurring when deleting the selected text with undo being disabled was fixed.
- Execution of nested MacroC calls could cause a corrupted MacroC stack causing wrong results. This is fixed.
- Several operators in MacroC had not been correctly implemented before (<=, >=, ...) and should work now.
- Several small issues regarding the compilation of PKSMacroC have been fixed:
   - `1+1` is now correctly evaluated (before you needed to write `1 + 1` - space delimited) 
   - integer values support a higher range of long long (C) now. Overflows during compilation are correctly reported as a compile error
   - while loop bodys may now contain multiple statements enclosed in `{` and `}`.
   - local function variables are now correctly scoped locally.
   - compiling bigger files with an error resulted in the macro compiler being not able to compile any macros afterwards.
- Some GDI resource leaks were fixed. In particular the Configure Document Types dialog box had been leaking resources but also opening
  and closing many editor tabs wasted considerable GDI resources.
- The code completion help secondary window was sometimes not correctly closed and stayed open on the screen - this is fixed.
- Convert spaces to tabs did not convert all possible spaces to tabs, but only the ones at the beginning of lines.
- Searching for \t, \f and \b had been broken when searching using regular expressions and did not find the respective characters.
- Replacing expressions with \n in the replace by patterns did not automatically split lines any more. This regression was fixed.
- A potential crash when redoing a sort operation was fixed.
- Undoing a sort operation will correctly reset the line flags.

## [2.1.0] - 2022-02-02

#### Added
- PKS Edit does not use an out-dated MDI style user interface any more, but manages its windows now in tabs which can be attached
  to resizable docks. Docking positions are persisted and restored, when PKS Edit will be opened again. Tabs can display the full
  filename path or can be displayed in `compact mode` (see layout options), where only the filename is being displayed.
- PKS Edit support now storing backup files in its `temp path` (new option `Place backup files in autosave directory`). Doing
  so has the disadvantage, that two files named `c:\temp1\x.c` and `c:\temp2\x.c` will create the same temp file having last one saved
  override the backup file created first. An advantage of using this option is, that working directories are not "polluted" by backup files.
- One may now save a file password protected by selecting the corresponding checkbox in the Save As dialog. PKS Edit will automatically
  recognize now password protected files (independent of their file extension) and will ask for the password to de-crypt encrypted files.
- PKS Edit supports now a preliminary `hex editor mode` to display the current file in an hex editor fashion. Editing is not yet
  correctly supported, but planned for later releases. PKS Edit will now by default open binary files in hex editor mode.
- PKS Edit supports a `wysiwyg mode` for markdown files now, which allows a preview of markdown formatted files (.md - format).
  The best way to use this mode is to clone the window of a .md-file and set the cloned window to wysiwyg mode (Editor Settings->Wysiwyg Display).
- The new `Watch Log File` mode can be used to edit log files and have them update their view and scroll to the end, when new log entries
  are added (seee Linux `tail -f`). In this mode editing the file is not possible - it is read-only in this case to avoid concurrent external
  and internal changes.
- One can now move the current selection (or - if no selection exists - current line) up and down using Alt+UP and Alt+DOWN respectively.
- Accessing copying and pasting to and from named buffers was made more accessible. Operations with named buffers are now available
  through the extended edit menu. The PKS Edit clipboard list may now also save a history of all clipboard contents created during
  a PKS Edit session (currently only privately created clipboards are considered). To enable the clipboard history, you must set
  the corresponding configuration option in the PKS Edit configuration in the `Autosave` settings tab.
- PKS Edit supports now the evaluation of either the current line or the current selection and selected grammars. For
  now only PKS macro code evaluation is supported, but we plan to support evaluation of other syntax as well.
- PKS Edit supports now switching between fullscreen and normal mode (press F11 to toggle)
- PKS Edit supports now autosaving the file directly rather than autosaving a temporary file
- PKS Edit Help is now provided by displaying the manual written in markdown format. The old Windows Help file was removed.
- PKS Edit will now **auto-detect codepages** when opening documents. Although it still has not double byte character support, 
  document e.g. written in UTF-8 format are now correctly converted upon reading to ISO-8859-1 encoding (standard encoding handled by PKS Edit). 
  Files are also saved in the encoding in which the were read. One can select the desired encoding when opening files (or let PKS Edit auto-detect
  the encoding) and may select a different encoding when saving files using "Save As". One may inspect the current encoding of a file in the 
  file buffer list.
- Search in files can now handle **pathes longer than 260 characters** and will not abort the search, when a file along the way cannot be opened.
- Toolbar icon sizes can now be configured in the configuration layout options section.
- The default text font used in editors can now be configured in the configuration misc.... section.
- One can select now the special theme "system default" to have PKS Edit honor the system settings for darkmode application display.
  If selected, PKS Edit shows up in darkmode, when darkmode applications mode is configured in Windows and in default mode otherwise.
- PKS Edit supports now context menus in editors. The contents of the context menu can be configured using the newly introduced `pksactionbindings.json`
  configuration file.
- PKS Edit supports now searching the word under the cursor or the current selected text on the Internet. One can configure the favorite
  search engine in the miscellaneous configuration dialog.
- The temp path of PKS Edit may be configured explicitly to point to an arbitrary directory, but will also default now to a directory named 
  "pksedit" located in the current users Windows temp directory. PKS Edit will now place all temporary files (autosave etc...) to the backup
  directory.
- The function keys display now a tooltip further explaining the action associated with the function key.
- The "find in files" function was improved:
  - resulting matches are now always displayed in a separate window, in which individual matches can be selected (double click on match line)
  - a summary of the search is printed at the end
  - two new options were added: one can search only considering locations found in a previous search and the search list can be augmented 
  in a new search.
  - one can also replace now all occurrences of a string in the files found.
- When comparing files, one may now select the file to compare with using a selector. This selector provides a new option for comparison:
  _Compare with history_, which allows to compare a file with a previously created backup file (if such exists). PKS Edit will now differentiate
  between added, deleted and modified lines and will synchronize cursor movements of files in comparison mode to be able to display identical lines
  "side by side".
- PKS Edit will now also correctly restore __cloned windows__ and will restore the current display
- mode of a window, when restarting from a saved session.
- PKS Edit uses *FontAwesome icons* now. This will in particular have the dark mode look better.
- Dark Mode has been improved and will display now more elements such as title bars and dialogs also in dark. Some open issues
  like setting dialog / file selector or native message boxes not being correctly colored remain.
- A new theme named "spring was added.
- New configuration options were added:
  - "Syntax Highlighting" can be switched on and off per view
  - "Line Selection" mode is an alternative to Column Selection and Normal selection mode, where selecting text with the caret or
the mouse will result always in complete lines being selected.
  - "Preserve Caret Column" can be switched off to have the caret not preserve its column position when moving vertically through a window
- Code completion is available now, when editing key and mouse bindings in PKS Edit.
- templates defined in the grammar may now explicitly marked with `auto-insert=true` to have the template define an abbreviation, inserted
  during editing without opening the code-completion window. The two new template variables `${tab}` and `${indent}` were added to allow
  for proper indenting in the code generated. See the [grammar](manual/grammar.md) documentation for details.
- PKS Edit will now perform document / grammar sensitive formatting and indenting of documents. The old "Bracket Indenting" option is gone.
  In code files the indent will be properly selected automatically based on the current opening and closing braces etc... A simple parser
  was added which is able to handle comments and string / character constants in the code possibly containing opening and closing brackets
  more gracefully. Formatting is currently rather limited except for text type files (and markdown).
- document types can now be selected not only based on the file name extension, but also based on the contents of the first line.
- text formatting works now also for code files (e.g. c++ or java). A very simple formatting algorithm was implemented so far only caring about
  indentation.
- PKS Edit now differentiates between "navigate to last" and "navigate to last insertion position" and provides a menu entry for each version. 
  Goto last position will navigate between all automatically defined marks and navigate to last insertion position will explicitly navigate to
  marks defined before the last modification of text.

### Changed
- Backup files will now be created by appending the configured backup extension to the file name (e.g. x.c will be backuped to x.c.bak rather
  than replacing the extension x.c -> x.bak). This has the advantage, that files named x.c and x.h will create different backup files now.
- Changing the display mode will not automatically save the editor configuration any more. Editor configurations are saved, when they
  were edited in the corresponding dialog.
- PKS Edit will now save and load all action bindings from a JSON file, which can be edited rather than from the binary compiled macro file.
 
### Fixed
- Long lines painted in a single color could have caused a crash. This is fixed.
- Find in files used to find a match only once per line. If a search would have matched multiple positions in one line in a file only the
  1st match had been reported before.
- The "Save All" action had been complaining about read-only files even if the file had not been changed.

### [2.0.2] - 2021-12-14

#### Added
- PKS Edit finally supports code completion. Press Ctrl+SPACE to open the suggestion window, which will display code templates
  configured for the current grammar and/or cross references defined as tags + other suggestions.
- For text type grammars (text and markdown) code completion includes all words found in the current text.
- The line numbers area displays now change annotations: changed lines are marked with a yellow annotation, changed lines,
  which have been saved are marked in green.
- PKS Edit supports now navigation to previous positions "cross open files" - aka global textmarks. One can used Alt+Left/Right
  to navigate between "editing positions" and Alt+Shift+Left/Right to navigate between "navigation positions". Editing positions
  can also be navigated through the new back and next toolbar buttons.
- Ctrl+TAB opens a window selector now allowing to visually tab through the list of open files to select a file.
- PKS Edit supports now selecting the UI theme (preview version only) and the current language from the settings dialog.
  Note, that updating the language will not be reflected before PKS Edit is restarted. Currently two languages are available: English and German.
- PKS Edit has a couple of new supported conversion operations (accessible through the new Functions->Convert menu):
  - convert line ends to LINUX (\n) style
  - convert line ends to Windows (\r\n) style
  - convert upper case to lower case, lower case to upper case, toggle case. The current character under the cursor is converted or - if a text selection
   exists, the selected text is converted.
- Support for Power Shell syntax was added.
- I18N - support of the English language. Almost all resources have been translated in this release. Function and option keys among others show
  now correctly translated English text, when English locale is selected.
- The function keys will now display the function keys for the currently pressed modified - no explicitly "function key switching" necessary any more.

#### Changed
- Character classes (for identifiers and mappings from lower case to upper case) are now defined in the grammar, which allows for language
  sensitive identifier (word) processing - e.g. when navigating tags or selecting words with mouse double click etc...

## Fixed
- A crash in Find & Replace was fixed, which could happen, if a wrong syntax for replacement (unknown capturing group) was used.
- A crash was fixed with redo: if no redo was available, the attempt to do a redo could result in a crash.
- A bug was fixed in the macro compiler causing it to fail to recognize pre-defined ENUM constants (e.g. in `CaretMove(MOT_SINGLE)` the string `MOT_SINGLE` was
  not properly recognized).
- Case ignore searches with umlauts did not work in some cases (depending on character class definitions in grammar).

### [2.0.1] - 2021-11-17

#### Added
- The new function "Toggle Comment" allows to comment in/out the range of selected lines. The function tries to use
  single line comments (e.g. // for c++), when available in the corresponding grammar or block comments otherwise 
- Templates support now template variables for various purpose. For details see (grammar.md).
- Grammar patterns support nested patterns now, which enables us to e.g. highlight nested tokens like JavaDoc tags (@param... inside block comments) or
  task tags ( TODO, FIXME inside single line comments).

#### Fixed
- An exception when compiling macros multiple times with showing the error list was fixed.
- Cursor movement and selection in "word mode" (e.g. Ctrl+Shift+Right) was made more intuitive.
- Deleting multiple fill characters (smart tab fill deletion) using Backspace was also made more
  intuitive.
- Inserting lines at the bottom of the file causing the editor to scroll as a side effect was causing repaint problems. This is fixed.
- Editing on multiple positions in the same line could not be undone correctly. This is fixed.

### [2.0.0] - 2021-11-01

#### Added
- PKS Edit supports now syntax highlighting based on syntax rules defined in a mylanguage.grammar.json file. Rules
  are for now limited to applying lexical analysis only. For some of the most common programming languages grammar files
  are provided (C/C++, Java, Typescript, Dart, JSON, Shell, Batch-Files)
- Zooming in and out of the current text window works now by pressing Ctrl+ and Ctrl- or by using Ctrl+Mousewheel.
- PKS Edit supports hyperlink navigation, where the syntax for detecting hyperlinks is defined in the grammar file. Hyperlinking
  may include opening a linked file (e.g. `#import "myfile.h"` in a C-source file), where linked files are looked up on an
  include path (by default "include;inc" - can be defined in an environment variable `PKS_INCLUDE_PATH`) and/or navigation to
  a URL to open in a browser.
- Matching brackets is grammar dependent now.
- Ctags support was updated to correctly support the newest default format as used by the Universal Ctags program.
- We started to finally implement an English version of PKS Edit. Language dependent files have not yet been all translated.
  In order to force PKS Edit to using English language resources, one can add to `Language=en-US` to the [desk] section
  of pksedit.ini.

#### Fixed
- Some issues with regular expressions (in particular a possible endless loop when compiling complex syntax) have been
  fixed.
- Some memory leaks were fixed, which could in particular be observed when performing complex operations on large files (like global 
  search&replace with many replacements).

### [1.9.1] - 2021-09-20

#### Fixed

- Some performance issues with undo and redo in large files were fixed.
- "Find next" will now always skip correctly over previously matched text (and not increment simply by 1).
- Search & replace with preserve case could lead to an exception - this is fixed.

### [1.9.0] - 2021-08-18

#### Added
- PKS Edit can now be installed using an InnoSetup based installer.
- Regular Expressions in PKS Edit were improved and support now in addition to the old simple UNIX 3 style regular expressions:
  - `(?:...)` non - capturing groups
  - `{...}?`, `+?`, `*?` - lazy quantifiers
  - `\s` - to match whitespaces
  - `\S` - to match non-whitespaces
  - `\w` - to match PKS Edit identifiers.
  - `\d`- to match digits
- PKS Edit supports now the definition of grammars for languages of text edited with PKS edit for the purpose
  of syntax highlighting and folding (not yet fully implemented).
- The configuration of document types is now placed in a more modern JSON format file (`pks_sys\pksconfig.json`) and
  is not any longer in a non editable binary set of .lin-files. Changes to the editor configuration are always
  persisted right away in the corresponding config file.
- A new option `Ignore Binary Files` was added to the find in files dialog, allowing to easily exclude
  unwanted matches.
- A new option `Delete multiple spaces` was added, which better supports `space indentation mode` in that
  during editing the editor handles spaces inserted instead of tabs like a tab.
- The find in files dialog now remembers the file name patterns for the next invocation.
- A new `incremental search` field was added to the toolbar area allowing to type in text to find in the current file.
- A new option `maximumNumberOfOpenWindows` was added to allow to recycle windows, when too many windows are opened.
- One can open multiple windows to edit one file now.
- Find in Files displays a folder browser now to select a path
- Find in Files will now highlight matches, when stepping through the results.
- Actions are now consistently enabled/disabled in the PKS Edit UI (also the virtual function keyboard etc...)

#### Fixed
- A repaint bug in the line number area caused linenumbers to not update correctly, when lines where inserted or deleted.
- Find in Files did not find regular expressions matching to the end of line.
- Space indentation did not correctly indent sometimes.
- Printing files: when the last line on a page was wrapped, it wrapped into the footer area rather than to the next page.
- Scrolling with the trackthumb in long files (> 32000 lines) was fixed and should work now as expected.

### [1.8.0] - 2021-05-04

#### Added
- Support for mousewheel scrolling added
- Column shape selection with the mouse will now work independently of line length
- Toolbar and menu actions are now properly enabled / disabled depending on availability
- Toolbar buttons display tooltips now
- Redo button in toolbar actually does a redo.
- File patterns support now ! as in !*.obj (everything but files ending with .obj)

