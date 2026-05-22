#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath>
#include <algorithm>

int main()
{
    std::cout << "\nSTEP 1: Sensor Data Dimensions (Integer Types at Scale)" << std::endl;

    int num_cameras = 8;
    int frame_width = 1280;
    int frame_height = 960;
    short num_sensors = 10;

    size_t pixels_per_frame = static_cast<size_t>(frame_width) * static_cast<size_t>(frame_height);
    size_t total_pixels_all_cameras = pixels_per_frame * static_cast<size_t>(num_cameras);

    std::cout << "  Cameras (int):         " << num_cameras << std::endl;
    std::cout << "  Resolution (int):      " << frame_width << " × " << frame_height << std::endl;
    std::cout << "  Sensors (short):       " << num_sensors << " (2 bytes each—saves memory in fleet!)" << std::endl;
    std::cout << "  Pixels/frame (size_t): " << pixels_per_frame << std::endl;
    std::cout << "  Total pixels (size_t): " << total_pixels_all_cameras << std::endl;

    size_t bytes_float = total_pixels_all_cameras * sizeof(float);
    size_t bytes_double = total_pixels_all_cameras * sizeof(double);

    std::cout << "\nMemory per frame (all cameras):" << std::endl;
    std::cout << "     Float32:  " << (bytes_float / 1024) << " KB" << std::endl;
    std::cout << "     Double64: " << (bytes_double / 1024) << " KB" << std::endl;
    std::cout << "     Savings:  " << ((bytes_double - bytes_float) / 1024) << " KB with Float32!" << std::endl;

    size_t bytes_float_per_second = bytes_float * 36;
    size_t bytes_double_per_second = bytes_double * 36;

    std::cout << "\nMemory bandwidth per second (all cameras @ 36 FPS):" << std::endl;
    std::cout << "     Float32:  " << (bytes_float_per_second / (1024 * 1024)) << " MB/s" << std::endl;
    std::cout << "     Double64: " << (bytes_double_per_second / (1024 * 1024)) << " MB/s" << std::endl;
    std::cout << "     Savings:  " << ((bytes_double_per_second - bytes_float_per_second) / (1024 * 1024)) << " MB/s with Float32!" << std::endl;

    long long fleet_cars = 100000;
    long long fleet_float_MB_per_second = static_cast<long long>(bytes_float_per_second / (1024 * 1024)) * fleet_cars;
    long long fleet_double_MB_per_second = static_cast<long long>(bytes_double_per_second / (1024 * 1024)) * fleet_cars;

    std::cout << "\nApproximate fleet bandwidth (" << fleet_cars << " cars):" << std::endl;
    std::cout << "     Float32:  " << fleet_float_MB_per_second << " MB/s across fleet" << std::endl;
    std::cout << "     Double64: " << fleet_double_MB_per_second << " MB/s across fleet" << std::endl;
    std::cout << "     Savings:  " << (fleet_double_MB_per_second - fleet_float_MB_per_second) << " MB/s saved fleet‑wide by using Float32." << std::endl;


    std::cout << "\nSTEP 2: LiDAR Point Cloud (Float Storage)" << std::endl;

    size_t num_points = 1000;

    std::vector<float> lidar_x(num_points);
    std::vector<float> lidar_y(num_points);
    std::vector<float> lidar_z(num_points);

    for(size_t i = 0; i < num_points; i++) {
        lidar_x[i] = static_cast<float>((i % 100) + 1) * 0.5f;
        lidar_y[i] = static_cast<float>((i % 80) + 1) * 0.5f;
        lidar_z[i] = static_cast<float>((i % 50) + 1) * 0.3f;
    }

    std::cout << "  LIDAR points (size_t): " << num_points << std::endl;
    std::cout << "  Storage: 3 x " << num_points << " x 4 bytes = " << (3 * num_points * sizeof(float)) << " bytes" << std::endl;
    std::cout << "  Sample point [0]: (" << lidar_x[0] << ", " << lidar_y[0] << ", " << lidar_z[0] << ") m" << std::endl;


    size_t real_points_per_scan = 300000;
    size_t coords_per_point = 3;
    size_t floats_per_scan = real_points_per_scan * coords_per_point;
    size_t bytes_per_scan_float = floats_per_scan * sizeof(float);
    size_t bytes_per_scan_double = floats_per_scan * sizeof(double);

    std::cout << "\nLIDAR memory per scan (realistic 300K point cloud): " << std::endl;
    std::cout << "  Float:  " << (bytes_per_scan_float / (1024 * 1024)) << " MB" << std::endl;
    std::cout << "  Double: " << (bytes_per_scan_double / (1024 * 1024)) << " MB" << std::endl;
    std::cout << "  Savings: " << ((bytes_per_scan_double - bytes_per_scan_float) / (1024 * 1024))
              << " MB saved per scan with Float" << std::endl;


    int scans_per_second = 10;

    size_t bytes_float_per_scan_lidar = bytes_per_scan_float * static_cast<size_t>(scans_per_second);
    size_t bytes_double_per_scan_lidar = bytes_per_scan_double * static_cast<size_t>(scans_per_second);

    std::cout << "\nLIDAR bandwidth (" << scans_per_second << " scans/sec):" << std::endl;
    std::cout << "  Float:  " << (bytes_float_per_scan_lidar / (1024 * 1024)) << " MB/s" << std::endl;
    std::cout << "  Double:  " << (bytes_double_per_scan_lidar / (1024 * 1024)) << " MB/s" << std::endl;
    std::cout << "  Savings:  " << ((bytes_double_per_scan_lidar - bytes_float_per_scan_lidar) / (1024 * 1024))
                 << " MB/s saved by using Float" << std::endl;


    std::cout << "\nSTEP 3: Distance Calculation (Double for Precision)" << std::endl;

    double sum_sq = 0.0;

    sum_sq += static_cast<double>(lidar_x[0]) * static_cast<double>(lidar_x[0]);
    sum_sq += static_cast<double>(lidar_y[0]) * static_cast<double>(lidar_y[0]);
    sum_sq += static_cast<double>(lidar_z[0]) * static_cast<double>(lidar_z[0]);

    double distance_double = std::sqrt(sum_sq);

    float distance_float = static_cast<float>(distance_double);

    std::cout << "    Point [0] distance (double): " << std::setprecision(2) << std::fixed << distance_double << " m" << std::endl;
    std::cout << "    Stored as float:             " << distance_float << " m" << std::endl;
    std::cout << "    Precision loss:              " << std::scientific << (distance_double - static_cast<double>(distance_float)) << std::endl;


    double max_abs_diff = 0.0;

    for(size_t i = 0; i < num_points; ++i) {
        double sum_sq_d = 0.0;

        sum_sq_d += static_cast<double>(lidar_x[i]) * static_cast<double>(lidar_x[i]);
        sum_sq_d += static_cast<double>(lidar_y[i]) * static_cast<double>(lidar_y[i]);
        sum_sq_d += static_cast<double>(lidar_z[i]) * static_cast<double>(lidar_z[i]);

        double dist_d = std::sqrt(sum_sq_d);

        float dist_pipeline = static_cast<float>(dist_d);

        float dx_f = lidar_x[i];
        float dy_f = lidar_y[i];
        float dz_f = lidar_z[i];
        float sum_sq_f = dx_f * dx_f + dy_f * dy_f + dz_f * dz_f;
        float dist_float_only = std::sqrt(sum_sq_f);

        double diff = std::abs(static_cast<double>(dist_pipeline) - static_cast<double>(dist_float_only));

        if(diff > max_abs_diff) {
            max_abs_diff = diff;
        }
    }

    std::cout << "    Max distance diff (double→float vs float-only: " << std::scientific << max_abs_diff << " m" << std::endl;

    std::cout << "\nSTEP 4: Object Detection (Probability → Class Label)" << std::endl;

    std::vector<double> raw_logits = {2.1, 0.3, 1.8, 0.1, 3.2};
    std::vector<short> ground_truth = {0, 1, 1, 0, 1};

    std::cout << "  Sample | Raw Logit | Probability | Predicted | Actual | Match" << std::endl;
    std::cout << " " << std::string(55, '-') << std::endl;

    int correct = 0;
    int tp = 0, fp = 0, tn = 0, fn = 0;

    for(size_t i = 0; i < raw_logits.size(); ++i) {
        double prob = 1.0 / (1.0 + std::exp(-raw_logits[i]));

        prob = std::clamp(prob, 0.0, 1.0);

        short pred = static_cast<short>(prob > 0.5);
        short actual = ground_truth[i];

        if(pred == actual) correct++;

        if(pred == 1 && actual == 1) {
            tp++;
        } else if(pred == 1 && actual == 0) {
            fp++;
        } else if(pred == 0 && actual == 0)  {
            tn++;
        } else if(pred == 0 && actual == 1) {
            fn++;
        }

        std::cout << "     " << i << "   |   " << std::setw(6) << std::setprecision(2) << raw_logits[i]
                  <<"   |   " << std::setw(6) << std::setprecision(4) << prob
                  << "   |   " << pred << "   |   " << ground_truth[i]
                  << "   |   " << (pred == ground_truth[i] ? "✓" : "✗") << std::endl;
    }

    double accuracy = static_cast<double>(correct) / static_cast<double>(raw_logits.size()) * 100.0;

    std::cout << "\nAccuracy: " << std::setprecision(1) << accuracy << "% (" << correct << "/" << raw_logits.size() << ")" << std::endl;
    std::cout << "Confusion matrix (threshold = 0.5):" << std::endl;
    std::cout << " TP: " << tp << ", FP: " << fp << ", TN: " << tn << ", FN: " << fn << std::endl;

    double thr_low = 0.3;
    double thr_high = 0.7;

    int correct_low = 0;
    int correct_high = 0;

    for(size_t i = 0; i< raw_logits.size(); ++i) {
        double prob = 1.0 / (1.0 + std::exp(-raw_logits[i]));

        prob = std::clamp(prob, 0.0, 1.0);

        short actual = ground_truth[i];
        short pred_low = static_cast<short>(prob > thr_low);
        short pred_high = static_cast<short>(prob > thr_high);

        if(pred_low == actual) correct_low++;
        if(pred_high == actual) correct_high++;
    }

    double acc_low = static_cast<double>(correct_low) / static_cast<double>(raw_logits.size()) * 100.0;
    double acc_high = static_cast<double>(correct_high) / static_cast<double>(raw_logits.size()) * 100.0;

    std::cout << "\nAccuracy vs Threshold:" << std::endl;
    std::cout << "  thr = 0.3 →  " << std::setprecision(1) << acc_low << "% (" << correct_low << "/" << raw_logits.size() << ")" << std::endl;
    std::cout << "  thr = 0.5 →  " << std::setprecision(1) << accuracy << "% (" << correct << "/" << raw_logits.size() << ")" << std::endl;
    std::cout << "  thr = 0.7 →  " << std::setprecision(1) << acc_high << "% (" << correct_high << "/" << raw_logits.size() << ")" << std::endl;


    std::cout << "\nSTEP 5: Fleet-Scale Processing (long long)" << std::endl;

    long long frames_per_day = 86400LL * 36;
    long long fleet_size = 100000;
    long long total_pixels_fleet = frames_per_day * static_cast<long long>(total_pixels_all_cameras) * fleet_size;

    std::cout << "  Frames/car/day (long long): " << frames_per_day << std::endl;
    std::cout << "  Fleet size (long long): " << fleet_size << std::endl;
    std::cout << "  Total pixels (long long): " << total_pixels_fleet << std::endl;
    std::cout << "  int overflow threshold:   2,147,483,647" << std::endl;

    double total_bytes_float_fleet = static_cast<double>(total_pixels_fleet) * static_cast<double>(sizeof(float));
    double total_bytes_double_fleet = static_cast<double>(total_pixels_fleet) * static_cast<double>(sizeof(double));
    double float_TB = total_bytes_float_fleet / (1024.0 * 1024.0 * 1024.0 * 1024.0);
    double double_TB = total_bytes_double_fleet / (1024.0 * 1024.0 * 1024.0 * 1024.0);

    std::cout << "\nApproximate storage for one day of fleet pixels:" << std::endl;
    std::cout << "  Float:  " << std::setprecision(2) << std::fixed << float_TB << " TB/day" << std::endl;
    std::cout << "  Double:  " << std::setprecision(2) << std::fixed << double_TB << " TB/day" << std::endl;
    std::cout << "  Savings:  " << (double_TB - float_TB) << " TB/day saved by not defaulting to double" << std::endl;

    return 0;
}
