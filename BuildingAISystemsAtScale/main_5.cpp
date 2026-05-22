#include <iostream>

// Using Declarations: Import only the specific items you need (e.g., using std::cout;). This reduces repetition without polluting the entire scope.
// Local Directives: If you must use using namespace, place it inside a specific function rather than at the top of the file to limit its impact.
// Namespace Aliases: For long custom namespaces, you can create a shorter alias (e.g., namespace ml = MyCompany::MyProject::MyModule;

using std::cout, std::endl;

int main()
{
    cout << "Hello AI Engineers!" << endl;

    const int ARRAY_SIZE = 10;

    // Initialize entire array to zero at compile time.
    int staticArray[ARRAY_SIZE] = {0};

// -------------------------------------------------------

    // Display zero-initialized array
    std::cout << "Zero-initialized array: ";
    for(int i = 0; i < ARRAY_SIZE; i++)
    {
        std::cout << staticArray[i] << " ";
    }
    std::cout << std::endl;

// -------------------------------------------------------

    // Index-based Array Access
    std::cout << "Array before transformations: ";
    for(int i = 0; i < ARRAY_SIZE; i++)
    {
        std::cout << staticArray[i] << " ";
    }
    std::cout << std::endl;

    // Fill array with squares
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        staticArray[i] = i * i;
    }
    std::cout << "Array after transformations: ";
    for (int i = 0; i < ARRAY_SIZE; i++)
        std::cout << staticArray[i] << " ";
    std::cout << std::endl;

    // Find maximum value and index:
    int max_val = staticArray[0];
    int max_index = 0;

    for (int i = 1; i < ARRAY_SIZE; i++) {
        if (staticArray[i] > max_val) {
            max_val = staticArray[i];
            max_index = i;
        }
    }

    // Display the result
    std::cout << "Maximum value: " << max_val << " at index: " << max_index << std::endl;

    return 0;
}
