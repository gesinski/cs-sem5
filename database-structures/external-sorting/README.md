# Project: External Sorting (Merging Methods)

## Project Objective
The goal of this assignment is to implement a program that sorts a file using **one of the external sorting methods**:
- **Natural merge sort**,
- **Merge sort with large buffers**, or
- **Polyphase merge sort**.

The tapes used in the sorting process should be implemented as **disk files**.

---

## Functional Requirements

The program should:

1. **Simulate block reading and writing** as a separate logical layer.  
   - This layer must provide at least two operations:
     - reading a single record,  
     - writing a single record.

2. **Generate records** to be sorted:
   - randomly, and  
   - from user input (keyboard).

3. **Display file contents**:
   - before sorting, and  
   - after sorting.

4. **Allow displaying the file after each sorting phase.**

5. **After sorting is complete**, display:
   - the total number of sorting phases, and  
   - the total number of disk page reads and writes.

6. **Allow loading test data** from an external test file.

---

## Experimental Study

An experiment should be carried out using the implemented program to:

1. Determine:
   - the number of sorting phases, and  
   - the number of disk page reads and writes  
     depending on the number of generated records  
     (for **at least 5 significantly different record counts**).

2. Calculate the **theoretical number of phases** and **disk operations** for those record counts.

3. **Present the results** on a graph, ensuring a proper scale on both axes  
   (the scale may be **linear** or **logarithmic**).

4. **Compare the experimental and theoretical results**,  
   and **explain any observed discrepancies.**

---

## Additional Information

- **Record Type** is surnames and lastnames in lexicographical order.  
- The program should be organized into distinct logical layers:
  - sorting logic,  
  - disk I/O simulation layer,  
  - input/output and data generation layer.

---
