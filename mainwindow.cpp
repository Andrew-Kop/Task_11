#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QDebug>
#include <QTextEdit>
#include <QFile>
#include <QTextStream>
#include "qcustomplot.h"
#include "Logic.h"
#include <QDebug>
#include <QMessageBox>
#include "secondwindow.h"

// Константы
double g = 9.8;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), SecondWindowptr(nullptr)
{
    ui->setupUi(this);

    // Установка заголовка окна
    this->setWindowTitle("Круговой математический маятник Копытцын Андрей ПМоп3");
    setWindowIcon(QIcon(":/icons/icons/icon.png"));

    // Поля ввода
    lengthInput = new QLineEdit(this);
    lengthInput->setPlaceholderText("Длина маятника (L)");

    u0Input = new QLineEdit(this);
    u0Input->setPlaceholderText("Начальный угол (u₀, рад)");

    v0Input = new QLineEdit(this);
    v0Input->setPlaceholderText("Начальная скорость (v₀)");

    stepInput = new QLineEdit(this);
    stepInput->setPlaceholderText("Шаг dt");

    bourdaryinput = new QLineEdit(this);
    bourdaryinput->setPlaceholderText("Точность выхода на границу");

    max_step = new QLineEdit(this);
    max_step->setPlaceholderText("Максимальное количество шагов");

    epsilonInput = new QLineEdit(this);
    epsilonInput->setPlaceholderText("Эпсилон (точность)");

    // Чекбокс
   // stepControlCheckbox = new QCheckBox("Использовать контроль шага", this);

    // Кнопка "Рассчитать"
    calculateButton = new QPushButton("Рассчитать", this);

    // Графики
    trajectoryPlot = new QCustomPlot(this);
    trajectoryPlot->setMinimumHeight(200);
    trajectoryPlot->xAxis->setLabel("Время (с)");
    trajectoryPlot->yAxis->setLabel("Угол u(t), рад");

    speedtrajectoryPlot = new QCustomPlot(this);
    speedtrajectoryPlot->setMinimumHeight(200);
    speedtrajectoryPlot->xAxis->setLabel("Время (с)");
    speedtrajectoryPlot->yAxis->setLabel("Скорость u'(t), рад");

    phasePortraitPlot = new QCustomPlot(this);
    phasePortraitPlot->setMinimumHeight(200);
    phasePortraitPlot->xAxis->setLabel("Угол u(t), рад");
    phasePortraitPlot->yAxis->setLabel("Скорость u'(t), рад/с");

    // Инициализация полей ввода для времени и циклов
    timeInput = new QLineEdit(this);
    timeInput->setPlaceholderText("Время расчёта (t_end)");


    // Layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QGridLayout *inputLayout = new QGridLayout();

    inputLayout->addWidget(lengthInput, 0, 0);
    inputLayout->addWidget(bourdaryinput, 0, 1);
    inputLayout->addWidget(max_step, 0, 2);
    inputLayout->addWidget(u0Input, 1, 0);
    inputLayout->addWidget(v0Input, 1, 1);
    inputLayout->addWidget(timeInput, 1, 2);
    inputLayout->addWidget(stepInput, 2, 0);
    inputLayout->addWidget(epsilonInput, 2, 1);
    inputLayout->addWidget(calculateButton, 3, 0, 1, 3);

    mainLayout->addLayout(inputLayout);
    mainLayout->addWidget(trajectoryPlot);
    mainLayout->addWidget(speedtrajectoryPlot);
    mainLayout->addWidget(phasePortraitPlot);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    // Подключение сигналов
    connect(calculateButton, &QPushButton::clicked, this, &MainWindow::calculatePendulum);

    // Применение стилей
    applyStyles();


}



void MainWindow::applyStyles()
{
    QString buttonStyle =
        "QPushButton {"
        "    background-color: #4CAF50;"  // Зеленый фон
        "    color: white;"
        "    border-radius: 12px;"
        "    padding: 10px 20px;"
        "    border: none;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "} "
        "QPushButton:hover {"
        "    background-color: #45a049;"
        "} "
        "QPushButton:pressed {"
        "    background-color: #388E3C;"
        "    padding: 9px 19px;"
        "} "
        "QPushButton:disabled {"
        "    background-color: #A5D6A7;"
        "    color: #B0BEC5;"
        "    border: none;"
        "}";

    calculateButton->setStyleSheet(buttonStyle);

}

MainWindow::~MainWindow()
{
    // Очистка динамически выделенных ресурсов
    delete ui;  // Удаляем интерфейс, если используется Qt Designer
    if (SecondWindowptr != nullptr) {
        delete SecondWindowptr; // Освобождаем память при закрытии главного окна
    }
    // Очистка остальных ресурсов, если это необходимо
    delete lengthInput;
    delete u0Input;
    delete v0Input;
    delete timeInput;
    delete stepInput;
    delete epsilonInput;
    delete calculateButton;
    delete trajectoryPlot;
    delete phasePortraitPlot;
    delete max_step;
    delete bourdaryinput;
}

