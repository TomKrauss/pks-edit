# Sorting Files

Using Functions->Sort Files... one can sort a selectable *range* of text in a file. Various options for sorting files exist which 
can be selected in the dialog that opens.

- Selection Criteria - this can be used to "select the lines" that should actually be sorted inside the selected range. Optionally specify here a 
  [regular expression](#regular-expressions) to select the lines, that should be sorted. The selection criteria can also be used to sort multi-line blocks
  in the file. To use this feature check the option `Bind to sort criteria`. PKS-Edit will now treat the lines matching the selection criteria as the head
  of a block of text up to the next line matching the selection criteria to be moved in the process of sorting in one piece.
- Field Delimiter - specify one or more characters (you may use special syntax as in `\t` to name special characters) to be used to define key fields
  in the text. To sort a CSV format file for instance one would use ",;" as possible delimiters. To skip multiple separators (e.g. when a line contains fields
  separated by a varying number of whitespaces), check the option `Skip Multiple Delimters`. To support CSV like quoting (using `"` characters to enclose
  separators), check the corresponding option, to support backslash quoting of delimiters also check the corresponding option.
- Key Fields - allow to define the details about comparing the lines. One uses the following syntax to define a *key field*: `{f[0-9]+{l[0-9]+}?{w[0-9]+}?[dDiabu-]}+`
  - `f` is followed by a number - the 0-based index of the field in the line
  - `l` is followed by a number - the 0-based index of a line in a block of lines to sort
  - `o` is followed by a number - an optional character based offset in the line, starting from where a comparison is being made. If fields 
     are separated using field separators, offset into the field otherwise offset into the line sorted
  - `w` is followed by a number - an optional character based width of a field in characters
  - These specifications may be extended by option characters defining options for the sort field
    - `a` - the fields are compared alphanumerically (default case)
    - `i` - ignore case during alphanumeric comparison
    - `d` - the field is interpreted as a number to sort. Numbers may be integers or floating point numbers with either , or . as the
    -       decimal separator. Other formats such as grouping chars or exponential formats are not yet supported.
    - `D` - the field is interpreted as a Date specification (various formats are supported - this will be made customizable in future versions of PKS-Edit).
            Currently the following formats are tried (note, that date, time and date+time formats are tried): `%m/%d/%y`, `%d.%m.%Y`,
	        `%Y-%m-%d`, `%d. %B %y`, `%Y-%m-%d %H:%M:%S`, `%m/%d/%y %H:%M:%S`, `%H:%M:%S`. 
    - `b` - skip leading blanks during comparison
    - `u` - treat fields as *unique*. All fields, which are not unique are marked for deletion and can be deleted after sorting.**
    - `-` - sort by this field descending rather than ascending

## Example 1

If one tries to sort the following table using the chemical sign (2<sup>nd</sup> column).

Element|Sign|\[g/cm<sup>3</sup>]
-------|----|-----
Chrome|Cr|5.102 
Iron|Fe|5.67 
Nickel|Ni|5.67

Then one would specify a separator field of ` \t` (check option skip multiple separators) and a key field of `f1` (2<sup>nd</sup> key in the line split by `|`).

To sort for the weight (3rd column) descending, one can specify `f2d-` (3<sup>rd</sup> column - sort *numerically*).

## Example 2

To sort the following CSV file, select \(highlight) the data lines of the following text, set the field separator to ";".

```csv
ID;Person;Birthday
1;Hans Miller;1983/1/3
2;Oscar Meyer;1988/4/12
```

To sort comparing two fields, the 1<sup>st</sup> one interpreting the value in the 3<sup>rd</sup> column as Date enforcing a field width
of 10 characters and the 2<sup>nd</sup> one numerically comparing descending numbers in the 1<sup>st</sup> column use: `f0-d,f2w10D`.





