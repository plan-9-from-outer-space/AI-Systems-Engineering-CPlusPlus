#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <vector>


class NumericalAnalyzer {
public:
    static double getMachineEpsilon() {
        double eps = 1.0;
        while (1.0 + eps != 1.0) {
            eps /= 2.0;
        }
        return eps * 2.0;
    }

    static void analyzePrecision(double value) {
        std::cout << std::setprecision(17);
        std::cout << "Value: " << value << std::endl;

        std::cout << "Next representable: " << std::nextafter(value, value + 1.0) << std::endl;

        std::cout << "Difference: " << std::nextafter(value, value + 1.0) - value << std::endl;
    }

    static double kahanSum(const std::vector<double> &data) {
        double sum = 0.0;
        double compensation = 0.0;

        for (double value : data) {
            double y = value - compensation;

            double t = sum + y;

            compensation = (t - sum) - y;

            sum = t;
        }

        return sum;
    }

    static double naiveSum(const std::vector<double> &data) {
        double sum = 0.0;
        for (double value : data) {
            sum += value;
        }
        return sum;
    }

    static double stableMean(const std::vector<double> &data) {
        if (data.empty())
            return 0.0;
        return kahanSum(data) / data.size();
    }

    static double stableVariance(const std::vector<double> &data) {
        if (data.size() <= 1)
            return 0.0;

        double mean = stableMean(data);
        double sumSquares = 0.0;

        for (double value : data) {
            double diff = value - mean;
            sumSquares += diff * diff;
        }

        return sumSquares / (data.size() - 1);
    }
};

double unstableCalculation(double x, double y) {
    if (x > y) {
        return (x - y) / (x + y);
    }
    return 0.0;
}

// [SPEAK] "And here's the hero - the stable version!
double stableCalculation(double x, double y) {
    if (x > y) {
        return 1.0 - 2.0 * y / (x + y);
    }
    return 0.0;
}

// =============================================================================
// Advanced helper functions used across multiple parts
// =============================================================================

double neumaierSum(const std::vector<double> &data) {
    double sum = 0.0;
    double c = 0.0; // running compensation
    for (double v : data) {
        double t = sum + v;
        if (std::abs(sum) >= std::abs(v))
            c += (sum - t) + v; // sum is big: v is lost in t
        else
            c += (v - t) + sum; // v is big: sum is lost in t
        sum = t;
    }
    return sum + c;
}

// Welford's online mean/variance algorithm (single-pass, numerically stable)
struct WelfordStats {
    long long n = 0;
    double mean = 0.0;
    double M2 = 0.0; // sum of squared deviations
    void update(double x) {
        ++n;
        double delta = x - mean;
        mean += delta / n;
        double delta2 = x - mean;
        M2 += delta * delta2;
    }
    double variance() const { return (n > 1) ? M2 / (n - 1) : 0.0; }
    double stddev() const { return std::sqrt(variance()); }
};

// Numerically stable log-sum-exp: log(sum(exp(x_i)))
double logSumExp(const std::vector<double> &logits) {
    double maxVal = *std::max_element(logits.begin(), logits.end());
    double sum = 0.0;
    for (double v : logits)
        sum += std::exp(v - maxVal);
    return maxVal + std::log(sum);
}

// 2×2 matrix-vector multiply: y = A*x
std::vector<double> matVec2(const std::vector<double> &A,
                            const std::vector<double> &x) {
    return {A[0] * x[0] + A[1] * x[1], A[2] * x[0] + A[3] * x[1]};
}

// 2-norm of a vector
double norm2(const std::vector<double> &v) {
    double s = 0.0;
    for (double x : v)
        s += x * x;
    return std::sqrt(s);
}

int main()
{
    {
        std::cout << "\n[Quadratic Formula — Discriminant Cancellation]" << std::endl;

        double a = 1.0, b = -1e8, c = 1.0;
        double disc = b * b - 4.0 * a * c;
        double sqrtDisc = std::sqrt(disc);
        double x1_naive  = (-b + sqrtDisc) / (2.0 * a);
        double x2_naive  = (-b - sqrtDisc) / (2.0 * a);
        double x1_stable = (-b + sqrtDisc) / (2.0 * a);
        double x2_stable = c / (a * x1_stable);

        std::cout << std::setprecision(15);

        std::cout << "  Naive   x1 = " << x1_naive  << "  x2 = " << x2_naive  << std::endl;
        std::cout << "  Stable  x1 = " << x1_stable << "  x2 = " << x2_stable << std::endl;
        std::cout << "  True x2 (c/b≈) = " << (c / std::abs(b)) << std::endl;
        std::cout << "  Error in naive x2: " << std::abs(x2_naive - x2_stable) << std::endl;
    }

    {
        std::cout << "\n[Binary Cross-Entropy — Numerically Stable vs Naive]" << std::endl;

        std::vector<std::pair<double, double>> cases = {
            {1.0, 1.0 - 1e-15},
            {0.0, 1e-300},
            {1.0, 0.5},
        };

        std::cout << std::setprecision(6);

        std::cout << "  y  p   Naive BCE       Stable BCE (logit form)" << std::endl;

        for (auto &[y, p] : cases) {
            double logit = std::log(p) - std::log(1.0 - p);
            double naive_bce = -(y * std::log(p) + (1.0 - y) * std::log(1.0 - p));
            double stable_bce = std::max(logit, 0.0) - y * logit + std::log(1.0 + std::exp(-std::abs(logit)));

            std::cout << " " << y << " " << p << "  naive=" << naive_bce << "  stable=" << stable_bce << std::endl;
        }
    }

    {
        std::cout << "\n[Subtractive Cancellation — ULP Analysis]" << std::endl;

        struct TestPair {
            double a, b;
            const char *label;
        };

        std::vector<TestPair> pairs = {
            {1.0000000000001, 1.0000000000000, "1e-12 apart"},
            {1.23456789012345, 1.23456789012344, "1e-14 apart"},
            {1.0 + 1e-15, 1.0, "1e-15 apart (near eps)"},
        };

        std::cout << std::setprecision(17);

        for (auto &p : pairs) {
            double diff = p.a - p.b;
            double rel_err = std::abs(diff) / std::max(std::abs(p.a), std::abs(p.b));
            int bits_lost = (rel_err > 0.0) ? static_cast<int>(-std::log2(rel_err)) : 52;

            std::cout << " [" << p.label << "]" << std::endl;
            std::cout << "  a - b      = " << diff << std::endl;
            std::cout << "  Rel. error = " << rel_err << std::endl;
            std::cout << "   Bits lost ~ " << bits_lost << " of 52 mantissa bits" << std::endl;
        }
    }

    return 0;
}
