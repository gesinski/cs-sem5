# P1 Assignment: Lexical Analyzer for a Simplified Simula Language

## 1. Objective

The goal of this exercise is to create a simple lexical analyzer for a very simplified version of the Simula programming language.

The analyzer should:
- Recognize Simula language elements and determine their values
- Remove whitespace and comments
- Detect lexical errors

## 2. Tasks to Complete

You need to complete the provided lexical analyzer skeleton and test it with the provided test programs to show that it works correctly.

The analyzer should print information about recognized terminal symbols in three columns:
1. Matched text
2. Recognized symbol
3. Symbol value (only if the symbol actually has a value)

The provided function `process_token()` should be used to print this information.  
The function returns the recognized symbol, so each recognition rule should include:

return process_token(...);

with the appropriate parameters.

## 3. Steps to Implement

Complete the provided code in the following order:

A. Print your name and surname (in the Bison file) ✓
B. Detect keywords defined using the %token directive in the Bison source file (names starting with KW_, except KW_COMMENT)  ✓
C. Remove whitespace  ✓
D. Remove inline comments (starting with !, ending with ;)  ✓
E. Remove end-line comments (starting with the keyword end and ending with a semicolon ; on the same line; the semicolon is not part of the comment) - worth 2 points  ✓
F. Detect multi-character operators (<=, :=, etc.) found in the test programs  ✓
G. Detect identifiers ✓ 
H. Detect unsigned integers (ignore different bases, only normal integers)  
I. Detect floating-point numbers (same rule as above)  
J. Detect string constants (without using start conditions; ignore double quotes inside strings, include this rule as a comment)  
K. Detect character constants  
L. Detect single-character terminal symbols (operators, punctuation marks, etc.)  
M. Detect strings using start conditions - two consecutive double quotes ("") represent a single quote  
N. Detect unclosed comments and indicate the line number where the comment started (using start conditions)

## 4. Grading

- Each element except E is worth 1 point  
- Element E is worth 2 points  
- Total: 15 points

Note:
- The lexical analyzer will be needed in future classes.
- The file must be submitted as an assignment on the eNauczanie platform.
- Elements that should be passed to the syntax analyzer must be returned using the return statement.
- Elements that should be removed (like comments or whitespace) must not use return.
