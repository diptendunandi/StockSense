#include "Matrix.h"
#include <sstream>
#include <iomanip>
#include <cmath>

namespace StockSense {

Matrix::Matrix() : rows_(0), cols_(0) {}

Matrix::Matrix(size_t rows, size_t cols, double initValue)
    : rows_(rows), cols_(cols), data_(rows, std::vector<double>(cols, initValue)) {
}

Matrix::Matrix(const std::vector<std::vector<double>>& data) {
    if (data.empty()) {
        rows_ = 0;
        cols_ = 0;
        return;
    }
    
    rows_ = data.size();
    cols_ = data[0].size();
    data_ = data;
}

Matrix::Matrix(const std::vector<double>& data) {
    rows_ = data.size();
    cols_ = 1;
    data_.resize(rows_);
    for (size_t i = 0; i < rows_; ++i) {
        data_[i] = {data[i]};
    }
}

double& Matrix::operator()(size_t row, size_t col) {
    if (row >= rows_ || col >= cols_) {
        throw std::out_of_range("Matrix index out of range");
    }
    return data_[row][col];
}

double Matrix::operator()(size_t row, size_t col) const {
    if (row >= rows_ || col >= cols_) {
        throw std::out_of_range("Matrix index out of range");
    }
    return data_[row][col];
}

Matrix Matrix::transpose() const {
    Matrix result(cols_, rows_);
    
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < cols_; ++j) {
            result(j, i) = data_[i][j];
        }
    }
    
    return result;
}

Matrix Matrix::multiply(const Matrix& other) const {
    if (cols_ != other.rows_) {
        throw std::invalid_argument("Matrix dimensions don't match for multiplication: "
            + std::to_string(rows_) + "x" + std::to_string(cols_) + " * "
            + std::to_string(other.rows_) + "x" + std::to_string(other.cols_));
    }
    
    Matrix result(rows_, other.cols_, 0.0);
    
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < other.cols_; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < cols_; ++k) {
                sum += data_[i][k] * other.data_[k][j];
            }
            result(i, j) = sum;
        }
    }
    
    return result;
}

Matrix Matrix::inverse() const {
    if (rows_ != cols_) {
        throw std::runtime_error("Cannot invert non-square matrix");
    }
    
    size_t n = rows_;
    
    // Create augmented matrix [A | I]
    Matrix augmented(n, 2 * n);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            augmented(i, j) = data_[i][j];
        }
        augmented(i, n + i) = 1.0;  // Identity matrix on the right
    }
    
    // Gauss-Jordan elimination
    for (size_t col = 0; col < n; ++col) {
        // Find pivot (largest absolute value in column)
        size_t maxRow = col;
        double maxVal = std::abs(augmented(col, col));
        
        for (size_t row = col + 1; row < n; ++row) {
            if (std::abs(augmented(row, col)) > maxVal) {
                maxVal = std::abs(augmented(row, col));
                maxRow = row;
            }
        }
        
        // Swap rows if needed
        if (maxRow != col) {
            augmented.swapRows(col, maxRow);
        }
        
        // Check for singularity
        double pivot = augmented(col, col);
        if (std::abs(pivot) < 1e-10) {
            throw std::runtime_error("Matrix is singular (not invertible). "
                "This usually means features are linearly dependent. "
                "Try adding regularization or removing correlated features.");
        }
        
        // Scale pivot row to make pivot = 1
        augmented.scaleRow(col, 1.0 / pivot);
        
        // Eliminate column in other rows
        for (size_t row = 0; row < n; ++row) {
            if (row != col) {
                double factor = -augmented(row, col);
                augmented.addScaledRow(row, col, factor);
            }
        }
    }
    
    // Extract inverse from augmented matrix
    Matrix result(n, n);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            result(i, j) = augmented(i, n + j);
        }
    }
    
    return result;
}

Matrix Matrix::operator+(const Matrix& other) const {
    if (rows_ != other.rows_ || cols_ != other.cols_) {
        throw std::invalid_argument("Matrix dimensions don't match for addition");
    }
    
    Matrix result(rows_, cols_);
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < cols_; ++j) {
            result(i, j) = data_[i][j] + other.data_[i][j];
        }
    }
    
    return result;
}

Matrix Matrix::operator-(const Matrix& other) const {
    if (rows_ != other.rows_ || cols_ != other.cols_) {
        throw std::invalid_argument("Matrix dimensions don't match for subtraction");
    }
    
    Matrix result(rows_, cols_);
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < cols_; ++j) {
            result(i, j) = data_[i][j] - other.data_[i][j];
        }
    }
    
    return result;
}

Matrix Matrix::operator*(double scalar) const {
    Matrix result(rows_, cols_);
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < cols_; ++j) {
            result(i, j) = data_[i][j] * scalar;
        }
    }
    return result;
}

Matrix Matrix::identity(size_t size) {
    Matrix result(size, size, 0.0);
    for (size_t i = 0; i < size; ++i) {
        result(i, i) = 1.0;
    }
    return result;
}

std::vector<double> Matrix::toVector() const {
    if (cols_ != 1) {
        throw std::runtime_error("Cannot convert non-column matrix to vector");
    }
    
    std::vector<double> result(rows_);
    for (size_t i = 0; i < rows_; ++i) {
        result[i] = data_[i][0];
    }
    
    return result;
}

std::string Matrix::toString() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(4);
    
    for (size_t i = 0; i < rows_; ++i) {
        ss << "[ ";
        for (size_t j = 0; j < cols_; ++j) {
            ss << std::setw(10) << data_[i][j] << " ";
        }
        ss << "]\n";
    }
    
    return ss.str();
}

void Matrix::swapRows(size_t row1, size_t row2) {
    std::swap(data_[row1], data_[row2]);
}

void Matrix::scaleRow(size_t row, double factor) {
    for (size_t j = 0; j < cols_; ++j) {
        data_[row][j] *= factor;
    }
}

void Matrix::addScaledRow(size_t targetRow, size_t sourceRow, double factor) {
    for (size_t j = 0; j < cols_; ++j) {
        data_[targetRow][j] += factor * data_[sourceRow][j];
    }
}

} // namespace StockSense
