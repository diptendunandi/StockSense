#ifndef LINEARREGRESSION_H
#define LINEARREGRESSION_H

#include "Matrix.h"
#include "StockData.h"
#include <vector>

namespace StockSense {

/**
 * @brief Linear Regression model implemented from scratch
 * 
 * Uses the Normal Equation (closed-form solution):
 *   θ = (X^T * X)^(-1) * X^T * y
 * 
 * Where:
 *   X = feature matrix (with bias column)
 *   y = target vector (close prices)
 *   θ = learned coefficients
 * 
 * This implementation requires NO external ML libraries.
 * Perfect for interview discussions about understanding ML fundamentals.
 */
class LinearRegression {
public:
    /**
     * @brief Construct Linear Regression model
     * @param addBias Whether to add a bias term (intercept). Default: true
     */
    explicit LinearRegression(bool addBias = true);
    
    /**
     * @brief Train the model on feature matrix and target vector
     * @param X Feature matrix (n_samples x n_features)
     * @param y Target vector (n_samples)
     * 
     * Learns θ using the Normal Equation.
     * 
     * @throws std::runtime_error if X^T*X is not invertible
     */
    void fit(const std::vector<std::vector<double>>& X, const std::vector<double>& y);
    
    /**
     * @brief Predict target values for new feature vectors
     * @param X Feature matrix for prediction (n_samples x n_features)
     * @return std::vector<double> Predicted values
     */
    std::vector<double> predict(const std::vector<std::vector<double>>& X) const;
    
    /**
     * @brief Predict a single sample
     * @param x Feature vector (n_features)
     * @return double Predicted value
     */
    double predictOne(const std::vector<double>& x) const;
    
    /**
     * @brief Get learned coefficients (including bias if enabled)
     * @return std::vector<double> Coefficient vector θ
     * 
     * If addBias=true, first element is the intercept (θ₀)
     */
    std::vector<double> getCoefficients() const;
    
    /**
     * @brief Get coefficient names for display
     * @param featureNames Names of input features
     * @return std::vector<std::string> Names including bias if applicable
     */
    std::vector<std::string> getCoefficientNames(const std::vector<std::string>& featureNames) const;
    
    /**
     * @brief Check if model has been trained
     */
    bool isTrained() const { return trained_; }
    
    /**
     * @brief Calculate R² score (coefficient of determination)
     * @param X Feature matrix
     * @param y Actual target values
     * @return double R² score (1.0 = perfect fit, 0.0 = no better than mean)
     */
    double score(const std::vector<std::vector<double>>& X, const std::vector<double>& y) const;
    
private:
    bool addBias_;
    bool trained_;
    std::vector<double> coefficients_;  // θ vector
    
    /**
     * @brief Add bias column (column of 1s) to feature matrix
     * @param X Original feature matrix
     * @return Matrix with bias column prepended
     */
    std::vector<std::vector<double>> addBiasColumn(const std::vector<std::vector<double>>& X) const;
};

} // namespace StockSense

#endif // LINEARREGRESSION_H
