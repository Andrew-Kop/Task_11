#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <iomanip>

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
void fixedStepRK4(State y0, double t0, double t_end, double h, double g, double L, const std::string& output_file) {
    std::ofstream out(output_file);
    if (!out.is_open()) {
        std::cerr << "Ошибка открытия файла для записи!" << std::endl;
        return;
    }

    out << "t,u,v\n";
    double t = t0;

    while (t <= t_end) {
        out << std::fixed << std::setprecision(6) << t << "," << y0[0] << "," << y0[1] << "\n";
        y0 = rungeKuttaStep(y0, h, g, L, pendulumRHS);
        t += h;
    }

    out.close();
    std::cout << "Результаты сохранены в файл: " << output_file << std::endl;
}

void adaptiveRK4(State y0, double t0, double t_end, double h0, double epsilon, double g, double L, const std::string& output_file) {
    std::ofstream out(output_file);
    if (!out.is_open()) {
        std::cerr << "Ошибка открытия файла для записи!" << std::endl;
        return;
    }

    out << "t,u,v\n";
    double t = t0;
    double h = h0;

    while (t < t_end) {
        out << std::fixed << std::setprecision(6) << t << "," << y0[0] << "," << y0[1] << "\n";

        State y_full = rungeKuttaStep(y0, h, g, L, pendulumRHS);
        State y_half = rungeKuttaStep(y0, h / 2.0, g, L, pendulumRHS);
        State y_half2 = rungeKuttaStep(y_half, h / 2.0, g, L, pendulumRHS);

        double error = std::sqrt(std::pow(y_full[0] - y_half2[0], 2) + std::pow(y_full[1] - y_half2[1], 2)) / (std::pow(2, P) - 1);

        if (error <= epsilon) {
            y0 = y_half2;
            t += h;
            if (error < epsilon / 2.0) {
                h *= 2.0;
            }
        } else {
            h /= 2.0;
        }

        if (t + h > t_end) {
            h = t_end - t;
        }
    }

    out.close();
    std::cout << "Результаты сохранены в файл: " << output_file << std::endl;
}

double solvePendulum(double u0, double t_end, double g, double L) {
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

double calculate_time(int N, double L, double g = 9.81) {
    double period = 2 * PI * sqrt(L / g);
    return N * period;
}
