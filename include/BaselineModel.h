#ifndef BASELINEMODEL_H
#define BASELINEMODEL_H

#include "StockData.h"
#include <vector>

namespace StockSense {

/**
 * @brief Baseline model using Simple Moving Average
 * 
 * Predicts tomorrow's price as the average of the last N days.
 * This serves as a baseline to compare against the Linear Regression model.
 * 
 * A good ML model should beat this simple baseline.
 */
class BaselineModel {
public:
    /**
     * @brief Construct baseline model
     * @param window Number of days to average (default: 5)
     */
    explicit BaselineModel(int window = 5);
    
    /**
     * @brief "Train" the model (just stores the training data for reference)
     * @param data Training data
     */
    void fit(const std::vector<EnhancedStockData>& data);
    
    /**
     * @brief Predict next price based on recent closes
     * @param recentCloses Vector of recent closing prices (most recent last)
     * @return double Predicted price (average of last N closes)
     */
    double predict(const std::vector<double>& recentCloses) const;
    
    /**
     * @brief Generate predictions for test data
     * @param trainData Training data (used to get initial window)
     * @param testData Test data to generate predictions for
     * @return std::vector<double> Predicted values
     * 
     * For each test day, predicts using the average of the last N known closes.
     */
    std::vector<double> predictAll(
        const std::vector<EnhancedStockData>& trainData,
        const std::vector<EnhancedStockData>& testData) const;
    
    /**
     * @brief Predict future prices beyond available data
     * @param lastCloses Last available closing prices
     * @param days Number of days to predict
     * @return std::vector<double> Future predicted prices
     * 
     * Uses recursive prediction: each predicted price becomes
     * part of the window for the next prediction.
     */
    std::vector<double> predictFuture(
        const std::vector<double>& lastCloses,
        int days) const;
    
    /**
     * @brief Get the window size
     */
    int getWindow() const { return window_; }
    
private:
    int window_;
    std::vector<EnhancedStockData> trainingData_;
};

} // namespace StockSense

#endif // BASELINEMODEL_H
