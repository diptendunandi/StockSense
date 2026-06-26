#include "LinearRegression.h"
#include <cmath>
#include <numeric>
#include <stdexcept>

namespace StockSense {

LinearRegression::LinearRegression(bool addBias)
    : addBias_(addBias), trained_(false) {
}

void LinearRegression::fit(const std::vector<std::vector<double>>& X, 
                           const std::vector<double>& y) {
    if (X.empty() || y.empty()) {
        throw std::invalid_argument("Training data cannot be empty");
    }
    
    if (X.size() != y.size()) {
        throw std::invalid_argument("X and y must have same number of samples");
    }
    
    // Optionally add bias column
    std::vector<std::vector<double>> X_train = addBias_ ? addBiasColumn(X) : X;
    
    // Convert to Matrix objects
    Matrix X_mat(X_train);
    Matrix y_mat(y);
    
    // Normal Equation: θ = (X^T * X)^(-1) * X^T * y
    // Step 1: X^T (transpose)
    Matrix X_T = X_mat.transpose();
    
    // Step 2: X^T * X
    Matrix XTX = X_T * X_mat;
    
    // Step 3: (X^T * X)^(-1)
    Matrix XTX_inv;
    try {
        XTX_inv = XTX.inverse();
    } catch (const std::runtime_error& e) {
        throw std::runtime_error(
            "Failed to fit Linear Regression: " + std::string(e.what()) +
            "\nConsider normalizing your features or removing correlated ones.");
    }
    
    // Step 4: (X^T * X)^(-1) * X^T
    Matrix XTX_inv_XT = XTX_inv * X_T;
    
    // Step 5: θ = (X^T * X)^(-1) * X^T * y
    Matrix theta = XTX_inv_XT * y_mat;
    
    // Store coefficients
    coefficients_ = theta.toVector();
    trained_ = true;
}

std::vector<double> LinearRegression::predict(const std::vector<std::vector<double>>& X) const {
    if (!trained_) {
        throw std::runtime_error("Model has not been trained. Call fit() first.");
    }
    
    std::vector<double> predictions;
    predictions.reserve(X.size());
    
    for (const auto& sample : X) {
        predictions.push_back(predictOne(sample));
    }
    
    return predictions;
}

double LinearRegression::predictOne(const std::vector<double>& x) const {
    if (!trained_) {
        throw std::runtime_error("Model has not been trained. Call fit() first.");
    }
    
    // Add bias term if needed
    std::vector<double> features = addBias_ ? std::vector<double>{1.0} : std::vector<double>{};
    if (addBias_) {
        features.insert(features.end(), x.begin(), x.end());
    } else {
        features = x;
    }
    
    // Check dimensions
    if (features.size() != coefficients_.size()) {
        throw std::invalid_argument(
            "Feature vector size (" + std::to_string(features.size()) + 
            ") doesn't match coefficient size (" + std::to_string(coefficients_.size()) + ")");
    }
    
    // Calculate prediction: ŷ = θ₀ + θ₁x₁ + θ₂x₂ + ... + θₙxₙ
    double prediction = 0.0;
    for (size_t i = 0; i < features.size(); ++i) {
        prediction += coefficients_[i] * features[i];
    }
    
    return prediction;
}

std::vector<double> LinearRegression::getCoefficients() const {
    return coefficients_;
}

std::vector<std::string> LinearRegression::getCoefficientNames(
        const std::vector<std::string>& featureNames) const {
    std::vector<std::string> names;
    
    if (addBias_) {
        names.push_back("Intercept (bias)");
    }
    
    for (const auto& name : featureNames) {
        names.push_back(name);
    }
    
    return names;
}

double LinearRegression::score(const std::vector<std::vector<double>>& X,
                               const std::vector<double>& y) const {
    if (!trained_) {
        throw std::runtime_error("Model has not been trained. Call fit() first.");
    }
    
    std::vector<double> predictions = predict(X);
    
    // Calculate R² = 1 - (SS_res / SS_tot)
    // SS_res = Σ(y_i - ŷ_i)²
    // SS_tot = Σ(y_i - ȳ)²
    
    double y_mean = std::accumulate(y.begin(), y.end(), 0.0) / y.size();
    
    double ss_res = 0.0;
    double ss_tot = 0.0;
    
    for (size_t i = 0; i < y.size(); ++i) {
        ss_res += std::pow(y[i] - predictions[i], 2);
        ss_tot += std::pow(y[i] - y_mean, 2);
    }
    
    if (ss_tot == 0.0) {
        return 1.0;  // Perfect fit if no variance
    }
    
    return 1.0 - (ss_res / ss_tot);
}

std::vector<std::vector<double>> LinearRegression::addBiasColumn(
        const std::vector<std::vector<double>>& X) const {
    std::vector<std::vector<double>> result;
    result.reserve(X.size());
    
    for (const auto& row : X) {
        std::vector<double> newRow = {1.0};  // Bias term
        newRow.insert(newRow.end(), row.begin(), row.end());
        result.push_back(newRow);
    }
    
    return result;
}

} // namespace StockSense
