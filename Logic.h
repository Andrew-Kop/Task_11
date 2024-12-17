#ifndef LOGIC_H
#define LOGIC_H

#include <vector>
#include <string>

// Тип данных для вектора состояния
using State = std::vector<double>;

// Прототипы функций
State pendulumRHS(const State& y, double g, double L);
State rungeKuttaStep(const State& y, double h, double g, double L, State (*f)(const State&, double, double));
void fixedStepRK4(State y0, double t0, double t_end, double h, double g, double L, const std::string& output_file);
void adaptiveRK4(State y0, double t0, double t_end, double h0, double epsilon, double g, double L, const std::string& output_file);
double solvePendulum(double u0, double t_end);
double findInitialCondition(double t_end, double target, double left, double right);
double radiansToDegrees(double radians);
double calculate_time(int N, double L, double g = 9.81);

#endif // LOGIC_H