# Recording Macros and the PKS-Edit Macro Language

## Macros in PKS-Edit

Macros in PKS-Edit can be recorded or manually edited using the _PKSMacroC Language_ and compiled inside PKS-Edit.
All known macros are loaded from a file called **pksedit.mac** loaded from the `pks_sys` directory.

The macro file contains currently:

- custom macros performing text operations
- keyboard, mouse and menu bindings to PKS-Edit commands and macros

## Recording macros

To record a macro, use "Macro->Record" and start doing some actions (e.g. move the cursor, insert text, etc...). When done press "Macro->Record" again. This will open a little dialog
allowing to "name" the macro and bind it to a keyboard key so it can be used subsequently. Note, that in order to make the macro persistent, you must save the the current set of macros.
When PKS-Edit exits and there are unsaved macros, it warns about that and asks the user to save the macros now.

## PKSMacroC Language

The PKS-Edit Macro Language is pretty similar to the C programming language and supports the most common operators 
and functions and variable declarations as a C program with some modifications and extensions. There is no explicit memory management
necessary - the macro interpreter performs all object allocations automatically for you.

Here is sample of a piece of macro code, which will calculate the factorial of a number to enter and display the
result in a message box:

```
#
# Calculate the factorial of a number input into a message box.
# Note: PKSMakroC - Integers are only 24 bit big, this macro
# will cause overflows for fac(>12) !
#
macro calculateFactorial() {
	int		count = 1;
	int		n = 1;
	int		save_n = 1;
	int		res = 1;
	string	result = "";

	n = (int)PromptAssign("Calculate factorial of: ","10");
	save_n = n;

	while(n > 1) {
		res = res * n;
		n = n - 1;
	}

    if (res < 0) {
        MessageBox(0, MB_ICONERROR|MB_OK, "Overflow during calculation.");
    } else {
	    result = "The factorial of " + (string) save_n + " is: " + (string) res;
        MessageBox(0, MB_ICONINFORMATION|MB_OK, result);
    }
}

```
