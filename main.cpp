#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QTableWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <QDateEdit>
#include <QInputDialog>
#include <QTabWidget>

using namespace std;

#include "library.h"

class libraryGUI : public QWidget {
public:
    library& myLibrary;

    libraryGUI(library& lib) : myLibrary(lib) {
        setWindowTitle("Libraria");
        resize(900, 600);

        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        QTabWidget* tabWidget = new QTabWidget(this);

        QWidget* booksTab = createBooksTab();
        QWidget* salesTab = createSalesTab();

        tabWidget->addTab(booksTab, "Inventory");
        tabWidget->addTab(salesTab, "Sales");
        
        mainLayout->addWidget(tabWidget);

        updateBooksTable();
        updateSalesTable();
    }

private:
    QTableWidget* booksTableWidget;
    QTableWidget* salesTableWidget;
    QLineEdit *txtTitle, *txtAuthor, *txtIsbn, *txtCopies, *txtPrice;
    QDateEdit* dateEdit;

    void updateBooksTable() {
        booksTableWidget->setRowCount(0);
        for (const auto& b : myLibrary.getBooks()) {
            int row = booksTableWidget->rowCount();
            booksTableWidget->insertRow(row);
            booksTableWidget->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(b.getTitle())));
            booksTableWidget->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(b.getAuthor())));
            booksTableWidget->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(b.getIsbn())));
            booksTableWidget->setItem(row, 3, new QTableWidgetItem(QString::number(b.getCopies())));
            booksTableWidget->setItem(row, 4, new QTableWidgetItem(QString::number(b.getPrice(), 'f', 2) + " €"));
            
            QString dateStr = QString("%1-%2-%3")
                              .arg(b.getPublishDate().year)
                              .arg(b.getPublishDate().month, 2, 10, QChar('0'))
                              .arg(b.getPublishDate().day, 2, 10, QChar('0'));
            booksTableWidget->setItem(row, 5, new QTableWidgetItem(dateStr));
        }
    }

    void updateSalesTable() {
        salesTableWidget->setRowCount(0);
        for (const auto& s : myLibrary.getSalesList()) {
            int row = salesTableWidget->rowCount();
            salesTableWidget->insertRow(row);

            QString dateStr = QString("%1-%2-%3")
                              .arg(s.getDay().year)
                              .arg(s.getDay().month, 2, 10, QChar('0'))
                              .arg(s.getDay().day, 2, 10, QChar('0'));

            salesTableWidget->setItem(row, 0, new QTableWidgetItem(dateStr));
            salesTableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(s.getSales())));
            salesTableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(s.getProfit(), 'f', 2) + " €"));
        }
    }

    void onAddClicked() {
        string title = txtTitle->text().toStdString();
        string author = txtAuthor->text().toStdString();
        string isbn = txtIsbn->text().toStdString();
        int copies = txtCopies->text().toInt();
        double price = txtPrice->text().toDouble();
        QDate qd = dateEdit->date();
        date d{qd.year(), qd.month(), qd.day()};

        if(isbn.empty() || title.empty()) {
            QMessageBox::warning(this, "Error", "Title and ISBN cannot be empty!");
            return;
        }

        book newBook(title, author, isbn, copies, d, price);

        if (!myLibrary.addBook(newBook, false)) {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "ISBN already exists", "Would you like to update the Details for this ISBN?", QMessageBox::Yes|QMessageBox::No);

            if (reply == QMessageBox::Yes) {
                myLibrary.addBook(newBook, true);
                QMessageBox::information(this, "Success", "Book Details updated.");
            }
        } else {
            QMessageBox::information(this, "Success", "Book successfully added.");
        }
        updateBooksTable();
    }

    void onDeleteClicked() {
        bool ok;
        QString text = QInputDialog::getText(this, "Delete Book", "Enter ISBN:", QLineEdit::Normal, "", &ok);
        if (ok && !text.isEmpty()) {
            if (myLibrary.removeBook(text.toStdString())) {
                QMessageBox::information(this, "Success", "Book was successfully deleted.");
                updateBooksTable();
            } else {
                QMessageBox::warning(this, "Error", "ISBN not found.");
            }
        }
    }

    void onSellClicked() {
        bool okIsbn, okAmount;
        QString isbn = QInputDialog::getText(this, "Sell Book", "Enter ISBN:", QLineEdit::Normal, "", &okIsbn);
        if (!okIsbn || isbn.isEmpty()) return;

        int amount = QInputDialog::getInt(this, "Sell Book", "Number of Copies:", 1, 1, 1000, 1, &okAmount);
        if (!okAmount) return;

        QDate today = QDate::currentDate();
        date sellDate{today.year(), today.month(), today.day()};

        int result = myLibrary.sellBookByIsbn(isbn.toStdString(), amount, sellDate);

        if (result == 0) {
            QMessageBox::information(this, "Success", "Sale successfully registered.");
            updateBooksTable();
            updateSalesTable();
        } else if (result == -1) {
            QMessageBox::warning(this, "Error", "ISBN does not exist.");
        } else if (result == -2) {
            QMessageBox::warning(this, "Error", "Not enough copies in stock.");
        }
    }

    void onSortTitleClicked() {
        myLibrary.sortBooks('t');
        updateBooksTable();
    }

    void onSortDateClicked() {
        myLibrary.sortBooks('d');
        updateBooksTable();
    }

    QWidget* createBooksTab() {
        QWidget* tab = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(tab);
        QHBoxLayout* inputLayout = new QHBoxLayout(); 
        QHBoxLayout* buttonLayout = new QHBoxLayout();

        txtTitle = new QLineEdit(this); txtTitle->setPlaceholderText("Title");
        txtAuthor = new QLineEdit(this); txtAuthor->setPlaceholderText("Author");
        txtIsbn = new QLineEdit(this); txtIsbn->setPlaceholderText("ISBN");
        txtCopies = new QLineEdit(this); txtCopies->setPlaceholderText("Copies");
        txtPrice = new QLineEdit(this); txtPrice->setPlaceholderText("Price");
        dateEdit = new QDateEdit(QDate::currentDate(), this);
        dateEdit->setCalendarPopup(true);

        inputLayout->addWidget(txtTitle);
        inputLayout->addWidget(txtAuthor);
        inputLayout->addWidget(txtIsbn);
        inputLayout->addWidget(txtCopies);
        inputLayout->addWidget(txtPrice);
        inputLayout->addWidget(dateEdit);

        QPushButton* btnAdd = new QPushButton("Add Book", this);
        QPushButton* btnDelete = new QPushButton("Delete Book", this);
        QPushButton* btnSell = new QPushButton("Sell Book", this);
        QPushButton* btnSortTitle = new QPushButton("Sort by: Title", this);
        QPushButton* btnSortDate = new QPushButton("Sort by: Date", this);

        buttonLayout->addWidget(btnAdd);
        buttonLayout->addWidget(btnDelete);
        buttonLayout->addWidget(btnSell);
        buttonLayout->addWidget(btnSortTitle);
        buttonLayout->addWidget(btnSortDate);

        booksTableWidget = new QTableWidget(this);
        booksTableWidget->setColumnCount(6);
        booksTableWidget->setHorizontalHeaderLabels({"Title", "Author", "ISBN", "Copies", "Price", "Publish Date"});
        booksTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        
        layout->addLayout(inputLayout);
        layout->addLayout(buttonLayout);
        layout->addWidget(booksTableWidget);

        connect(btnAdd, &QPushButton::clicked, this, &libraryGUI::onAddClicked);
        connect(btnDelete, &QPushButton::clicked, this, &libraryGUI::onDeleteClicked);
        connect(btnSell, &QPushButton::clicked, this, &libraryGUI::onSellClicked);
        connect(btnSortTitle, &QPushButton::clicked, this, &libraryGUI::onSortTitleClicked);
        connect(btnSortDate, &QPushButton::clicked, this, &libraryGUI::onSortDateClicked);
        
        return tab;
    }

    QWidget* createSalesTab() {
        QWidget* tab = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(tab);

        salesTableWidget = new QTableWidget(this);
        salesTableWidget->setColumnCount(3);
        salesTableWidget->setHorizontalHeaderLabels({"Date", "Sold Copies", "Profit"});
        salesTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        layout->addWidget(salesTableWidget);
        return tab;
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    library lib(filename);
    lib.loadFromFile();
    libraryGUI gui(lib);
    gui.show();

    return app.exec();
}