void MainWindow::plotGraphs(const std::vector<StepData>& steps)
{
    QVector<double> timeData, angleData, velocityData;

    for (const auto& stepData : steps) {
        timeData.append(stepData.t);
        angleData.append(stepData.v11i);
        velocityData.append(stepData.v21i);
    }

    // Построение графика угла как функции времени
    plotGraph(trajectoryPlot, timeData, angleData, "Время (с)", "Угол u(t), рад");

    // Построение графика скорости как функции времени
    plotGraph(speedtrajectoryPlot, timeData, velocityData, "Время (с)", "Скорость v(t), рад/с");

    // Построение фазового портрета (скорость как функция угла)
    plotGraph(phasePortraitPlot, angleData, velocityData, "Угол u(t), рад", "Скорость v(t), рад/с");

}

void MainWindow::plotGraph(QCustomPlot* plot, const QVector<double>& xData, const QVector<double>& yData, const QString& xAxisLabel, const QString& yAxisLabel)
{
    if (xData.isEmpty() || yData.isEmpty()) {
        qDebug() << "Error: Empty data for plotting.";
        return;
    }

    plot->yAxis->setNumberFormat("e");     // Фиксированная точка
    plot->yAxis->setNumberPrecision(1);

    plot->clearGraphs();
    plot->addGraph();
    plot->graph(0)->setData(xData, yData);

    double xMin = *std::min_element(xData.begin(), xData.end());
    double xMax = *std::max_element(xData.begin(), xData.end());
    double yMin = *std::min_element(yData.begin(), yData.end());
    double yMax = *std::max_element(yData.begin(), yData.end());

    double xPadding = (xMax - xMin) * 0.05;
    double yPadding = (yMax - yMin) * 0.05;

    plot->xAxis->setRange(xMin - xPadding, xMax + xPadding);
    plot->yAxis->setRange(yMin - yPadding, yMax + yPadding);

    plot->xAxis->setLabel(xAxisLabel);
    plot->yAxis->setLabel(yAxisLabel);

    plot->xAxis->setTickLabelPadding(0);
    plot->yAxis->setTickLabelPadding(0);
    plot->xAxis->setLabelPadding(0);
    plot->yAxis->setLabelPadding(0);

    plot->replot();
}

bool MainWindow::validateInput(InputData& data) {
    bool ok;

    // Проверка длины (L)
    QString length_text = lengthInput->text();
    if (length_text.isEmpty()) {
        data.length = 0.1;
    } else {
        data.length = lengthInput->text().toDouble(&ok);
        if (!ok || data.length <= 0.0) {
            QMessageBox::warning(this, "Некорректный ввод", "Введите положительное число для длины.");
            return false;
        }
    }

    // Проверка начального угла (u0)
    QString u0_text = u0Input->text();
    if (u0_text.isEmpty()) {
        data.u0 = 0.314;
    } else {
        data.u0 = u0Input->text().toDouble(&ok);
        if (!ok) {
            QMessageBox::warning(this, "Некорректный ввод", "Введите допустимое число для начального угла.");
            return false;
        }
    }

    // Проверка начальной угловой скорости (v0)
    QString v0_text = v0Input->text();
    if (v0_text.isEmpty()) {
        data.v0 = 0.0;
    } else {
        data.v0 = v0Input->text().toDouble(&ok);
        if (!ok) {
            QMessageBox::warning(this, "Некорректный ввод", "Введите допустимое число для начальной угловой скорости.");
            return false;
        }
    }

    // Проверка шага (h0)
    QString step_text = stepInput->text();
    if (step_text.isEmpty()) {
        data.h0 = 0.001;
    } else {
        data.h0 = stepInput->text().toDouble(&ok);
        if (!ok || data.h0 <= 0.0) {
            QMessageBox::warning(this, "Некорректный ввод", "Введите положительное число для начального шага.");
            return false;
        }
    }

    // Проверка epsilon
    QString epsilon_text = epsilonInput->text();
    if (epsilon_text.isEmpty()) {
        data.epsilon = 1e-5;
    } else {
        data.epsilon = epsilonInput->text().toDouble(&ok);
        if (!ok || data.epsilon <= 0.0) {
            QMessageBox::warning(this, "Некорректный ввод", "Введите положительное число для Epsilon.");
            return false;
        }
    }

    // Проверка времени расчета (t_end)
    QString time_text = timeInput->text();
    if (time_text.isEmpty()) {
        data.time = 10.0;
    } else {
        data.time = timeInput->text().toDouble(&ok);
        if (!ok || data.time <= 0.0) {
            QMessageBox::warning(this, "Некорректный ввод", "Введите положительное число для времени расчета.");
            return false;
        }
    }

    // Проверка границы (boundary_epsilon)
    QString bourdary_text = bourdaryinput->text();
    if (bourdary_text.isEmpty()) {
        data.boundary = 1e-8;
    } else {
        data.boundary = bourdaryinput->text().toDouble(&ok);
        if (!ok || data.boundary <= 0.0) {
            QMessageBox::warning(this, "Некорректный ввод", "Введите положительное число для границы.");
            return false;
        }
    }

    // Проверка максимального шага (max_steps)
    QString max_step_text = max_step->text();
    if (max_step_text.isEmpty()) {
        data.maxSteps = 3000;
    } else {
        data.maxSteps = max_step->text().toInt(&ok);
        if (!ok || data.maxSteps < 0 || data.maxSteps == std::numeric_limits<int>::max()) {
            QMessageBox::warning(this, "Некорректный ввод", "Введите неотрицательное целое число для максимального количества шагов.");
            return false;
        }
    }

    return true;
}

