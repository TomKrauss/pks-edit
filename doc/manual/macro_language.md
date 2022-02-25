# Recording Macros and the PKS-Edit Macro Language

## Macros in PKS-Edit

Macros in PKS-Edit can be recorded or manually edited using the _PKSMacroC Language_ and compiled inside PKS-Edit.
All known macros are loaded from a file called **pksedit.mac** loaded from the `pks_sys` directory.

The macro file contains currently:

- custom macros performing text operations - for instance used as post-save actions or the like.

## Recording macros

To record a macro, use "Macro->Record" and start doing some actions (e.g. move the cursor, insert text, etc...). When done press "Macro->Record" again. This will open a little dialog
allowing to "name" the macro and bind it to a keyboard key so it can be used subsequently. Note, that in order to make the macro persistent, you must save the the current set of macros.
When PKS-Edit exits and there are unsaved macros, it warns about that and asks the user to save the macros now.

## The Maco menu

Allows you to edit macros defined. One may add comments / remove macros, decompile a recorded macro, disassemble a macro from here.

## Compiling macros

To compile one or more macros, you open a macro file (see the `macros` sub-directory for samples) and press F10 once the file is loaded and then OK. If the compilation is successful
a message is displayed, that no error was found. Otherwise an error log is opened, which can be navigated with Ctrl+N and Ctrl+P to jump to the corresponding line numbers with the
erors.

## The PKSMacroC Language

The PKS-Edit Macro Language is pretty similar to the C programming language and supports the most common operators 
and functions and variable declarations as a C program with some modifications and extensions. There is no explicit memory management
necessary - the macro interpreter performs all object allocations automatically for you.

Here is a sample showing a piece of macro code, which will calculate the factorial of a number to enter and display the
result in a message box:

```
/*
 * Calculate the factorial of a number input into a message box.
 * Note: PKSMakroC - PKS MacroC Integers are 64 bit big, this macro
 * will cause overflows for fac(>16) !
 */
macro calculateFactorial() {
    int     n = 1;
    int     nSelected = 1;
    int     result = 1;

    nSelected = (int)PromptAssign("Calculate factorial of: ","10");
    if (nSelected == 0) {
        MessageBox(MB_ICONINFORMATION|MB_OK, "Calculation aborted.");
        return;
    }
    n = nSelected;

    while(n > 1) {
        result *= n;
        n--;
    }

    if (result < 0) {
        MessageBox(MB_ICONERROR|MB_OK, "Overflow during calculation.");
    } else {
        MessageBox(MB_ICONINFORMATION|MB_OK, "The factorial of " + nSelected + " is: " + result);
    }
}
```

## PKSMacroC Types

**PKSMacroC** supports 3 types of commments:

- `#` as first character introduces single-line comments
- `//` also introduces single-line comments
- `/*` and `*/` may introduce multi-line comments similar to C. If one places a multi-lane comment before a macro, the comment will be used as 
  the description / help (in code completion) text for that macro.
 
The **PKSMacroC** languages supports the following types, which can be used:

- `string` as the name implies a string (internally implemented using zero terminated strings). Strings in PKS-Edit are dynamically allocated and de-allocated.
  no memory management is necessary in PKSMacroC code. One can concatenate strings by using the '+' operator. `string` literals follow C syntax and are enclosed
  in `"´` characters.
- `int` represents integer numbers in a 64 bit value range. `int` literals may be written as either decimal numbers (`123`), hexadecimal numbers (`0xAF34`) or
  octal numbers (`\0234`).
- `float` represents floating point numbers. Floating point literals contain either a `.` character and may have a trailing `d` suffix (`12d`).
- `boolean` boolean types are very often not explicitly defined, but created temporarily by executing a logical / relational operator on other types.

PKSMacoC will try to coerce values to their "right" types or you may `cast` a value type to another explicitly using a cast operator. The following examples
shows implicit and explicit coercions.

```
(int)"123"                -> 123
(string)1                 -> "1"
// implicit coercion
"Hello " + 3 + " worlds"  -> "Hello 3 worlds"
```

## PKSMacroC operators

PKSMacroC supports the most common operators also supported in the C language.

- `+`, `-`, `*`, `/`, `%`   	   arithmetic operators
- `!`, `<`, `<=`, `>`, `>=`, `==`, `!=`, `&&`, `||`  logical operators
- `&`, `|`, `^` bitwise operators
- `~`, `!~` _matches_ / _does_not_match_ operator. Can be used to match a string with a regular expression (`"aa" ~ "[a]+"`)
- `^^`  power to operator to be applied to float or int data types

**PKSMacroC** also allows to use the typical C assignment shorthand notations such as

- `i++`, `i--` increment or decrement operators
- `i += x`, `i *= x`, ... shorthand assignment and calculation operators.

