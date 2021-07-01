# PKS Edit

## Release Notes

### 1.9.0

#### New
- PKS Edit can now be installed using an InnoSetup based installer.
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

