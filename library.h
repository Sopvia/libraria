#ifndef LIBRARY_H
#define LIBRARY_H

#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
using namespace std;

const string filename = "library_books.txt";
const string sales_filename = "library_sales.txt";

struct date {
    int year, month, day;
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

    void setTitle(string t) {title = t;}
    void setAuthor(string a) {author = a;}
    void setCopies(int c) {copies = c;}
    void setPublishDate(date p) {publish_date = p;}
    void setPrice(double pr) {price = pr;}
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
};

class library {
    private:
    vector<book> books;
    vector<sales> sales_list;
    const string fileName;
    const string salesFilename;

    void appendSaleToFile(const sales& s) {
        fstream file(sales_filename, ios::app);
        if (file.is_open()) {
            file << s.getDay().year << ";" << s.getDay().month << ";" << s.getDay().day << ";"
                    << s.getSales() << ";" << s.getProfit() << "\n";
        }
    }

    public:
    library(string datei) : fileName(datei) {
        loadFromFile();
        loadSalesFromFile();
    };

    const vector<book>& getBooks() const { return books; }
    const vector<sales>& getSalesList() const { return sales_list; }

    bool addBook(const book& Book, bool forceUpdate = false) {
        auto it = find_if(books.begin(), books.end(), [&](book& b) {
            return b.getIsbn() == Book.getIsbn();
        });

        if (it != books.end()) {
            if (forceUpdate) {
                it->setTitle(Book.getTitle());
                it->setAuthor(Book.getAuthor());
                it->setCopies(Book.getCopies());
                it->setPrice(Book.getPrice());
                it->setPublishDate(Book.getPublishDate());
                saveAllToFile();
                return true;
            }
            return false;
        } else {
            books.push_back(Book);
            appendBookToFile(Book);
            return true;
        }
    };

    void appendBookToFile(const book& b) {
        ofstream file(filename, ios::app);
        if (!file.is_open()) {return;}

        file << b.getTitle() << ";"
            << b.getAuthor() << ";"
            << b.getIsbn() << ";"
            << b.getCopies() << ";"
            << b.getPrice() << ";"
            << b.getPublishDate().year << ";"
            << b.getPublishDate().month << ";"
            << b.getPublishDate().day << "\n";

        file.close();
    };

    void saveAllToFile() {
        ofstream file(filename);
        if (!file.is_open()) return;
        for (const auto& b : books) {
            file << b.getTitle() << ";" << b.getAuthor() << ";" << b.getIsbn() << ";"
                 << b.getCopies() << ";" << b.getPrice() << ";"
                 << b.getPublishDate().year << ";" << b.getPublishDate().month << ";" << b.getPublishDate().day << "\n";
        }
        file.close();
    }

    bool removeBook(const string& isbn) {
        auto it = remove_if(books.begin(), books.end(), [&](book& b) {
            return b.getIsbn() == isbn;
        });

        if (it != books.end()) {
            books.erase(it, books.end());
            saveAllToFile();

            return true;
        } else {
            return false;
        }
    };

/*     void searchByTitle(const string& title) {
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
    };

    bool searchByIsbn(const string& isbn) {
        auto it = find_if(books.begin(), books.end(), [&](book& b) {
            return b.getIsbn() == isbn;
        });

        if (it != books.end()) {
            it->display();
        } else {
            return false;
        }
    }; */

    int sellBookByIsbn(const string& isbn, const int& copies, const date& sell_date) {
        auto it = find_if(books.begin(), books.end(), [&](book& b) {
            return b.getIsbn() == isbn;
        });

        if (it == books.end()) {return -1;}
        if (it->getCopies() < copies) {return -2;}

        it->setCopies(it->getCopies() - copies);
        double calc_profit = it->getPrice() * copies;

        saveAllToFile();

        auto s_it = find_if(sales_list.begin(), sales_list.end(), [&](const sales& s) {
            return s.getDay().day == sell_date.day && 
                    s.getDay().month == sell_date.month && 
                    s.getDay().year == sell_date.year;
        });

        if (s_it != sales_list.end()) {
            s_it->setSales(s_it->getSales() + copies);
            s_it->setProfit(s_it->getProfit() + calc_profit);
        } else {
            sales_list.push_back(sales(copies, sell_date, calc_profit));
        }
        
        saveSalesToFile();
        return 0;
    };


    void sortBooks(char sort_by) {
        if (sort_by == 't') {
            sort(books.begin(), books.end(), [](const book& a, const book& b) {
                return a.getTitle() < b.getTitle();
            });
        } else {
            sort(books.begin(), books.end(), [](const book& a, const book& b) {
                date da = a.getPublishDate();
                date db = b.getPublishDate();
                if (da.year != db.year) return da.year < db.year;
                if (da.month != db.month) return da.month < db.month;
                return da.day < db.day;
            });
        }
    }

     void loadFromFile() {
        ifstream file(filename);
        if (!file.is_open()) {
            ofstream newFile(filename);
            newFile.close();
        }
        books.clear();

        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;

            stringstream ss(line);
            string title, author, isbn, copiesStr, priceStr, yearStr, monthStr, dayStr;

            getline(ss, title, ';');
            getline(ss, author, ';');
            getline(ss, isbn, ';');
            getline(ss, copiesStr, ';');
            getline(ss, priceStr, ';');
            getline(ss, yearStr, ';');
            getline(ss, monthStr, ';');
            getline(ss, dayStr, '\n');

            date p_date = { stoi(yearStr), stoi(monthStr), stoi(dayStr) };
            books.push_back(book(title, author, isbn, stoi(copiesStr), p_date, stod(priceStr)));
        }
        file.close();
    };

    void saveSalesToFile() {
        ofstream file(sales_filename);
        if (!file.is_open()) {return;}
        for (const auto& s : sales_list) {
            file << s.getSales() << ";" << s.getProfit() << ";"
                 << s.getDay().year << ";" << s.getDay().month << ";" << s.getDay().day << "\n";
        }
        file.close();
    }

    void loadSalesFromFile() {
        ifstream file(sales_filename);
        if (!file.is_open()) {return;}
        sales_list.clear();

        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string y, m, d, s, p;
            if (getline(ss, y, ';') && getline(ss, m, ';') && getline(ss, d, ';') &&
                getline(ss, s, ';') && getline(ss, p, ';')) {
                sales_list.emplace_back(stoi(s), date{stoi(y), stoi(m), stoi(d)}, stod(p));
            }
        }
        file.close();
    }

/*     double getTotalProfit() {
        double total = 0/0;
        total = 0.0;
        for (const auto& s : sales_list) {
            total += s.getProfit();
        }
        return total;
    } */
};
#endif