## Selecting text

You may select text in PKS-Edit using either the mouse and dragging over a range of text to select 
or by using the cursor keys in combination with the `Shift` key as in other text editors.

PKS-Edit supports three selection shapes:

- _standard selection_, where a selection starts in a column c1 and a line l1 and ends in a column
 c2 and a line l2 (which might be identical to l1).
- _blockmode selection_, where the selection has a rectangular form allowing to select only text
between columns c1 and c2 in a lines exclusively
- _line selection_, where always complete lines are selected.

The shape of the selection can be selected e.g. in the `Editor Settings` menu.

## Editing text

PKS-Edit supports the most commonly used editing features provided by nowadays code editors:

- inserting text with a selection present, will by default delete the selected text (this behavior 
can be configured in `Editor Settings` menu)
- all insert, modify and delete operations can be undone and an undone operation can be redone. The
size of the undo buffer in PKS-Edit can be configured in the configuration.
- copying selected text will make the text also available on the system clipboard for other applications.
You may also insert text placed onto the clipboard by other applications. Note, that PKS-Edit maintains a 
history of clipboards automatically and allows to save text onto _named_ clipboards (use `Shift+Ctrl+C` to
create a named clipboard). To access historic clipboards and named clipboards use `Shift+Ctrl+V`.
- PKS-Edit supports typical code editor commands like `Alt+Cursor Up/Down` to move the current selection or
the line with the caret up and down, Use `Ctrl+Shift+7` to toggle line comments, pressing `Shift+Tab` or `Tab`
with a current selection to outdent/indent the selected lines of text. For more code editing commands refer
to the `Function` menu of PKS-Edit.

## Options affecting the editing of text

Several options can be enabled to affect the way text is edited.

- _Autoindent_ - when set, PKS-Edit will automatically indent new lines the same way, lines above are indented
- _Check Brackets_ - when set, PKS-Edit will automatically check for matching code brackets, when brackets are inserted.
What is considered a bracket is defined in the _grammar_ of the file edited (which is assigned through matching the file extension).
- _Wrap_ - when set, PKS-Edit will automatically wrap the text to new lines, when the line will be "too long". The right margin
of a line can be configured in the editor settings.
- _Shortcuts_ - when set, PKS-Edit will automatically expand certain keywords treated as shortcuts for longer texts. Shortcut
templates can also be configured in the _grammar_ of a file.
- _Read only_ - read-only files will be by default opened in a mode, where editing is not permitted.
- _Logfile mode_ - logfile mode can be used to automatically reload the file contents (or the end of a file for big files), when the
opened file changes as new text is appended to that file. Files opened in _Logfile mode_ are by default also not editable.

