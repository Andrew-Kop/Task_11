#include <vector>
#include <cmath>


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
    //double time_diff; // Разница между последним t и t_end
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

std::pair<std::vector<StepData>, RK4Params> adaptiveRK4(State y0, double t0, double t_end, double h0, double epsilon, double g, double L, int max_steps, double boundary_epsilon) {
    std::vector<StepData> steps;
    double t = t0;
    double h = h0;
    int step = 0;
    int C1 = 0;
    int C2 = 0;
    double max_error = 0.0;
    double max_h = h0;
    double min_h = h0;

    double max_diff1 = -std::numeric_limits<double>::infinity();
    int max_diff1_step = -1;
    double min_diff1 = std::numeric_limits<double>::infinity();
    int min_diff1_step = -1;
    double max_diff2 = -std::numeric_limits<double>::infinity();
    int max_diff2_step = -1;
    double min_diff2 = std::numeric_limits<double>::infinity();
    int min_diff2_step = -1;

    steps.push_back({step, t, y0[0], 0.0, 0.0, y0[1], 0.0, 0.0, 0.0, h, C1, C2});

    while (t < t_end && step < max_steps) { // Добавлено условие по max_steps
        step++;
        State y_full = rungeKuttaStep(y0, h, g, L, pendulumRHS);
        State y_half = rungeKuttaStep(y0, h / 2.0, g, L, pendulumRHS);
        State y_half2 = rungeKuttaStep(y_half, h / 2.0, g, L, pendulumRHS);

        double diff1 = std::abs(y_full[0] - y_half2[0]);
        double diff2 = std::abs(y_full[1] - y_half2[1]);
        double error = std::sqrt(std::pow(diff1, 2) + std::pow(diff2, 2)) / (std::pow(2, P) - 1);

        if (error <= epsilon) {
            y0 = y_full;
            t += h;

            steps.push_back({step, t, y_full[0], y_half2[0], diff1, y_full[1], y_half2[1], diff2, error, h, C1, C2});
            max_error = std::max(max_error, error);
            max_diff1 = std::max(max_diff1, diff1);
            if(max_diff1 == diff1) max_diff1_step = step;
            min_diff1 = std::min(min_diff1, diff1);
            if(min_diff1 == diff1) min_diff1_step = step;
            max_diff2 = std::max(max_diff2, diff2);
            if(max_diff2 == diff2) max_diff2_step = step;
            min_diff2 = std::min(min_diff2, diff2);
            if(min_diff2 == diff2) min_diff2_step = step;

            if (error <= epsilon / (2 * std::pow(2, P))) {
                h *= 2.0;
                C2++;
                max_h = std::max(max_h, h);
            }
        } else {
            h /= 2.0;
            C1++;
            min_h = std::min(min_h, h);
            continue;
        }

        if (std::abs(t - t_end) < boundary_epsilon) { // Проверка точности на границе
            break; // Выход из цикла, если достигнута заданная точность
        }
        if (t + h > t_end) {
            h = t_end - t;
        }
    }

    //double time_diff = h;
    RK4Params params = {C1, C2, max_h, min_h, step, max_error, max_diff1, max_diff1_step, min_diff1, min_diff1_step, max_diff2, max_diff2_step, min_diff2, min_diff2_step};
    return {steps, params};
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
