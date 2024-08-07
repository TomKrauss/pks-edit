# Find and Replace

## Finding Text

PKS Edit supports the following options when searching for text:

- <a name="find-pattern"></a>_Regular Expressions_ - the searched pattern is interpreted as a [regular expression](#regular-expressions)
- <a name="ignore-case"></a>_Ignore Case_ - upper lower case spelling of matched text and pattern are ignored
- <a name="shell-wildcards"></a>_Shell Wildcards_ - only the special characters ? (matches single char) and * (matches a group of any characters) have a special meaning in the provided 
pattern
- <a name="wrap-search"></a>_Wrap Search_ - if a text cannot be found towards the end of a file, the search starts from the beginning.

## Regular Expressions

Regular Expression in PKS Edit permit the following special characters:

- `.`  matches any character
- `[a-z]` matches any character a-z
- `[^a-z]` matches any character except a-z
- `\d` matches a digit
- `\w` matches all chars of a word
- `\s` matches all space characters
- `\S` matches all non space characters
- `^` matches the beginning if the line
- `$` matches the end of the line
- `<` matches the beginning of a word
- `>` matches the end of a word

Character specifications can be combined with an occurrence specification

- `*` matches 0 - n times (maximum 255)
- `?` matches 0 or 1 time
- `+` matches 1 or n times (maximum 255)
- `{1,5}` matches a minimum of 1 and a maximum of 5 times in a greedy way
- `{3,}` matches a minimum of 3 and a maximum of 255 times in a greedy way
- `{3,}?` matches a minimum of 3 and a maximum of 255 times in a non-greedy way

Character groups can be used to match repeating sections
- `(group).*\1` matches the specified group, than any character and then the group again
- `(:?[a-z]+)` non-capturing group repitition

## Replacing Text

<a name="replacing-text-with"></a>In the text specified with _Replace with_, the following special options are supported:

- Range: controls the range of text in which the replacement is performed.
- On Confirmation: when set, the user is asked before replacing an occurrence of the matched strings.
- Only in marked lines: when set, the text is only replaced in lines marked with "mark text" before in the find dialog.
- Preserve upper/lower case: when set, the upper/lower case spelling is preserved. Let\`s assume we replace all occurences of `cat` by `doc` and the option
  is set. In this case "`Cat`" will be replaced by "`Dog`" and "`CAT`" will be replaced by "`DOG`".

In replaced text one can also use special patterns:

- `\1` refers to the 1st subgroup matched in a regular expression.
- `\&` refers to the complete text matched by the search expression.
- `\l` will have all successive replacement text being translated to lower case. If one searches for "`[A-Z]{2}`" and find "`AN`" in a text, one could replace that using "`\l\&`" to "`an`".
- `\u` will have all successive replacement text being translated to lower case. 
- `\e` will end the range of text in the replacement pattern where an upper/lower case conversion is performed.

### Replacing Text Dialog Options

- _On Confirmation_ - PKS Edit will popup a confirmation dialog before every text fragment to be replaced.
- _Only in marked Lines_ - PKS Edit will only replace text in all lines previously _marked_ (not selected - see below for details
  about marking).
- _Preserve upper/lower case_ - PKS Edit will preserve the spelling, when replacing. "Hello" will be replaced
  by "World", while "hello" will be replaced by "world" for instance.
- <a name="replacing-text-count"></a>_Count_ pressing this button will count found occurrences rather than replacing them.
- <a name="replacing-text-mark"></a>_Mark_ will _mark_ all lines with occurrences rather than replacing them. This can be used in combination with _Only in marked Lines_.

## Find in Files

This option allows to find a text in a directory of files (and optionally replace all occurrences). When the search is started, PKS Edit will open a new window, displaying 
all matching results in the _search result list_. You may navigate through the list of matches by either clicking into the search result list or by pressing `Ctrl+n` to move to the next match and by pressing
`Ctrl+P` to navigate to the previous match. The _search result list_ ist by default named `pksedit.grp` and is by default placed in the `pks_sys` directory. One may open a previously created
search result list to navigate the results any time later using menu "Find->Use previous Search Results".

When finding/replacing text in multiple files, the following special options (in addition to normal search options) are available:

- <a name="find-in-files-single-match"></a>_Single Match in File_ will try to find only the 1st occurrence of the search pattern in a file to quickly scan through all
  files with matches.
- <a name="find-in-files-ignore-binary"></a>_Ignore Binary Files_ will ignore all files with binary content (zip-files, exe-files, obj-files, etc...)
- <a name="find-in-files-append"></a>_Append to previous Result_ will extend the previously displayed _search result list_.
- <a name="find-in-files-search-in-previous"></a>_Search in previous Results_ will only consider files mentioned in the currently displayed _search result list_.

If the option `Search in previous Results` is not specified, all files in the selected path with the given _filename pattern_ are searched recursively (one may specify a maximum folder depth for
the search). The filename pattern might be a simple pattern such as `*.txt` or a combination of file name patterns all to consider such as `*.c;*.h`.

### Navigating the Find in Files Result List

All matches found during `Find in Files...` are displayed in a _Find Files Result File List_ which is by default attached
as a bottom window. 

To select one of the matches in that list 
- `Shift+Click Left Mouse Button` to select the line showing the match.
- use `Shift+Ctrl+N` and `Shift+Ctrl+P` or the menu entries _Extras->Next/Previous Build Error or File Match_ to
navigate to the next respectively to the previous match 
- move the caret to the matching entry in the list and press `Space` to select activate the current line
- To jump to the begin or the end of the list you may use `Alt+Ctrl+P` and `Alt+Ctrl+N` respectively






