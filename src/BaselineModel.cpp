#include "BaselineModel.h"
#include <numeric>
#include <algorithm>

namespace StockSense {

BaselineModel::BaselineModel(int window) : window_(window) {
    if (window < 1) {
        window_ = 5;  // Default to 5-day moving average
    }
}

void BaselineModel::fit(const std::vector<EnhancedStockData>& data) {
    trainingData_ = data;
}

double BaselineModel::predict(const std::vector<double>& recentCloses) const {
    if (recentCloses.empty()) {
        return 0.0;
    }
    
    // Use the last 'window_' closes, or all available if less
    size_t count = std::min(static_cast<size_t>(window_), recentCloses.size());
    double sum = 0.0;
    
    for (size_t i = recentCloses.size() - count; i < recentCloses.size(); ++i) {
        sum += recentCloses[i];
    }
    
    return sum / count;
}

std::vector<double> BaselineModel::predictAll(
        const std::vector<EnhancedStockData>& trainData,
        const std::vector<EnhancedStockData>& testData) const {
    
    std::vector<double> predictions;
    predictions.reserve(testData.size());
    
    // Build a running list of closes from training data
    std::vector<double> closes;
    closes.reserve(trainData.size() + testData.size());
    
    for (const auto& d : trainData) {
        closes.push_back(d.raw.close);
    }
    
    // For each test sample, predict using the last N known closes
    for (const auto& d : testData) {
        double pred = predict(closes);
        predictions.push_back(pred);
        
        // Add actual close to history for next prediction
        closes.push_back(d.raw.close);
    }
    
    return predictions;
}

std::vector<double> BaselineModel::predictFuture(
        const std::vector<double>& lastCloses,
        int days) const {
    
    std::vector<double> predictions;
    predictions.reserve(days);
    
    // Copy the last closes as our working window
    std::vector<double> window = lastCloses;
    
    for (int i = 0; i < days; ++i) {
        // Predict next day
        double pred = predict(window);
        predictions.push_back(pred);
        
        // Add prediction to window for next iteration
        // This is recursive prediction: each prediction feeds into the next
        window.push_back(pred);
    }
    
    return predictions;
}

} // namespace StockSense
