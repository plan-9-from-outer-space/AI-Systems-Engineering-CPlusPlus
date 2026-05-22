#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>
#include <cctype>
#include <limits>

class FileHandler {

public:

    // FileHandler::readFileToVector ("sample_data.csv", fileContent)) { ... }

    static bool readFileToVector (const std::string& filename, std::vector<std::string>& lines) {
        std::ifstream file (filename);

        if (! file.is_open()) {
            std::cerr << "Error: Cannot open file " << filename << std::endl;
            return false;
        }

        lines.clear();  // Causes a warning.

        std::string line;
        while (std::getline(file, line)) {
            lines.push_back(line);
        }

        file.close();
        return true;
    }

    static bool writeVectorToFile (const std::string& filename, const std::vector<std::string>& lines) {
        std::ofstream file (filename);

        if (! file.is_open()) {
            std::cerr << "Error: Cannot create file " << filename << std::endl;
            return false;
        }

        // In C++, auto& tells the compiler to automatically deduce the data type of a variable 
        //   and bind it as a reference (an alias) to the original object, rather than creating a copy.
        // When you don't intend to change the data: If you just want to read the values without changing them, 
        //   use const auto&. This guarantees read-only safety while still avoiding expensive copies.
        for (const auto& line : lines) {
            file << line << std::endl;
        }

        file.close();
        return true;
    }
};

class CSVParser {

private:

    char delimiter;  // comma, tab, etc

public:

    // Class Constructor
    CSVParser (char delim = ',') : delimiter(delim) {}

    std::vector<std::string> parseLine (const std::string& line) {

        std::vector<std::string> tokens;  // vector of strings
        std::stringstream ss(line);
        std::string token;

        while (std::getline(ss, token, delimiter)) {

            // Trim leading whitespace
            token.erase(token.begin(), std::find_if(token.begin(), token.end(), [](unsigned char ch) { return !std::isspace(ch); }));

            // Trim trailing whitespace
            token.erase(std::find_if(token.rbegin(), token.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), token.end());

            tokens.push_back(token);
        }

        return tokens;
    }

    std::vector<std::string> parseQuotedLine (const std::string& line) {

        std::vector<std::string> tokens;
        std::string token;
        bool inQuotes = false;  // Keep track of whether we're inside quotes

        // Char by char parsing
        for (size_t i = 0; i < line.length(); ++i) {
            char c = line[i];

            if (c == '"') {
                inQuotes = !inQuotes;
            } else if (c == delimiter && !inQuotes) {
                tokens.push_back(token);
                token.clear();
            } else {
                token += c;
            }
        }

        if (! token.empty()) {
            tokens.push_back(token);
        }

        return tokens;
    }
};

struct Dataset {

    std::vector<std::vector<double>> features;
    std::vector<double> labels;
    std::vector<std::string> featureNames;

    size_t numSamples;
    size_t numFeatures;

    // Constructor
    Dataset() : numSamples(0), numFeatures(0) {}
};

class DataLoader {

private:

    CSVParser parser;

public:

    std::unique_ptr<Dataset> loadCSV (const std::string& filename, bool hasHeader = true, int labelColumn = -1) {

        auto dataset = std::make_unique<Dataset>();
        std::vector<std::string> lines;

        if (! FileHandler::readFileToVector (filename, lines)) {
            return nullptr;
        }

        if (lines.empty()) {
            std::cerr << "❌ Error: Empty file" << std::endl;
            return nullptr;
        }

        size_t startRow = hasHeader ? 1 : 0;

        // Parse header if present
        if (hasHeader) {
            auto headerTokens = parser.parseLine(lines[0]);
            dataset->featureNames = headerTokens;

            if (labelColumn >= 0 && static_cast<size_t>(labelColumn) < headerTokens.size()) {
                dataset->featureNames.erase(dataset->featureNames.begin() + labelColumn);
            }
        }

        // Parse data sample rows
        for (size_t i = startRow; i < lines.size(); ++i) {

            auto tokens = parser.parseLine(lines[i]);

            if (tokens.empty()) continue;

            std::vector<double> featureRow;

            for (size_t j = 0; j < tokens.size(); ++j) {

                if (static_cast<int>(j) == labelColumn) {
                    try {
                        double label = std::stod(tokens[j]);
                        dataset->labels.push_back(label);
                    } catch (const std::exception& e) {
                        std::cerr << "⚠️ Warning: Invalid label at row " << i << ": " << tokens[j] << std::endl;
                        dataset->labels.push_back(0.0); // Default value
                    }
                } else {
                    try {
                        double feature = std::stod(tokens[j]);
                        featureRow.push_back(feature);
                    } catch (const std::exception& e) {
                        std::cerr << "⚠️ Warning: Invalid feature at row " << i << ", col " << j << ": " << tokens[j] << std::endl;
                        featureRow.push_back(0.0); // Default value
                    }
                }
            }

            if (! featureRow.empty()) {
                dataset->features.push_back(featureRow);
            }
        }

        dataset->numSamples = dataset->features.size();
        dataset->numFeatures = dataset->features.empty() ? 0 : dataset->features[0].size();

        return dataset;
    }

    bool saveDataset(const std::string& filename, const Dataset& dataset, bool includeHeader = true) {

        std::vector<std::string> lines;

        if (includeHeader) {
            std::string header;
            for (size_t i = 0; i < dataset.featureNames.size(); ++i) {
                if (i > 0) header += ",";
                header += dataset.featureNames[i];
            }
            header += ",label";
            lines.push_back(header);
        }

        for (size_t i = 0; i < dataset.numSamples; ++i) {
            std::string line;
            for (size_t j = 0; j < dataset.numFeatures; ++j) {
                if (j > 0) line += ",";
                line += std::to_string(dataset.features[i][j]);
            }
            line += "," + std::to_string(dataset.labels[i]);
            lines.push_back(line);
        }

        return FileHandler::writeVectorToFile (filename, lines);
    }
};

