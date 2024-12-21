#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QCheckBox>
#include <QCustomPlot.h>
#include "secondwindow.h"

struct InputData {
    double length;
    double u0;
    double v0;
    double h0; // h0 - начальный шаг
    double epsilon;
    double time;
    double boundary;
    int maxSteps;
};


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    secondwindow *SecondWindowptr;
    // Поля ввода
    QLineEdit *lengthInput;
    QLineEdit *u0Input;
    QLineEdit *v0Input;
    QLineEdit *stepInput;
    QLineEdit *epsilonInput;
    QLineEdit *bourdaryinput;
    QLineEdit *timeInput;        // Поле для ввода времени
    QLineEdit *max_step;

    // Кнопка "Рассчитать"
    QPushButton *calculateButton;

    // Графики
    QCustomPlot *trajectoryPlot;
    QCustomPlot *speedtrajectoryPlot;
    QCustomPlot *phasePortraitPlot;
    void plotEllipse(QCustomPlot* plot, const QVector<double>& xData, const QVector<double>& yData, const QString& xAxisLabel, const QString& yAxisLabel);

    // Функции для отображения и расчёта
    void applyStyles();
    void toggleTimeOrCycles();

    void calculatePendulum();
    bool validateInput(InputData& data);
    void plotGraphs(const QString &outputFile);
    void handleSecondWindowDestroyed(std::vector<StepData> steps, std::unique_ptr<secondwindow> windowPtr);
    void plotGraph(QCustomPlot* plot, const QVector<double>& xData, const QVector<double>& yData, const QString& xAxisLabel, const QString& yAxisLabel);
    void plotGraphs(const std::vector<StepData>& steps);

};

#endif // MAINWINDOW_H
