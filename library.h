#ifndef LIBRARY_H
#define LIBRARY_H

#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
using namespace std;

const string filename = "build/library_books.txt";
const string sales_filename = "build/library_sales.txt";

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
    const string dbPath;

    void initDatabase() {
        QSqlDatabase db = QSqlDatabase::database();

        if (!db.isOpen()) {
            db.setDatabaseName("library_system.db");
            if (!db.open()) {
                qDebug() << "Database cannot open:" << db.lastError().text();
            return;
            }
        }

        QSqlQuery query(db);

        query.exec("CREATE TABLE IF NOT EXISTS books ("
                    "isbn TEXT PRIMARY KEY, "
                    "title TEXT, "
                    "author TEXT, "
                    "copies INTEGER, "
                    "price REAL, "
                    "year INTEGER, "
                    "month INTEGER, "
                    "day INTEGER"");");

        query.exec("CREATE TABLE IF NOT EXISTS sales ("
                    "year INTEGER, "
                    "month INTEGER, "
                    "day INTEGER, "
                    "sales_count INTEGER, "
                    "profit REAL, "
                    "PRIMARY KEY (year, month, day));");
    }

    public:
    library(string dbDatei) : dbPath(dbDatei) {
        initDatabase();
        loadFromDb();
        loadSalesFromDb();
    };

    const vector<book>& getBooks() const { return books; }
    const vector<sales>& getSalesList() const { return sales_list; }

    bool addBook(const book& Book, bool forceUpdate = false) {
        auto it = find_if(books.begin(), books.end(), [&](book& b) {
            return b.getIsbn() == Book.getIsbn();
        });

        QSqlQuery query;
        if (it != books.end()) {
            if (forceUpdate) {
                *it = Book;

                query.prepare("UPDATE books SET title = :title, author = :author, copies = :copies, "
                "price = :price, year = :year, month = :month, day = :day WHERE isbn = :isbn");
                query.bindValue(":title", QString::fromStdString(Book.getTitle()));
                query.bindValue(":author", QString::fromStdString(Book.getAuthor()));
                query.bindValue(":copies", Book.getCopies());
                query.bindValue(":price", Book.getPrice());
                query.bindValue(":year", Book.getPublishDate().year);
                query.bindValue(":month", Book.getPublishDate().month);
                query.bindValue(":day", Book.getPublishDate().day);
                query.bindValue(":isbn", QString::fromStdString(Book.getIsbn()));
                query.exec();

                return true;
            }
            return false;
        } else {
            books.push_back(Book);

            query.prepare("INSERT INTO books (isbn, title, author, copies, price, year, month, day) "
                "VALUES (:isbn, :title, :author, :copies, :price, :year, :month, :day)");
            query.bindValue(":isbn", QString::fromStdString(Book.getIsbn()));
            query.bindValue(":title", QString::fromStdString(Book.getTitle()));
            query.bindValue(":author", QString::fromStdString(Book.getAuthor()));
            query.bindValue(":copies", Book.getCopies());
            query.bindValue(":price", Book.getPrice());
            query.bindValue(":year", Book.getPublishDate().year);
            query.bindValue(":month", Book.getPublishDate().month);
            query.bindValue(":day", Book.getPublishDate().day);
            query.exec();

            return true;
        }
    }

    bool removeBook(const string& isbn) {
        auto it = remove_if(books.begin(), books.end(), [&](book& b) {
            return b.getIsbn() == isbn;
        });

        if (it != books.end()) {
            books.erase(it, books.end());

            QSqlQuery query;
            query.prepare("DELETE FROM books WHERE isbn = :isbn");
            query.bindValue(":isbn", QString::fromStdString(isbn));
            query.exec();

            return true;
        } else {
            return false;
        }
    }

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

        QSqlQuery query;
        query.prepare("UPDATE books SET copies = :copies WHERE isbn = :isbn");
        query.bindValue(":copies", it->getCopies());
        query.bindValue(":isbn", QString::fromStdString(isbn));
        query.exec();

        auto s_it = find_if(sales_list.begin(), sales_list.end(), [&](const sales& s) {
            return s.getDay().day == sell_date.day && 
                    s.getDay().month == sell_date.month && 
                    s.getDay().year == sell_date.year;
        });

        if (s_it != sales_list.end()) {
            s_it->setSales(s_it->getSales() + copies);
            s_it->setProfit(s_it->getProfit() + calc_profit);

            query.prepare("UPDATE sales SET sales_count = :sales_count, profit = :profit "
            "WHERE year = :year AND month = :month AND day = :day");
            query.bindValue(":sales_count", s_it->getSales());
            query.bindValue(":profit", s_it->getProfit());
            query.bindValue(":year", sell_date.year);
            query.bindValue(":month", sell_date.month);
            query.bindValue(":day", sell_date.day);
            query.exec();
        } else {
            sales_list.push_back(sales(copies, sell_date, calc_profit));
            
            query.prepare("INSERT INTO sales (year, month, day, sales_count, profit) "
            "VALUES (:year, :month, :day, :sales_count, :profit)");
            query.bindValue(":year", sell_date.year);
            query.bindValue(":month", sell_date.month);
            query.bindValue(":day", sell_date.day);
            query.bindValue(":sales_count", copies);
            query.bindValue(":profit", calc_profit);
            query.exec();
        }
        
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

     void loadFromDb() {
        books.clear();
        QSqlQuery query("SELECT title, author, isbn, copies, price, year, month, day FROM books");

        string line;
        while (query.next()) {
            std::string title = query.value(0).toString().toStdString();
            std::string author = query.value(1).toString().toStdString();
            std::string isbn = query.value(2).toString().toStdString();
            int copies = query.value(3).toInt();
            double price = query.value(4).toDouble();
            date p_date = { query.value(5).toInt(), query.value(6).toInt(), query.value(7).toInt() };

            books.push_back(book(title, author, isbn, copies, p_date, price));
        }
    }

    void loadSalesFromDb() {
        sales_list.clear();
        QSqlQuery query("SELECT sales_count, profit, year, month, day FROM sales");

        while (query.next()) {
            int count = query.value(0).toInt();
            double profit = query.value(1).toDouble();
            date s_date = { query.value(2).toInt(), query.value(3).toInt(), query.value(4).toInt() };

            sales_list.emplace_back(count, s_date, profit);
        }
    }
};
#endif