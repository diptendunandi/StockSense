#ifndef DATACLEANER_H
#define DATACLEANER_H

#include "StockData.h"
#include <vector>
#include <functional>

namespace StockSense {

/**
 * @brief Data Cleaner for stock data preprocessing
 * 
 * Handles:
 * - Missing value imputation (forward-fill)
 * - Outlier detection and removal
 * - Zero-volume day filtering
 * - Data validation
 * - Sorting by date
 */
class DataCleaner {
public:
    /**
     * @brief Cleaning statistics
     */
    struct CleaningStats {
        size_t originalCount;       // Original number of rows
        size_t cleanedCount;        // Final number of rows
        size_t missingValuesFilled; // Number of forward-fills performed
        size_t outliersRemoved;     // Number of outlier rows removed
        size_t zeroVolumeRemoved;   // Number of zero-volume days removed
        size_t invalidRowsRemoved;  // Number of invalid rows removed
        
        CleaningStats() 
            : originalCount(0), cleanedCount(0), missingValuesFilled(0),
              outliersRemoved(0), zeroVolumeRemoved(0), invalidRowsRemoved(0) {}
    };
    
    /**
     * @brief Construct a new Data Cleaner
     * @param outlierStdDev Number of standard deviations for outlier detection (default: 3.0)
     */
    explicit DataCleaner(double outlierStdDev = 3.0);
    
    /**
     * @brief Clean the stock data
     * @param data Vector of raw stock data
     * @return std::vector<StockData> Cleaned data
     */
    std::vector<StockData> clean(const std::vector<StockData>& data);
    
    /**
     * @brief Get cleaning statistics from last clean() call
     */
    CleaningStats getStats() const { return stats_; }
    
    /**
     * @brief Enable/disable removal of zero-volume days
     */
    void setRemoveZeroVolume(bool remove) { removeZeroVolume_ = remove; }
    
    /**
     * @brief Enable/disable outlier removal
     */
    void setRemoveOutliers(bool remove) { removeOutliers_ = remove; }
    
private:
    double outlierStdDev_;
    bool removeZeroVolume_;
    bool removeOutliers_;
    CleaningStats stats_;
    
    /**
     * @brief Remove invalid rows (missing required fields)
     */
    std::vector<StockData> removeInvalidRows(const std::vector<StockData>& data);
    
    /**
     * @brief Forward-fill missing values
     * Uses the previous row's value if current value is 0 or NaN
     */
    void forwardFillMissing(std::vector<StockData>& data);
    
    /**
     * @brief Detect and remove outliers using Z-score method
     * An outlier is any day where daily return exceeds N standard deviations
     */
    std::vector<StockData> removeOutliers(const std::vector<StockData>& data);
    
    /**
     * @brief Remove days with zero trading volume
     */
    std::vector<StockData> removeZeroVolumeDays(const std::vector<StockData>& data);
    
    /**
     * @brief Sort data by date in ascending order
     */
    void sortByDate(std::vector<StockData>& data);
    
    /**
     * @brief Calculate mean of a series
     */
    double calculateMean(const std::vector<double>& values);
    
    /**
     * @brief Calculate standard deviation of a series
     */
    double calculateStdDev(const std::vector<double>& values, double mean);
};

} // namespace StockSense

#endif // DATACLEANER_H
