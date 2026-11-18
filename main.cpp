// ============================================================================
//  SMART LIBRARY MANAGEMENT SYSTEM  (DSA Term 1 Group Project)
//  Console-based C++ application
//  Data structures: Singly Linked List, FIFO Queue, BST, Stack, Sorting, File I/O
//
//  HOW TO WORK AS A TEAM
//  - Every member only edits THEIR OWN section (between the START/END markers).
//  - Section 0 (shared structs + prototypes) is agreed by everybody. If you need
//    to change it, tell the team first.
//  - All functions are already declared (prototypes) so everyone can compile
//    at any time, even if other members have not finished yet.
//  - When finished, copy your section into the main project's main.cpp.
//
//  TASK DIVISION (matches the team's Notion board)
//    KHEANG  : Book Entity & Data Validation          (Phase 1, Week 4)
//              BookList (SLL) & Catalog Operations     (Phase 1, Week 4)
//              Recent Returns Stack (LIFO)             (Phase 1, Week 5)
//    JAME    : Student Entity & StudentList (SLL)     (Phase 1, Week 4)
//              LoanList (SLL) & Borrow/Return Logic    (Phase 1, Week 5)
//              Add Book Sorting (deferred)             (Phase 2, Week 8)
//    KEN     : Book Waitlist (FIFO Queue) & Resolution (Phase 1, Week 5)
//              Upgrade Book Lookup to BST (deferred)   (Phase 2, Week 7)
//    PHANEA  : CLI Menu & Role Navigation             (Phase 1, Week 5)
//              File I/O (CSV) Persistence              (Phase 1, Week 5)
//              Destructors, Guards & Edge-Case Hardening (Phase 3, Week 9)
//
//  SECTIONS IN THIS FILE
//    0. Shared (structs, globals, prototypes, helpers)   -> everyone
//    1. KHEANG
//    2. JAME
//    3. KEN
//    4. PHANEA
// ============================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <limits>
#include <ctime>
using namespace std;

// ============================================================================
// SECTION 0: SHARED  (structs, globals, prototypes, helpers)
// ============================================================================

// ---------- Reservation waitlist (FIFO queue, one per book) ----------
struct WaitNode {
    string studentId;
    string requestTimestamp;
    WaitNode* next;
};

struct WaitQueue {
    WaitNode* front = nullptr;
    WaitNode* rear  = nullptr;
    int size        = 0;
};

// ---------- Core entities (Singly Linked Lists) ----------
struct Book {
    string id;
    string title;
    string author;
    string category;
    int totalCopies     = 0;
    int availableCopies = 0;
    WaitQueue waitlist;          // reservation queue for this book
    Book* next          = nullptr;
};

struct Student {
    string id;
    string fullName;
    string major;
    Student* next = nullptr;
};

struct Loan {
    string loanId;
    string studentId;
    string bookId;
    string borrowDate;
    string returnDate;           // "" while still borrowed
    string status;               // "Borrowed" or "Returned"
    Loan* next = nullptr;
};

// ---------- BST (search books by Book ID) ----------
struct BSTNode {
    Book* book;                  // points to the node in the book linked list
    BSTNode* left  = nullptr;
    BSTNode* right = nullptr;
};

// ---------- Stack (recent return actions) ----------
struct ReturnNode {
    string loanId;
    string studentId;
    string bookId;
    string returnDate;
    ReturnNode* next = nullptr;
};

// ---------- Global data (heads of the structures) ----------
Book*       bookHead    = nullptr;
Student*    studentHead = nullptr;
Loan*       loanHead    = nullptr;
BSTNode*    bstRoot     = nullptr;
ReturnNode* returnTop   = nullptr;
int         loanCounter = 0;     // used to generate unique Loan IDs

// ---------- File names ----------
const string BOOKS_FILE        = "data/books.csv";
const string STUDENTS_FILE     = "data/students.csv";
const string LOANS_FILE        = "data/loans.csv";
const string RESERVATIONS_FILE = "data/reservations.csv";

