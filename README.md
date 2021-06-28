# PKS Edit

The famous arcade times text editor from Pahlen & Krauß.

## Features

The editor contains only some basic features compared to modern text editors such as 
Ultra Edit:

- search & replace with options like regular expression matching
- search & replace in multiple files
- compare files
- sort files
- follow cross references in code (requires a tag file - e.g. created using ctags)
- auto-indentation and formatting
- record & play of macros
- it is pretty fast, if files do not get too big (no memory mapped files support yet)
- different selection styles (block, ...)
- basic support for different file types
- and some more - but that's it

## Installing and Using PKS Edit

To install PKS-Edit, copy the executable ``PKSEDIT.EXE`` along with the
directory PKS_SYS to a directory of your choice. The directory PKS_SYS
must be published in the ``WIN.INI`` file in an entry similar to the following:

```
[PksEdit]
PKS_SYS=C:\TOOLS\PKSEDIT\PKS_SYS
```

Usually you will use PKS-Edit by associating it with certain file types.
The registry file pksedit.reg contains an example on how 
to register some default types (*.java, *.txt) to be edited by PKS-Edit.

## Release Notes

-   Here is the link to the [Release Notes](doc/release_notes.md)

Enjoy Tom & Rolf
