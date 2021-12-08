## Finding text

PKS-Edit supports the following options when searching for text:

- _Regular Expressions_ - the searched pattern is interpreted as a [regular expression](#regular-expressions)
- _Ignore Case_ - upper lower case spelling of matched text and pattern are ignored
- _Shell Wildcards_ - only the special characters ? (matches single char) and * (matches a group of any characters) have a special meaning in the provided 
pattern
- _Wrap Search_ - if a text cannot be found towards the end of a file, the search starts from the beginning.

## Regular Expressions

Regular Expression in PKS-Edit permit the following special characters:

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

## Replacing text

When replacing text, the following special options are supported:

- Range: controls the range of text in which the replacement is performed.
- On Confirmation: when set, the user is asked before replacing an occurrence of the matched strings.
- Only in marked lines: when set, the text is only replaced in lines marked with "mark text" before in the find dialog.
- Preserve upper/lower case: when set, the upper/lower case spelling is preserved. Let\`s assume we replace all occurences of `cat` by `doc` and the option
  is set. In this case `Cat` will be replaced by `Dog` and `CAT` will be replaced by `DOG`.

## Find in Files

This option allows to find a text in a directory of files (and optionally replace all occurrences). When the search is started, PKS-Edit will open a new window, displaying 
all matching results. You may navigate through the list of matches by either clicking into the search result list or by pressing `Ctrl+n` to move to the next match and by pressing
`Ctrl+P` to navigate to the previous match.

 