// ---------- Prototypes: KHEANG (Book entity, BookList, Returns stack) ----------
bool  isValidBookInput(const string& id, const string& title, const string& author,
                       const string& category, int totalCopies);   // data validation
void  addBook();
void  displayBooks();
void  updateBook();
void  removeBook();
void  checkAvailability();
Book* findBookById(const string& id);        // linked-list lookup
void  searchByTitleOrAuthor();               // linear search (catalog operation)
void  pushReturn(const Loan& loan);          // Recent Returns Stack: push
void  showRecentReturns();                   // Recent Returns Stack: display / pop

// ---------- Prototypes: JAME (Student list, Loan list, Borrow/Return, Sorting) ----------
void     addStudent();
void     displayStudents();
void     updateStudent();
void     removeStudent();                    // only if no active loans
Student* findStudentById(const string& id);
void     borrowBook(const string& studentId, const string& bookId);
void     returnBook();                       // by Loan ID or Student ID + Book ID
void     displayActiveLoans();
void     displayBorrowingRecords();          // report for librarian
string   generateLoanId();
bool     hasActiveLoanForStudent(const string& studentId);
bool     hasActiveLoanForBook(const string& bookId);
void     sortBooks();                        // Phase 2: by ID / Title / available copies

// ---------- Prototypes: KEN (Waitlist FIFO queue, BST) ----------
void   enqueueStudent(Book* book, const string& studentId);
string dequeueStudent(Book* book);           // returns "" if empty
bool   isInQueue(Book* book, const string& studentId);
void   showReservationStatus(const string& studentId);
void   monitorReservationQueues();           // librarian view
void   clearQueue(Book* book);
// Phase 2: BST lookup
BSTNode* bstInsert(BSTNode* root, Book* book);
Book*    bstSearch(BSTNode* root, const string& id);
BSTNode* bstRemove(BSTNode* root, const string& id);
void     rebuildBST();
void     freeBST(BSTNode* root);
void     searchById();                       // uses BST

// ---------- Prototypes: PHANEA (Menus, File I/O, Cleanup) ----------
void loadAllData();                          // read CSV files at start
void saveAllData();                          // write CSV files before exit
void freeAllMemory();                        // destructors: delete every node
void librarianMenu();
void studentMenu();
void mainMenu();

// ---------- Shared helpers (ready to use by everyone) ----------
void notImplemented(const string& name) {
    cout << "[TODO] " << name << " is not implemented yet.\n";
}

void pauseScreen() {
    cout << "\nPress Enter to continue...";
    cin.get();
}

string readLine(const string& prompt) {
    string s;
    cout << prompt;
    getline(cin, s);
    return s;
}

// Reads an integer in [minVal, maxVal], repeats until valid.
int readInt(const string& prompt, int minVal, int maxVal) {
    while (true) {
        cout << prompt;
        string line;
        getline(cin, line);
        try {
            size_t pos = 0;
            int v = stoi(line, &pos);
            if (pos == line.size() && v >= minVal && v <= maxVal) return v;
        } catch (...) {}
        cout << "Invalid input. Enter a number from " << minVal << " to " << maxVal << ".\n";
    }
}

string getToday() {                           // "YYYY-MM-DD"
    time_t t = time(nullptr);
    char buf[16];
    strftime(buf, sizeof(buf), "%Y-%m-%d", localtime(&t));
    return string(buf);
}

string getTimestamp() {                       // "YYYY-MM-DD HH:MM:SS"
    time_t t = time(nullptr);
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&t));
    return string(buf);
}

