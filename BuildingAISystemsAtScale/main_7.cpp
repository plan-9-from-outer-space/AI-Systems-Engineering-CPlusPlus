#include <iostream>
#include <vector>
#include <cmath>

int main()
{
    std::cout << "\nCreating empty vector for AI features:" << std::endl;

    std::vector<double> features;  // high precision

    // Reserve memory upfront:
    std::cout << "\nReserving memory for 100 features:" << std::endl;
    features.reserve(100);
    std::cout << "Memory reserved for: " << features.capacity() << " elements" << std::endl;


    // Fill vector with normalized values:
    for(int i = 0; i < 100; i++)
    {
        double normalized_value = static_cast<double>(i) / 99.0;

        features.push_back(normalized_value);

        if (i < 5)
        {
            std::cout << "Feature " << i << ": " << std::fixed << normalized_value << std::endl;
        }
    }
    std::cout << "... (continuing to fill " << 100 - 5 << " more features)" << std::endl;


    // Display first 10 normalized features:
    std::cout << "\nFirst 10 normalized features:" << std::endl;
    for(int i = 0; i < 10; i++)
    {
        std::cout << features[i] << " ";
    }
    std::cout << "\nVector size: " << features.size() << ", Capacity: " << features.capacity() << std::endl;



    // Example 1: Loading time series data (stock prices simulation):
    std::cout << "\nExample 1 -> Loading financial time series data:" << std::endl;
    std::vector<double> stock_prices;
    stock_prices.reserve(365); // One year of daily prices

    for(int day = 0; day < 365; day++)
    {
        double price = 100.0 + sin(day * 0.017) * 10.0; // Simulated price movement

        stock_prices.push_back(price);
    }
    std::cout << "Loaded " << stock_prices.size() << " days of stock data" << std::endl;



    // Example 2: Loading text sequence data (word indices):
    std::cout << "\nExample 2 -> Loading text sequence data for NLP:" << std::endl;
    std::vector<int> word_indices = {5, 12, 8, 15, 23, 4, 19, 7}; // Word indices in vocabulary
    std::cout << "Text sequence length: " << word_indices.size() << std::endl;



    // Example 3: Loading image batch data:
    std::cout << "\nExample 3 -> Loading batch of images:" << std::endl;

    std::vector<std::vector<double>> image_batch;  // matrix
    const int BATCH_SIZE = 32;
    const int IMAGE_SIZE = 784; // 28 x 28 flattened (grayscale, one channel)
    image_batch.reserve(BATCH_SIZE);

    for (int i = 0; i < BATCH_SIZE; i++)
    {
        std::vector<double> single_image;   // vector
        single_image.reserve(IMAGE_SIZE);
        
        for (int pixel = 0; pixel < IMAGE_SIZE; pixel++)
        {
            // Convert pixel values to doubles for usage in a NN on a GPU.
            single_image.push_back(static_cast<double>(rand()) / RAND_MAX); // Random pixel values
        }
        image_batch.push_back(single_image);  // Add the image vector to the image batch
    }
    std::cout << "Loaded batch of " << image_batch.size() << " images" << std::endl;

    return 0;
}
