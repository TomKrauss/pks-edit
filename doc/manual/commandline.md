
## PKS-Edit command line arguments

PKS-Edit may be started passing it several options. An option argument starts 
with a `-` or a `/` character and is followed by one character defining the option
plus optional characters defining the option parameters. Every command line argument
not starting with `-` or `/` is interpreted as the name of a file to open. Subsequent
options apply to a previously opened file.

- `/g`, `/l` may both be followed a line number to jump to the line of the file just opened. Example: to open a file test.c and jump to line 100, specify:
  `pksedit.exe test.c /l100`
- `/c` may both be followed a line number to jump to the column in the current line in the file just opened.
- `//` may be followed by a pattern to search in the file just opened.
- `/w` switch to wysiwyg mode in the file just opened
- `/i` run pksedit in _iconic_ mode (do not open it full screen).
- `/t` may be followed by the name of a tag to navigate to the tag in the file just opened.
- '/T' may be followed by the name of a result list containing search results for a list of matches in files.


