If you work with PKS-Edit for the very first time you might find some useful hints in this section.

## Starting PKS-Edit

To start PKS-Edit double click the program icon or the icon of a file associated with PKS-Edit.
PKS-Edit will then open the associated file and will restore the list of previously opened files.
If not file was clicked and you open PKS-Edit the very first time, PKS-Edit will display a _file selector_ 
to allow you to open a file.

Instead of opening a file, you may use the "New File" menu or icon to create a new file. New files
will be named by default newfile.txt. Saving them will have PKS-Edit prompt for the file name
to use.

PKS-Edit allows to associate document types with file extensions. Depending on a file type, one
may configure:

- a custom **grammar** used for thinks like syntax highlighting, bracket matching and code completion
- custom editing modes (hexadecimal display or _logile_ mode (aka `tail -f mode`))

**Tip**: you may drag&drop a document onto an open PKS-Edit to edit that document. By default 
documents double clicked are opened in an already open and running PKS-Edit instance.

## Closing PKS-Edit

To close PKS-Edit, press `Alt+F4` or alternatively press File->Exit. If there are files not yet saved,
PKS-Edit will prompt you to save the files or to ignore all changes. By default PKS-Edit will 
save the current session to allow to re-open the last set of files when it is restarted.

_Note_: you may disable the session preservation feature in the TREND configuration.
