# Indexed Sequential File Organization

## Introduction

Indexed Sequential File Organization is a classic method of organizing data on disk, primarily used in file systems and older database systems. It combines two data access approaches:

- **Sequential access**, where records are stored in sorted order by key.
- **Indexed access**, which enables fast location of the block containing a desired record using a dedicated index structure.

This combination allows efficient single-record searches while still supporting insert operations.

---

## How It Works

### Structure Organization

The indexed sequential structure consists of three main areas:

- **Index**  
  Contains index entries pointing to the first record on each page along with references to the corresponding data pages.

- **Primary Area**  
  The main storage area holding records sorted by key. If there is no space for a new record, some entries contain pointers to the overflow area.

- **Overflow Area**  
  Stores records that could not be placed in their correct position in the primary area. Records that logically belong to the same page form an ordered linked list in this area.

---

### Structural Implications

Searching for records located in the overflow area increases access time. Therefore, periodic **reorganization** of the structure is required. Reorganization involves moving records from the overflow area back into the primary area and re-sorting the data.

Indexes are always stored in ascending order, which allows fast searching and significantly improves the process of locating the correct data pages.

---

## Implementation

The indexed sequential structure is implemented in **C++**. The program consists of two main layers:

- **Presentation layer**  
  Responsible for user interaction, menu handling, and control logic.
- **Logic layer**  
  Implements the data structure algorithms and mechanisms for block-based reading and writing of data to files.

At any given time, auxiliary variables and at most **four disk pages** are stored in main memory.

Each record contains:
- A **unique key**: a non-negative 4-byte integer.
- A **pointer** of the same size, used to manage the overflow area.
- Associated data (e.g. name and surname).

---

### Supported Operations

After selecting an operation from the menu, the user typically provides a record key. The following operations are supported:

- **Insert**  
  The algorithm searches for the correct insertion location. First, the index file is read to find the appropriate page—one where the record key is greater than or equal to the smallest key on the page and smaller than the smallest key of the next index entry.  
  If there is free space on the page, the record is inserted there. Otherwise, it is placed in the overflow area and pointers are updated accordingly.

- **Fetch**  
  The record is located in the same way as during insertion, then read and displayed.

- **Delete**  
  After locating the record (as in `Fetch`), it is marked as deleted, for example by setting its data value to `-1`. Such records are skipped during the next reorganization.  
  If the smallest record on a page is deleted, reorganization is triggered automatically.

- **Update**  
  The record is found using the same method as above, and its value is replaced with a new one provided by the user.

---

### Pages and Reorganization

Each area of the structure is divided into pages containing **`b` records**.  
The overflow area consists of a single page. Reorganization is triggered when:

- The overflow page becomes full, or
- The smallest record on a page is deleted.

---

## Test File Format

The test file consists of a sequence of records describing operations performed on the indexed sequential structure.

Each record starts with a **1-byte `char`** indicating the operation type:

- `i` — insert  
- `d` — delete  
- `u` — update  

This is followed by a **4-byte non-negative integer key**.

For operations that require additional data (`insert` and `update`), a text field in the format: Nazwisko Imię
is appended. The text is stored on a single line, separated by a single space. Due to variable name lengths, a fixed-size text field of **up to 24 bytes** is used.

### Example (Human-Readable Form)


```
i 5 Zuniga Janet
i 26 Johnston Daisy
i 73 Benson Jon
u 26 Merritt Breanna
i 14 Barnes Emma
d 26
u 5 Flowers Gabriel
u 73 Savage Lorraine
d 14
```


In its actual form, the test file is **binary** and not human-readable, as it contains no whitespace and stores data in binary format.

---

## Program Output and Statistics

After each operation that modifies the structure, the program can display the contents of the data and index files in a human-readable format. All pages of the **index**, **primary**, and **overflow** files are shown to present the internal structure.

After completing an operation, the program reports whether it was successful and displays the following statistics:

- **Disk writes** — number of data blocks written to disk  
- **Disk reads** — number of data blocks read from disk  

A single disk operation corresponds to reading or writing one block containing **`b` records**.

## Bugs

While deleting element that have a pointer to overflow section. The files from overflow section are also deleted after reorganization.