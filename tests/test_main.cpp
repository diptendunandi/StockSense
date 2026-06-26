#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <string>

// Include all headers
#include "StockData.h"
#include "CSVParser.h"
#include "DataCleaner.h"
#include "FeatureEngine.h"
#include "Matrix.h"
#include "LinearRegression.h"
#include "BaselineModel.h"
#include "Evaluator.h"

using namespace StockSense;

// Test helper
#define TEST(name) void name(); \
    struct name##_register { name##_register() { tests.push_back({#name, name}); } } name##_instance; \
    void name()

std::vector<std::pair<std::string, void(*)()>> tests;

// Helper to check floating point equality
bool approxEqual(double a, double b, double epsilon = 0.001) {
    return std::abs(a - b) < epsilon;
}

// ==================== CSV Parser Tests ====================

void test_csv_parser_split() {
    std::cout << "  Testing CSV line splitting... ";
    // This would require access to private method, skip for now
    std::cout << "SKIPPED (private method)\n";
}

// ==================== Data Cleaner Tests ====================

void test_data_cleaner_basic() {
    std::cout << "  Testing DataCleaner basic cleaning... ";
    
    std::vector<StockData> data = {
        {"2023-01-01", 100.0, 105.0, 99.0, 102.0, 102.0, 1000000},
        {"2023-01-02", 102.0, 108.0, 101.0, 106.0, 106.0, 1200000},
        {"2023-01-03", 106.0, 110.0, 104.0, 108.0, 108.0, 1100000}
    };
    
    DataCleaner cleaner;
    auto cleaned = cleaner.clean(data);
    
    assert(cleaned.size() == 3);
    assert(cleaner.getStats().originalCount == 3);
    assert(cleaner.getStats().cleanedCount == 3);
    
    std::cout << "PASSED\n";
}

void test_data_cleaner_removes_zero_volume() {
    std::cout << "  Testing DataCleaner zero volume removal... ";
    
    std::vector<StockData> data = {
        {"2023-01-01", 100.0, 105.0, 99.0, 102.0, 102.0, 1000000},
        {"2023-01-02", 102.0, 108.0, 101.0, 106.0, 106.0, 0},  // Zero volume
        {"2023-01-03", 106.0, 110.0, 104.0, 108.0, 108.0, 1100000}
    };
    
    DataCleaner cleaner;
    auto cleaned = cleaner.clean(data);
    
    assert(cleaned.size() == 2);
    assert(cleaner.getStats().zeroVolumeRemoved == 1);
    
    std::cout << "PASSED\n";
}

// ==================== Feature Engine Tests ====================

void test_feature_engine_sma() {
    std::cout << "  Testing FeatureEngine SMA calculation... ";
    
    std::vector<StockData> data;
    // Create 10 days of data with increasing prices
    for (int i = 0; i < 10; ++i) {
        double price = 100.0 + i * 2.0;  // 100, 102, 104, ..., 118
        data.push_back({
            "2023-01-" + std::string(i < 9 ? "0" : "") + std::to_string(i + 1),
            price, price + 2, price - 1, price, price, 1000000
        });
    }
    
    FeatureEngine engine(5, 5);  // 5-day SMA for both
    auto enhanced = engine.computeFeatures(data);
    
    assert(enhanced.size() == 10);
    
    // SMA-5 for day 5 (index 4) should be average of days 1-5: (100+102+104+106+108)/5 = 104
    // Close prices: 100, 102, 104, 106, 108 -> SMA = 104
    assert(approxEqual(enhanced[4].sma5, 104.0));
    
    std::cout << "PASSED\n";
}

void test_feature_engine_daily_return() {
    std::cout << "  Testing FeatureEngine daily return... ";
    
    std::vector<StockData> data = {
        {"2023-01-01", 100.0, 105.0, 99.0, 100.0, 100.0, 1000000},
        {"2023-01-02", 100.0, 110.0, 99.0, 105.0, 105.0, 1000000}  // 5% increase
    };
    
    FeatureEngine engine;
    auto enhanced = engine.computeFeatures(data);
    
    // Daily return = (105 - 100) / 100 * 100 = 5%
    assert(approxEqual(enhanced[1].dailyReturn, 5.0));
    
    std::cout << "PASSED\n";
}

// ==================== Matrix Tests ====================

void test_matrix_creation() {
    std::cout << "  Testing Matrix creation... ";
    
    Matrix m(3, 3, 1.0);
    assert(m.rows() == 3);
    assert(m.cols() == 3);
    assert(approxEqual(m(0, 0), 1.0));
    assert(approxEqual(m(2, 2), 1.0));
    
    std::cout << "PASSED\n";
}

