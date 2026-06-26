#ifndef CSVPARSER_H
#define CSVPARSER_H

#include "StockData.h"
#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>

namespace StockSense {

/**
 * @brief CSV Parser for stock data files
 * 
 * Parses CSV files in the standard Yahoo Finance format:
 * Date,Open,High,Low,Close,Adj Close,Volume
 * 
 * Features:
 * - Header detection and validation
 * - Robust error handling for malformed rows
 * - Support for different date formats
 * - Memory-efficient streaming parser
 */
class CSVParser {
public:
    /**
     * @brief Construct a new CSV Parser
     * @param filename Path to the CSV file
     */
    explicit CSVParser(const std::string& filename);
    
    /**
     * @brief Parse the entire CSV file
     * @return std::vector<StockData> Vector of parsed stock data rows
     * @throws std::runtime_error if file cannot be opened or parsed
     */
    std::vector<StockData> parse();
    
    /**
     * @brief Get the number of rows parsed (excluding header)
     */
    size_t getRowCount() const { return rowCount_; }
    
    /**
     * @brief Get the number of rows that failed to parse
     */
    size_t getErrorCount() const { return errorCount_; }
    
    /**
     * @brief Check if file was successfully opened
     */
    bool isOpen() const { return fileOpen_; }
    
private:
    std::string filename_;
    size_t rowCount_;
    size_t errorCount_;
    bool fileOpen_;
    
    /**
     * @brief Split a line by delimiter
     * @param line The line to split
     * @param delimiter The delimiter character
     * @return std::vector<std::string> Vector of fields
     */
    std::vector<std::string> splitLine(const std::string& line, char delimiter = ',');
    
    /**
     * @brief Parse a single row into StockData
     * @param fields Vector of string fields from CSV
     * @param data Output StockData struct
     * @return true if parsing succeeded, false otherwise
     */
    bool parseRow(const std::vector<std::string>& fields, StockData& data);
    
    /**
     * @brief Trim whitespace from string
     */
    std::string trim(const std::string& str);
    
    /**
     * @brief Validate header row
     * @param header The header row fields
     * @return true if header is valid
     */
    bool validateHeader(const std::vector<std::string>& header);
};

} // namespace StockSense

#endif // CSVPARSER_H
