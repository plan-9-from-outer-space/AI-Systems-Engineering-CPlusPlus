
#include <iomanip>
#include <iostream>

int main()
{
    std::cout << "\nAll the Static Arrays Video Concepts Together" << std::endl;


    // Step 1: Define compile-time constant
    const int NUM_CLASSES = 10;  // 10 classification classes
    std::cout << "\n[Step 1] Define array size: " << NUM_CLASSES << " classes" << std::endl;


    // Step 2: Zero initialization
    double rawScores[NUM_CLASSES] = {0.0};          // Raw classification scores
    double transformedScores[NUM_CLASSES] = {0.0};  // Transformed scores
    double sum = 0.0;                               // Zero-initialized accumulator
    std::cout << "[Step 2] Arrays initialized to zero (clean start!)" << std::endl;


    // Step 3: Fill raw scores with some values
    for(int i = 0; i < NUM_CLASSES; i++)
    {
        rawScores[i] = i * 1.5 + 2.0;  // Example: scores from 2.0 to 15.5
    }
    std::cout << "[Step 3] Raw scores calculated" << std::endl;


    // Step 4: Element-wise operations
    for(int i = 0; i < NUM_CLASSES; i++)
    {
        transformedScores[i] = rawScores[i] * rawScores[i];  // Element-wise squaring
        // Accumulate sum (zero-initialized accumulator) 
        sum += transformedScores[i];  
    }
    std::cout << "[Step 4] Element-wise squaring applied to all scores" << std::endl;


    // Step 5: Element-wise normalization:
    for(int i = 0; i < NUM_CLASSES; i++)
    {
        // Used to compute probabilities that sum to 1.0
        transformedScores[i] /= sum;  // Element-wise division (normalization)
    }
    std::cout << "[Step 5] Scores normalized (element-wise division)" << std::endl;


    // Step 6: Argmax -> Find best class
    int bestClass = 0;
    double bestScore = transformedScores[0];

    for (int i = 1; i < NUM_CLASSES; i++)
    {
        if (transformedScores[i] > bestScore)
        {
            bestScore = transformedScores[i];
            bestClass = i;  // Argmax -> index of maximum value
        }
    }
    std::cout << "\n[Step 6] Argmax operation complete!" << std::endl;


    std::cout << "Transformed scores: ";
    for(int i = 0; i < NUM_CLASSES; i++)
    {
        std::cout << std::fixed << std::setprecision(3) << transformedScores[i] << " ";
    }
    std::cout << "\nBest class: " << bestClass << " (score: " << bestScore << ")" << std::endl;

    return 0;
}
