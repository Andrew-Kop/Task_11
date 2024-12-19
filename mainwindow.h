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
    QCustomPlot *phasePortraitPlot;

    // Функции для отображения и расчёта
    void applyStyles();
    void toggleTimeOrCycles();

    void calculatePendulum();
    bool validateInput();
    void plotGraphs(const QString &outputFile);
    void handleSecondWindowDestroyed(std::vector<StepData> steps, std::unique_ptr<secondwindow> windowPtr);

};

#endif // MAINWINDOW_H
