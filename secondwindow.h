#ifndef SECONDWINDOW_H
#define SECONDWINDOW_H

#include <QWidget>
#include <QTableWidget>
#include <QTextEdit>

struct StepData {
    int step;          // Номер шага
    double t;          // Время
    double vi;          // Численное решение v
    double v2i;     // Решение с половинным шагом v
    double diff;  //Vi-V2i
    double error;      // Локальная ошибка (ОЛП)
    double hi;          // Шаг
    int C1; // количество делений шага
    int C2; // количество удвоений шага
};

namespace Ui {
class secondwindow;
}

class secondwindow : public QWidget
{
    Q_OBJECT

public:
    explicit secondwindow(QWidget *parent = nullptr);
    ~secondwindow();
    void setResultsText(const QString& text);
    void fillTable(const std::vector<StepData>& steps);

private:
    Ui::secondwindow *ui;
    QTableWidget *resultsTableMainTask;
    QTextEdit *resultsTextEdit;

};

#endif // SECONDWINDOW_H
