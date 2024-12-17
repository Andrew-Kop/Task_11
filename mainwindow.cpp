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

// Константы
const double PI = 3.141592653589793;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Установка заголовка окна
    this->setWindowTitle("Круговой математический маятник Копытцын Андрей ПМоп3");
    setWindowIcon(QIcon(":/icons/icons/icon.png"));

    // Поля ввода
    lengthInput = new QLineEdit(this);
    lengthInput->setPlaceholderText("Длина маятника (L)");

    gravityInput = new QLineEdit(this);
    gravityInput->setPlaceholderText("Ускорение свободного падения (g)");

    massInput = new QLineEdit(this);
    massInput->setPlaceholderText("Масса маятника (m)");

    u0Input = new QLineEdit(this);
    u0Input->setPlaceholderText("Начальный угол (u₀, рад)");

    v0Input = new QLineEdit(this);
    v0Input->setPlaceholderText("Начальная скорость (v₀)");

    stepInput = new QLineEdit(this);
    stepInput->setPlaceholderText("Шаг dt");

    epsilonInput = new QLineEdit(this);
    epsilonInput->setPlaceholderText("Эпсилон (точность)");
    epsilonInput->setVisible(false); // По умолчанию скрыто

    // Чекбокс
    stepControlCheckbox = new QCheckBox("Использовать контроль шага", this);

    // Кнопка "Рассчитать"
    calculateButton = new QPushButton("Рассчитать", this);

    // Поле справки
    helpField = new QTextEdit(this);
    helpField->setReadOnly(true);
    helpField->setPlaceholderText("Справочная информация появится здесь.");

    // Графики
    trajectoryPlot = new QCustomPlot(this);
    trajectoryPlot->setMinimumHeight(250);
    trajectoryPlot->xAxis->setLabel("Время (с)");
    trajectoryPlot->yAxis->setLabel("Угол u(t), рад");

    phasePortraitPlot = new QCustomPlot(this);
    phasePortraitPlot->setMinimumHeight(250);
    phasePortraitPlot->xAxis->setLabel("Угол u(t), рад");
    phasePortraitPlot->yAxis->setLabel("Скорость u'(t), рад/с");

    // Инициализация полей ввода для времени и циклов
    timeInput = new QLineEdit(this);
    timeInput->setPlaceholderText("Время расчёта (t_end)");

    QLabel *helpLabel = new QLabel("Справка:", this);
    helpLabel->setStyleSheet("font-weight: bold; font-size: 16px; color: #4CAF50;");
    // Layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QGridLayout *inputLayout = new QGridLayout();

    inputLayout->addWidget(lengthInput, 0, 0);
    inputLayout->addWidget(gravityInput, 0, 1);
    inputLayout->addWidget(massInput, 0, 2);
    inputLayout->addWidget(u0Input, 1, 0);
    inputLayout->addWidget(v0Input, 1, 1);
    inputLayout->addWidget(timeInput, 1, 2);
    inputLayout->addWidget(stepInput, 2, 0);
    inputLayout->addWidget(epsilonInput, 2, 2);
    inputLayout->addWidget(stepControlCheckbox, 2, 1);
    inputLayout->addWidget(calculateButton, 3, 0, 1, 3);

    mainLayout->addLayout(inputLayout);
    mainLayout->addWidget(trajectoryPlot);
    mainLayout->addWidget(phasePortraitPlot);
    mainLayout->addWidget(helpLabel);
    mainLayout->addWidget(helpField);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);


    // Подключение сигналов
    connect(calculateButton, &QPushButton::clicked, this, &MainWindow::calculatePendulum);
    connect(stepControlCheckbox, &QCheckBox::toggled, this, &MainWindow::toggleEpsilonInput);

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

    QString textEditStyle =
        "QTextEdit {"
        "    font-size: 16px;"
        "    color: black;"
        "    background-color: #F1F8E9;"  // Светлый фон для текстового поля
        "    border: 1px solid #A5D6A7;"  // Обводка текста
        "    border-radius: 8px;"  // Закругленные углы
        "    padding: 5px;"
                            "}";

    helpField->setStyleSheet(textEditStyle);

}

void MainWindow::toggleEpsilonInput(bool checked)
{
    epsilonInput->setVisible(checked);
}



MainWindow::~MainWindow()
{
    // Очистка динамически выделенных ресурсов
    delete ui;  // Удаляем интерфейс, если используется Qt Designer

    // Очистка остальных ресурсов, если это необходимо
    delete lengthInput;
    delete gravityInput;
    delete massInput;
    delete u0Input;
    delete v0Input;
    delete timeInput;
    delete stepInput;
    delete epsilonInput;
    delete stepControlCheckbox;
    delete calculateButton;
    delete helpField;
    delete trajectoryPlot;
    delete phasePortraitPlot;
}