// ============================================================================
// SECTION 1: KHEANG
//   - Create Book Entity & Data Validation        (Phase 1, Week 4)
//   - Create BookList (SLL) & Catalog Operations  (Phase 1, Week 4)
//   - Implement Recent Returns Stack (LIFO)       (Phase 1, Week 5)
// ----------------------------------------------------------------------------
// Tasks:
//  - isValidBookInput: non-empty fields, no commas (CSV-safe), copies > 0.
//  - addBook: unique Book ID, title, author, category, total copies
//             (available = total). Insert into the book linked list AND the BST
//             (call bstInsert from Ken once it exists).
//  - displayBooks: formatted table of all books.
//  - updateBook: edit title/author/category/total copies.
//  - removeBook: only if hasActiveLoanForBook() is false; also remove from BST
//             and call clearQueue() on the book.
//  - checkAvailability: show total / borrowed / available copies.
//  - findBookById: linked-list lookup (returns nullptr if not found).
//  - searchByTitleOrAuthor: linear search with partial match (not listed on the
//             Notion board; placed here as a catalog operation, move if needed).
//  - pushReturn / showRecentReturns: stack (LIFO) of recent return actions.
// ============================================================================
// ---------- KHEANG START ----------

bool isValidBookInput(const string& id, const string& title, const string& author,
                      const string& category, int totalCopies) {
    (void)id; (void)title; (void)author; (void)category; (void)totalCopies;
    return true; // TODO
}

void addBook() {
    notImplemented("addBook");
}

void displayBooks() {
    notImplemented("displayBooks");
}

void updateBook() {
    notImplemented("updateBook");
}

void removeBook() {
    notImplemented("removeBook");
}

void checkAvailability() {
    notImplemented("checkAvailability");
}

Book* findBookById(const string& id) {
    // TODO: traverse bookHead and return the matching node
    (void)id;
    return nullptr;
}

void searchByTitleOrAuthor() {
    notImplemented("searchByTitleOrAuthor");
}

void pushReturn(const Loan& loan) {
    (void)loan;
    // TODO: push onto returnTop
}

void showRecentReturns() {
    notImplemented("showRecentReturns");
}

// ---------- KHEANG END ----------

// ============================================================================
// SECTION 2: JAME
//   - Create Student Entity & StudentList (SLL)   (Phase 1, Week 4)
//   - Create LoanList (SLL) & Borrow/Return Logic (Phase 1, Week 5)
//   - Add Book Sorting (deferred)                 (Phase 2, Week 8)
// ----------------------------------------------------------------------------
// Tasks:
//  - addStudent: unique Student ID, full name, major.
//  - displayStudents / updateStudent.
//  - removeStudent: only if hasActiveLoanForStudent() is false.
//  - findStudentById: linked-list lookup.
//  - borrowBook: validate Student ID + Book ID; if availableCopies > 0 ->
//             decrease by 1, generateLoanId(), borrowDate = getToday(),
//             status "Borrowed", append to loan list. If 0 copies -> ask the
//             student to join the waitlist (enqueueStudent from Ken, avoid
//             duplicates with isInQueue).
//  - returnBook: find active loan by Loan ID OR Student ID + Book ID, mark
//             "Returned", set returnDate. Then: if the book's queue has students
//             -> dequeueStudent() and reserve the copy for them + notify
//             librarian; else availableCopies++. Call pushReturn() (Kheang).
//  - displayActiveLoans / displayBorrowingRecords (all loans, report).
//  - generateLoanId: e.g. "L001", "L002" using loanCounter.
//  - hasActiveLoanForStudent / hasActiveLoanForBook: used by remove functions.
//  - sortBooks (Phase 2): by Book ID, Title, or available copies on the book
//             linked list. Do not break the BST.
// ============================================================================
// ---------- JAME START ----------

// ----- Student entity & StudentList -----
void addStudent() {
    notImplemented("addStudent");
}

void displayStudents() {
    notImplemented("displayStudents");
}

void updateStudent() {
    notImplemented("updateStudent");
}

void removeStudent() {
    notImplemented("removeStudent");
}

Student* findStudentById(const string& id) {
    // TODO: traverse studentHead and return the matching node
    (void)id;
    return nullptr;
}

// ----- LoanList & Borrow / Return logic -----
void borrowBook(const string& studentId, const string& bookId) {
    (void)studentId; (void)bookId;
    notImplemented("borrowBook");
}

