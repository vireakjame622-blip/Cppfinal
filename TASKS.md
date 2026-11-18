# Smart Library Management System - Task Split (from the team Notion board)

Run: open this folder in VS Code, press Ctrl+Shift+B to build, then run `./library`
(Windows: `.\library.exe`). Run it from the project folder so `data/` is found.

Rule: edit ONLY your own section in main.cpp (between its START / END markers).

## Kheang  (Section 1)
| Task | Phase / Week | Functions |
|------|--------------|-----------|
| Create Book Entity & Data Validation | Phase 1 (W4) / Week 4 | isValidBookInput |
| Create BookList (SLL) & Catalog Operations | Phase 1 (W4) / Week 4 | addBook, displayBooks, updateBook, removeBook, checkAvailability, findBookById, searchByTitleOrAuthor* |
| Implement Recent Returns Stack (LIFO) | Phase 1 (W5) / Week 5 | pushReturn, showRecentReturns |

## Jame  (Section 2)
| Task | Phase / Week | Functions |
|------|--------------|-----------|
| Create Student Entity & StudentList (SLL) | Phase 1 (W4) / Week 4 | addStudent, displayStudents, updateStudent, removeStudent, findStudentById |
| Create LoanList (SLL) & Borrow/Return Logic | Phase 1 (W5) / Week 5 | borrowBook, returnBook, displayActiveLoans, displayBorrowingRecords, generateLoanId, hasActiveLoanForStudent, hasActiveLoanForBook |
| Add Book Sorting (Deferred: after Sorting lecture) | Phase 2 (W6-8) / Week 8 | sortBooks |

## Ken  (Section 3)
| Task | Phase / Week | Functions |
|------|--------------|-----------|
| Create Book Waitlist (FIFO Queue) & Resolution | Phase 1 (W4-5) / Week 5 | enqueueStudent, dequeueStudent, isInQueue, showReservationStatus, monitorReservationQueues, clearQueue |
| Upgrade Book Lookup to BST (Deferred: after Tree lecture) | Phase 2 (W6-8) / Week 7 | bstInsert, bstSearch, bstRemove, rebuildBST, freeBST, searchById |

## Phanea  (Section 4)
| Task | Phase / Week | Functions |
|------|--------------|-----------|
| Create CLI Menu & Role Navigation | Phase 1 (W4-5) / Week 5 | librarianMenu, studentMenu, mainMenu, main |
| Create File I/O (CSV) Persistence | Phase 1 (W4-5) / Week 5 | loadAllData, saveAllData |
| Destructors, Guards & Edge-Case Hardening | Phase 3: Demo & Q&A (W9-10) / Week 9 | freeAllMemory, input guards, edge cases |

\* `searchByTitleOrAuthor` is not on the Notion board; it is placed with Kheang's catalog operations. Move it if you assign it to someone else.

## Dependencies between members
- Jame's borrowBook / returnBook call Ken's enqueueStudent / dequeueStudent / isInQueue and Kheang's findBookById / pushReturn.
- Kheang's removeBook calls Jame's hasActiveLoanForBook and Ken's clearQueue / bstRemove.
- Phanea's loadAllData uses every entity struct and calls Ken's rebuildBST after loading books.
- Until a function exists, its stub in main.cpp keeps the project compiling.

## CSV column order (agreed format)
- books.csv: BookID,Title,Author,Category,TotalCopies,AvailableCopies
- students.csv: StudentID,FullName,Major
- loans.csv: LoanID,StudentID,BookID,BorrowDate,ReturnDate,Status
- reservations.csv: BookID,StudentID,RequestTimestamp (in queue order, front first)
- No commas inside fields (validated by Kheang's isValidBookInput, Jame's student checks).

## Merge checklist
1. Each member sends their section only.
2. Paste into the matching START/END block of the final main.cpp.
3. Build with zero errors, then test: add book -> add student -> borrow -> reserve -> return -> save/exit -> reopen.
