**********************************************************
* COBCALC                                                *
*                                                        *
* A simple program that allows financial functions to    *
* be performed using intrinsic functions.                *
*                                                        *
**********************************************************
   IDENTIFICATION DIVISION.
   PROGRAM-ID. COBCALC.
   ENVIRONMENT DIVISION.
   DATA DIVISION.
   WORKING-STORAGE SECTION.
   01  PARM-1.
       05  CALL-FEEDBACK     PIC XX.
   01  FIELDS.
       05  INPUT-1           PIC X(10).
   01  INPUT-BUFFER-FIELDS.
       05  BUFFER-PTR        PIC 9.
       05  BUFFER-DATA.
           10  FILLER        PIC X(10)  VALUE "LOAN".
           10  FILLER        PIC X(10)  VALUE "PVALUE".
           10  FILLER        PIC X(10)  VALUE "pvalue".
           10  FILLER        PIC X(10)  VALUE "END".
       05  BUFFER-ARRAY    REDEFINES BUFFER-DATA
                           OCCURS 4 TIMES
                             PIC X(10).

   PROCEDURE DIVISION.
       DISPLAY "CALC Begins." UPON CONSOLE.
       MOVE 1 TO BUFFER-PTR.
       MOVE SPACES TO INPUT-1.
  * Keep processing data until END requested
       PERFORM ACCEPT-INPUT UNTIL INPUT-1 EQUAL TO "END".
  * END requested
       DISPLAY "CALC Ends." UPON CONSOLE.
       GOBACK.
  * End of program.

  *
  * Accept input data from buffer
  *
   ACCEPT-INPUT.
       MOVE BUFFER-ARRAY (BUFFER-PTR) TO INPUT-1.
       ADD 1 BUFFER-PTR GIVING BUFFER-PTR.
  * Allow input data to be in UPPER or lower case
       EVALUATE FUNCTION UPPER-CASE(INPUT-1)     CALC1
         WHEN "END"
           MOVE "END" TO INPUT-1
         WHEN "LOAN"
           PERFORM CALCULATE-LOAN
         WHEN "PVALUE"
           PERFORM CALCULATE-VALUE
         WHEN OTHER
           DISPLAY "Invalid input: " INPUT-1
       END-EVALUATE.
  *
  * Calculate Loan via CALL to subprogram
  *
   CALCULATE-LOAN.
       CALL "COBLOAN" USING CALL-FEEDBACK.
       IF CALL-FEEDBACK IS NOT EQUAL "OK" THEN
         DISPLAY "Call to COBLOAN Unsuccessful.".
  *
  * Calculate Present Value via CALL to subprogram
  *
   CALCULATE-VALUE.
       CALL "COBVALU" USING CALL-FEEDBACK.
       IF CALL-FEEDBACK IS NOT EQUAL "OK" THEN
         DISPLAY "Call to COBVALU Unsuccessful.".
