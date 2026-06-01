#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

struct date {
    int year;
    int month;
    int day;
};

class book {
    private:
    string title, author, isbn;
    int copies;
    date publish_date;

    public:
    book(string t, string a, string i, int c, date p)
    : title(t), author(a), isbn(i), copies(c), publish_date(p) {}

    string getTitle() const {return title;}
    string getAuthor() const {return author;}
    string getIsbn() const {return isbn;}
    int getCopies() const {return copies;}
    date getPublishDate() const {return publish_date;}

    void setTitle(std::string t) {title = t;}
    void setAuthor(std::string a) {author = a;}
    void setCopies(int c) {copies = c;}
    void setPublishDate(date p) {publish_date = p;}

    void display() const {
        cout << left << setw(40) << title
                << setw(20) << author
                << setw(15) << isbn
                << setw(10) << copies;
        
        cout << publish_date.year << "-"
            << setfill('0') << setw(2) << publish_date.month << "-"
            << setfill('0') << setw(2) << publish_date.day;

        cout << setfill(' ') << "\n";
    }
};

class library {
    private:
    vector<book> books;

    public:
    void addBook(const book& Book) {
        auto it = std::find_if(books.begin(), books.end(), [&](book& b) {
            return b.getIsbn() == Book.getIsbn();
        });

        if (it != books.end()) {
            char x;
            cout << "Book with same ISBN exists. Updating details? Yes=Y No=N\n";
            cin >> x;
            if (x == 'Y') {
                it->setTitle(Book.getTitle());
                it->setAuthor(Book.getAuthor());
                it->setCopies(Book.getCopies());
                cout << "Details updated successfully.\n";
            } else {
                cout << "Details not updated.\n";
            }
        } else {
            books.push_back(Book);
            cout << "Book added successfully.\n";
        }
    }

    void removeBook(const string& isbn) {
        auto it = std::remove_if(books.begin(), books.end(), [&](book& b) {
            return b.getIsbn() == isbn;
        });

        if (it != books.end()) {
            books.erase(it, books.end());
            cout << "Book removed successfully.\n";
        } else {
            cout << "No Book found with said ISBN.\n";
        }
    }

    void searchByTitle(const string& title) {
        bool found = false;
        for (auto& b : books) {
            if (b.getTitle() == title) {
                b.display();
                found = true;
            }
        }

        if (!found) {
            cout << "No Book found with said Title.\n";
        }
    }

    void searchByIsbn(const string& isbn) {
        auto it = std::find_if(books.begin(), books.end(), [&](book& b) {
            return b.getIsbn() == isbn;
        });

        if (it != books.end()) {
            it->display();
        } else {
            cout << "No Book found with said ISBN.\n";
        }
    }

    void displayBooks(const char& sort_by) {
        if (books.empty()) {
            cout << "Library is empty.\n";
            return;
        }

        if (sort_by == 't') {
            sort(books.begin(), books.end(), [](const book& a, const book& b) {
                return a.getTitle() < b.getTitle();
            });
        } else {
            sort(books.begin(), books.end(), [](const book& a, const book& b) {
                date da = a.getPublishDate();
                date db = b.getPublishDate();

                if (da.year != db.year) return da.year < db.year;
                if (db.month != db.month) return da.month < db.month;
                return da.day < db.day;
            });
        }

        cout << left << setw(40) << "Title"
                << setw(20) << "Author"
                << setw(15) << "ISBN"
                << setw(10) << "Copies"
                << setw(10) << "Publish Date" << "\n";
        cout << string(100, '-') << "\n";

        for (const auto& b : books) {
            b.display();
        }
    }
};

int main() {
    library lib;
    int choice;

    do {
        cout << "\n Mini Library Management System - Libraria \n";
        cout << "1. Add Book\n";
        cout << "2. Remove Book by ISBN\n";
        cout << "3. Search Book by Title\n";
        cout << "4. Search Book by ISBN\n";
        cout << "5. Display all Books\n";
        cout << "6. Exit\n";
        cout << "6. Enter your choice: ";
        cin >> choice;
        cin.ignore();

        string title, author, isbn;
        int copies;
        char sort_by;
        date p_date;

        switch (choice) {
            case 1:
                cout << "Enter Title: ";
                getline(cin, title);
                cout << "Enter Author: ";
                getline(cin, author);
                cout << "Enter ISBN: ";
                getline(cin, isbn);
                cout << "Enter number of copies: ";
                cin >> copies;
                cout << "Enter Publish Year(YYYY): ";
                cin >> p_date.year;
                cout << "Enter Publish Month(MM): ";
                cin >> p_date.month;
                cout << "Enter Publish Day(DD): ";
                cin >> p_date.day;

                lib.addBook(book(title, author, isbn, copies, p_date));
                break;
            case 2:
                cout << "Enter ISBN to remove Book: ";
                getline(cin, isbn);
                lib.removeBook(isbn);
                break;
            case 3:
                cout << "Enter Title to search: ";
                getline(cin, title);
                lib.searchByTitle(title);
                break;
            case 4:
                cout << "Enter ISBN to search: ";
                getline(cin, isbn);
                lib.searchByIsbn(isbn);
                break;
            case 5:
                cout << "Sort Books by Title(t) or Date(d)?";
                cin >> sort_by;
                cin.ignore();
                if (sort_by != 't' && sort_by != 'd') {
                    cout << "Invalid Input. Try again.\n";
                    break;
                } else {
                    lib.displayBooks(sort_by);
                    break;
                }
            case 6:
                cout << "Exiting the program.\n";
                break;
            default:
                cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 6);

    return 0;
}