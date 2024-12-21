#include "secondwindow.h"
#include "ui_secondwindow.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QHeaderView>

secondwindow::secondwindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::secondwindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Необходимая информация");  // Добавьте заголовок окна
    setWindowIcon(QIcon(":/icons/icons/icon.png"));

    resultsTableMainTask = new QTableWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(resultsTableMainTask);
    this->setLayout(layout);
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

void secondwindow::fillTable(const std::vector<StepData>& steps) {
    if (!resultsTableMainTask) { // Проверяем, что таблица инициализирована
        qWarning() << "Table widget is not initialized!";
        return;
    }

    // Устанавливаем количество строк и столбцов
    resultsTableMainTask->setRowCount(steps.size());
    resultsTableMainTask->setColumnCount(12); // Номер шага, t, Vi, V2i, Vi - V2i, ОЛП, h, C1, C2

    // Устанавливаем заголовки столбцов
    QStringList headers = {"i", "t", "V11i", "V1_2i", "|V11i-V1_2i|", "V21i", "V2_2i", "|V21i-V2_2i|", "||S||", "h", "C1", "C2"};
    resultsTableMainTask->setHorizontalHeaderLabels(headers);
    resultsTableMainTask->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Заполняем таблицу данными
    for (size_t i = 0; i < steps.size(); ++i) {
        resultsTableMainTask->setItem(i, 0, new QTableWidgetItem(QString::number(steps[i].step)));       // Номер шага
        resultsTableMainTask->setItem(i, 1, new QTableWidgetItem(QString::number(steps[i].t, 'f', 4))); // Время t
        resultsTableMainTask->setItem(i, 2, new QTableWidgetItem(QString::number(steps[i].v11i, 'e', 6))); // Vi
        resultsTableMainTask->setItem(i, 3, new QTableWidgetItem(QString::number(steps[i].v1_2i, 'e', 6))); // V2i
        resultsTableMainTask->setItem(i, 4, new QTableWidgetItem(QString::number(steps[i].diff1, 'e', 6))); // Разность Vi - V2i
        resultsTableMainTask->setItem(i, 5, new QTableWidgetItem(QString::number(steps[i].v21i, 'e', 3)));
        resultsTableMainTask->setItem(i, 6, new QTableWidgetItem(QString::number(steps[i].v2_2i, 'e', 6))); // Шаг h
        resultsTableMainTask->setItem(i, 7, new QTableWidgetItem(QString::number(steps[i].diff2, 'e', 6)));        // C1 (счётчик делений шага)
        resultsTableMainTask->setItem(i, 8, new QTableWidgetItem(QString::number(steps[i].error, 'e', 6)));        // C2 (счётчик удвоений шага)
        resultsTableMainTask->setItem(i, 9, new QTableWidgetItem(QString::number(steps[i].hi)));
        resultsTableMainTask->setItem(i, 10, new QTableWidgetItem(QString::number(steps[i].C1)));
        resultsTableMainTask->setItem(i, 11, new QTableWidgetItem(QString::number(steps[i].C2)));
    }

    // Настройка отображения таблицы
    resultsTableMainTask->resizeColumnsToContents();
    resultsTableMainTask->resizeRowsToContents();
    resultsTableMainTask->update();
}


secondwindow::~secondwindow()
{
    delete ui;
}
