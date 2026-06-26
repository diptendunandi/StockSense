#ifndef PREDICTOR_H
#define PREDICTOR_H

#include "StockData.h"
#include "LinearRegression.h"
#include "BaselineModel.h"
#include "FeatureEngine.h"
#include <vector>
#include <memory>

namespace StockSense {

/**
 * @brief High-level predictor that orchestrates the ML pipeline
 * 
 * Handles:
 * - Train/test splitting (time-aware)
 * - Model training
 * - Prediction generation
 * - Future price forecasting
 */
class Predictor {
public:
    /**
     * @brief Construct predictor
     * @param config Configuration settings
     */
    explicit Predictor(const Config& config);
    
    /**
     * @brief Run the full prediction pipeline
     * @param data Enhanced stock data with features
     * @return std::vector<PredictionResult> Test set predictions
     */
    std::vector<PredictionResult> run(const std::vector<EnhancedStockData>& data);
    
    /**
     * @brief Predict future prices beyond the dataset
     * @param data Full dataset for feature computation
     * @param days Number of future days to predict
     * @return std::vector<PredictionResult> Future predictions
     */
    std::vector<PredictionResult> predictFuture(
        const std::vector<EnhancedStockData>& data, 
        int days);
    
    /**
     * @brief Get Linear Regression model metrics
     */
    ModelMetrics getLRMetrics() const { return lrMetrics_; }
    
    /**
     * @brief Get Baseline model metrics
     */
    ModelMetrics getBaselineMetrics() const { return baselineMetrics_; }
    
    /**
     * @brief Get training data size
     */
    size_t getTrainSize() const { return trainSize_; }
    
    /**
     * @brief Get test data size
     */
    size_t getTestSize() const { return testSize_; }
    
    /**
     * @brief Get training date range
     */
    std::pair<std::string, std::string> getTrainDateRange() const { return trainDateRange_; }
    
    /**
     * @brief Get test date range
     */
    std::pair<std::string, std::string> getTestDateRange() const { return testDateRange_; }
    
    /**
     * @brief Get the trained Linear Regression coefficients
     */
    std::vector<double> getCoefficients() const;
    
private:
    Config config_;
    std::unique_ptr<LinearRegression> lrModel_;
    std::unique_ptr<BaselineModel> baselineModel_;
    std::unique_ptr<FeatureEngine> featureEngine_;
    
    ModelMetrics lrMetrics_;
    ModelMetrics baselineMetrics_;
    
    size_t trainSize_;
    size_t testSize_;
    std::pair<std::string, std::string> trainDateRange_;
    std::pair<std::string, std::string> testDateRange_;
    
    /**
     * @brief Split data into train and test sets (time-aware)
     * @param data Full dataset
     * @param trainRatio Fraction for training (e.g., 0.8)
     * @return pair of (train_data, test_data)
     * 
     * IMPORTANT: This is a time-aware split, NOT random shuffle.
     * Training data comes BEFORE test data chronologically.
     */
    std::pair<std::vector<EnhancedStockData>, std::vector<EnhancedStockData>>
    trainTestSplit(const std::vector<EnhancedStockData>& data, double trainRatio);
    
    /**
     * @brief Generate date string for future predictions
     * @param lastDate Last known date
     * @param daysAhead Number of days ahead
     * @return std::string Future date string
     */
    std::string generateFutureDate(const std::string& lastDate, int daysAhead);
};

} // namespace StockSense

#endif // PREDICTOR_H
