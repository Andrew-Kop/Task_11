#include "secondwindow.h"
#include "ui_secondwindow.h"
#include <QVBoxLayout>
#include <QLabel>

secondwindow::secondwindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::secondwindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Второе окно");  // Добавьте заголовок окна
    resultsTableMainTask = new QTableWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(resultsTableMainTask);
    this->setLayout(layout);
    this->resize(400, 300);
    resultsTextEdit = new QTextEdit(this); // Создаем QTextEdit
    resultsTextEdit->setReadOnly(true); // Только для чтения
    if(!layout)
        layout = new QVBoxLayout(this);
    QLabel *helpLabel = new QLabel("Справка:", this);
    helpLabel->setStyleSheet("font-weight: bold; font-size: 16px; color: #4CAF50;");
    layout->addWidget(helpLabel);
    layout->addWidget(resultsTextEdit); // Добавляем QTextEdit в layout
    this->setLayout(layout);
    QString textEditStyle =
        "QTextEdit {"
        "    font-size: 16px;"
        "    color: black;"
        "    background-color: #F1F8E9;"  // Светлый фон для текстового поля
        "    border: 1px solid #A5D6A7;"  // Обводка текста
        "    border-radius: 8px;"  // Закругленные углы
        "    padding: 5px;"
        "}";

    resultsTextEdit->setStyleSheet(textEditStyle);
}

void secondwindow::setResultsText(const QString& text)
{
    resultsTextEdit->setText(text);
}

secondwindow::~secondwindow()
{
    delete ui;
}
