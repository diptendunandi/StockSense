#include "DataCleaner.h"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace StockSense {

DataCleaner::DataCleaner(double outlierStdDev)
    : outlierStdDev_(outlierStdDev),
      removeZeroVolume_(true),
      removeOutliers_(true) {
}

std::vector<StockData> DataCleaner::clean(const std::vector<StockData>& data) {
    stats_ = CleaningStats();
    stats_.originalCount = data.size();
    
    if (data.empty()) {
        return data;
    }
    
    // Step 1: Remove invalid rows
    std::vector<StockData> result = removeInvalidRows(data);
    
    // Step 2: Sort by date
    sortByDate(result);
    
    // Step 3: Forward-fill missing values
    forwardFillMissing(result);
    
    // Step 4: Remove zero-volume days
    if (removeZeroVolume_) {
        result = removeZeroVolumeDays(result);
    }
    
    // Step 5: Remove outliers
    if (removeOutliers_) {
        result = removeOutliers(result);
    }
    
    stats_.cleanedCount = result.size();
    return result;
}

std::vector<StockData> DataCleaner::removeInvalidRows(const std::vector<StockData>& data) {
    std::vector<StockData> result;
    result.reserve(data.size());
    
    for (const auto& row : data) {
        if (row.isValid()) {
            result.push_back(row);
        } else {
            stats_.invalidRowsRemoved++;
        }
    }
    
    return result;
}

void DataCleaner::forwardFillMissing(std::vector<StockData>& data) {
    if (data.empty()) return;
    
    for (size_t i = 1; i < data.size(); ++i) {
        // Check for zero or NaN values and forward-fill from previous row
        if (data[i].open <= 0 || std::isnan(data[i].open)) {
            data[i].open = data[i-1].open;
            stats_.missingValuesFilled++;
        }
        if (data[i].high <= 0 || std::isnan(data[i].high)) {
            data[i].high = data[i-1].high;
            stats_.missingValuesFilled++;
        }
        if (data[i].low <= 0 || std::isnan(data[i].low)) {
            data[i].low = data[i-1].low;
            stats_.missingValuesFilled++;
        }
        if (data[i].close <= 0 || std::isnan(data[i].close)) {
            data[i].close = data[i-1].close;
            stats_.missingValuesFilled++;
        }
        if (data[i].adjClose <= 0 || std::isnan(data[i].adjClose)) {
            data[i].adjClose = data[i-1].adjClose;
            stats_.missingValuesFilled++;
        }
    }
}

std::vector<StockData> DataCleaner::removeOutliers(const std::vector<StockData>& data) {
    if (data.size() < 2) return data;
    
    // Calculate daily returns
    std::vector<double> returns;
    returns.reserve(data.size() - 1);
    
    for (size_t i = 1; i < data.size(); ++i) {
        double ret = (data[i].close - data[i-1].close) / data[i-1].close * 100.0;
        returns.push_back(ret);
    }
    
    // Calculate mean and std dev of returns
    double mean = calculateMean(returns);
    double stdDev = calculateStdDev(returns, mean);
    
    // Filter out rows where return exceeds threshold
    std::vector<StockData> result;
    result.push_back(data[0]);  // Keep first row (no return for it)
    
    for (size_t i = 1; i < data.size(); ++i) {
        double ret = returns[i-1];
        double zScore = (stdDev > 0) ? std::abs(ret - mean) / stdDev : 0;
        
        if (zScore <= outlierStdDev_) {
            result.push_back(data[i]);
        } else {
            stats_.outliersRemoved++;
        }
    }
    
    return result;
}

std::vector<StockData> DataCleaner::removeZeroVolumeDays(const std::vector<StockData>& data) {
    std::vector<StockData> result;
    result.reserve(data.size());
    
    for (const auto& row : data) {
        if (row.volume > 0) {
            result.push_back(row);
        } else {
            stats_.zeroVolumeRemoved++;
        }
    }
    
    return result;
}

void DataCleaner::sortByDate(std::vector<StockData>& data) {
    std::sort(data.begin(), data.end(), [](const StockData& a, const StockData& b) {
        return a.date < b.date;  // Lexicographic comparison works for YYYY-MM-DD
    });
}

double DataCleaner::calculateMean(const std::vector<double>& values) {
    if (values.empty()) return 0.0;
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

double DataCleaner::calculateStdDev(const std::vector<double>& values, double mean) {
    if (values.size() < 2) return 0.0;
    
    double sumSqDiff = 0.0;
    for (double val : values) {
        double diff = val - mean;
        sumSqDiff += diff * diff;
    }
    
    return std::sqrt(sumSqDiff / (values.size() - 1));
}

} // namespace StockSense