void test_matrix_transpose() {
    std::cout << "  Testing Matrix transpose... ";
    
    std::vector<std::vector<double>> data = {
        {1, 2, 3},
        {4, 5, 6}
    };
    Matrix m(data);
    Matrix t = m.transpose();
    
    assert(t.rows() == 3);
    assert(t.cols() == 2);
    assert(approxEqual(t(0, 0), 1.0));
    assert(approxEqual(t(0, 1), 4.0));
    assert(approxEqual(t(2, 0), 3.0));
    
    std::cout << "PASSED\n";
}

void test_matrix_multiply() {
    std::cout << "  Testing Matrix multiplication... ";
    
    std::vector<std::vector<double>> a_data = {
        {1, 2},
        {3, 4}
    };
    std::vector<std::vector<double>> b_data = {
        {5, 6},
        {7, 8}
    };
    
    Matrix a(a_data);
    Matrix b(b_data);
    Matrix c = a * b;
    
    // [1,2; 3,4] * [5,6; 7,8] = [19,22; 43,50]
    assert(approxEqual(c(0, 0), 19.0));
    assert(approxEqual(c(0, 1), 22.0));
    assert(approxEqual(c(1, 0), 43.0));
    assert(approxEqual(c(1, 1), 50.0));
    
    std::cout << "PASSED\n";
}

void test_matrix_inverse() {
    std::cout << "  Testing Matrix inverse... ";
    
    std::vector<std::vector<double>> data = {
        {4, 7},
        {2, 6}
    };
    
    Matrix m(data);
    Matrix inv = m.inverse();
    
    // Inverse of [4,7; 2,6] = [0.6, -0.7; -0.2, 0.4]
    assert(approxEqual(inv(0, 0), 0.6));
    assert(approxEqual(inv(0, 1), -0.7));
    assert(approxEqual(inv(1, 0), -0.2));
    assert(approxEqual(inv(1, 1), 0.4));
    
    // Verify: M * M^-1 = I
    Matrix identity = m * inv;
    assert(approxEqual(identity(0, 0), 1.0));
    assert(approxEqual(identity(1, 1), 1.0));
    assert(approxEqual(identity(0, 1), 0.0, 0.01));
    assert(approxEqual(identity(1, 0), 0.0, 0.01));
    
    std::cout << "PASSED\n";
}

// ==================== Linear Regression Tests ====================

void test_linear_regression_simple() {
    std::cout << "  Testing LinearRegression simple fit... ";
    
    // Simple linear relationship: y = 2x + 1
    std::vector<std::vector<double>> X = {{1}, {2}, {3}, {4}, {5}};
    std::vector<double> y = {3, 5, 7, 9, 11};  // y = 2x + 1
    
    LinearRegression lr;
    lr.fit(X, y);
    
    auto coeffs = lr.getCoefficients();
    // Should have 2 coefficients: intercept (1) and slope (2)
    assert(coeffs.size() == 2);
    assert(approxEqual(coeffs[0], 1.0, 0.1));  // Intercept ≈ 1
    assert(approxEqual(coeffs[1], 2.0, 0.1));  // Slope ≈ 2
    
    // Test prediction
    double pred = lr.predictOne({6});
    assert(approxEqual(pred, 13.0, 0.1));  // y = 2*6 + 1 = 13
    
    std::cout << "PASSED\n";
}

void test_linear_regression_multivariate() {
    std::cout << "  Testing LinearRegression multivariate... ";
    
    // y = 1 + 2*x1 + 3*x2
    std::vector<std::vector<double>> X = {
        {1, 1},   // y = 1 + 2 + 3 = 6
        {2, 1},   // y = 1 + 4 + 3 = 8
        {1, 2},   // y = 1 + 2 + 6 = 9
        {2, 2},   // y = 1 + 4 + 6 = 11
        {3, 3}    // y = 1 + 6 + 9 = 16
    };
    std::vector<double> y = {6, 8, 9, 11, 16};
    
    LinearRegression lr;
    lr.fit(X, y);
    
    auto coeffs = lr.getCoefficients();
    assert(coeffs.size() == 3);  // Intercept + 2 features
    
    // Test prediction
    double pred = lr.predictOne({4, 4});  // y = 1 + 8 + 12 = 21
    assert(approxEqual(pred, 21.0, 0.5));
    
    std::cout << "PASSED\n";
}

// ==================== Evaluator Tests ====================

