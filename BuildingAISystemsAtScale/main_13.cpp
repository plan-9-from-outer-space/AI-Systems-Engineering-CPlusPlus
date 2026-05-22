#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

class Matrix {

private:

    std::vector<std::vector<double>> data;
    size_t rows;
    size_t cols;

public:

    Matrix(size_t r, size_t c) : rows(r), cols(c) {
        data.resize(rows, std::vector<double>(cols, 0.0));
    }

    Matrix(size_t r, size_t c, double initialValue) : rows(r), cols(c) {
        data.resize(rows, std::vector<double>(cols, initialValue));
    }

    Matrix(const std::vector<std::vector<double>>& inputData) {
        if (inputData.empty() || inputData[0].empty()) {
            throw std::invalid_argument("Empty matrix data provided");
        }

        rows = inputData.size();
        cols = inputData[0].size();

        for (const auto& row : inputData) {
            if (row.size() != cols) {
                throw std::invalid_argument("Inconsistent row sizes in matrix data");
            }
        }

        data = inputData;
    }

    double& operator()(size_t i, size_t j) {
        if (i >= rows || j >= cols) {
            throw std::out_of_range("Matrix indices out of range");
        }
        return data[i][j];
    }

    double operator()(size_t i, size_t j) const {
        if (i >= rows || j >= cols) {
            throw std::out_of_range("Matrix indices out of range");
        }
        return data[i][j];
    }

    size_t getRows() const { return rows; }
    size_t getCols() const { return cols; }

    Matrix operator+(const Matrix& other) const {
        if (rows != other.rows || cols != other.cols) {
            throw std::invalid_argument("Matrix dimensions don't match for addition");
        }

        Matrix result(rows, cols);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result(i, j) = data[i][j] + other(i, j);
            }
        }
        return result;
    }

    Matrix operator*(const Matrix& other) const {
        if (cols != other.rows) {
            throw std::invalid_argument("Matrix dimensions don't match for multiplication");
        }

        Matrix result(rows, other.cols, 0.0);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < other.cols; ++j) {
                for (size_t k = 0; k < cols; ++k) {
                    result(i, j) += data[i][k] * other(k, j);
                }
            }
        }

        return result;
    }

    std::vector<double> multiplyVector(const std::vector<double>& vec) const {
        if (vec.size() != cols) {
            throw std::invalid_argument("Vector size doesn't match matrix columns");
        }

        std::vector<double> result(rows, 0.0);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result[i] += data[i][j] * vec[j];
            }
        }

        return result;
    }

    Matrix transpose() const {
        Matrix result(cols, rows);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result(j, i) = data[i][j];
            }
        }

        return result;
    }
};

