#ifndef DISPLAY_H
#define DISPLAY_H

#include "StockData.h"
#include <vector>
#include <string>

namespace StockSense {

/**
 * @brief Console display for results and dashboard
 * 
 * Creates a professional-looking ASCII dashboard showing:
 * - Dataset information
 * - Model comparison
 * - Predictions
 */
class Display {
public:
    /**
     * @brief Print the main dashboard
     * @param config Configuration used
     * @param trainRange Training date range (start, end)
     * @param testRange Test date range (start, end)
     * @param baselineMetrics Baseline model metrics
     * @param lrMetrics Linear Regression metrics
     * @param futurePredictions Future price predictions
     */
    static void printDashboard(
        const Config& config,
        const std::pair<std::string, std::string>& trainRange,
        const std::pair<std::string, std::string>& testRange,
        const ModelMetrics& baselineMetrics,
        const ModelMetrics& lrMetrics,
        const std::vector<PredictionResult>& futurePredictions);
    
    /**
     * @brief Print a summary of the dataset
     * @param filename Input file name
     * @param totalRows Total rows in dataset
     * @param trainRows Rows used for training
     * @param testRows Rows used for testing
     */
    static void printDataSummary(
        const std::string& filename,
        size_t totalRows,
        size_t trainRows,
        size_t testRows);
    
    /**
     * @brief Print cleaning statistics
     * @param originalCount Original row count
     * @param cleanedCount Cleaned row count
     * @param missingFilled Missing values filled
     * @param outliersRemoved Outliers removed
     */
    static void printCleaningStats(
        size_t originalCount,
        size_t cleanedCount,
        size_t missingFilled,
        size_t outliersRemoved);
    
    /**
     * @brief Print model comparison table
     * @param baselineName Name of baseline model
     * @param baselineMetrics Baseline metrics
     * @param modelName Name of ML model
     * @param modelMetrics ML model metrics
     */
    static void printModelComparison(
        const std::string& baselineName, const ModelMetrics& baselineMetrics,
        const std::string& modelName, const ModelMetrics& modelMetrics);
    
    /**
     * @brief Print future predictions
     * @param predictions Vector of future predictions
     */
    static void printFuturePredictions(const std::vector<PredictionResult>& predictions);
    
    /**
     * @brief Print Linear Regression coefficients
     * @param coefficients Model coefficients
     * @param featureNames Names of features
     */
    static void printCoefficients(
        const std::vector<double>& coefficients,
        const std::vector<std::string>& featureNames);
    
    /**
     * @brief Print a horizontal separator line
     * @param width Width of the line
     * @param c Character to use (default: '=')
     */
    static void printSeparator(int width = 56, char c = '=');
    
    /**
     * @brief Print a boxed header
     * @param title Title text
     * @param width Box width
     */
    static void printHeader(const std::string& title, int width = 56);
    
    /**
     * @brief Print progress message
     * @param message Progress message
     */
    static void printProgress(const std::string& message);
    
    /**
     * @brief Print error message
     * @param message Error message
     */
    static void printError(const std::string& message);
    
    /**
     * @brief Print success message
     * @param message Success message
     */
    static void printSuccess(const std::string& message);
    
private:
    /**
     * @brief Center a string within a given width
     */
    static std::string centerText(const std::string& text, int width);
    
    /**
     * @brief Format a number with specified precision
     */
    static std::string formatNumber(double value, int precision = 2);
    
    /**
     * @brief Format currency value
     */
    static std::string formatCurrency(double value);
    
    /**
     * @brief Format percentage value
     */
    static std::string formatPercent(double value);
};

} // namespace StockSense

#endif // DISPLAY_H
