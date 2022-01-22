# PKS Edit

This file contains the release notes for the latest **PKS Edit** releases.

--- 
### 2.1.0

#### New
- PKS Edit does not use an out-dated MDI style user interface any more, but manages its windows now in tabs which can be attached
  to resizable docks. Docking positions are persisted and restored, when PKS Edit will be opened again. Tabs can display the full
  filename path or can be displayed in `compact mode` (see layout options), where only the filename is being displayed.
- PKS Edit support now storing backup files in its `temp path` (new option `Place backup files in autosave directory`). Doing
  so has the disadvantage, that two files named `c:\temp1\x.c` and `c:\temp2\x.c` will create the same temp file having last one saved
  override the backup file created first. An advantage of using this option is, that working directories are not "polluted" by backup files.
- One may now save a file in encrypted mode by selecting the corresponding checkbox in the Save As dialog. PKS Edit will automatically
  recognize now encrypted files (independent of their file extension) and will ask for the password to de-crypt encrypted files.
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
- PKS-Edit will now **auto-detect codepages** when opening documents. Although it still has not double byte character support, 
  document e.g. written in UTF-8 format are now correctly converted upon reading to ISO-8859-1 encoding (standard encoding handled by PKS-Edit). 
  Files are also saved in the encoding in which the were read. One can select the desired encoding when opening files (or let PKS-Edit auto-detect
  the encoding) and may select a different encoding when saving files using "Save As". One may inspect the current encoding of a file in the 
  file buffer list.
- Search in files can now handle **pathes longer than 260 characters** and will not abort the search, when a file along the way cannot be opened.
- Toolbar icon sizes can now be configured in the configuration layout options section.
- The default text font used in editors can now be configured in the configuration misc.... section.
- One can select now the special theme "system default" to have PKS-Edit honor the system settings for darkmode application display.
  If selected, PKS-Edit shows up in darkmode, when darkmode applications mode is configured in Windows and in default mode otherwise.
- PKS-Edit supports now context menus in editors. The contents of the context menu can be configured using the newly introduced `pksactionbindings.json`
  configuration file.
- PKS-Edit supports now searching the word under the cursor or the current selected text on the Internet. One can configure the favorite
  search engine in the miscellaneous configuration dialog.

### Improved
  The temp path of PKS Edit may be configured explicitly to point to an arbitrary directory, but will also default now to a directory named 
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
- PKS-Edit will now perform document / grammar sensitive formatting and indenting of documents. The old "Bracket Indenting" option is gone.
  In code files the indent will be properly selected automatically based on the current opening and closing braces etc... A simple parser
  was added which is able to handle comments and string / character constants in the code possibly containing opening and closing brackets
  more gracefully. Formatting is currently rather limited except for text type files (and markdown).
- document types can now be selected not only based on the file name extension, but also based on the contents of the first line.
- text formatting works now also for code files (e.g. c++ or java). A very simple formatting algorithm was implemented so far only caring about
  indentation.
- PKS-Edit now differentiates between "navigate to last" and "navigate to last insertion position" and provides a menu entry for each version. 
  Goto last position will navigate between all automatically defined marks and navigate to last insertion position will explicitly navigate to
  marks defined before the last modification of text.

### Changed
- Backup files will now be created by appending the configured backup extension to the file name (e.g. x.c will be backuped to x.c.bak rather
  than replacing the extension x.c -> x.bak). This has the advantage, that files named x.c and x.h will create different backup files now.
- Changing the display mode will not automatically save the editor configuration any more. Editor configurations are saved, when they
  were edited in the corresponding dialog.
- PKS-Edit will now save and load all action bindings from a JSON file, which can be edited rather than from the binary compiled macro file.
- 
### Fixes
- Long lines painted in a single color could have caused a crash. This is fixed.
- Find in files used to find a match only once per line. If a search would have matched multiple positions in one line in a file only the
  1st match had been reported before.
- The "Save All" action had been complaining about read-only files even if the file had not been changed.

### 2.0.2

