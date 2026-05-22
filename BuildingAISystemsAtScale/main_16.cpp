#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <random>
#include <iomanip>

class PerformanceOptimizer {

public:

    static double naiveMatrixMultiply (
        const std::vector<std::vector<double>>& A,
        const std::vector<std::vector<double>>& B,
              std::vector<std::vector<double>>& C          // result
        ) {
        
        size_t n = A.size();
        size_t m = B[0].size();
        size_t p = A[0].size();

        // Time measurement
        auto start = std::chrono::high_resolution_clock::now();

        // Standard ijk loop ordering - poor cache performance
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < m; ++j) {
                C[i][j] = 0.0;
                for (size_t k = 0; k < p; ++k) {
                    C[i][j] += A[i][k] * B[k][j];
                }
            }
        }

        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(end - start).count();
    }

    static double optimizedMatrixMultiply (const std::vector<std::vector<double>>& A,
                                           const std::vector<std::vector<double>>& B,
                                           std::vector<std::vector<double>>& C) {
        size_t n = A.size();
        size_t m = B[0].size();
        size_t p = A[0].size();

        auto start = std::chrono::high_resolution_clock::now();

        // Loop reordering: ikj instead of ijk for better cache performance
        // This accesses A[i][k] sequentially and improves memory access patterns
        for (size_t i = 0; i < n; ++i) {
            for (size_t k = 0; k < p; ++k) {
                // Note: We could add a pragma here for vectorization
                // #pragma omp simd
                for (size_t j = 0; j < m; ++j) {
                    C[i][j] += A[i][k] * B[k][j];
                }
            }
        }

        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(end - start).count();
    }

    static double naiveDotProduct (const std::vector<double>& a, const std::vector<double>& b) {
        // Input validation would typically be added here
        // if (a.size() != b.size()) throw std::invalid_argument ("Size mismatch");

        double sum = 0.0;
        // Simple element-wise multiplication and accumulation
        for (size_t i = 0; i < a.size(); ++i) {
            sum += a[i] * b[i];
        }
        return sum;
    }

    static double optimizedDotProduct (const std::vector<double>& a, const std::vector<double>& b) {
        double sum = 0.0;
        size_t i = 0;
        size_t size = a.size();

        // Process 4 elements at a time (loop unrolling)
        // These can be computed in parallel
        for (; i + 3 < size; i += 4) {
            sum += a[i    ] * b[i    ];
            sum += a[i + 1] * b[i + 1];
            sum += a[i + 2] * b[i + 2];
            sum += a[i + 3] * b[i + 3];
        }

        // Handle remaining elements
        for (; i < size; ++i) {
            sum += a[i] * b[i];
        }

        return sum;
    }
};

class MemoryOptimizer {

public:

    static void demonstrateCacheFriendlyAccess () {

        const size_t SIZE = 1000;
        std::vector<std::vector<double>> matrix (SIZE, std::vector<double>(SIZE));

        // Initialize matrix
        for (size_t i = 0; i < SIZE; ++i) {
            for (size_t j = 0; j < SIZE; ++j) {
                matrix[i][j] = static_cast<double>(i + j);
            }
        }

        // Cache-friendly access (row-major order)
        auto start = std::chrono::high_resolution_clock::now();
        double sum1 = 0.0;
        for (size_t i = 0; i < SIZE; ++i) {
            for (size_t j = 0; j < SIZE; ++j) {
                sum1 += matrix[i][j];
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        double time1 = std::chrono::duration<double, std::milli>(end - start).count();

        // Cache-unfriendly access (column-major order)
        // Causes massive cache misses
        start = std::chrono::high_resolution_clock::now();
        double sum2 = 0.0;
        for (size_t j = 0; j < SIZE; ++j) {
            for (size_t i = 0; i < SIZE; ++i) {
                sum2 += matrix[i][j];
            }
        }
        end = std::chrono::high_resolution_clock::now();
        double time2 = std::chrono::duration<double, std::milli>(end - start).count();

        // Display cache performance results:
        std::cout << "Cache-friendly (row-major): " << time1 << " ms" << std::endl;
        std::cout << "Cache-unfriendly (column-major): " << time2 << " ms" << std::endl;
        std::cout << "Speedup: " << (time2 / time1) << "x faster with cache-friendly access!" << std::endl;
        std::cout << "Results match: " << (std::abs(sum1 - sum2) < 1e-10 ? "Positive" : "Negative") << std::endl;
    }

    static void demonstrateMemoryPooling () {

        const size_t NUM_ALLOCATIONS = 1000;
        const size_t ALLOCATION_SIZE = 1000; // total memory = 1000 x 1000 x 8 bytes = 8 MB

        // Demonstrate inefficient multiple small allocations:
        auto start = std::chrono::high_resolution_clock::now();
        std::vector<std::vector<double>> smallVectors;
        for (size_t i = 0; i < NUM_ALLOCATIONS; ++i) {
            smallVectors.push_back(std::vector<double>(ALLOCATION_SIZE, i));
        }
        auto end = std::chrono::high_resolution_clock::now();
        double small_alloc_time = std::chrono::duration<double, std::milli>(end - start).count();

        // Demonstrate efficient single large allocation:
        start = std::chrono::high_resolution_clock::now();
        std::vector<double> largeVector(NUM_ALLOCATIONS * ALLOCATION_SIZE);
        for (size_t i = 0; i < largeVector.size(); ++i) {
            largeVector[i] = sin(i * 0.001);
        }
        end = std::chrono::high_resolution_clock::now();
        double large_alloc_time = std::chrono::duration<double, std::milli>(end - start).count();

        // Display memory allocation results:
        std::cout << "Multiple small allocations: " << small_alloc_time << " ms" << std::endl;
        std::cout << "Single large allocation: " << large_alloc_time << " ms" << std::endl;
        std::cout << "Speedup: " << (small_alloc_time / large_alloc_time) << "x faster with memory pooling!" << std::endl;
        std::cout << "Memory efficiency: " << (NUM_ALLOCATIONS * ALLOCATION_SIZE * sizeof(double) / 1024) << " KB total" << std::endl;
    }
};

void demonstrateAlgorithmComplexity () {

    std::vector<size_t> sizes = {100, 500, 1000, 2000};

    std::cout << "Algorithm Complexity Scaling:" << std::endl;
    std::cout << "n\tO(n)\tO(n²)\tO(n³)" << std::endl;
    std::cout << "---------------------------" << std::endl;

    for (size_t n : sizes) {
        double linear = n;
        double quadratic = n * n;
        double cubic = n * n * n;

        std::cout << n << "\t" << linear << "\t" << quadratic << "\t" << cubic << std::endl;
    }

    std::cout << "\nNotice how cubic grows much faster than quadratic!" << std::endl;
}

int main() {

    std::cout << "\nWildfire Dispatch Complexity Budgets" << std::endl;

    struct ComplexityScenario {
        const char *workload;
        size_t problemSize;
        double latencyBudgetMs;    // milliseconds
        const char *stackHint;
    };

    std::vector <ComplexityScenario> complexityScenarios = {
        {"Live tile lookup by zone", 1200, 40.0, "unordered_map + hot zone cache"},
        {"Drone candidate rerank", 8000, 40.0, "partial_sort / Top-K heap"},
        {"Threat score refresh", 24000, 120.0, "SIMD dot products / BLAS"},
        {"Naive drone-tile cross product", 60000, 40.0, "prefilter first, then score"}
    };

    const double EDGE_CPU_OPS_PER_MS = 50000.0;

    std::cout << std::left << std::setw(35) << "Dispatch task"
              << std::right << std::setw(15) << "n"
              << std::setw(19) << "Budget ms"
              << std::setw(18) << "n log n"
              << std::setw(15) << "n^2"
              << std::setw(15) << "n^3" << std::endl;

    for (const auto &scenario : complexityScenarios) {

        double logarithmic = std::log2(static_cast<double>(scenario.problemSize));
        double linear = static_cast<double>(scenario.problemSize);
        double linearithmic = linear * logarithmic;
        double quadratic = linear * linear;
        double cubic = quadratic * linear;

        double budgetOperations = scenario.latencyBudgetMs * EDGE_CPU_OPS_PER_MS;

        const char *firstCurveToBreak =
            linearithmic > budgetOperations ? "n log n"
                : quadratic > budgetOperations ? "n^2"
                : cubic > budgetOperations ? "n^3"
                : "none";

        std::cout << std::left << std::setw(35) << scenario.workload;
        std::cout << std::right << std::setw(16) << scenario.problemSize;
        std::cout << std::setw(16) << scenario.latencyBudgetMs;
        std::cout << std::setw(18) << static_cast<size_t>(linearithmic);
        std::cout << std::setw(18) << static_cast<size_t>(quadratic);
        std::cout << std::setw(18) << static_cast<size_t>(cubic > 9.22e18 ? 9.22e18 : cubic) << std::endl;

        double quadraticPenalty  = linearithmic > 0.0 ? quadratic / linearithmic : 0.0;
        double cubicPenalty  = linearithmic > 0.0 ? cubic / linearithmic : 0.0;

        std::cout << "   Recommended stack: " << scenario.stackHint << std::endl;
        std::cout << "   Edge operation budget: " << static_cast<size_t>(budgetOperations) << " ops" << std::endl;
        std::cout << "   Quadratic penalty vs n log n: " << quadraticPenalty << "x" << std::endl;
        std::cout << "   Cubic penalty vs n log n: " << cubicPenalty << "x" << std::endl;
        std::cout << "   First curve that breaks budget: " << firstCurveToBreak << std::endl << std::endl;
    }

    return 0;
}
