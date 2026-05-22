#include <iostream>
#include <memory>
#include <vector>
#include <string>

class SmartPointerDemo {

public:

    static void demonstrateUniquePtr() {

        std::unique_ptr<std::vector<double>> data = std::make_unique<std::vector<double>>();
        data->resize(1000, 1.5);

        std::cout << "Created unique_ptr with " << data->size() << " elements" << std::endl;

        auto data2 = std::move(data);

        std::cout << "Ownership transferred to data2" << std::endl;
        std::cout << "data2 is now the owner: " << data->size() << " elements" << std::endl;

        if (! data) {
            std::cout << "data is now null, ownership was transferred" << std::endl;
        }

        // When data2 goes out of scope, the vector is automatically deleted
        std::cout << "Memory will be automatically cleaned up when data2 goes out of scope" << std::endl;
    }

    static void demonstrateSharedPtr() {

        std::shared_ptr<std::vector<double>> data = std::make_shared<std::vector<double>>(100, 2.5);

        std::cout << "Shared pointer created, use count: " << data.use_count() << std::endl;

        auto data_copy = data;
        std::cout << "After copy, use count: " << data.use_count() << std::endl;

        std::weak_ptr<std::vector<double>> weak_data = data;
        std::cout << "Created weak_ptr, use count remains the same: " << data.use_count() << std::endl;

        // Check if weak pointer is still valid (resource hasn't been deleted)
        if (auto locked = weak_data.lock()) {
            std::cout << "Weak pointer is valid, size: " << locked->size() << std::endl;
        }

        // When data and data_copy go out of scope, reference count becomes zero and the vector is automatically deleted
        std::cout << "Memory will be cleaned up when all shared_ptr instances are destroyed" << std::endl;

        // Reset one shared pointer
        // data_copy.reset();
        // std::cout << "After reset, use count: " << data.use_count() << std::endl;
    }
};

static void demonstrateRAII() {

    class ResourceManager {

    private:

        std::vector<double> data;

    public:

        ResourceManager(size_t size) : data(size, 0.0) { }

        ~ResourceManager() { }  // RAII performs automatic cleanup

        void process() {
            for (auto& val : data) {
                val += 1.0;
            }
        }
    };

    {
        ResourceManager manager(1000);
        manager.process();
    }

    // RAII: Resource automatically freed here when 'ResourceManager' goes out of scope

}

class MemoryPool {

private:

    std::vector<std::unique_ptr<std::vector<double>>> pool;
    size_t chunk_size;

public:

    MemoryPool (size_t pool_size, size_t chunk_sz) : chunk_size(chunk_sz) {
        // This is expensive, but it is only called once at startup
        pool.reserve(pool_size);
        for (size_t i = 0; i < pool_size; ++i) {
            pool.push_back(std::make_unique<std::vector<double>>(chunk_size));
        }
    }

    std::unique_ptr<std::vector<double>> acquire() {
        if (! pool.empty()) {
            auto ptr = std::move(pool.back());
            pool.pop_back();
            return ptr;
        }
        // Fallback to new allocation (if pool is empty)
        return std::make_unique<std::vector<double>>(chunk_size);
    }

    void release (std::unique_ptr<std::vector<double>> ptr) {
        if (ptr && ptr->size() == chunk_size) {
            ptr->clear(); // Reset the vector
            pool.push_back(std::move(ptr)); // move it back to the pool
        }
    }

    size_t available() const { return pool.size(); }

    void demonstrateMemoryPool() {
        std::cout << "Initial pool size: " << available() << std::endl;

        // Acquire some vectors
        auto vec1 = acquire();
        auto vec2 = acquire();

        // Use the vectors
        vec1->resize(50, 1.0);
        vec2->resize(30, 2.0);

        // Return them to pool
        release(std::move(vec1));
        release(std::move(vec2));
    }
};

void demonstrateRawPointerProblems () {

    // Problem 1: Memory leak - forgot to deallocate
    double* leaky_ptr = new double[1000];
    // Forgot to delete[] leaky_ptr;

    // Problem 2: Dangling pointer - accessing deleted memory
    double* dangling_ptr = new double(42.0);
    double* another_ptr = dangling_ptr;
    delete dangling_ptr;
    // another_ptr now points to freed memory!

    // Problem 3: Double deletion - undefined behavior
    double* double_delete_ptr = new double(99.0);
    delete double_delete_ptr;
    // delete double_delete_ptr; // This would crash!

}

class AIDataManager {

private:

    std::vector<std::shared_ptr<std::vector<double>>> datasets;
    std::vector<std::unique_ptr<std::string>> dataset_names;

public:

    void addDataset (const std::string& name, const std::vector<double>& data) {

        // Create shared pointer for the dataset (can be shared across models)
        auto dataset_ptr = std::make_shared<std::vector<double>>(data);

        // Create unique pointer for the name (owned exclusively)
        auto name_ptr = std::make_unique<std::string>(name);

        datasets.push_back(dataset_ptr);
        dataset_names.push_back(std::move(name_ptr));
    }

    void trainModel (size_t dataset_index) {

        if (dataset_index >= datasets.size()) {
            // Prevent a crash
            return;
        }

        auto dataset = datasets[dataset_index];
        auto name = *dataset_names[dataset_index];

        // Simulate training (just calculate mean)
        double sum = 0.0;
        for (double val : *dataset) {
            sum += val;
        }

        double mean = sum / dataset->size();
        std::cout << "Training complete. Dataset mean: " << mean << std::endl;
    }

