# dsc-project
# SaveSphere

SaveSphere is a **C-based Personal Finance and Goal Tracking System** designed to help users manage savings goals, monthly budgets, and expenses efficiently using core **Data Structures and Algorithms (DSA)** concepts.

The project demonstrates practical implementation of:

- Structures
- Arrays
- Functions
- Pointers
- Macros
- Linked Lists
- Stack
- Queue
- Searching
- File Handling (CSV)

---

# Features

## Goal Management
- Add financial goals
- View all goals
- Search goals using Linear Search
- Update savings progress
- Delete goals

## Monthly Financial Records
- Store monthly income, savings, expenses, and budget
- Detect budget overflow
- Generate financial reports

## Expense Management
- Add expenses
- Undo last expense using Stack
- Store pending expenses using Queue

## Reports & Warnings
- Average income, savings, and expenses
- Budget exceeded warnings
- Low savings alerts
- Goal progress tracking

## CSV File Storage
Data is automatically stored in:
- `goals.csv`
- `records.csv`
- `expenses.csv`

---

# Concepts Used

| Concept | Usage |
|----------|--------|
| Structures | Store Goals, Expenses, Records |
| Arrays | Monthly records & expense storage |
| Linked List | Goal management |
| Stack | Undo expense feature |
| Queue | Pending expenses |
| Searching | Goal search |
| Functions | Modular programming |
| Pointers | Linked list traversal |
| Macros | Fixed limits/constants |
| File Handling | CSV read/write |

---

# Project Structure

```bash
SaveSphere/
│
├── main.c
├── goals.csv
├── records.csv
├── expenses.csv
└── README.md
```

---

# How to Compile & Run

## Using GCC

```bash
gcc main.c -o savesphere
./savesphere
```

## On Windows

```bash
gcc main.c -o savesphere.exe
savesphere.exe
```

---

# Menu Options

```text
1. Add Goal
2. View Goals
3. Update Savings
4. Delete Goal
5. Add Record
6. Add Expense
7. View Expenses
8. Undo Expense
9. Queue Pending Expense
10. Process Pending Expense
11. Search Goal
12. Report & Warnings
13. Exit
```

---

# Data Structures Explanation

## Linked List
Used for storing financial goals dynamically.

## Stack
Used for implementing the Undo Expense feature (LIFO).

## Queue
Used for storing pending expenses (FIFO).

## Arrays
Used for storing monthly financial records and expense history.

---

# Sample Functionalities

## Add Goal
Users can add:
- Goal Name
- Target Amount
- Saved Amount
- Deadline Months

## Budget Warning
If:

```text
Expenses > Budget
```

The system displays:

```text
WARNING: Budget Exceeded!
```

## Savings Warning
If savings are less than 20% of income:

```text
Low savings warning displayed
```

---

# GitHub Auto Save

On Exit, the program automatically executes:

```bash
git add *.csv
git commit -m "Auto-update"
git push
```

This keeps CSV data updated on GitHub.

---

# Limitations

- Maximum records: 50
- Maximum expenses: 100
- Console-based UI only
- No authentication system

---

# Future Improvements

- User Login System
- Graphical User Interface (GUI)
- Database Integration
- Expense Categories Analytics
- Sorting Algorithms
- Monthly Charts & Reports

---

# Author

Developed as a DSA-based Finance Management Project in C.

---

# License

This project is open-source and free to use for educational purposes.
