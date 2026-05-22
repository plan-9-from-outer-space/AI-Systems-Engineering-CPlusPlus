#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <cmath>

int main()
{
   std::cout << "\nScenario 1: Probability to Class Label" << std::endl;
   
   std::vector<double> probabilities = {0.87, 0.42, 0.63, 0.18, 0.48, 0.52};

   double default_threshold = 0.5;
   double fraud_threshold = 0.3;
   double strict_threshold = 0.7;

   std::cout << "Probabilities: ";

   for (double p : probabilities) {
       std::cout << p << " ";
   }
   std::cout << std::endl;

   std::cout << "Predicted classes (threshold = " << default_threshold << "): ";

   for (double p : probabilities) {
       int label = static_cast<int>(p > default_threshold);
       std::cout << label << " ";
   }
   std::cout << std::endl;

   std::cout << "Predicted classes (fraud threshold = " << fraud_threshold << "): ";

   for (double p : probabilities) {
       int label = static_cast<int>(p > fraud_threshold);
       std::cout << label << " ";
   }
   std::cout << std::endl;

   std::cout << "Predicted classes (strict threshold = " << strict_threshold << "): ";

   for (double p : probabilities) {
       int label = static_cast<int>(p > strict_threshold);

       std::cout << label << " ";
   }
   std::cout << std::endl;

   int count_default = 0, count_fraud = 0, count_strict = 0;

   for (double p : probabilities) {
       if (p > default_threshold) ++count_default;
       if (p > fraud_threshold) ++count_fraud;
       if (p > strict_threshold) ++count_strict;
   }
   std::cout << "Summary: default (0.5) -> " << count_default << " positive(s); fraud (0.3) -> " << count_fraud
             << " positive(s); strict (0.7) -> " << count_strict << " positive(s)" << std::endl;
    return 0;
}
