#ifndef FEATUREENGINE_H
#define FEATUREENGINE_H

#include "StockData.h"
#include <vector>

namespace StockSense {

/**
 * @brief Feature Engineering for stock data
 * 
 * Computes technical indicators from raw OHLCV data:
 * - Simple Moving Averages (SMA)
 * - Daily Returns
 * - Price Range (Volatility)
 * - Volume Change
 * - (Optional) EMA, RSI, MACD
 * 
 * All features are computed without external libraries.
 */
class FeatureEngine {
public:
    /**
     * @brief Construct Feature Engine
     * @param shortWindow Short-term SMA window (default: 5)
     * @param longWindow Long-term SMA window (default: 20)
     */
    FeatureEngine(int shortWindow = 5, int longWindow = 20);
    
    /**
     * @brief Compute all features for the dataset
     * @param data Vector of cleaned stock data
     * @return std::vector<EnhancedStockData> Data with computed features
     * 
     * Note: First (longWindow - 1) rows will have incomplete features
     */
    std::vector<EnhancedStockData> computeFeatures(const std::vector<StockData>& data);
    
    /**
     * @brief Get only rows with all features computed
     * @param data Vector of enhanced stock data
     * @return std::vector<EnhancedStockData> Filtered data with complete features
     */
    std::vector<EnhancedStockData> getCompleteRows(const std::vector<EnhancedStockData>& data);
    
    /**
     * @brief Extract feature matrix for ML training
     * @param data Vector of enhanced stock data
     * @return std::vector<std::vector<double>> Feature matrix (rows = samples, cols = features)
     * 
     * Features in order: SMA5, SMA20, DailyReturn, PriceRange, VolumeChange
     */
    std::vector<std::vector<double>> extractFeatureMatrix(const std::vector<EnhancedStockData>& data);
    
    /**
     * @brief Extract target values (close prices) for ML training
     * @param data Vector of enhanced stock data
     * @return std::vector<double> Vector of close prices
     */
    std::vector<double> extractTargets(const std::vector<EnhancedStockData>& data);
    
    /**
     * @brief Get feature names
     */
    std::vector<std::string> getFeatureNames() const;
    
private:
    int shortWindow_;
    int longWindow_;
    
    /**
     * @brief Calculate Simple Moving Average
     * @param prices Vector of prices
     * @param index Current index
     * @param window Window size
     * @return double SMA value, or 0 if not enough data
     */
    double calculateSMA(const std::vector<double>& prices, size_t index, int window);
    
    /**
     * @brief Calculate daily return percentage
     * @param currentClose Current day's close
     * @param previousClose Previous day's close
     * @return double Return percentage
     */
    double calculateDailyReturn(double currentClose, double previousClose);
    
    /**
     * @brief Calculate price range (intraday volatility)
     * @param high Day's high
     * @param low Day's low
     * @return double Price range
     */
    double calculatePriceRange(double high, double low);
    
    /**
     * @brief Calculate volume change percentage
     * @param currentVolume Current day's volume
     * @param previousVolume Previous day's volume
     * @return double Volume change percentage
     */
    double calculateVolumeChange(long long currentVolume, long long previousVolume);
    
    /**
     * @brief Calculate Exponential Moving Average (for future MACD)
     * @param prices Vector of prices
     * @param index Current index
     * @param window Window size
     * @param prevEMA Previous EMA value
     * @return double EMA value
     */
    double calculateEMA(const std::vector<double>& prices, size_t index, int window, double prevEMA);
};

} // namespace StockSense

#endif // FEATUREENGINE_H
