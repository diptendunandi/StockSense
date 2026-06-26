#include "Predictor.h"
#include "Evaluator.h"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <ctime>

namespace StockSense {

Predictor::Predictor(const Config& config)
    : config_(config),
      lrModel_(std::make_unique<LinearRegression>(true)),
      baselineModel_(std::make_unique<BaselineModel>(config.smaShortWindow)),
      featureEngine_(std::make_unique<FeatureEngine>(config.smaShortWindow, config.smaLongWindow)),
      trainSize_(0),
      testSize_(0) {
}

std::vector<PredictionResult> Predictor::run(const std::vector<EnhancedStockData>& data) {
    // Get only rows with complete features
    std::vector<EnhancedStockData> completeData = featureEngine_->getCompleteRows(data);
    
    if (completeData.size() < 10) {
        throw std::runtime_error("Not enough data for training after feature computation");
    }
    
    // Time-aware train/test split
    auto [trainData, testData] = trainTestSplit(completeData, config_.trainTestSplit);
    
    trainSize_ = trainData.size();
    testSize_ = testData.size();
    trainDateRange_ = {trainData.front().raw.date, trainData.back().raw.date};
    testDateRange_ = {testData.front().raw.date, testData.back().raw.date};
    
    // Extract features and targets for training
    auto X_train = featureEngine_->extractFeatureMatrix(trainData);
    auto y_train = featureEngine_->extractTargets(trainData);
    
    // Train Linear Regression
    lrModel_->fit(X_train, y_train);
    
    // Train Baseline (just stores data)
    baselineModel_->fit(trainData);
    
    // Generate predictions on test set
    auto X_test = featureEngine_->extractFeatureMatrix(testData);
    auto y_test = featureEngine_->extractTargets(testData);
    
    // Linear Regression predictions
    std::vector<double> lr_predictions = lrModel_->predict(X_test);
    
    // Baseline predictions
    std::vector<double> baseline_predictions = baselineModel_->predictAll(trainData, testData);
    
    // Evaluate both models
    lrMetrics_ = Evaluator::evaluate(y_test, lr_predictions);
    baselineMetrics_ = Evaluator::evaluate(y_test, baseline_predictions);
    
    // Generate detailed results for LR model (primary model)
    std::vector<std::string> testDates;
    testDates.reserve(testData.size());
    for (const auto& d : testData) {
        testDates.push_back(d.raw.date);
    }
    
    return Evaluator::generateResults(testDates, y_test, lr_predictions);
}

std::vector<PredictionResult> Predictor::predictFuture(
        const std::vector<EnhancedStockData>& data,
        int days) {
    
    std::vector<PredictionResult> results;
    results.reserve(days);
    
    if (data.empty() || !lrModel_->isTrained()) {
        return results;
    }
    
    // Get the last complete row for feature reference
    std::vector<EnhancedStockData> completeData = featureEngine_->getCompleteRows(data);
    if (completeData.empty()) {
        return results;
    }
    
    // For simple future prediction, use the baseline model's recursive approach
    // (LR would need future features which we don't have)
    std::vector<double> lastCloses;
    lastCloses.reserve(completeData.size());
    for (const auto& d : completeData) {
        lastCloses.push_back(d.raw.close);
    }
    
    std::vector<double> futurePredictions = baselineModel_->predictFuture(lastCloses, days);
    
    // Generate dates for future predictions
    std::string lastDate = completeData.back().raw.date;
    
    for (int i = 0; i < days; ++i) {
        PredictionResult result;
        result.date = generateFutureDate(lastDate, i + 1);
        result.predictedClose = futurePredictions[i];
        result.actualClose = 0.0;  // Unknown
        result.error = 0.0;
        result.errorPercent = 0.0;
        results.push_back(result);
    }
    
    return results;
}

std::vector<double> Predictor::getCoefficients() const {
    if (lrModel_) {
        return lrModel_->getCoefficients();
    }
    return {};
}

std::pair<std::vector<EnhancedStockData>, std::vector<EnhancedStockData>>
Predictor::trainTestSplit(const std::vector<EnhancedStockData>& data, double trainRatio) {
    // TIME-AWARE SPLIT: Training data comes BEFORE test data
    // This is critical for time-series data to avoid data leakage
    
    size_t splitIndex = static_cast<size_t>(data.size() * trainRatio);
    
    // Ensure at least some data in each split
    if (splitIndex < 10) splitIndex = 10;
    if (splitIndex >= data.size() - 5) splitIndex = data.size() - 5;
    
    std::vector<EnhancedStockData> train(data.begin(), data.begin() + splitIndex);
    std::vector<EnhancedStockData> test(data.begin() + splitIndex, data.end());
    
    return {train, test};
}

std::string Predictor::generateFutureDate(const std::string& lastDate, int daysAhead) {
    // Parse date string (YYYY-MM-DD)
    int year, month, day;
    char sep1, sep2;
    std::istringstream iss(lastDate);
    iss >> year >> sep1 >> month >> sep2 >> day;
    
    // Use simple date arithmetic
    // (In production, use a proper date library)
    day += daysAhead;
    
    // Handle month overflow (simplified - doesn't account for exact days per month)
    while (day > 28) {  // Conservative to avoid invalid dates
        day -= 28;
        month++;
        if (month > 12) {
            month = 1;
            year++;
        }
    }
    
    std::ostringstream oss;
    oss << year << "-"
        << std::setfill('0') << std::setw(2) << month << "-"
        << std::setfill('0') << std::setw(2) << day;
    
    return oss.str();
}

} // namespace StockSense
