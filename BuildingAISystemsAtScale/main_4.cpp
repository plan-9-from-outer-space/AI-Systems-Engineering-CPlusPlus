#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <cmath>

int main()
{
    std::cout << "\nSCENARIO 3: DATASET LOADING" << std::endl;

    std::vector<std::string> csv_row = {"3.14159", "85.5", "NaN", "invalid42", "0.95"};

    std::cout << "Raw CSV row values: ";

    for (const auto& value : csv_row) {
        std::cout << "'" << value << "' ";
    }
    std::cout << std::endl;

    std::vector<float> parsed_values;
    parsed_values.reserve(csv_row.size());

    std::size_t total_cells = csv_row.size();
    std::size_t parsed_ok = 0;
    std::size_t fallback_non_finite = 0;
    std::size_t fallback_exception = 0;

    for (std::size_t col = 0; col < csv_row.size(); ++col) {
        const auto& value = csv_row[col];

        // Another option is to just skip bad values, instead of using zero.
        try {
            float numeric = std::stof(value);
            if (std::isfinite(numeric)) {
                parsed_values.push_back(numeric);
                ++parsed_ok;
            }  else {
                float fallback = 0.0f;
                parsed_values.push_back(fallback);
                ++fallback_non_finite;
                std::cout << "Non-finite value in column " << col << " '" << value << "' converted to fallback " << fallback << std::endl;
            }
        } catch (const std::exception& e) {
            float fallback = 0.0f;
            parsed_values.push_back(fallback);
            ++fallback_exception;
            std::cout << "Conversion failed in column " << col << " for '" << value << "': " << e.what()
                      << " -> using fallback " << fallback << std::endl;
        }
    }

    std::cout << "\nRow parse summary: "
              << parsed_ok << " ok, "
              << fallback_non_finite << " non-finite mapped to fallback, "
              << fallback_exception << " hard failures mapped to fallback, out of "
              << total_cells << " raw cells." << std::endl;

    std::cout << "Parsed numeric values (Float): ";

    for(float v : parsed_values) {
        std::cout << v << "f ";
    }
    std::cout << std::endl;

    return 0;
}