    void demonstrateSharedOwnership() {

        if (! datasets.empty()) {

            auto dataset_ref = datasets[0];
            std::cout << "Dataset use count: " << dataset_ref.use_count() << std::endl;

            // Create another reference (like passing to another function)
            auto another_ref = dataset_ref;
            std::cout << "After creating another reference: " << dataset_ref.use_count() << std::endl;

            // Simulate passing to a model
            auto model_ref = dataset_ref;
            std::cout << "After passing to model: " << dataset_ref.use_count() << std::endl;
        }
    }

    size_t getDatasetCount() const {
        return datasets.size();
    }
};

class NeuralNetworkLayer {

private:

    std::string layer_name;
    std::unique_ptr<std::vector<double>> weights;
    std::unique_ptr<std::vector<double>> biases;
    size_t input_size;
    size_t output_size;

public:

    // Constructor - Resource Acquisition (RAII)
    NeuralNetworkLayer (const std::string& name, size_t in_size, size_t out_size)
        : layer_name(name), input_size(in_size), output_size(out_size) {

        // Allocate resources safely in constructor
        weights = std::make_unique<std::vector<double>>(in_size * out_size, 0.1);
        biases  = std::make_unique<std::vector<double>>(out_size, 0.0);
    }

    // Destructor - Resource Cleanup (automatic!)
    ~NeuralNetworkLayer() { }  // RAII

    // Forward Pass
    std::vector<double> forward(const std::vector<double>& input) {

        if (input.size() != input_size) {
            throw std::invalid_argument("Input size mismatch");
        }

        std::vector<double> output(output_size, 0.0);

        // Simple linear transformation: output = weights * input + bias
        for (size_t out_idx = 0; out_idx < output_size; ++out_idx) {
            for (size_t in_idx = 0; in_idx < input_size; ++in_idx) {
                size_t weight_idx = out_idx * input_size + in_idx;
                output[out_idx] += (*weights)[weight_idx] * input[in_idx];
            }
            output[out_idx] += (*biases)[out_idx];
        }

        return output;
    }

    void printInfo() const {
        std::cout << "Layer '" << layer_name << "': " << input_size << " -> " << output_size << std::endl;
    }
};


int main() {

    std::cout << "\nPart 1: Manage Exclusive Ownership" << std::endl;

    const size_t HEAD_ROWS = 96;
    const size_t HEAD_COLS = 128;
    const double HEAD_LEARNING_RATE = 0.0005;

    std::cout << "Scenario: handing a wildfire dispatch scoring head from model loader to live scorer" << std::endl;

    double rawHeadChecksum = 0.0;

    std::cout << "\n  Raw pointer ownership path" << std::endl;

    try {
        auto *raw_weights = new std::vector<std::vector<double>>();
        raw_weights->resize(HEAD_ROWS);

        for (auto &row : *raw_weights) {
            row.resize(HEAD_COLS, 0.05);
        }

        std::cout << "   Loaded raw head with " << HEAD_ROWS * HEAD_COLS << " weights" << std::endl;

        for (size_t row_idx = 0; row_idx < 12; ++row_idx) {
            for (size_t col_idx = 0; col_idx < 24; ++col_idx) {
                (*raw_weights)[row_idx][col_idx] += HEAD_LEARNING_RATE * static_cast<double>((row_idx + col_idx) % 7 + 1);
                rawHeadChecksum += (*raw_weights)[row_idx][col_idx];
            }
        }
        std::cout << "   Raw head checksum after update: " << rawHeadChecksum << std::endl;

        if (rawHeadChecksum > 0.0) {
            throw std::runtime_error("dispatch scorer restarted before manual cleanup of raw head");
        }

        delete raw_weights;

    } catch (const std::exception &ex) {
        std::cout << "   Failure: " << ex.what() << std::endl;
        std::cout << "   Raw head leaked after interruption" << std::endl;
    }

    double safeHeadChecksum = 0.0;

    std::cout << "\n unique_ptr ownership path" << std::endl;

    std::unique_ptr<std::vector<std::vector<double>>> smart_weights = std::make_unique<std::vector<std::vector<double>>>();
    smart_weights->resize(HEAD_ROWS);

    for (auto &row : *smart_weights) {
        row.resize(HEAD_COLS, 0.05);
    }

    std::cout << "   Loaded smart head with " << HEAD_ROWS * HEAD_COLS << " weights" << std::endl;

    auto trainer_weights = std::move(smart_weights);

    std::cout << "    Ownership transferred to scorer safely" << std::endl;

    if (! smart_weights) {
        std::cout << "   Source handle is now empty after move" << std::endl;
    }

    try {
        for (size_t row_idx = 0; row_idx < 12; ++row_idx) {
            for (size_t col_idx = 0; col_idx < 24; ++col_idx) {
                (*trainer_weights)[row_idx][col_idx] += HEAD_LEARNING_RATE * static_cast<double>((row_idx + col_idx) % 7 + 1);
                safeHeadChecksum += (*trainer_weights)[row_idx][col_idx];
            }
        }

        std::cout << "   Smart head checksum after update: " << safeHeadChecksum << std::endl;

        if (safeHeadChecksum > 0.0) {
            throw std::runtime_error("dispatch scorer restarted but unique_ptr will clean the head");
        }

    } catch (const std::exception &ex) {
        std::cout << "   Failure: " << ex.what() << std::endl;
        std::cout << "   unique_ptr keeps the head on a safe cleanup path" << std::endl;
    }

    std::cout << "Raw dispatch-head checksum: " << rawHeadChecksum << std::endl;
    std::cout << "Smart dispatch-head checksum: " << safeHeadChecksum << std::endl;

    return 0;
}