#### New
- PKS Edit finally supports code completion. Press Ctrl+SPACE to open the suggestion window, which will display code templates
  configured for the current grammar and/or cross references defined as tags + other suggestions.
- For text type grammars (text and markdown) code completion includes all words found in the current text.
- The line numbers area displays now change annotations: changed lines are marked with a yellow annotation, changed lines,
  which have been saved are marked in green.
- PKS-Edit supports now navigation to previous positions "cross open files" - aka global textmarks. One can used Alt+Left/Right
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

#### Changed
- Character classes (for identifiers and mappings from lower case to upper case) are now defined in the grammar, which allows for language
  sensitive identifier (word) processing - e.g. when navigating tags or selecting words with mouse double click etc...

#### Improved
- Support for Power Shell syntax was added.
- I18N - support of the English language. Almost all resources have been translated in this release. Function and option keys among others show
  now correctly translated English text, when English locale is selected.
- The function keys will now display the function keys for the currently pressed modified - no explicitly "function key switching" necessary any more.

### Fixes
- A crash in Find & Replace was fixed, which could happen, if a wrong syntax for replacement (unknown capturing group) was used.
- A crash was fixed with redo: if no redo was available, the attempt to do a redo could result in a crash.
- A bug was fixed in the macro compiler causing it to fail to recognize pre-defined ENUM constants (e.g. in `CaretMove(MOT_SINGLE)` the string `MOT_SINGLE` was
  not properly recognized).
- Case ignore searches with umlauts did not work in some cases (depending on character class definitions in grammar).

### 2.0.1

#### New
- The new function "Toggle Comment" allows to comment in/out the range of selected lines. The function tries to use
  single line comments (e.g. // for c++), when available in the corresponding grammar or block comments otherwise 
- Templates support now template variables for various purpose. For details see (grammar.md).

#### Improved
- Grammar patterns support nested patterns now, which enables us to e.g. highlight nested tokens like JavaDoc tags (@param... inside block comments) or
  task tags ( TODO, FIXME inside single line comments).

#### Fixed
- An exception when compiling macros multiple times with showing the error list was fixed.
- Cursor movement and selection in "word mode" (e.g. Ctrl+Shift+Right) was made more intuitive.
- Deleting multiple fill characters (smart tab fill deletion) using Backspace was also made more
  intuitive.
- Inserting lines at the bottom of the file causing the editor to scroll as a side effect was causing repaint problems. This is fixed.
- Editing on multiple positions in the same line could not be undone correctly. This is fixed.

### 2.0.0

#### New
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
  In order to force PKS-Edit to using English language resources, one can add to `Language=en-US` to the [desk] section
  of pksedit.ini.

#### Fixed
- Some issues with regular expressions (in particular a possible endless loop when compiling complex syntax) have been
  fixed.
- Some memory leaks were fixed, which could in particular be observed when performing complex operations on large files (like global 
  search&replace with many replacements).

### 1.9.1

#### Fixed

- Some performance issues with undo and redo in large files were fixed.
- "Find next" will now always skip correctly over previously matched text (and not increment simply by 1).
- Search & replace with preserve case could lead to an exception - this is fixed.

### 1.9.0

#### New
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

#### Improved
- Find in Files displays a folder browser now to select a path
- Find in Files will now highlight matches, when stepping through the results.
- Actions are now consistently enabled/disabled in the PKS Edit UI (also the virtual function keyboard etc...)

#### Fixed
- A repaint bug in the line number area caused linenumbers to not update correctly, when lines where inserted or deleted.
- Find in Files did not find regular expressions matching to the end of line.
- Space indentation did not correctly indent sometimes.
- Printing files: when the last line on a page was wrapped, it wrapped into the footer area rather than to the next page.
- Scrolling with the trackthumb in long files (> 32000 lines) was fixed and should work now as expected.

### 1.8.0

#### New
- Support for mousewheel scrolling added
- Column shape selection with the mouse will now work independently of line length
- Toolbar and menu actions are now properly enabled / disabled depending on availability
- Toolbar buttons display tooltips now
- Redo button in toolbar actually does a redo.
- File patterns support now ! as in !*.obj (everything but files ending with .obj)