void MainWindow::calculatePendulum()
{
    InputData inputData;
    if (!validateInput(inputData)) {
        return; // Если ввод некорректен, выходим из функции
    }
    double L = inputData.length;
    double u0 = inputData.u0;
    double v0 = inputData.v0;
    double dt = inputData.h0;
    double epsilon = inputData.epsilon;
    double t_end = inputData.time;
    double bourdary = inputData.boundary;
    int  step_max = inputData.maxSteps;

    // Подготовка начального состояния
    State y0 = {u0, v0}; // Начальные значения u и v
    double t0 = 0.0;     // Начальное время
    std::pair<std::vector<StepData>, RK4Params> result = adaptiveRK4(y0, t0, t_end, dt, epsilon, g, L, step_max, bourdary);
    plotGraphs(result.first);
    if (SecondWindowptr == nullptr) {
        SecondWindowptr = new secondwindow();

        // Создаём соединение для освобождения указателя на второе окно
        connect(SecondWindowptr, &secondwindow::destroyed, this, [this]() {
            SecondWindowptr = nullptr;
        });
    }

    // Передаём данные для заполнения таблицы
    SecondWindowptr->fillTable(result.first);
    SecondWindowptr->show();



    // Построение графиков
    //plotGraphs(outputFile);

    QString resultsText = "Расчёт завершён.\n"
                          "L = " + QString::number(L) + " м\n"
                                                 "g = " + QString::number(g) + " м/с²\n"
                                                 "Начальный угол u₀ = " + QString::number(u0) + " рад\n"
                                                  "Начальная скорость v₀ = " + QString::number(v0) + " м/с\n"
                                                  "Начальный шаг времени (dt) = " + QString::number(dt) + " с\n"
                                                  "Точность (эпсилон) = " + QString::number(epsilon) + "\n"
                                                       "Время расчёта = " + QString::number(t_end) + " с\n"
                                                     "Максимальное количество шагов: " + QString::number(step_max) + "\n" // Добавлено
                                                        "Точность выхода на границу: " + QString::number(bourdary) + "\n" // Добавлено
                                                        "Использован адаптивный контроль шага\n"
                                                        "------------------------------------\n"
                                                        "Параметры адаптивного метода:\n"
                                                        "Количество делений шага (C1): " + QString::number(result.second.C1) + "\n"
                                                                "Количество удвоений шага (C2): " + QString::number(result.second.C2) + "\n"
                                                                "Максимальный шаг: " + QString::number(result.second.max_h) + " с\n"
                                                                   "Минимальный шаг: " + QString::number(result.second.min_h) + " с\n"
                                                                   "Общее количество шагов: " + QString::number(result.second.num_steps) + "\n"
                                                                       "Максимальная олп: " + QString::number(result.second.max_error) + "\n"
                                                                       "Максимальный |V11i-V1_2i|: " + QString::number(result.second.max_diff1) + " (на шаге " + QString::number(result.second.max_diff1_step) + ")\n"
                                                                                                                                      "Минимальный |V11i-V1_2i|: " + QString::number(result.second.min_diff1) + " (на шаге " + QString::number(result.second.min_diff1_step) + ")\n"
                                                                                                                                      "Максимальный |V21i-V2_2i|: " + QString::number(result.second.max_diff2) + " (на шаге " + QString::number(result.second.max_diff2_step) + ")\n"
                                                                                                                                      "Минимальный |V21i-V2_2i|: " + QString::number(result.second.min_diff2) + " (на шаге " + QString::number(result.second.min_diff2_step) + ")\n";



    SecondWindowptr->setResultsText(resultsText); // Вызываем слот для установки текста
    SecondWindowptr->show();

}

