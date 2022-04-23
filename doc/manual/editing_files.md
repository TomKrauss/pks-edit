# Editing Files

## Selecting text

You may select text in PKS-Edit using either the mouse and dragging over a range of text to select 
or by using the cursor keys in combination with the `Shift` key as in other text editors.

PKS-Edit supports three selection shapes:

- *standard selection*, where a selection starts in a column c1 and a line l1 and ends in a column
 c2 and a line l2 (which might be identical to l1).
- *blockmode selection*, where the selection has a rectangular form allowing to select only text
between columns c1 and c2 in a lines exclusively
- *line selection*, where always complete lines are selected.

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
- _Autowrap_ - when set, PKS-Edit will automatically wrap the text to new lines, when the line will be "too long". The right margin
of a line can be configured in the editor settings.
- _Shortcuts_ - when set, PKS-Edit will automatically expand certain keywords treated as shortcuts for longer texts. Shortcut
templates can also be configured in the _grammar_ of a file.
- _Read only_ - read-only files will be by default opened in a mode, where editing is not permitted.
- _Logfile mode_ - logfile mode can be used to automatically reload the file contents (or the end of a file for big files), when the
opened file changes as new text is appended to that file. Files opened in _Logfile mode_ are by default also not editable.

## Multi-caret editing

PKS-Edit supports the definition of multiple insertion carets where changes can be made in a corresponding
way on various places in the text/code edited. Multiple carets in PKS-Edit can be created by:

- clicking to a place to add a secondary caret using `Alt+Click`
- moving the caret up and down using `Ctrl+Alt+Up/Down` to add vertical secondary carets
- by adding a `multi-word-match caret`: pressing `Ctrl+Shift+L` will place a secondary caret on all words
**currently visible on the screen** matching the word close to the main input caret allowing to change all identical words
with one operation.
- inserting a template defining one or more secondary caret places for editing.

Multi caret mode ends, when an operation is performed inside PKS-Edit other than inserting characters and deleting characters.

## Coding functions

### Code Completion

PKS-Edit will show in the code completion window (open by default by pressing Ctrl+SPACE) by default a selection of the keywords
of the current language + the templates to be inserted.

Some templates are marked as `auto-insert`, which means PKS-Edit will right away expand the corresponding template, when it recoginzes,
the user has just entered the matching pattern of the template. Note, that auto-insertable templates may be marked to only be inserted
in lexical contexts, where the expansion makes sense. Entering a `{` character for instance in a C code file will only automatically
add a closing bracket and open a new line for typing, if the character is not entered inside a string, character literal or comment.
One can completely disable autocompletion by setting the option `Autocomplete` to off.

### Bracket Operations

One can find a matching bracket explicitly by using the `Find->Goto matching bracket` menu. 

When the `Check Brackets` option is enabled, PKS-Edit will check for bracket matches while you type and display a warning,
if e.g. a closing bracket has no corresponding opening bracket.

### Code Formatting

Code Formatting will be triggered by menu `Functions->Format` in PKS-Edit, but is currently very limited for code type files. 
It will update the indentation according to the defined block structure of a document. Formatting is a bit more advanced for text
type files (.txt or .md), where one has several options regarding the expected output.
