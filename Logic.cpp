#include <vector>
#include <cmath>


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
// Тип данных для вектора состояния
using State = std::vector<double>;

const double PI = 3.14159265358979323846;
const int P = 4; // Порядок метода Рунге-Кутта


State pendulumRHS(const State& y, double g, double L) {
    return {y[1], -g / L * sin(y[0])};
}

// Метод Рунге-Кутта 4-го порядка
State rungeKuttaStep(const State& y, double h, double g, double L, State (*f)(const State&, double, double)) {
    State k1 = f(y, g, L);
    State k2 = f({y[0] + h * 0.5 * k1[0], y[1] + h * 0.5 * k1[1]}, g, L);
    State k3 = f({y[0] + h * 0.5 * k2[0], y[1] + h * 0.5 * k2[1]}, g, L);
    State k4 = f({y[0] + h * k3[0], y[1] + h * k3[1]}, g, L);

    return {
        y[0] + h / 6.0 * (k1[0] + 2.0 * k2[0] + 2.0 * k3[0] + k4[0]),
        y[1] + h / 6.0 * (k1[1] + 2.0 * k2[1] + 2.0 * k3[1] + k4[1])
    };
}

// Функция численного решения с фиксированным шагом
/*
std::vector<StepData> fixedStepRK4(State y0, double t0, double t_end, double h, double g, double L) {
    std::vector<StepData> steps;
    double t = t0;

    steps.push_back({step, t, y0[0], y_half2[0], y_full[0]-y_half2[0], error, h, 0, 0});
    while (t <= t_end) {
        y0 = rungeKuttaStep(y0, h, g, L, pendulumRHS);
        t += h;
    }

    return steps;
}
*/
std::vector<StepData> adaptiveRK4(State y0, double t0, double t_end, double h0, double epsilon, double g, double L) {

    std::vector<StepData> steps;
    double t = t0;
    double h = h0;

    int C1 = 0; // Счетчик деления шага
    int C2 = 0; // Счетчик удвоения шага
    double max_error = 0.0; // Максимальная локальная ошибка
    double max_h = h0, min_h = h0; // Максимальный и минимальный шаг

    int step = 0;

    while (t < t_end) {
        step++;
        State y_full = rungeKuttaStep(y0, h, g, L, pendulumRHS);
        State y_half = rungeKuttaStep(y0, h / 2.0, g, L, pendulumRHS);
        State y_half2 = rungeKuttaStep(y_half, h / 2.0, g, L, pendulumRHS);

        double error = std::sqrt(std::pow(y_full[0] - y_half2[0], 2) + std::pow(y_full[1] - y_half2[1], 2)) / (std::pow(2, P) - 1);

        max_error = std::max(max_error, error); // неправильный олп

        steps.push_back({step, t, y_full[0], y_half2[0], y_full[0]-y_half2[0], error, h, C1, C2}); // не здесь push_back

        if (error <= epsilon) {
            // Accept the step and potentially double the step size
            y0 = y_full;
            t += h;
            if (error <= epsilon / (2 * std::pow(2, P))) {
                h *= 2.0;
                C2++;
                max_h = std::max(max_h, h);
            }
        }
        else {
            h /= 2.0;
            C1++;
            min_h = std::min(min_h, h);
        }

        if (t + h > t_end) {
            h = t_end - t;
        }
    }
    return steps;
}

double solvePendulum(double u0, double t_end, double g, double L) { // скорее всего не правильно, так как не принимаю значения шага
    State y = {u0, 0.0};
    double t = 0.0;
    double h = 0.01;

    while (t < t_end) {
        y = rungeKuttaStep(y, h, g, L, pendulumRHS);
        t += h;
    }

    return y[0];
}

double findInitialCondition(double t_end, double target, double left, double right, double epsilon, double g, double L) {
    double mid;
    while (std::abs(right - left) > epsilon) {
        mid = (left + right) / 2.0;
        double u_mid = solvePendulum(mid, t_end, g, L);

        if (u_mid < target) {
            left = mid;
        } else {
            right = mid;
        }
    }
    return (left + right) / 2.0;
}

double radiansToDegrees(double radians) {
    return radians * 180.0 / PI;
}
