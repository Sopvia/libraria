#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QCryptographicHash>
#include "register.h"

class LoginDialog : public QDialog {
    Q_OBJECT

    public:
    LoginDialog(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("Libraria Login");

        usernameEdit = new QLineEdit(this);
        usernameEdit->setPlaceholderText("Username");

        passwordEdit = new QLineEdit(this);
        passwordEdit->setPlaceholderText("Password");
        passwordEdit->setEchoMode(QLineEdit::Password);

        loginButton = new QPushButton("Login", this);
        registerButton = new QPushButton("Register", this);

        QHBoxLayout *buttonLayout = new QHBoxLayout();
        buttonLayout->addWidget(loginButton);
        buttonLayout->addWidget(registerButton);

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->addWidget(usernameEdit);
        layout->addWidget(passwordEdit);
        layout->addLayout(buttonLayout);

        connect(loginButton, &QPushButton::clicked, this, &LoginDialog::attemptLogin);
        connect(registerButton, &QPushButton::clicked, this, &LoginDialog::openRegisterDialog);
        
        initDatabase();
    }

    private slots:
    void attemptLogin() {
        QString user = usernameEdit->text();

        QByteArray passwordData = passwordEdit->text().toUtf8();
        QByteArray hashedPass = QCryptographicHash::hash(passwordData, QCryptographicHash::Sha256).toHex();

        QSqlQuery query;
        query.prepare("SELECT * FROM users WHERE username = :user AND password = :pass");
        query.bindValue(":user", user);
        query.bindValue(":pass", QString(hashedPass));

        if (query.exec() && query.next()) {
            accept();
        } else {
            QMessageBox::warning(this, "Error", "Invalid username or Password.");
        }
    }

    void openRegisterDialog() {
        RegisterDialog regDialog(this);
        regDialog.exec();
    }

    private:
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginButton;
    QPushButton *registerButton;

    void initDatabase() {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("library_system.db");

        if (!db.open()) {
            QMessageBox::critical(this, "Database Error", db.lastError().text());
            return;
        }

        QSqlQuery query;
        query.exec("CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY , username TEXT, password TEXT)");
        query.exec("INSERT OR IGNORE INTO users (username, password) VALUES ('admin, '24039260de887e14bc547e17cd5d3aa2a17f6942c7526978df02506bc666a4bc')");
    }
};

#endif