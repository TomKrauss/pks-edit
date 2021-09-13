# PKS Edit

## Release Notes

### 2.1.0

#### New
- PKS Edit does not use an out-dated MDI pattern for managing its windows any more, but manages its windows now in tabs.

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
-- convert line ends to LINUX (\n) style
-- convert line ends to Windows (\r\n) style
-- convert upper case to lower case, lower case to upper case, toggle case. The current character under the cursor is converted or - if a text selection
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
- A bug was fixed in the macro compiler causing it to fail to recognize pre-defined ENUM constants (e.g. in CaretMove(MOT_SINGLE) the string MOT_SINGLE was
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
  may include opening a linked file (e.g. #import "myfile.h" in a C-source file), where linked files are looked up on an
  include path (by default "include;inc" - can be defined in an environment variable PKS_INCLUDE_PATH) and/or navigation to
  a URL to open in a browser.
- Matching brackets is Grammar dependent now.
- Ctags support was updated to correctly support the newest default format as used by the Universal Ctags program.
- We started to finally implement an English version of PKS Edit. Language dependent files have not yet been all translated.
  In order to force PKS-Edit to using English language resources, one can add to "Language=en-US" to the [desk] section
  of pksedit.ini.

#### Fixed
- Some issues with regular expressions (in particular a possible endless loop when compiling complex syntax) have been
  fixed.
- Some memory leaks were fixed, which could in particular observed when performing complex operations on large files (like global 
  search&replace with many replacements).

### 1.9.1

#### Fixed

- Some performance issues with undo and redo in large files was fixed.
- Find next will now always skip correctly over previously matched text (and not increment simply by 1).
- Search & replace with preserve case could lead to an exception - this is fixed.

### 1.9.0

#### New
- PKS Edit can now be installed using an InnoSetup based installer.
- Regular Expressions in PKS Edit were improved and support now in addition to the old simple UNIX 3 style regular expressions:
-- (?:...) non - capturing groups
-- {...}?, +?, *? - lazy quantifiers
-- \s - to match whitespaces
-- \S - to match non-whitespaces
-- \w - to match PKS Edit identifiers.
-- \d - to match digits
- PKS Edit supports now the definition of grammars for languages of text edited with PKS edit for the purpose
  of syntax highlighting and folding (not yet fully implemented).
- The configuration of document types is now placed in a more modern JSON format file (pks_sys\pksconfig.json) and
  is not any longer in a non editable binary set of .lin-files. Changes to the editor configuration are always
  persisted right away in the corresponding config file.
- A new option "Ignore Binary Files" was added to the find in files dialog, allowing to easily exclude
  unwanted matches.
- A new option "Delete multiple spaces" was added, which better supports "space indentation mode" in that
  during editing the editor handles spaces inserted instead of tabs like a tab.
- The find in files dialog now remembers the file name patterns for the next invocation.
- A new "incremental search" field was added to the toolbar area allowing to type in text to find in the current file.
- A new option "maximumNumberOfOpenWindows" was added to allow to recycle windows, when too many windows are opened.
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
- Block selection with the mouse will now work independently of line length
- Toolbar and Menu actions are now properly enabled / disabled depending on availability
- Toolbar Buttons display Tooltips now
- Redo Button in Toolbar actually does a redo.
- File Patterns support now ! as in !*.obj (everything but files ending with .obj)