struct WelfordStats {
    long long n = 0;
    double mean = 0.0;
    double M2 = 0.0;

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

inline double norm2(const std::vector<double>& v) {
    double s = 0.0;
    for (double x : v) {
        s += x * x;
    }
    return std::sqrt(s);
}

inline void print_matrix(const std::string& title, const Matrix& m) {
    std::ios::fmtflags old_flags = std::cout.flags();
    std::streamsize old_precision = std::cout.precision();

    std::cout << "\n" << title << " (" << m.getRows() << "x" << m.getCols() << ")\n";
    for (size_t r = 0; r < m.getRows(); ++r) {
        std::cout << "  [";
        for (size_t c = 0; c < m.getCols(); ++c) {
            std::cout << std::fixed << std::setprecision(4) << std::setw(9) << m(r, c);
            if (c + 1 < m.getCols()) {
                std::cout << ", ";
            }
        }
        std::cout << "]\n";
    }

    std::cout.flags(old_flags);
    std::cout.precision(old_precision);
}

Matrix add_bias_rowwise(const Matrix& m, const std::vector<double>& bias) {
    Matrix out = m;
    for (size_t r = 0; r < out.getRows(); ++r) {
        for (size_t c = 0; c < out.getCols(); ++c) {
            out(r, c) += bias[c];
        }
    }
    return out;
}

Matrix relu(const Matrix& m) {
    Matrix out = m;
    for (size_t r = 0; r < out.getRows(); ++r) {
        for (size_t c = 0; c < out.getCols(); ++c) {
            out(r, c) = std::max(0.0, out(r, c));
        }
    }
    return out;
}

double sigmoid(double x) {
    if (x >= 0.0) {
        double z = std::exp(-x);
        return 1.0 / (1.0 + z);
    }
    double z = std::exp(x);
    return z / (1.0 + z);
}

std::vector<double> sigmoid_column(const Matrix& logits) {
    std::vector<double> probs(logits.getRows(), 0.0);
    for (size_t r = 0; r < logits.getRows(); ++r) {
        probs[r] = sigmoid(logits(r, 0));
    }
    return probs;
}

double stable_bce_from_logit(double logit, double y) {
    return std::max(logit, 0.0) - y * logit + std::log1p(std::exp(-std::abs(logit)));
}

double naive_bce_from_probability(double p, double y) {
    if ((y == 1.0 && p <= 0.0) || (y == 0.0 && p >= 1.0)) {
        return std::numeric_limits<double>::infinity();
    }
    return -(y * std::log(p) + (1.0 - y) * std::log(1.0 - p));
}

double mean_bce_from_logits(const Matrix& logits, const std::vector<double>& labels) {
    double sum = 0.0;
    for (size_t r = 0; r < logits.getRows(); ++r) {
        sum += stable_bce_from_logit(logits(r, 0), labels[r]);
    }
    return sum / static_cast<double>(logits.getRows());
}

std::vector<double> clip_by_global_norm(const std::vector<double>& grad, double max_norm) {
    double norm = norm2(grad);
    if (norm <= max_norm || norm == 0.0) {
        return grad;
    }
    double scale = max_norm / norm;
    std::vector<double> out = grad;
    for (double& v : out) {
        v *= scale;
    }
    return out;
}

double symmetric_2x2_condition_number(double a, double b, double d) {
    double trace = a + d;
    double gap = std::sqrt((a - d) * (a - d) + 4.0 * b * b);
    double lambda_max = 0.5 * (trace + gap);
    double lambda_min = 0.5 * (trace - gap);
    if (std::abs(lambda_min) < 1e-12) {
        return std::numeric_limits<double>::infinity();
    }
    return std::abs(lambda_max) / std::abs(lambda_min);
}

std::pair<double, double> solve_2x2(double a, double b, double c, double d, double y1, double y2) {
    double det = a * d - b * c;
    return {
        (d * y1 - b * y2) / det,
        (-c * y1 + a * y2) / det
    };
}

int main()
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    std::cout << "\n Step 1: Incoming Payments Become a Feature Matrix" << std::endl;

    std::vector<std::string> transaction_ids = {
        "txn_001",
        "txn_002",
        "txn_003",
        "txn_004"
    };

    Matrix X ({
        {-0.35, 0.10, 0.00, 0.05},
        { 2.80, 3.40, 1.00, 0.95},
        { 1.90, 2.60, 1.00, 0.75},
        {-0.10, 0.30, 0.00, 0.15}
    });

    print_matrix ("Feature matrix X = [amount_z, velocity_1h, geo_mismatch, device_risk]", X);

    std::cout << "\nTransaction batch legend: \n";