class DataPreprocessor {

public:

    // Zero mean, unit variance
    static void normalizeFeatures(Dataset& dataset) {

        if (dataset.numFeatures == 0 || dataset.numSamples == 0) return;

        std::vector<double> means (dataset.numFeatures, 0.0);
        std::vector<double> stddevs (dataset.numFeatures, 0.0);

        // Calculate mean of each feature
        for (size_t j = 0; j < dataset.numFeatures; ++j) {
            double sum = 0.0;
            for (size_t i = 0; i < dataset.numSamples; ++i) {
                sum += dataset.features[i][j];
            }
            means[j] = sum / dataset.numSamples;
        }

        // Calculate standard deviation of each feature
        for (size_t j = 0; j < dataset.numFeatures; ++j) {
            double sumSquares = 0.0;
            for (size_t i = 0; i < dataset.numSamples; ++i) {
                double diff = dataset.features[i][j] - means[j];
                sumSquares += diff * diff;
            }
            stddevs[j] = sqrt(sumSquares / dataset.numSamples);
        }

        // Calculate z-score normalization for each feature
        for (size_t j = 0; j < dataset.numFeatures; ++j) {

            if (stddevs[j] > 1e-10) { // Avoid division by zero
                for (size_t i = 0; i < dataset.numSamples; ++i) {
                    dataset.features[i][j] = (dataset.features[i][j] - means[j]) / stddevs[j];
                }
            }
        }
    }

    static void handleMissingValues(Dataset& dataset, double defaultValue = 0.0) {

        // features
        for (auto& sample : dataset.features) {
            for (double& feature : sample) {
                if (std::isnan(feature) || std::isinf(feature)) {
                    feature = defaultValue;
                }
            }
        }

        // labels
        for (double& label : dataset.labels) {
            if (std::isnan(label) || std::isinf(label)) {
                label = defaultValue;
            }
        }
    }
};

void createSampleCSV() {

    std::vector<std::string> lines = {
        "temperature_c,vibration_mm_s,pressure_kpa,cycle_time_ms,anomaly_label", // header row
        "68.5,1.80,101.2,320.0,0",
        "72.1,2.40,104.8,334.0,0",
        "78.9,3.10,110.5,348.0,0",
        "84.2,4.80,116.9,372.0,1",
        "88.7,6.10,121.4,395.0,1",
        "92.3,7.40,126.8,410.0,1"
    };

    FileHandler::writeVectorToFile ("sample_data.csv", lines);
}

int main()
{
    std::cout << "\nSensor Export Ingestion" << std::endl;

    std::cout << "\nStep 1: Creating staged sensor export..." << std::endl;

    createSampleCSV();

    std::cout << "\nStep 2: Loading sensor export into memory..." << std::endl;

    std::vector<std::string> fileContent;

    // Create an actual data structure so the 'clear()' function does not issue a compiler warning.
    fileContent.reserve(1);

    if (FileHandler::readFileToVector ("sample_data.csv", fileContent)) {

        std::cout << "   File loaded: " << fileContent.size() << " lines read" << std::endl;
        std::cout << "\nStep 3: Export statistics:" << std::endl;

        size_t totalLines = fileContent.size();
        size_t headerLines = (totalLines > 0) ? 1 : 0;
        size_t dataLines = (totalLines > headerLines) ? (totalLines - headerLines) : 0;

        std::cout << "   Total lines: " << totalLines << std::endl;
        std::cout << "   Header lines: " << headerLines << std::endl;
        std::cout << "   Data rows: " << dataLines << std::endl;

        std::cout << "\nStep 4: Raw export preview:" << std::endl;

        for (size_t i = 0; i < fileContent.size(); ++i) {
            std::cout << std::setw(3) << i << " | " << fileContent[i] << std::endl;
        }

        std::cout << "\nStep 5: Schema health check:" << std::endl;

        if (fileContent.size() > 0) {  // totalLines > 0

            std::string expectedHeader = "temperature_c,vibration_mm_s,pressure_kpa,cycle_time_ms,anomaly_label";
            std::string header = fileContent[0];

            bool headerMatches = (header == expectedHeader);

            size_t fieldCount = std::count (header.begin(), header.end(), ',') + 1;

            std::cout << "   Header matches expected sensor schema: " << (headerMatches ? "yes" : "no") << std::endl;
            std::cout << "   Number of fields: " << fieldCount << std::endl;
            std::cout << "   File content size: " << fileContent.size() << std::endl;

            bool structureValid = true;

            for (size_t i = 0; i < fileContent.size(); ++i) {

                size_t rowFields = std::count (fileContent[i].begin(), fileContent[i].end(), ',') + 1;

                if (rowFields != fieldCount) {
                    std::cout << "   Warning: Line " << i << " has " << rowFields << " fields (expected " << fieldCount << ")" << std::endl;
                    structureValid = false;
                }
            }

            std::cout << "   Schema ready: " << (structureValid ? "yes" : "no") << std::endl;

            bool stagingReady = headerMatches && structureValid && dataLines >= 4;

            std::cout << "   Staging ready: " << (stagingReady ? "yes" : "no") << std::endl;

        }
    } else {
        std::cout << "Failed to read file: sample_data.csv" << std::endl;
    }

    return 0;
}
