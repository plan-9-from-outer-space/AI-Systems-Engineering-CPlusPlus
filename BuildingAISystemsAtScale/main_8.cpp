
#include <iostream>
#include <vector>
#include <cmath>
#include <numeric> // Required for std::accumulate

int main() {
    std::cout << "\n\nAll Dynamic Vectors Concepts Together" << std::endl;

    // Step 1: Simulate unknown dataset size
    int actual_samples = 50 + (rand() % 51);  // Random size between 50-100

    std::cout << "\n[Step 1] Simulating real-world data: " << actual_samples << " samples incoming (unknown size!)" << std::endl;

    // Step 2: Memory reservation for performance optimization
    const int ESTIMATED_SIZE = 100;  // Estimate based on typical data volume
    std::vector<double> rawFeatures;

    rawFeatures.reserve(ESTIMATED_SIZE);

    std::cout << "[Step 2] Memory reserved for " << ESTIMATED_SIZE << " samples (capacity: " << rawFeatures.capacity() << ")" << std::endl;

    // Step 3: Dynamic data loading with push_back
    std::cout << "[Step 3] Loading data dynamically with push_back..." << std::endl;

    for (int i = 0; i < actual_samples; i++) {
        double raw_value = 50.0 + (rand() % 100);  // Random values 50-150

        rawFeatures.push_back(raw_value);

        if (i < 5) {
            std::cout << "  Sample " << i << ": raw value = " << raw_value << " (pushed to vector)" << std::endl;
        }
    }
    std::cout << "  ... (loaded remaining " << (actual_samples - 5) << " samples)" << std::endl;
    std::cout << "  Final vector size: " << rawFeatures.size() << ", capacity: " << rawFeatures.capacity() << std::endl;

    // Step 4: Find min and max for normalization (data preprocessing)
    double min_value = rawFeatures[0];
    double max_value = rawFeatures[0];

    for (int i = 1; i < rawFeatures.size(); i++) {
        if (rawFeatures[i] < min_value) {
            min_value = rawFeatures[i];
        }
        if (rawFeatures[i] > max_value) {
            max_value = rawFeatures[i];
        }
    }
    std::cout << "\n[Step 4] Data range analysis: min = " << min_value << ", max = " << max_value << std::endl;

    // Step 5: Normalize data to 0-1 range
    std::vector<double> normalizedFeatures;
    normalizedFeatures.reserve(rawFeatures.size());  // Reserve exact size we need
    std::cout << "[Step 5] Normalizing data to 0-1 range..." << std::endl;

    double range = max_value - min_value;
    for (int i = 0; i < rawFeatures.size(); i++) {
        double normalized = (rawFeatures[i] - min_value) / range;

        normalizedFeatures.push_back(normalized);

        if (i < 5) {
            std::cout << "  Sample " << i << ": " << rawFeatures[i] << " → " << normalized << " (normalized)" << std::endl;
        }
    }
    std::cout << "  ... (normalized remaining " << (rawFeatures.size() - 5) << " samples)" << std::endl;
    std::cout << "  Normalized data ready! Size: " << normalizedFeatures.size() << std::endl;

    // Step 6: Calculate statistics on normalized data

    // Method 1: for loop
    double sum_normalized = 0.0;
    for (double val : normalizedFeatures) {
        sum_normalized += val;
    }
    double mean = sum_normalized / normalizedFeatures.size();

    // Method 2: accumulate
    double sum_normalized_2 = std::accumulate(normalizedFeatures.begin(), normalizedFeatures.end(), 0.0);
    double mean_2 = sum_normalized_2 / normalizedFeatures.size();
 
    std::cout << "\n[Step 6] Statistics on normalized data:" << std::endl;
    std::cout << "  Mean 1: " << mean   << " (should be around 0.5 for uniform data)" << std::endl;
    std::cout << "  Mean 2: " << mean_2 << " (should be around 0.5 for uniform data)" << std::endl;

    // Step 7: Create training batches
    const int BATCH_SIZE = 10;

    std::vector<std::vector<double>> trainingBatches;

    std::cout << "[Step 7] Creating training batches (batch size: " << BATCH_SIZE << ")..." << std::endl;

    std::vector<double> currentBatch;
    currentBatch.reserve(BATCH_SIZE);

    for (int i = 0; i < normalizedFeatures.size(); i++) {
        currentBatch.push_back(normalizedFeatures[i]);

        if(currentBatch.size() == BATCH_SIZE || i == normalizedFeatures.size() - 1) {
            trainingBatches.push_back(currentBatch);

            // if(trainingBatches.size() <= 2) {
            //     std::cout << "  Created batch " << trainingBatches.size() << " with " << currentBatch.size() << " samples" << std::endl;
            // }
            std::cout << "  Created batch " << trainingBatches.size() << " with " << currentBatch.size() << " samples" << std::endl;

            currentBatch.clear();  // Reset for next batch
            currentBatch.reserve(BATCH_SIZE);
        }
    }
    std::cout << "  ... (created remaining batches)" << std::endl;
    std::cout << "  Total batches: " << trainingBatches.size() << std::endl;
    std::cout << "  Last batch size: " << trainingBatches.back().size() << " samples" << std::endl;

    return 0;
}