void test_evaluator_rmse() {
    std::cout << "  Testing Evaluator RMSE... ";
    
    std::vector<double> actual = {3, 5, 7, 9, 11};
    std::vector<double> predicted = {3, 5, 7, 9, 11};  // Perfect predictions
    
    double rmse = Evaluator::calculateRMSE(actual, predicted);
    assert(approxEqual(rmse, 0.0));
    
    // Test with some error
    std::vector<double> predicted2 = {4, 6, 8, 10, 12};  // All off by 1
    double rmse2 = Evaluator::calculateRMSE(actual, predicted2);
    assert(approxEqual(rmse2, 1.0));
    
    std::cout << "PASSED\n";
}

void test_evaluator_mae() {
    std::cout << "  Testing Evaluator MAE... ";
    
    std::vector<double> actual = {100, 200, 300};
    std::vector<double> predicted = {110, 190, 305};  // Errors: 10, 10, 5
    
    double mae = Evaluator::calculateMAE(actual, predicted);
    assert(approxEqual(mae, 8.333, 0.01));  // (10 + 10 + 5) / 3
    
    std::cout << "PASSED\n";
}

void test_evaluator_mape() {
    std::cout << "  Testing Evaluator MAPE... ";
    
    std::vector<double> actual = {100, 200, 400};
    std::vector<double> predicted = {110, 220, 440};  // All 10% off
    
    double mape = Evaluator::calculateMAPE(actual, predicted);
    assert(approxEqual(mape, 10.0));  // 10%
    
    std::cout << "PASSED\n";
}

// ==================== Baseline Model Tests ====================

void test_baseline_model() {
    std::cout << "  Testing BaselineModel prediction... ";
    
    BaselineModel model(3);  // 3-day moving average
    
    std::vector<double> closes = {100, 102, 104, 106, 108};
    double pred = model.predict(closes);
    
    // Average of last 3: (104 + 106 + 108) / 3 = 106
    assert(approxEqual(pred, 106.0));
    
    std::cout << "PASSED\n";
}

// ==================== Main ====================

int main() {
    std::cout << "\n";
    std::cout << "========================================\n";
    std::cout << "    StockSense Unit Tests\n";
    std::cout << "========================================\n\n";
    
    int passed = 0;
    int failed = 0;
    
    // Data Cleaner Tests
    std::cout << "DataCleaner Tests:\n";
    try { test_data_cleaner_basic(); passed++; } catch (...) { failed++; std::cout << "FAILED\n"; }
    try { test_data_cleaner_removes_zero_volume(); passed++; } catch (...) { failed++; std::cout << "FAILED\n"; }
    
    // Feature Engine Tests
    std::cout << "\nFeatureEngine Tests:\n";
    try { test_feature_engine_sma(); passed++; } catch (...) { failed++; std::cout << "FAILED\n"; }
    try { test_feature_engine_daily_return(); passed++; } catch (...) { failed++; std::cout << "FAILED\n"; }
    
    // Matrix Tests
    std::cout << "\nMatrix Tests:\n";
    try { test_matrix_creation(); passed++; } catch (...) { failed++; std::cout << "FAILED\n"; }
    try { test_matrix_transpose(); passed++; } catch (...) { failed++; std::cout << "FAILED\n"; }
    try { test_matrix_multiply(); passed++; } catch (...) { failed++; std::cout << "FAILED\n"; }
    try { test_matrix_inverse(); passed++; } catch (...) { failed++; std::cout << "FAILED\n"; }
    
    // Linear Regression Tests
    std::cout << "\nLinearRegression Tests:\n";
    try { test_linear_regression_simple(); passed++; } catch (...) { failed++; std::cout << "FAILED\n"; }
    try { test_linear_regression_multivariate(); passed++; } catch (...) { failed++; std::cout << "FAILED\n"; }
    
    // Evaluator Tests
    std::cout << "\nEvaluator Tests:\n";
    try { test_evaluator_rmse(); passed++; } catch (...) { failed++; std::cout << "FAILED\n"; }
    try { test_evaluator_mae(); passed++; } catch (...) { failed++; std::cout << "FAILED\n"; }
    try { test_evaluator_mape(); passed++; } catch (...) { failed++; std::cout << "FAILED\n"; }
    
    // Baseline Model Tests
    std::cout << "\nBaselineModel Tests:\n";
    try { test_baseline_model(); passed++; } catch (...) { failed++; std::cout << "FAILED\n"; }
    
    // Summary
    std::cout << "\n========================================\n";
    std::cout << "Results: " << passed << " passed, " << failed << " failed\n";
    std::cout << "========================================\n\n";
    
    return failed > 0 ? 1 : 0;
}
