#include "Display.h"
#include "Evaluator.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>

namespace StockSense {

void Display::printDashboard(
        const Config& config,
        const std::pair<std::string, std::string>& trainRange,
        const std::pair<std::string, std::string>& testRange,
        const ModelMetrics& baselineMetrics,
        const ModelMetrics& lrMetrics,
        const std::vector<PredictionResult>& futurePredictions) {
    
    const int WIDTH = 56;
    
    std::cout << "\n";
    
    // Top border
    std::cout << "\033[1;36m";  // Cyan bold
    std::cout << std::string(WIDTH, '=') << "\n";
    std::cout << "         StockSense - Stock Price Predictor           \n";
    std::cout << std::string(WIDTH, '=') << "\n";
    std::cout << "\033[0m";  // Reset
    
    // Dataset info
    std::cout << "\033[1;33m";  // Yellow
    std::cout << " Dataset:        " << config.inputFile << "\n";
    std::cout << " Training Set:   " << trainRange.first << " to " << trainRange.second << "\n";
    std::cout << " Testing Set:    " << testRange.first << " to " << testRange.second << "\n";
    std::cout << " Features Used:  SMA5, SMA20, DailyReturn,\n";
    std::cout << "                 PriceRange, VolumeChange\n";
    std::cout << "\033[0m";
    
    std::cout << std::string(WIDTH, '-') << "\n";
    
    // Model comparison
    std::cout << "\033[1;37m MODEL COMPARISON:\033[0m\n\n";
    
    std::cout << " Moving Average Baseline:\n";
    std::cout << "   RMSE: " << formatCurrency(baselineMetrics.rmse)
              << "  |  MAE: " << formatCurrency(baselineMetrics.mae)
              << "  |  MAPE: " << formatPercent(baselineMetrics.mape) << "\n\n";
    
    std::cout << " Linear Regression:\n";
    std::cout << "   RMSE: " << formatCurrency(lrMetrics.rmse)
              << "  |  MAE: " << formatCurrency(lrMetrics.mae)
              << "  |  MAPE: " << formatPercent(lrMetrics.mape) << "\n\n";
    
    // Determine winner
    double improvement = Evaluator::calculateImprovement(baselineMetrics.mape, lrMetrics.mape);
    if (improvement > 0) {
        std::cout << "\033[1;32m";  // Green
        std::cout << " [OK] Linear Regression wins by " << std::fixed << std::setprecision(1) 
                  << improvement << "% (MAPE)\n";
        std::cout << "\033[0m";
    } else {
        std::cout << "\033[1;31m";  // Red
        std::cout << " [!!] Baseline wins - model may need tuning\n";
        std::cout << "\033[0m";
    }
    
    std::cout << std::string(WIDTH, '-') << "\n";
    
    // Future predictions
    if (!futurePredictions.empty()) {
        std::cout << "\033[1;37m NEXT " << futurePredictions.size() << "-DAY PREDICTION:\033[0m\n";
        
        for (size_t i = 0; i < futurePredictions.size(); ++i) {
            std::cout << "   Day " << (i + 1) << ": " 
                      << formatCurrency(futurePredictions[i].predictedClose);
            if (i < futurePredictions.size() - 1) {
                std::cout << "  |";
            }
            if ((i + 1) % 2 == 0 || i == futurePredictions.size() - 1) {
                std::cout << "\n";
            }
        }
    }
    
    // Bottom border
    std::cout << "\033[1;36m";
    std::cout << std::string(WIDTH, '=') << "\n";
    std::cout << "\033[0m\n";
}

void Display::printDataSummary(
        const std::string& filename,
        size_t totalRows,
        size_t trainRows,
        size_t testRows) {
    
    std::cout << "\n Data Summary:\n";
    std::cout << "   File: " << filename << "\n";
    std::cout << "   Total Rows: " << totalRows << "\n";
    std::cout << "   Training: " << trainRows << " (" 
              << std::fixed << std::setprecision(1)
              << (100.0 * trainRows / totalRows) << "%)\n";
    std::cout << "   Testing: " << testRows << " ("
              << (100.0 * testRows / totalRows) << "%)\n\n";
}

void Display::printCleaningStats(
        size_t originalCount,
        size_t cleanedCount,
        size_t missingFilled,
        size_t outliersRemoved) {
    
    std::cout << " Data Cleaning:\n";
    std::cout << "   Original rows: " << originalCount << "\n";
    std::cout << "   Cleaned rows: " << cleanedCount << "\n";
    std::cout << "   Missing values filled: " << missingFilled << "\n";
    std::cout << "   Outliers removed: " << outliersRemoved << "\n\n";
}

void Display::printModelComparison(
        const std::string& baselineName, const ModelMetrics& baselineMetrics,
        const std::string& modelName, const ModelMetrics& modelMetrics) {
    
    std::cout << "\n" << std::string(60, '-') << "\n";
    std::cout << std::setw(20) << "Metric" 
              << std::setw(20) << baselineName 
              << std::setw(20) << modelName << "\n";
    std::cout << std::string(60, '-') << "\n";
    
    std::cout << std::setw(20) << "RMSE"
              << std::setw(20) << formatCurrency(baselineMetrics.rmse)
              << std::setw(20) << formatCurrency(modelMetrics.rmse) << "\n";
    
    std::cout << std::setw(20) << "MAE"
              << std::setw(20) << formatCurrency(baselineMetrics.mae)
              << std::setw(20) << formatCurrency(modelMetrics.mae) << "\n";
    
    std::cout << std::setw(20) << "MAPE"
              << std::setw(20) << formatPercent(baselineMetrics.mape)
              << std::setw(20) << formatPercent(modelMetrics.mape) << "\n";
    
    std::cout << std::string(60, '-') << "\n";
}

void Display::printFuturePredictions(const std::vector<PredictionResult>& predictions) {
    if (predictions.empty()) {
        std::cout << " No future predictions available.\n";
        return;
    }
    
    std::cout << "\n Future Predictions:\n";
    std::cout << std::string(40, '-') << "\n";
    std::cout << std::setw(15) << "Date" << std::setw(20) << "Predicted Price" << "\n";
    std::cout << std::string(40, '-') << "\n";
    
    for (const auto& pred : predictions) {
        std::cout << std::setw(15) << pred.date 
                  << std::setw(20) << formatCurrency(pred.predictedClose) << "\n";
    }
    
    std::cout << std::string(40, '-') << "\n";
}

void Display::printCoefficients(
        const std::vector<double>& coefficients,
        const std::vector<std::string>& featureNames) {
    
    std::cout << "\n Linear Regression Coefficients:\n";
    std::cout << std::string(45, '-') << "\n";
    
    size_t nameIdx = 0;
    bool hasBias = (coefficients.size() == featureNames.size() + 1);
    
    for (size_t i = 0; i < coefficients.size(); ++i) {
        std::string name;
        if (i == 0 && hasBias) {
            name = "Intercept (bias)";
        } else {
            name = (nameIdx < featureNames.size()) ? featureNames[nameIdx++] : "Unknown";
        }
        
        std::cout << "   " << std::setw(20) << std::left << name 
                  << std::setw(15) << std::right << std::fixed << std::setprecision(6)
                  << coefficients[i] << "\n";
    }
    
    std::cout << std::string(45, '-') << "\n";
}

void Display::printSeparator(int width, char c) {
    std::cout << std::string(width, c) << "\n";
}

void Display::printHeader(const std::string& title, int width) {
    std::cout << std::string(width, '=') << "\n";
    std::cout << centerText(title, width) << "\n";
    std::cout << std::string(width, '=') << "\n";
}

void Display::printProgress(const std::string& message) {
    std::cout << "\033[1;34m[...] \033[0m" << message << "\n";
}

void Display::printError(const std::string& message) {
    std::cout << "\033[1;31m[ERR] \033[0m" << message << "\n";
}

void Display::printSuccess(const std::string& message) {
    std::cout << "\033[1;32m[OK]  \033[0m" << message << "\n";
}

std::string Display::centerText(const std::string& text, int width) {
    if (static_cast<int>(text.length()) >= width) {
        return text;
    }
    int padding = (width - text.length()) / 2;
    return std::string(padding, ' ') + text;
}

std::string Display::formatNumber(double value, int precision) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision) << value;
    return oss.str();
}

std::string Display::formatCurrency(double value) {
    std::ostringstream oss;
    oss << "$" << std::fixed << std::setprecision(2) << value;
    return oss.str();
}

std::string Display::formatPercent(double value) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << value << "%";
    return oss.str();
}

} // namespace StockSense
