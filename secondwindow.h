#ifndef SECONDWINDOW_H
#define SECONDWINDOW_H

#include <QWidget>
#include <QTableWidget>
#include <QTextEdit>

struct StepData {
    int step;          // Номер шага
    double t;          // Время
    double v11i;          // Численное решение v по первой компоненте
    double v1_2i;
    double v21i;     // Решение с половинным шагом v
    double v2_2i;
    double diff1;  //V11i-V1_2i
    double diff2; //v21i-v2_2i
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