    for (size_t i = 0; i < transaction_ids.size(); ++i) {
        std::cout << "  " << transaction_ids[i]
                  << " -> amount_z=" << X(i, 0)
                  << ", velocity_1h=" << X(i, 1)
                  << ", geo_mismatch=" << X(i, 2)
                  << ", device_risk=" << X(i, 3) << "\n";
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    std::cout << "\n Step 2: Matrix Forward Pass for Fraud Risk \n";

    Matrix W1 ({
        {0.70, 0.30, 0.50},
        {0.90, 0.60, 0.40},
        {1.10, 0.20, 0.30},
        {0.80, 0.40, 0.90}
    });

    print_matrix("Hidden-layer weights W1", W1);

    std::vector<double> b1 = {0.05, -0.10, 0.02};

    Matrix hidden_layer = X * W1;
    print_matrix("Hidden pre-activation X * W1", hidden_layer);

    Matrix hidden_shifted = add_bias_rowwise (hidden_layer, b1);
    print_matrix("Hidden pre-activation plus bias", hidden_shifted);

    Matrix hidden = relu (hidden_shifted);
    print_matrix("Hidden activation after ReLU", hidden);

    // Output layer
    Matrix W2 (std::vector<std::vector<double>>{
        {0.90},
        {1.20},
        {0.85}
    });

    print_matrix ("Output-layer weights W2", W2);

    std::vector<double> b2 = {-1.10}; // output bias

    Matrix logits_linear = hidden * W2;
    Matrix logits = add_bias_rowwise (logits_linear, b2);
    print_matrix ("Fraud logits", logits);

    std::vector<double> probs = sigmoid_column(logits);

    const double review_threshold = 0.70;

    std::cout << "\nScoring table:\n";
    std::cout << " Transaction      Logit       Probability     Decision\n";
    for (size_t i = 0; i < transaction_ids.size(); ++i) {
        std::string decision = probs[i] >= review_threshold ? "REVIEW" : "APPROVE";
        std::cout << "  " << std::setw(10) << std::left << transaction_ids[i]
                  << std::right << "  " << std::setw(9) << logits(i, 0)
                  << "    " << std::setw(11) << probs[i]
                  << "    " << decision << "\n";
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    std::cout << "\n Step 3: Stable Fraud Loss Under Extreme Logits \n";

    std::vector<double> labels = {0.0, 1.0, 1.0, 0.0};

    double batch_stable_loss = mean_bce_from_logits (logits, labels);
    std::cout << " Stable batch BCE from current fraud logits: " << batch_stable_loss << "\n";

    std::vector<double> extreme_logits = {12.0, -12.0, 1000.0, -1000.0};
    std::vector<double> extreme_labels = {1.0, 0.0, 0.0, 1.0};

    std::cout << "\nExtreme-logit BCE comparison:\n";
    std::cout << "  Logits      Label       Sigmoid(p)      Naive BCE       Stable BCE\n";

    for (size_t i = 0; i < extreme_logits.size(); ++i) {
        double p = sigmoid (extreme_logits[i]);
        double naive = naive_bce_from_probability (p, extreme_labels[i]);
        double stable = stable_bce_from_logit (extreme_logits[i], extreme_labels[i]);

        std::cout << "  " << std::setw(9) << extreme_logits[i]
                  << "      " << std::setw(5) << extreme_labels[i]
                  << "      " << std::setw(12) << p
                  << "      ";
        if(std::isinf(naive)) {
            std::cout << std::setw(12) << "inf";
        } else {
            std::cout << std::setw(12) << naive;
        }
        std::cout << "      " << std::setw(12) << stable << "\n";
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    std::cout << "\n Step 4: Streaming Transaction Statistics with Welford \n";

    std::vector<double> streamed_amounts = {18.9, 21.15, 19.80, 23.40, 4999.0, 27.30, 18.40};

    WelfordStats amount_stats;

    std::cout << "  Sample      Amount      RunningMean     RunningStd\n";

    for (size_t i = 0; i < streamed_amounts.size(); ++i) {
        amount_stats.update(streamed_amounts[i]);
        std::cout << "  "     << std::setw(4)  << (i + 1)
                  << "      " << std::setw(10) << streamed_amounts[i]
                  << "      " << std::setw(12) << amount_stats.mean
                  << "      " << std::setw(10) << amount_stats.stddev() << "\n";
    }

    double new_amount = 3200.0;
    double new_amount_z = (new_amount - amount_stats.mean) / amount_stats.stddev();

    std::cout << "\n New transaction amount: " << new_amount << "\n";
    std::cout << " Approximate live z-score from streaming stats: " << new_amount_z << "\n";

    ///////////////////////////////////////////////////////////////////////////////////////////
    std::cout << "\n Step 5: Gradient Check and Gradient Clipping \n";

    Matrix residuals(probs.size(), 1);

    for (size_t i = 0; i < probs.size(); ++i) {
        residuals(i, 0) = (probs[i] - labels[i]) / static_cast<double>(probs.size());
    }
    print_matrix("Output residuals (p - y) / batch_size", residuals);

    Matrix hidden_t = hidden.transpose();
    Matrix grad_w2 = hidden_t * residuals;

    print_matrix("Analitic gradient for W2", grad_w2);

    const double epsilon = 1e-4;

    Matrix W2_plus = W2;
    Matrix W2_minus = W2;

    W2_plus(0, 0) += epsilon;
    W2_minus(0, 0) -= epsilon;

    Matrix logits_plus = add_bias_rowwise (hidden * W2_plus, b2);
    Matrix logits_minus = add_bias_rowwise (hidden * W2_minus, b2);

    double loss_plus = mean_bce_from_logits (logits_plus,  labels);
    double loss_minus = mean_bce_from_logits (logits_minus, labels);
    double numerical_grad = (loss_plus - loss_minus) / (2.0 * epsilon);
    double analitic_grad = grad_w2(0, 0);

    std::cout << "  Analitic  gradient for W2[0, 0]: " << analitic_grad << "\n";
    std::cout << "  Numerical gradient for W2[0, 0]: " << numerical_grad << "\n";
    std::cout << "  Asolute difference:              " << std::abs(analitic_grad - numerical_grad) << "\n";

    std::vector<double> noisy_grad = {
        grad_w2(0, 0) * 80.0,
        grad_w2(1, 0) * 80.0,
        grad_w2(2, 0) * 80.0
    };

    double grad_norm_before = norm2(noisy_grad);

    std::vector<double> clipped_grad = clip_by_global_norm (noisy_grad, 1.5);
    double grad_norm_after = norm2(clipped_grad);

    std::cout << "\n Gradient norm before clipping: " << grad_norm_before << "\n";
    std::cout << " Gradient norm after clipping: " << grad_norm_after << "\n";
    std::cout << " Clipped gradient vector:      [";

    for (size_t i = 0; i < clipped_grad.size(); ++i) {
        std::cout << clipped_grad[i];
        if(i + 1 < clipped_grad.size()) {
            std::cout << ", ";
        }
    }
    std::cout << "]\n";

    ///////////////////////////////////////////////////////////////////////////////////////////
    std::cout << "\n Step 6: Correlated Features and Ridge Regularization \n";

    double a = 1000.0;
    double b = 999.5;
    double d = 999.05;
    double y1 = 120.0;
    double y2 = 119.70;
    double y2_shifted = 119.71;
    double lambda = 10.0;
    double cond_raw = symmetric_2x2_condition_number (a, b, d);
    double cond_ridge = symmetric_2x2_condition_number (a + lambda, b, d + lambda);

    std::pair<double, double> w_raw           = solve_2x2 (a,          b, b, d,          y1, y2);
    std::pair<double, double> w_raw_shifted   = solve_2x2 (a,          b, b, d,          y1, y2_shifted);
    std::pair<double, double> w_ridge         = solve_2x2 (a + lambda, b, b, d + lambda, y1, y2);
    std::pair<double, double> w_ridge_shifted = solve_2x2 (a + lambda, b, b, d + lambda, y1, y2_shifted);

    std::cout << " Raw condition number:  " << cond_raw << "\n";
    std::cout << " Ridge condition number: " << cond_ridge << "\n";
    std::cout << "\n Raw solve weights:     [" << w_raw.first << ", " << w_raw.second << "]\n";
    std::cout << " Raw solve after tiny shift: [" << w_raw_shifted.first << ", " << w_raw_shifted.second << "]\n";
    std::cout << " Ridge solve weights:     [" << w_ridge.first << ", " << w_ridge.second << "]\n";
    std::cout << " Ridge solve after shift:     [" << w_ridge_shifted.first << ", " << w_ridge_shifted.second << "]\n";

    return 0;
}