void MainWindow::plotGraphs(const QString &outputFile)
{
    // Здесь нужно реализовать логику построения графиков.
    // Пример:

    QVector<double> timeData, angleData;

    // Чтение данных из файла (вы можете адаптировать в зависимости от вашего формата данных)
    QFile file(outputFile);
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        in.readLine();  // Пропускаем заголовок

        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList fields = line.split(",");

            if (fields.size() >= 2) {
                double t = fields[0].toDouble();
                double u = fields[1].toDouble();
                timeData.append(t);
                angleData.append(u);
            }
        }
        file.close();
    }

    // Построение графика угла как функции времени
    trajectoryPlot->clearGraphs();  // Очищаем старые графики
    trajectoryPlot->addGraph();
    trajectoryPlot->graph(0)->setData(timeData, angleData);

    // Подстраиваем оси под данные
    double timeMin = *std::min_element(timeData.begin(), timeData.end());
    double timeMax = *std::max_element(timeData.begin(), timeData.end());
    double angleMin = *std::min_element(angleData.begin(), angleData.end());
    double angleMax = *std::max_element(angleData.begin(), angleData.end());

    // Добавление небольших отступов
    double timePadding = (timeMax - timeMin) * 0.05;  // 5% от максимального диапазона времени
    double anglePadding = (angleMax - angleMin) * 0.05;  // 5% от максимального диапазона угла

    trajectoryPlot->xAxis->setRange(timeMin - timePadding, timeMax + timePadding);
    trajectoryPlot->yAxis->setRange(angleMin - anglePadding, angleMax + anglePadding);

    trajectoryPlot->xAxis->setTickLabelPadding(0);  // Уменьшаем отступы для меток
    trajectoryPlot->yAxis->setTickLabelPadding(0);  // Уменьшаем отступы для меток
    trajectoryPlot->xAxis->setLabelPadding(0);      // Убираем отступы для подписи оси X
    trajectoryPlot->yAxis->setLabelPadding(0);      // Убираем отступы для подписи оси Y

    trajectoryPlot->replot();

    // Для фазового портрета (скорость как функция угла)
    QVector<double> velocityData;
    QFile file2(outputFile);  // Используем тот же файл для фазового портрета
    if (file2.open(QIODevice::ReadOnly)) {
        QTextStream in2(&file2);
        in2.readLine();  // Пропускаем заголовок

        while (!in2.atEnd()) {
            QString line = in2.readLine();
            QStringList fields = line.split(",");

            if (fields.size() >= 3) {
                double u = fields[1].toDouble();
                double v = fields[2].toDouble();
                velocityData.append(v);
            }
        }
        file2.close();
    }

    // Построение фазового портрета
    phasePortraitPlot->clearGraphs();  // Очищаем старые графики
    phasePortraitPlot->addGraph();
    phasePortraitPlot->graph(0)->setData(angleData, velocityData);

    // Подстраиваем оси для фазового портрета
    double velocityMin = *std::min_element(velocityData.begin(), velocityData.end());
    double velocityMax = *std::max_element(velocityData.begin(), velocityData.end());

    // Добавление небольших отступов
    double velocityPadding = (velocityMax - velocityMin) * 0.05;  // 5% от максимального диапазона скорости

    phasePortraitPlot->xAxis->setRange(angleMin - anglePadding, angleMax + anglePadding);
    phasePortraitPlot->yAxis->setRange(velocityMin - velocityPadding, velocityMax + velocityPadding);

    phasePortraitPlot->xAxis->setTickLabelPadding(0);  // Уменьшаем отступы для меток
    phasePortraitPlot->yAxis->setTickLabelPadding(0);  // Уменьшаем отступы для меток
    phasePortraitPlot->xAxis->setLabelPadding(0);      // Убираем отступы для подписи оси X
    phasePortraitPlot->yAxis->setLabelPadding(0);      // Убираем отступы для подписи оси Y

    phasePortraitPlot->replot();
}


void MainWindow::calculatePendulum()
{
    // Получение параметров из интерфейса
    double L = lengthInput->text().toDouble();
    double g = gravityInput->text().toDouble();
    double u0 = u0Input->text().toDouble();
    double v0 = v0Input->text().toDouble();
    bool useStepControl = stepControlCheckbox->isChecked();

    double dt = stepInput->text().toDouble();
    double epsilon = epsilonInput->text().toDouble();

    // Логика выбора времени расчёта или количества циклов
    double t_end = timeInput->text().toDouble();

    // Подготовка начального состояния
    State y0 = {u0, v0}; // Начальные значения u и v
    double t0 = 0.0;     // Начальное время
    QString outputFile = "pendulum_output.csv";


    // Запуск расчёта
    if (useStepControl) {
        adaptiveRK4(y0, t0, t_end, dt, epsilon, g, L, outputFile.toStdString()); // Адаптивный шаг
    } else {
        fixedStepRK4(y0, t0, t_end, dt, g, L, outputFile.toStdString()); // ФиксЙированный шаг
    }

    // Построение графиков
    plotGraphs(outputFile);

    helpField->setText("Расчёт завершён.\n"
                       "L = " + QString::number(L) + " м\n" +
                       "g = " + QString::number(g) + " м/с²\n" +
                       "Начальный угол u₀ = " + QString::number(u0) + " рад\n" +
                       "Начальная скорость v₀ = " + QString::number(v0) + " м/с\n" +
                       "Шаг времени (dt) = " + QString::number(dt) + " с\n" +
                       "Точность (эпсилон) = " + QString::number(epsilon) + "\n" +
                       "Время расчёта = " + QString::number(t_end) + " с\n" +
                       "Использован контроль шага: " + (useStepControl ? "Да" : "Нет") + "\n");

}
