#include "FeatureEngine.h"
#include <cmath>
#include <numeric>

namespace StockSense {

FeatureEngine::FeatureEngine(int shortWindow, int longWindow)
    : shortWindow_(shortWindow), longWindow_(longWindow) {
}

std::vector<EnhancedStockData> FeatureEngine::computeFeatures(const std::vector<StockData>& data) {
    std::vector<EnhancedStockData> result;
    result.reserve(data.size());
    
    if (data.empty()) return result;
    
    // Extract close prices for SMA calculations
    std::vector<double> closePrices;
    closePrices.reserve(data.size());
    for (const auto& d : data) {
        closePrices.push_back(d.close);
    }
    
    for (size_t i = 0; i < data.size(); ++i) {
        EnhancedStockData enhanced;
        enhanced.raw = data[i];
        
        // Calculate SMA (Simple Moving Averages)
        enhanced.sma5 = calculateSMA(closePrices, i, shortWindow_);
        enhanced.sma20 = calculateSMA(closePrices, i, longWindow_);
        
        // Calculate Daily Return
        if (i > 0) {
            enhanced.dailyReturn = calculateDailyReturn(data[i].close, data[i-1].close);
        } else {
            enhanced.dailyReturn = 0.0;
        }
        
        // Calculate Price Range (intraday volatility)
        enhanced.priceRange = calculatePriceRange(data[i].high, data[i].low);
        
        // Calculate Volume Change
        if (i > 0) {
            enhanced.volumeChange = calculateVolumeChange(data[i].volume, data[i-1].volume);
        } else {
            enhanced.volumeChange = 0.0;
        }
        
        // Check if all features are available
        // Need at least longWindow_ days of history for all features
        enhanced.hasAllFeatures = (i >= static_cast<size_t>(longWindow_ - 1));
        
        result.push_back(enhanced);
    }
    
    return result;
}

std::vector<EnhancedStockData> FeatureEngine::getCompleteRows(const std::vector<EnhancedStockData>& data) {
    std::vector<EnhancedStockData> result;
    
    for (const auto& row : data) {
        if (row.hasAllFeatures) {
            result.push_back(row);
        }
    }
    
    return result;
}

std::vector<std::vector<double>> FeatureEngine::extractFeatureMatrix(const std::vector<EnhancedStockData>& data) {
    std::vector<std::vector<double>> matrix;
    matrix.reserve(data.size());
    
    for (const auto& row : data) {
        if (row.hasAllFeatures) {
            std::vector<double> features = {
                row.sma5,
                row.sma20,
                row.dailyReturn,
                row.priceRange,
                row.volumeChange
            };
            matrix.push_back(features);
        }
    }
    
    return matrix;
}

std::vector<double> FeatureEngine::extractTargets(const std::vector<EnhancedStockData>& data) {
    std::vector<double> targets;
    targets.reserve(data.size());
    
    for (const auto& row : data) {
        if (row.hasAllFeatures) {
            targets.push_back(row.raw.close);
        }
    }
    
    return targets;
}

std::vector<std::string> FeatureEngine::getFeatureNames() const {
    return {"SMA5", "SMA20", "DailyReturn", "PriceRange", "VolumeChange"};
}

double FeatureEngine::calculateSMA(const std::vector<double>& prices, size_t index, int window) {
    if (index < static_cast<size_t>(window - 1)) {
        // Not enough data for full window
        // Return partial SMA with available data
        double sum = 0.0;
        for (size_t i = 0; i <= index; ++i) {
            sum += prices[i];
        }
        return sum / (index + 1);
    }
    
    // Calculate SMA over the window
    double sum = 0.0;
    for (size_t i = index - window + 1; i <= index; ++i) {
        sum += prices[i];
    }
    
    return sum / window;
}

double FeatureEngine::calculateDailyReturn(double currentClose, double previousClose) {
    if (previousClose <= 0) return 0.0;
    return (currentClose - previousClose) / previousClose * 100.0;
}

double FeatureEngine::calculatePriceRange(double high, double low) {
    return high - low;
}

double FeatureEngine::calculateVolumeChange(long long currentVolume, long long previousVolume) {
    if (previousVolume <= 0) return 0.0;
    return static_cast<double>(currentVolume - previousVolume) / previousVolume * 100.0;
}

double FeatureEngine::calculateEMA(const std::vector<double>& prices, size_t index, 
                                    int window, double prevEMA) {
    double multiplier = 2.0 / (window + 1);
    
    if (index < static_cast<size_t>(window - 1)) {
        // Not enough data, fall back to SMA
        return calculateSMA(prices, index, window);
    }
    
    if (index == static_cast<size_t>(window - 1)) {
        // First EMA is just SMA
        return calculateSMA(prices, index, window);
    }
    
    // EMA formula: EMA_today = (Price_today * multiplier) + (EMA_yesterday * (1 - multiplier))
    return (prices[index] * multiplier) + (prevEMA * (1 - multiplier));
}

} // namespace StockSense
