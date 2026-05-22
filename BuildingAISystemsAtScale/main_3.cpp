#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <cmath>

int main()
{
   std::cout << "\nScenario 2: Loss Value Conversion" << std::endl;
   
   std::vector<double> epoch_losses = {
       0.845123456789012,
       0.432987654321098,
       0.210456789012345,
       0.105234567890123
   };
   
   std::cout << "Epoch losses (double64, full precision):" << std::endl;
   
   for (size_t epoch = 0; epoch < epoch_losses.size(); ++epoch) {
       std::cout << "  Epoch " << epoch + 1 << ": "
                 << std::fixed << std::setprecision(15) << epoch_losses[epoch] << std::endl;
   }

   std::vector<float> logged_epoch_losses;

   logged_epoch_losses.reserve(epoch_losses.size()); // Reserve memory upfront

   for (double loss : epoch_losses) {
       float logged = static_cast<float>(loss);
       logged_epoch_losses.push_back(logged);
   }

   std::cout << "\nEpoch losses (float32, logging view):" << std::endl;

   for (size_t epoch = 0; epoch < logged_epoch_losses.size(); ++epoch) {
       std::cout << "  Epoch " << epoch + 1 << ": "
                 << std::setprecision(7) << logged_epoch_losses[epoch] << std::endl;
   }

   std::cout << "\nPer-epoch rounding error (|Double64 - Float32|):" << std::endl;

   double max_rounding_error = 0.0;

   for (size_t epoch = 0; epoch < epoch_losses.size(); ++epoch) {

       double err = std::fabs(static_cast<double>(logged_epoch_losses[epoch]) - epoch_losses[epoch]);

       if (err > max_rounding_error) max_rounding_error = err;

       std::cout << "  Epoch " << epoch + 1 << ": " << std::scientific << std::setprecision(4) << err << std::endl;
   }
   std::cout << "Max rounding error over epochs: " << std::scientific << std::setprecision(4) << max_rounding_error << std::endl;
  
   double final_loss_double = epoch_losses.back();
   float final_loss_float = logged_epoch_losses.back();

   std::cout << "\nFinal loss comparison:" << std::endl;

   std::cout << std::fixed;

   std::cout << "  Double64: " << std::setprecision(15) << final_loss_double << std::endl;
   std::cout << "  Float32:  " << std::setprecision(7) << final_loss_float << std::endl;
   
   return 0;
}
