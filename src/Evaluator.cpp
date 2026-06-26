#include "Evaluator.h"
#include <cmath>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <algorithm>

namespace StockSense {

double Evaluator::calculateRMSE(const std::vector<double>& actual, 
                                 const std::vector<double>& predicted) {
    if (actual.size() != predicted.size() || actual.empty()) {
        throw std::invalid_argument("Vectors must be non-empty and same size");
    }
    
    double sumSquaredError = 0.0;
    for (size_t i = 0; i < actual.size(); ++i) {
        double error = actual[i] - predicted[i];
        sumSquaredError += error * error;
    }
    
    return std::sqrt(sumSquaredError / actual.size());
}

double Evaluator::calculateMAE(const std::vector<double>& actual, 
                                const std::vector<double>& predicted) {
    if (actual.size() != predicted.size() || actual.empty()) {
        throw std::invalid_argument("Vectors must be non-empty and same size");
    }
    
    double sumAbsError = 0.0;
    for (size_t i = 0; i < actual.size(); ++i) {
        sumAbsError += std::abs(actual[i] - predicted[i]);
    }
    
    return sumAbsError / actual.size();
}

double Evaluator::calculateMAPE(const std::vector<double>& actual, 
                                 const std::vector<double>& predicted) {
    if (actual.size() != predicted.size() || actual.empty()) {
        throw std::invalid_argument("Vectors must be non-empty and same size");
    }
    
    double sumAbsPercentError = 0.0;
    int validCount = 0;
    
    for (size_t i = 0; i < actual.size(); ++i) {
        if (actual[i] != 0) {
            sumAbsPercentError += std::abs(actual[i] - predicted[i]) / std::abs(actual[i]);
            validCount++;
        }
    }
    
    if (validCount == 0) {
        return 0.0;
    }
    
    return (sumAbsPercentError / validCount) * 100.0;  // Return as percentage
}

ModelMetrics Evaluator::evaluate(const std::vector<double>& actual, 
                                  const std::vector<double>& predicted) {
    ModelMetrics metrics;
    
    if (actual.empty() || predicted.empty() || actual.size() != predicted.size()) {
        return metrics;
    }
    
    metrics.rmse = calculateRMSE(actual, predicted);
    metrics.mae = calculateMAE(actual, predicted);
    metrics.mape = calculateMAPE(actual, predicted);
    metrics.sampleSize = static_cast<int>(actual.size());
    
    return metrics;
}

std::vector<PredictionResult> Evaluator::generateResults(
        const std::vector<std::string>& dates,
        const std::vector<double>& actual,
        const std::vector<double>& predicted) {
    
    std::vector<PredictionResult> results;
    
    size_t n = std::min({dates.size(), actual.size(), predicted.size()});
    results.reserve(n);
    
    for (size_t i = 0; i < n; ++i) {
        results.emplace_back(dates[i], actual[i], predicted[i]);
    }
    
    return results;
}

std::string Evaluator::compareModels(
        const std::string& model1Name, const ModelMetrics& metrics1,
        const std::string& model2Name, const ModelMetrics& metrics2) {
    
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    
    ss << "Model Comparison:\n";
    ss << std::string(50, '-') << "\n";
    
    ss << std::setw(20) << "Metric" 
       << std::setw(15) << model1Name 
       << std::setw(15) << model2Name << "\n";
    ss << std::string(50, '-') << "\n";
    
    ss << std::setw(20) << "RMSE" 
       << std::setw(15) << ("$" + std::to_string(metrics1.rmse).substr(0, 5))
       << std::setw(15) << ("$" + std::to_string(metrics2.rmse).substr(0, 5)) << "\n";
    
    ss << std::setw(20) << "MAE" 
       << std::setw(15) << ("$" + std::to_string(metrics1.mae).substr(0, 5))
       << std::setw(15) << ("$" + std::to_string(metrics2.mae).substr(0, 5)) << "\n";
    
    ss << std::setw(20) << "MAPE" 
       << std::setw(15) << (std::to_string(metrics1.mape).substr(0, 4) + "%")
       << std::setw(15) << (std::to_string(metrics2.mape).substr(0, 4) + "%") << "\n";
    
    ss << std::string(50, '-') << "\n";
    
    // Determine winner based on MAPE
    if (metrics1.mape < metrics2.mape) {
        double improvement = calculateImprovement(metrics2.mape, metrics1.mape);
        ss << model1Name << " wins by " << std::setprecision(1) << improvement << "% (MAPE)\n";
    } else if (metrics2.mape < metrics1.mape) {
        double improvement = calculateImprovement(metrics1.mape, metrics2.mape);
        ss << model2Name << " wins by " << std::setprecision(1) << improvement << "% (MAPE)\n";
    } else {
        ss << "Models are tied on MAPE\n";
    }
    
    return ss.str();
}

double Evaluator::calculateImprovement(double baseline, double improved) {
    if (baseline == 0.0) return 0.0;
    return ((baseline - improved) / baseline) * 100.0;
}

} // namespace StockSense
