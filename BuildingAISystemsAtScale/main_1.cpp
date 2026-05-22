#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <cmath>

int main()
{
    // Type conversion demonstration
    std::cout << "\nTYPE CONVERSION DEMONSTRATION:" << std::endl;
    
    double precise_value = 3.141592653589793238462643383279; // Pi with maximum precision

    std::cout << "Original double value (high precision): " << std::fixed << std::setprecision(15) << precise_value << std::endl;

    // Float conversion
    std::cout << "\n\nConverting to float (GPU training scenario):" << std::endl;

    float less_precise = static_cast<float>(precise_value);

    std::cout << "Float32 result: " << std::fixed << std::setprecision(10) << less_precise << std::endl;
    
    std::cout << "Precision loss: " << std::scientific << std::setprecision(2) << (precise_value - less_precise) << std::endl;
    
    // Integer conversion (classification)
    std::cout << "\n\nConverting to int (classification scenario):" << std::endl;

    int integer_version = static_cast<int>(precise_value);

    std::cout << "Integer result: " << integer_version << std::endl;

    std::cout << "Truncated portion: " << std::fixed << std::setprecision(6) << (precise_value - integer_version) << std::endl;
    
    // Display conversion comparison
    std::cout << "\n\nCOMPARISON OF ALL CONVERSIONS:" << std::endl;

    std::cout << "Original double: " << std::fixed << std::setprecision(15) << precise_value << std::endl;

    std::cout << "Converted to float: " << std::fixed << std::setprecision(7) << less_precise << std::endl;
    
    std::cout << "Converted to int: " << integer_version << std::endl;
    
    return 0;
}
