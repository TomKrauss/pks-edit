# PKS Edit

## Release Notes

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

