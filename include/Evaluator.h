#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "StockData.h"
#include <vector>

namespace StockSense {

/**
 * @brief Model Evaluator for computing error metrics
 * 
 * Computes standard regression metrics:
 * - RMSE (Root Mean Square Error): Penalizes large errors more
 * - MAE (Mean Absolute Error): Average dollar error
 * - MAPE (Mean Absolute Percentage Error): Average % error
 * 
 * Also handles:
 * - Comparison between models
 * - Result reporting
 */
class Evaluator {
public:
    /**
     * @brief Calculate RMSE (Root Mean Square Error)
     * @param actual Actual values
     * @param predicted Predicted values
     * @return double RMSE value
     * 
     * Formula: sqrt(mean((actual - predicted)²))
     * 
     * Interpretation: Average error in same units as the target.
     * Penalizes larger errors more heavily due to squaring.
     */
    static double calculateRMSE(const std::vector<double>& actual, 
                                const std::vector<double>& predicted);
    
    /**
     * @brief Calculate MAE (Mean Absolute Error)
     * @param actual Actual values
     * @param predicted Predicted values
     * @return double MAE value
     * 
     * Formula: mean(|actual - predicted|)
     * 
     * Interpretation: Average dollar error per prediction.
     * More robust to outliers than RMSE.
     */
    static double calculateMAE(const std::vector<double>& actual, 
                               const std::vector<double>& predicted);
    
    /**
     * @brief Calculate MAPE (Mean Absolute Percentage Error)
     * @param actual Actual values
     * @param predicted Predicted values
     * @return double MAPE value (as percentage, e.g., 3.2 = 3.2%)
     * 
     * Formula: mean(|actual - predicted| / actual) * 100
     * 
     * Interpretation: On average, predictions are off by this %.
     * Scale-independent, so useful for comparing across different stocks.
     */
    static double calculateMAPE(const std::vector<double>& actual, 
                                const std::vector<double>& predicted);
    
    /**
     * @brief Calculate all metrics at once
     * @param actual Actual values
     * @param predicted Predicted values
     * @return ModelMetrics Struct with all metrics
     */
    static ModelMetrics evaluate(const std::vector<double>& actual, 
                                 const std::vector<double>& predicted);
    
    /**
     * @brief Generate prediction results with error details
     * @param dates Dates for each prediction
     * @param actual Actual values
     * @param predicted Predicted values
     * @return std::vector<PredictionResult> Detailed results per prediction
     */
    static std::vector<PredictionResult> generateResults(
        const std::vector<std::string>& dates,
        const std::vector<double>& actual,
        const std::vector<double>& predicted);
    
    /**
     * @brief Compare two models and determine winner
     * @param model1Name Name of first model
     * @param metrics1 Metrics for first model
     * @param model2Name Name of second model
     * @param metrics2 Metrics for second model
     * @return std::string Comparison summary
     */
    static std::string compareModels(
        const std::string& model1Name, const ModelMetrics& metrics1,
        const std::string& model2Name, const ModelMetrics& metrics2);
    
    /**
     * @brief Calculate improvement percentage
     * @param baseline Baseline metric value
     * @param improved Improved metric value
     * @return double Improvement percentage (positive = better)
     */
    static double calculateImprovement(double baseline, double improved);
};

} // namespace StockSense

#endif // EVALUATOR_H