void returnBook() {
    notImplemented("returnBook");
}

void displayActiveLoans() {
    notImplemented("displayActiveLoans");
}

void displayBorrowingRecords() {
    notImplemented("displayBorrowingRecords");
}

string generateLoanId() {
    // TODO: increment loanCounter and format as L001, L002, ...
    return "";
}

bool hasActiveLoanForStudent(const string& studentId) {
    (void)studentId;
    return false; // TODO
}

bool hasActiveLoanForBook(const string& bookId) {
    (void)bookId;
    return false; // TODO
}

// ----- Book sorting (Phase 2, deferred until after the Sorting lecture) -----
void sortBooks() {
    notImplemented("sortBooks");
}

// ---------- JAME END ----------

// ============================================================================
// SECTION 3: KEN
//   - Create Book Waitlist (FIFO Queue) & Resolution   (Phase 1, Week 5)
//   - Upgrade Book Lookup to BST (deferred)            (Phase 2, Week 7)
// ----------------------------------------------------------------------------
// Tasks:
//  - Reservation FIFO queue: enqueueStudent / dequeueStudent / isInQueue /
//         clearQueue (queue lives inside each Book: book->waitlist).
//  - showReservationStatus: student's position in each queue.
//  - monitorReservationQueues: librarian sees queues of all books.
//  - Resolution: when a book is returned, the first student in the queue gets
//         the copy (Jame calls dequeueStudent from returnBook).
//  - BST (Phase 2, after the Tree lecture): bstInsert / bstSearch / bstRemove /
//         rebuildBST / freeBST (key = Book ID, node stores Book*).
//  - searchById: use bstSearch and show the book + availability. Until the BST
//         is ready, you can fall back to findBookById().
// ============================================================================
// ---------- KEN START ----------

// ----- Reservation queue (FIFO) -----
void enqueueStudent(Book* book, const string& studentId) {
    (void)book; (void)studentId; // TODO
}

string dequeueStudent(Book* book) {
    (void)book;
    return ""; // TODO
}

bool isInQueue(Book* book, const string& studentId) {
    (void)book; (void)studentId;
    return false; // TODO
}

void showReservationStatus(const string& studentId) {
    (void)studentId;
    notImplemented("showReservationStatus");
}

void monitorReservationQueues() {
    notImplemented("monitorReservationQueues");
}

void clearQueue(Book* book) {
    (void)book; // TODO: dequeue until empty
}

// ----- BST (Phase 2) -----
BSTNode* bstInsert(BSTNode* root, Book* book) {
    (void)book;
    return root; // TODO
}

Book* bstSearch(BSTNode* root, const string& id) {
    (void)root; (void)id;
    return nullptr; // TODO
}

BSTNode* bstRemove(BSTNode* root, const string& id) {
    (void)id;
    return root; // TODO
}

void rebuildBST() {
    // TODO: freeBST(bstRoot); then insert every book from bookHead
}

void freeBST(BSTNode* root) {
    (void)root; // TODO: post-order delete
}

void searchById() {
    notImplemented("searchById");
}

// ---------- KEN END ----------

// ============================================================================
// SECTION 4: PHANEA
//   - Create CLI Menu & Role Navigation              (Phase 1, Week 5)
//   - Create File I/O (CSV) Persistence              (Phase 1, Week 5)
//   - Destructors, Guards & Edge-Case Hardening      (Phase 3, Week 9)
// ----------------------------------------------------------------------------
// Tasks:
//  - Menus: librarianMenu, studentMenu, mainMenu (already wired below to every
//         function, improve the UI/formatting and role checks as you like).
//  - loadAllData: read books, students, loans, reservations from CSV (skip the
//         header line). After loading books call rebuildBST(). Set loanCounter
//         so new Loan IDs stay unique.
//  - saveAllData: write all four CSV files (with header line).
//  - freeAllMemory: delete all nodes of every structure (books + their queues,
//         students, loans, return stack, BST) to avoid memory leaks.
//  - Guards & edge cases (Week 9): empty lists, invalid IDs, missing CSV files,
//         bad menu input, duplicate IDs, etc.
//  CSV headers are already in the data/ folder. Column order is in TASKS.md.
// ============================================================================
// ---------- PHANEA START ----------

