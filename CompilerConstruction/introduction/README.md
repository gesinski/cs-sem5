# Lab I – Regular Expressions in Flex

The goal of this lab is to **learn how to use the Flex lexical analyzer generator** in a Linux environment.  
During the exercises, you will create simple text processors, code analyzers, and lexical analyzers using regular expressions.

---

## Compilation

- Source files should have the `.l` extension, e.g. `z1.l` or `z2.l`
- Compile the program using the provided `Makefile`:
  ```
  make
  ```

---

## TASK 1

The input data consists of lines of characters.
Sequences of digits are copied to the output without any changes.
Sequences of other (non-digit) characters between them are replaced by single plus signs (+).
After printing the interpreted characters for each line, an equal sign (=) is printed, followed by the number representing the result of the addition, and finally a newline character.
Additionally, leading and trailing sequences of non-digit characters may be removed.

To implement this task, the atoi() function (see man atoi) from the stdlib library may be useful - remember to include the header file stdlib.h.

Example:
```
1,b22ad55QW11ɬ
1+22+55+11=89
13(any)| text31ɬ
13+31=44
```

## TASK 2 - Program Metrics and Basic Lexical Analysis

The program should detect (printing each element on a separate line) and, after the end of the input stream, print the total count of selected C language elements found in the input.  

In the following stages, the program should be extended with the following functionalities:

### 1. Detection, counting, and final printing of:
- **Keywords** (e.g., `int`, `char`, `void`, `for`, `if`, `else`, …)
- **Identifiers** (sequences of letters, digits, and underscores starting with a letter), e.g., `a`, `a1`, `aA`, `A3a` — excluding keywords
- **Unsigned integers**, e.g., `1`, `007`
- **Unsigned real numbers**, e.g., `12.345`, `.678`, `123.`
- **String literals** (in double quotes), e.g., `"Alf - ?"`
- **Character constants**, e.g., `'a'`
- **Arithmetic operators**, e.g., `+`, `-`, `*`, `/`, `++`, `--`

### 2. Counting and printing at the end:
- **Characters** (all - total number equals the file length)
- **Words** (sequences of letters; a subset of the sum of keywords and identifiers, which should therefore be separated into two categories)
- **Lines**

### 3. Additional features:
- Counting and **removing whitespace characters** (spaces, tabs)
- Ignoring **C language comments** (non-nested, single-line comments)