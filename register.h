#ifndef REGISTER_H
#define REGISTER_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QCryptographicHash>

class RegisterDialog : public QDialog {
    Q_OBJECT

    public:
    RegisterDialog(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("Register");
        setMinimumSize(340, 260);
        setModal(true);

        usernameEdit = new QLineEdit(this);
        usernameEdit->setPlaceholderText("Username");

        passwordEdit = new QLineEdit(this);
        passwordEdit->setPlaceholderText("Password");
        passwordEdit->setEchoMode(QLineEdit::Password);

        passwordConfirmEdit = new QLineEdit(this);
        passwordConfirmEdit->setPlaceholderText("Repeat password");
        passwordConfirmEdit->setEchoMode(QLineEdit::Password);

        registerButton = new QPushButton("Create Account", this);

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(25, 25, 25, 25);
        layout->setSpacing(15);

        layout->addWidget(usernameEdit);
        layout->addWidget(passwordEdit);
        layout->addWidget(passwordConfirmEdit);
        layout->addWidget(registerButton);

        connect(registerButton, &QPushButton::clicked, this, &RegisterDialog::attemptRegister);
    }


    private slots:
    void attemptRegister() {
        QString user = usernameEdit->text().trimmed();
        QString pass = passwordEdit->text();
        QString confirm = passwordConfirmEdit->text();

        if (user.isEmpty() || pass.isEmpty()) {
            QMessageBox::warning(this, "Error", "Inputs cannot be empty.");
            return;
        }

        if (pass != confirm) {
            QMessageBox::warning(this, "Error", "Passwords don't match.");
            return;
        }

        QSqlQuery checkQuery;
        checkQuery.prepare("SELECT username FROM users WHERE username = :user");
        checkQuery.bindValue(":user", user);

        if (checkQuery.exec() && checkQuery.next()) {
            QMessageBox::warning(this, "Error", "Username already exists.");
            return;
        }

        QByteArray passwordData = pass.toUtf8();
        QByteArray hashedPass = QCryptographicHash::hash(passwordData, QCryptographicHash::Sha256).toHex();

        QSqlQuery insertQuery;
        insertQuery.prepare("INSERT INTO users (username, password) VALUES (:user, :pass)");
        insertQuery.bindValue(":user", user);
        insertQuery.bindValue(":pass", QString(hashedPass));

        if (insertQuery.exec()) {
            QMessageBox::information(this, "Success", "Register was successful!");
            accept();
        } else {
            QMessageBox::critical(this, "Error", "Database Error: " + insertQuery.lastError().text());
        }
    }

    private:
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QLineEdit *passwordConfirmEdit;
    QPushButton *registerButton;
};

#endif