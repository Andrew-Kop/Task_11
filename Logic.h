#ifndef LOGIC_H
#define LOGIC_H

#include <vector>
#include <string>

struct RK4Params {
    int C1;          // Количество делений шага
    int C2;          // Количество удвоений шага
    double max_h;    // Максимальный шаг
    double min_h;    // Минимальный шаг
    int num_steps;   // Количество шагов
    double max_error; // Максимальная локальная ошибка
    double max_diff1; // Максимальное значение diff1
    int max_diff1_step; // Шаг, на котором достигнут max_diff1
    double min_diff1; // Минимальное значение diff1
    int min_diff1_step; // Шаг, на котором достигнут min_diff1
    double max_diff2; // Максимальное значение diff2
    int max_diff2_step; // Шаг, на котором достигнут max_diff2
    double min_diff2; // Минимальное значение diff2
    int min_diff2_step; // Шаг, на котором достигнут min_diff2
    double time_diff; // Разница между последним t и t_end
};

// Тип данных для вектора состояния
using State = std::vector<double>;

struct StepData;

// Прототипы функций
State pendulumRHS(const State& y, double g, double L);
State rungeKuttaStep(const State& y, double h, double g, double L, State (*f)(const State&, double, double));
std::vector<StepData> fixedStepRK4(State y0, double t0, double t_end, double h, double g, double L);
std::pair<std::vector<StepData>, RK4Params> adaptiveRK4(State y0, double t0, double t_end, double h0, double epsilon, double g, double L, int max_steps, double boundary_epsilon);
double solvePendulum(double u0, double t_end);
double findInitialCondition(double t_end, double target, double left, double right);
double radiansToDegrees(double radians);
double calculate_time(int N, double L, double g = 9.81);

#endif // LOGIC_H
