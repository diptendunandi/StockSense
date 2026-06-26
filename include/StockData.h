#ifndef STOCKDATA_H
#define STOCKDATA_H

#include <string>
#include <vector>

namespace StockSense {

/**
 * @brief Raw stock data from CSV
 * Represents one row of OHLCV (Open, High, Low, Close, Volume) data
 */
struct StockData {
    std::string date;       // Date in YYYY-MM-DD format
    double open;            // Opening price
    double high;            // Highest price of the day
    double low;             // Lowest price of the day
    double close;           // Closing price
    double adjClose;        // Adjusted closing price (accounts for splits/dividends)
    long long volume;       // Trading volume
    
    // Default constructor
    StockData() : open(0), high(0), low(0), close(0), adjClose(0), volume(0) {}
    
    // Parameterized constructor
    StockData(const std::string& d, double o, double h, double l, double c, double ac, long long v)
        : date(d), open(o), high(h), low(l), close(c), adjClose(ac), volume(v) {}
    
    // Check if this row has valid data
    bool isValid() const {
        return !date.empty() && open > 0 && high > 0 && low > 0 && close > 0 && volume >= 0;
    }
};

/**
 * @brief Enhanced stock data with computed features
 * Contains the original data plus technical indicators
 */
struct EnhancedStockData {
    // Original data
    StockData raw;
    
    // Computed features
    double sma5;            // 5-day Simple Moving Average
    double sma20;           // 20-day Simple Moving Average
    double dailyReturn;     // Daily return percentage: (close - prevClose) / prevClose * 100
    double priceRange;      // Intraday range: high - low
    double volumeChange;    // Volume change percentage from previous day
    
    // Additional features (optional, for extension)
    double ema12;           // 12-day Exponential Moving Average (for MACD)
    double ema26;           // 26-day Exponential Moving Average (for MACD)
    double rsi;             // Relative Strength Index (14-day)
    
    // Flags
    bool hasAllFeatures;    // True if all features could be computed
    
    EnhancedStockData() 
        : sma5(0), sma20(0), dailyReturn(0), priceRange(0), volumeChange(0),
          ema12(0), ema26(0), rsi(0), hasAllFeatures(false) {}
};

/**
 * @brief Prediction result for a single day
 */
struct PredictionResult {
    std::string date;           // Date of prediction
    double actualClose;         // Actual closing price (if known)
    double predictedClose;      // Model's predicted price
    double error;               // Absolute error
    double errorPercent;        // Error as percentage
    
    PredictionResult() : actualClose(0), predictedClose(0), error(0), errorPercent(0) {}
    
    PredictionResult(const std::string& d, double actual, double predicted)
        : date(d), actualClose(actual), predictedClose(predicted) {
        error = std::abs(actual - predicted);
        errorPercent = (actual > 0) ? (error / actual * 100.0) : 0;
    }
};

/**
 * @brief Model evaluation metrics
 */
struct ModelMetrics {
    double rmse;    // Root Mean Square Error
    double mae;     // Mean Absolute Error
    double mape;    // Mean Absolute Percentage Error
    int sampleSize; // Number of predictions used
    
    ModelMetrics() : rmse(0), mae(0), mape(0), sampleSize(0) {}
};

/**
 * @brief Configuration for the predictor
 */
struct Config {
    std::string inputFile;          // Path to input CSV
    std::string outputFile;         // Path for output predictions
    int predictionDays;             // Number of days to predict
    double trainTestSplit;          // Fraction for training (e.g., 0.8)
    std::string modelType;          // "lr" for Linear Regression, "ma" for Moving Average
    int smaShortWindow;             // Short SMA window (default: 5)
    int smaLongWindow;              // Long SMA window (default: 20)
    bool verbose;                   // Print detailed output
    
    Config() 
        : inputFile("data/AAPL.csv"),
          outputFile("output/predictions.csv"),
          predictionDays(5),
          trainTestSplit(0.8),
          modelType("lr"),
          smaShortWindow(5),
          smaLongWindow(20),
          verbose(false) {}
};

} // namespace StockSense

#endif // STOCKDATA_H