// ----- File I/O (CSV) -----
void loadAllData() {
    notImplemented("loadAllData");
}

void saveAllData() {
    notImplemented("saveAllData");
}

// ----- Destructors / cleanup -----
void freeAllMemory() {
    // TODO: free books (+ queues), students, loans, return stack, BST
}

// ----- Menus & role navigation -----
void librarianMenu() {
    int choice;
    do {
        cout << "\n===== LIBRARIAN MENU =====\n"
             << " 1. Add Book\n"
             << " 2. Display Books\n"
             << " 3. Update Book\n"
             << " 4. Remove Book\n"
             << " 5. Check Availability\n"
             << " 6. Sort Books\n"
             << " 7. Register Student\n"
             << " 8. Display Students\n"
             << " 9. Update Student\n"
             << "10. Remove Student\n"
             << "11. Borrow Book (for a student)\n"
             << "12. Return Book\n"
             << "13. View Active Loans\n"
             << "14. View Borrowing Records\n"
             << "15. Monitor Reservation Queues\n"
             << "16. Recent Returns\n"
             << " 0. Back\n";
        choice = readInt("Choose: ", 0, 16);
        switch (choice) {
            case 1:  addBook(); break;
            case 2:  displayBooks(); break;
            case 3:  updateBook(); break;
            case 4:  removeBook(); break;
            case 5:  checkAvailability(); break;
            case 6:  sortBooks(); break;
            case 7:  addStudent(); break;
            case 8:  displayStudents(); break;
            case 9:  updateStudent(); break;
            case 10: removeStudent(); break;
            case 11: {
                string sid = readLine("Student ID: ");
                string bid = readLine("Book ID: ");
                borrowBook(sid, bid);
                break;
            }
            case 12: returnBook(); break;
            case 13: displayActiveLoans(); break;
            case 14: displayBorrowingRecords(); break;
            case 15: monitorReservationQueues(); break;
            case 16: showRecentReturns(); break;
            default: break;
        }
        if (choice != 0) pauseScreen();
    } while (choice != 0);
}

void studentMenu() {
    string sid = readLine("Enter your Student ID: ");
    if (!findStudentById(sid)) {
        cout << "Student ID not found.\n";
        return;
    }
    int choice;
    do {
        cout << "\n===== STUDENT MENU (" << sid << ") =====\n"
             << "1. Search Book by ID\n"
             << "2. Search Book by Title/Author\n"
             << "3. Borrow Book\n"
             << "4. Return Book\n"
             << "5. Check Reservation Status\n"
             << "0. Back\n";
        choice = readInt("Choose: ", 0, 5);
        switch (choice) {
            case 1: searchById(); break;
            case 2: searchByTitleOrAuthor(); break;
            case 3: {
                string bid = readLine("Book ID: ");
                borrowBook(sid, bid);
                break;
            }
            case 4: returnBook(); break;
            case 5: showReservationStatus(sid); break;
            default: break;
        }
        if (choice != 0) pauseScreen();
    } while (choice != 0);
}

void mainMenu() {
    int choice;
    do {
        cout << "\n=====================================\n"
             << "   SMART LIBRARY MANAGEMENT SYSTEM\n"
             << "=====================================\n"
             << "1. Librarian\n"
             << "2. Student\n"
             << "0. Exit\n";
        choice = readInt("Choose: ", 0, 2);
        if (choice == 1) librarianMenu();
        else if (choice == 2) studentMenu();
    } while (choice != 0);
}

int main() {
    loadAllData();      // Phanea
    mainMenu();         // Phanea
    saveAllData();      // Phanea
    freeAllMemory();    // Phanea
    cout << "Goodbye!\n";
    return 0;
}

// ---------- PHANEA END ----------
