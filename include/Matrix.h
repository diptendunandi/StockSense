#ifndef MATRIX_H
#define MATRIX_H

#include <vector>
#include <stdexcept>
#include <cmath>
#include <string>

namespace StockSense {

/**
 * @brief Simple Matrix class for Linear Algebra operations
 * 
 * Implements basic matrix operations needed for Linear Regression:
 * - Transpose
 * - Multiplication
 * - Inverse (using Gauss-Jordan elimination)
 * - Addition/Subtraction
 * 
 * All operations implemented from scratch - no external libraries.
 * This is intentionally simple for interview discussions.
 */
class Matrix {
public:
    /**
     * @brief Construct an empty matrix
     */
    Matrix();
    
    /**
     * @brief Construct a matrix with given dimensions
     * @param rows Number of rows
     * @param cols Number of columns
     * @param initValue Initial value for all elements (default: 0)
     */
    Matrix(size_t rows, size_t cols, double initValue = 0.0);
    
    /**
     * @brief Construct from 2D vector
     * @param data 2D vector of values
     */
    explicit Matrix(const std::vector<std::vector<double>>& data);
    
    /**
     * @brief Construct from 1D vector (column vector)
     * @param data 1D vector of values
     */
    explicit Matrix(const std::vector<double>& data);
    
    // Accessors
    size_t rows() const { return rows_; }
    size_t cols() const { return cols_; }
    bool empty() const { return data_.empty(); }
    
    /**
     * @brief Access element at (row, col)
     */
    double& operator()(size_t row, size_t col);
    double operator()(size_t row, size_t col) const;
    
    /**
     * @brief Get underlying data
     */
    const std::vector<std::vector<double>>& data() const { return data_; }
    
    // Matrix Operations
    
    /**
     * @brief Transpose the matrix
     * @return Matrix Transposed matrix (M^T)
     */
    Matrix transpose() const;
    
    /**
     * @brief Matrix multiplication
     * @param other Right-hand matrix
     * @return Matrix Result of this * other
     * @throws std::invalid_argument if dimensions don't match
     */
    Matrix multiply(const Matrix& other) const;
    
    /**
     * @brief Matrix multiplication operator
     */
    Matrix operator*(const Matrix& other) const { return multiply(other); }
    
    /**
     * @brief Calculate matrix inverse using Gauss-Jordan elimination
     * @return Matrix Inverse matrix (M^-1)
     * @throws std::runtime_error if matrix is singular (not invertible)
     */
    Matrix inverse() const;
    
    /**
     * @brief Matrix addition
     */
    Matrix operator+(const Matrix& other) const;
    
    /**
     * @brief Matrix subtraction
     */
    Matrix operator-(const Matrix& other) const;
    
    /**
     * @brief Scalar multiplication
     */
    Matrix operator*(double scalar) const;
    
    /**
     * @brief Create identity matrix
     * @param size Dimension of the square identity matrix
     * @return Matrix Identity matrix
     */
    static Matrix identity(size_t size);
    
    /**
     * @brief Convert column vector to std::vector
     * @return std::vector<double> Vector of values
     * @throws std::runtime_error if not a column vector
     */
    std::vector<double> toVector() const;
    
    /**
     * @brief Print matrix for debugging
     */
    std::string toString() const;
    
private:
    size_t rows_;
    size_t cols_;
    std::vector<std::vector<double>> data_;
    
    /**
     * @brief Swap two rows (used in Gauss-Jordan elimination)
     */
    void swapRows(size_t row1, size_t row2);
    
    /**
     * @brief Scale a row by a factor
     */
    void scaleRow(size_t row, double factor);
    
    /**
     * @brief Add a multiple of one row to another
     */
    void addScaledRow(size_t targetRow, size_t sourceRow, double factor);
};

} // namespace StockSense

#endif // MATRIX_H
