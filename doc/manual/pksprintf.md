
# PKS Printf Format

PKS Edit allows to use a special format to format e.g. the headers and footers in printouts and/or the text to be displayed in the 
statusline using the **PKS Printf Format**.

This works similar to *printf* format, but allowing to use placeholders to refer to printable information. 
One sample would be `%s$f` which evaluates to the name of a file (`$f`) in string format (`%s`).

Here is a list of the placeholders supported:

- `$O` - file offset in bytes
- `$C` - the ascii value of the current character under the caret
- `$l` - the current line number
- `$c` - the current column number
- `$f` - the simple filename of the current document
- `$F` - the full path of the current document
- `$b` - The filename without extension
- `$w` - the window handle id (PKS Edit internal ID) of the current editor window
- `$h` - the pathes for help and cross references
- `$*` - if the file is modified, evaluates to "*"
- `$&` - if the file is modified, evaluates to the string _modified_ in the current language
- `$r` - if the current document is read only, evaluates to the string _readonly_ in the current language
- `$m` - the modification time of the current document
- `$M` - the modification date of the current document
- `$s` - the last save time of the current document
- `$S` - the last save date of the current document
- `$c` - the creation time of the current document
- `$C` - the creation date of the current document


