#include <stdio.h>

/*
 * This program demonstrates how to let the user input two values to be multiplied
 * using a C program.
 */
int main() {
    double a, b, product;
    printf("Enter two numbers:\n");
	// Scan input from stdin
    scanf("%lf %lf", &a, &b);

    // Calculating product
    product = a * b;

    // %.2lf displays number up to 2 decimal point
    printf("Product = %.2lf %c", product, '\'');

    return 0;
}
