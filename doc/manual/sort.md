# Sorting Files

Using Functions->Sort Files... one can sort a selectable *range* of text in a file. Various options for sorting files exist which 
can be selected in the dialog that opens.

- Selection Criteria - this can be used to "select the lines" that should actually be sorted inside the selected range. Optionally specify here a 
  [regular expression](#regular-expressions) to select the lines, that should be sorted. The selection criteria can also be used to sort multi-line blocks
  in the file. To use this feature check the option `Bind to sort criteria`. PKS-Edit will now treat the lines matching the selection criteria as the head
  of a block of text up to the next line matching the selection criteria to be moved in the process of sorting in one piece.
- Field Delimiter - specify one or more characters (you may use special syntax as in `\t` to name special characters) to be used to define key fields
  in the text. To sort a CSV format file for instance one would use ",;" as possible delimiters. To skip multiple separators (e.g. when a line contains fields
  separated by a varying number of whitespaces), check the option `Skip Multiple Delimters`.
- Key Fields - allow to define the details about comparing the lines. One uses the following syntax to define a *key field*: `{[l.f.o]{c},}`
  - `l` is the 0-based index of a line in a block of lines to sort
  - `f` is the 0-based index of the field in the line
  - `o` is an optional character based offset in the line, starting from where a comparison is being made
  - `c` specifies an option for comparison of the field
    - `a` - the fields are compared alphanumerically (default case)
    - `i` - ignore case during alphanumeric comparison
    - `d` - the field is interpreted as a number to sort
    - `D` - the field is interpreted as a Date specification (various formats are supported - this will be improved in future versions of PKS-Edit)
    - `b` - skip leading blanks during comparison
    - `u` - treat fields as *unique*. All fields, which are not unique are marked for deletion and can be deleted after sorting.

## Example

If one tries to sort the following table using the chemical sign (2<sup>nd</sup> column).

Element|Sign|\[g/cm<sup>3</sup>]
-------|----|-----
Chrome|Cr|5.102 
Iron|Fe|5.67 
Nickel|Ni|5.67

Then one would specify a key field of `0.1` (1<sup>st</sup> line, 2<sup>nd</sup> column).

To sort for the weight (3rd column), one can specify `0.2.d` (1<sup>st</sup> line, 3<sup>rd</sup> column - sort *numerically*).
