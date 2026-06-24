#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
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
    double price;

    public:
    book(string t, string a, string i, int c, date p, double pr)
    : title(t), author(a), isbn(i), copies(c), publish_date(p), price(pr) {}

    string getTitle() const {return title;}
    string getAuthor() const {return author;}
    string getIsbn() const {return isbn;}
    int getCopies() const {return copies;}
    date getPublishDate() const {return publish_date;}
    double getPrice() const {return price;}

    void setTitle(std::string t) {title = t;}
    void setAuthor(std::string a) {author = a;}
    void setCopies(int c) {copies = c;}
    void setPublishDate(date p) {publish_date = p;}
    void setPrice(double pr) {price = pr;}

    void display() const {
        cout << left << setw(40) << title
                << setw(15) << author
                << setw(15) << isbn
                << setw(10) << copies
                << setw(10) << price;
        
        cout << publish_date.year << "-"
            << setfill('0') << setw(2) << publish_date.month << "-"
            << setfill('0') << setw(2) << publish_date.day;

        cout << setfill(' ') << "\n";
    }
};

class sales {
    private:
    int num_sales;
    date day;
    double profit;

    public:
    sales(int s, date d, double p) : num_sales(s), day(d), profit(p) {};
    sales() = default;

    int getSales() const {return num_sales;}
    date getDay() const {return day;}
    double getProfit() const {return profit;}

    void setSales(int s) {num_sales = s;}
    void setDay(date d) {day = d;}
    void setProfit(double p) {profit = p;}

    void display() const {
        cout << day.year << "-"
            << setw(2) << day.month << "-"
            << setw(2) << day.day;
        cout << setw(20) << ""; 

        cout << left << setw(20) << num_sales
                    << setw(20) << profit << "\n";
    };
};

class library {
    private:
    vector<book> books;
    vector<sales> sales_list;

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
                << setw(15) << "Author"
                << setw(15) << "ISBN"
                << setw(10) << "Copies"
                << setw(10) << "Price"
                << setw(10) << "Publish Date" << "\n";
        cout << string(100, '-') << "\n";

        for (const auto& b : books) {
            b.display();
        }
    }

    void sellBookByIsbn(const string& isbn, const int& copies, const date& sell_date) {
        auto it = std::find_if(books.begin(), books.end(), [&](book& b) {
            return b.getIsbn() == isbn;
        });

        if (it != books.end()) {
            book& chosen_book = *it;
            int current_copies = chosen_book.getCopies();
            int copies_now = current_copies-copies;
            chosen_book.setCopies(copies_now);
            double calc_profit = chosen_book.getPrice()*copies;

            auto i_t = std::find_if(sales_list.begin(), sales_list.end(), [&](const sales& s) {
                return s.getDay().day == sell_date.day && 
                       s.getDay().month == sell_date.month && 
                       s.getDay().year == sell_date.year;
            });

            if (i_t != sales_list.end()) {
                sales& day_sold = *i_t;
                day_sold.setSales(day_sold.getSales()+copies);
                day_sold.setProfit(day_sold.getProfit()+calc_profit);
            } else {
                sales newSale(copies, sell_date, calc_profit);
                sales_list.push_back(newSale);
            }
            cout << "Sale successfully documented.\n";
        } else {
            cout << "No Book found with said ISBN.\n";
        }
    };

    void displaySales() {
        if (sales_list.empty()) {
            cout << "No Sales yet.\n";
            return;
        }

        sort(sales_list.begin(), sales_list.end(), [](const sales& a, const sales& b) {
            date da = a.getDay();
            date db = b.getDay();
            if (da.year != db.year) return da.year < db.year;
            if (da.month != db.month) return da.month < db.month;
            return da.day < db.day;
        });

        cout << left << setw(30) << "Day"
                << setw(20) << "Books sold"
                << setw(20) << "Profit" << "\n";
        cout << string(70, '-') << "\n";

        for (const auto& s : sales_list) {
            s.display();
        }
    };
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
        cout << "6. Sell Book\n";
        cout << "7. Display Sales\n";
        cout << "8. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore();

        string title, author, isbn;
        int copies;
        char sort_by;
        date p_date;
        date today;
        double price;

        switch (choice) {
            case 1: {
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
                cout << "Enter Price: ";
                cin >> price;
                cin.ignore();

                lib.addBook(book(title, author, isbn, copies, p_date, price));
                
                cout << "\nPress Enter to return to main menu...";
                cin.ignore();
                cin.get();
                break;
            }
            case 2: {
                cout << "Enter ISBN to remove Book: ";
                getline(cin, isbn);
                lib.removeBook(isbn);

                cout << "\nPress Enter to return to main menu...";
                cin.ignore();
                cin.get();
                break;
            }
            case 3: {
                cout << "Enter Title to search: ";
                getline(cin, title);
                lib.searchByTitle(title);

                cout << "\nPress Enter to return to main menu...";
                cin.ignore();
                cin.get();
                break;
            }
            case 4: {
                cout << "Enter ISBN to search: ";
                getline(cin, isbn);
                lib.searchByIsbn(isbn);

                cout << "\nPress Enter to return to main menu...";
                cin.ignore();
                cin.get();
                break;
            }
            case 5: {
                cout << "Sort Books by Title(t) or Date(d)?";
                cin >> sort_by;
                cin.ignore();
                if (sort_by != 't' && sort_by != 'd') {
                    cout << "Invalid Input. Try again.\n";
                } else {
                    lib.displayBooks(sort_by);
                }

                cout << "\nPress Enter to return to main menu...";
                cin.ignore();
                cin.get();
                break;
            }
            case 6: {
                cout << "Enter ISBN of sold Book: ";
                getline(cin, isbn);
                cout << "Enter Copies sold: ";
                cin >> copies;
                cin.ignore();
                std::time_t timestamp = std::time(nullptr);
                std::tm* lokal = std::localtime(&timestamp);
                int day = lokal->tm_mday;
                int month = lokal->tm_mon + 1;
                int year = lokal->tm_year + 1900;
                today.day = day;
                today.month = month;
                today.year = year;

                lib.sellBookByIsbn(isbn, copies, today);

                cout << "\nPress Enter to return to main menu...";
                cin.ignore();
                cin.get();
                break;
            }
            case 7: {
                lib.displaySales();

                cout << "\nPress Enter to return to main menu...";
                cin.ignore();
                cin.get();
                break;
            }
            case 8: {
                cout << "Exiting the program.\n";
                break;
            }
            default: {
                cout << "Invalid choice. Try again.\n";

                cout << "\nPress Enter to return to main menu...";
                cin.ignore();
                cin.get();
            }
        }
    } while (choice != 8);

    return 0;
}