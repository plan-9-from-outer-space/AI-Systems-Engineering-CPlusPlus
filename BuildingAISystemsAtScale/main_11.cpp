#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <vector>


class NumericalAnalyzer {

public:

    static double getMachineEpsilon () {
        double eps = 1.0;
        while (1.0 + eps != 1.0) {
            eps /= 2.0;
        }
        return eps * 2.0;
    }

    static void analyzePrecision (double value) {
        std::cout << std::setprecision(17);

        std::cout << "Value: " << value << std::endl;

        std::cout << "Next representable: " << std::nextafter(value, value + 1.0) << std::endl;

        std::cout << "Difference: " << std::nextafter(value, value + 1.0) - value << std::endl;
    }

    // math cop
    static double kahanSum (const std::vector<double> &data) {
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

    static double naiveSum (const std::vector<double> &data) {
        double sum = 0.0;
        for (double value : data) {
            sum += value; // accumulates errors
        }
        return sum;
    }

    static double stableMean (const std::vector<double> &data) {
        if (data.empty())
            return 0.0;
        return kahanSum(data) / data.size();
    }

    static double stableVariance (const std::vector<double> &data) {
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

// Don't do this!
double unstableCalculation (double x, double y) {
    if (x > y) {
        return (x - y) / (x + y);
    }
    return 0.0;
}

// [SPEAK] "And here's the hero - the stable version!
double stableCalculation (double x, double y) {
    if (x > y) {
        return 1.0 - (2.0 * y) / (x + y);
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
    std::cout << " Part 1: IEEE 754 bit-level Analysis" << std::endl;

    std::cout << std::setprecision(17);

    auto inspectDouble = [](const char *label, double val) {
        uint64_t bits = 0;
        std::memcpy(&bits, &val, sizeof(bits));
        uint64_t sign = (bits >> 63) & 0x1;
        uint64_t exponent = (bits >> 52) & 0x7FF;
        uint64_t mantissa = bits & 0x000FFFFFFFFFFFFFULL;

        int truExp = static_cast<int>(exponent) - 1023;

        double ulp = std::nextafter(val, val + 1.0) - val;

        std::cout << "\n[" << label << "]" << std::endl;
        std::cout << " Stored value  : " << val << std::endl;
        std::cout << " Sign          : " << sign << " (" << (sign ? "negative" : "positive") << ")" << std::endl;
        std::cout << " Exponent bits : " << exponent << " => true exponent = 2& " << truExp << std::endl;
        std::cout << " Mantissa bits : 0x" << std::hex << mantissa << std::dec << std::endl;
        std::cout << " ulp (step)    : " << ulp << std::endl;
        std::cout << " Next repr.    : " << std::nextafter(val, val + 1.0) << std::endl;
    };

    inspectDouble("1.0", 1.0);
    inspectDouble("0.1", 0.1);
    inspectDouble("0.1 + 0.2", 0.1 + 0.2);

    // why 0.1 + 0.2 != 0.3 exactly

    std:: cout << std::setprecision(17);
    std::cout << "\n 0.1 + 0.2 == 0.3? " << std::boolalpha << (0.1 + 0.2 == 0.3) << std::endl;
    std::cout << " Error: " << std::abs((0.1 + 0.2) - 0.3) << std::endl;

    // Machine Epsilon

    double eps_d = NumericalAnalyzer::getMachineEpsilon();

    float eps_f = 1.0f;

    while (1.0f + eps_f != 1.0f) {
        eps_f /= 2.0f;
    }

    eps_f *= 2.0f;

    std::cout << "\n Machine epsilon (double): " << eps_d << std::endl;
    std::cout << " Machine epsilon (float) : " << eps_f << std::endl;
    std::cout << " Ratio (double/float)    : " << eps_d / eps_f << std::endl;

    return 0;
}
