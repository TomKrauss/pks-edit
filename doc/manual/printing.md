
## Printing

PKS-Edit can be used to print files. Currently printing is always performed in Ascii mode. For files supporting other 
rendering modes (hex editor mode or wysiwyg mode rendering in markdown files), the special rendering modes will currently
**not be used** during printing.

One can either print the current active document (menu `File->Print->Current Window` or pressing the printer icon in the toolbar)
or an arbitrary document to be opened for printing using the file selector. When printing is started, a print dialog is opened
allowing to enter additional options like the range of text to print (selection - if a piece of the text is selected or a range of pages)
and to select the printer to which the print output should be directed.

### Print Settings

The `Print Settings` page allows to specify additional hints regarding the layout of the text to print like the specification of
optional header and footer sections, options regarding the way the text is formatted and options affecting the number of lines and
characters to print by default.

For headers and footers one can specify:

- `template`: this is the text to print in the header / footer. It may contain arbitrary characters + additional special information 
following the (pks-print specification)[pksprintf.md]. The text may contain three *parts* separated by a `!` characters to 
create three segments of text to be printed in the left / center / right part of the headline / footline.
- `alignment`: this is in particular useful to determine the alignment of the header/footer text on the screen, if only one`*segment* 
is defined in the template.
- `font`: one may select a special font to be used for the header / footer.
- `decoration`: one may define, whether the header/footer is separated from the body of the printout using a selectable decoration.

**Note**: in order to make the print settings persistent (so they are used the next time, you startup PKS-Edit automatically), you must press
the `Save Settings` button.
