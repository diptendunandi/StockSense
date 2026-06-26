#include "CSVParser.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>

namespace StockSense {

CSVParser::CSVParser(const std::string& filename)
    : filename_(filename), rowCount_(0), errorCount_(0), fileOpen_(false) {
}

std::vector<StockData> CSVParser::parse() {
    std::vector<StockData> result;
    std::ifstream file(filename_);
    
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename_);
    }
    
    fileOpen_ = true;
    std::string line;
    bool headerSkipped = false;
    
    while (std::getline(file, line)) {
        // Skip empty lines
        if (line.empty() || line.find_first_not_of(" \t\r\n") == std::string::npos) {
            continue;
        }
        
        std::vector<std::string> fields = splitLine(line);
        
        // Handle header row
        if (!headerSkipped) {
            if (validateHeader(fields)) {
                headerSkipped = true;
                continue;
            } else {
                // First row might be data if no header
                headerSkipped = true;
            }
        }
        
        // Parse data row
        StockData data;
        if (parseRow(fields, data)) {
            result.push_back(data);
            rowCount_++;
        } else {
            errorCount_++;
        }
    }
    
    file.close();
    return result;
}

std::vector<std::string> CSVParser::splitLine(const std::string& line, char delimiter) {
    std::vector<std::string> fields;
    std::stringstream ss(line);
    std::string field;
    
    while (std::getline(ss, field, delimiter)) {
        fields.push_back(trim(field));
    }
    
    return fields;
}

bool CSVParser::parseRow(const std::vector<std::string>& fields, StockData& data) {
    // Support two CSV formats:
    // Format 1 (Yahoo Finance): Date, Open, High, Low, Close, Adj Close, Volume (7 cols)
    // Format 2 (Kaggle):        Date, Close, High, Low, Open, Volume (6 cols)
    
    if (fields.size() < 6) {
        return false;
    }
    
    try {
        data.date = fields[0];
        
        // Validate date format (basic check)
        if (data.date.length() < 8) {
            return false;
        }
        
        if (fields.size() >= 7) {
            // Yahoo Finance format: Date, Open, High, Low, Close, Adj Close, Volume
            data.open = std::stod(fields[1]);
            data.high = std::stod(fields[2]);
            data.low = std::stod(fields[3]);
            data.close = std::stod(fields[4]);
            data.adjClose = std::stod(fields[5]);
            data.volume = std::stoll(fields[6]);
        } else {
            // Kaggle format: Date, Close, High, Low, Open, Volume
            data.close = std::stod(fields[1]);
            data.high = std::stod(fields[2]);
            data.low = std::stod(fields[3]);
            data.open = std::stod(fields[4]);
            data.volume = std::stoll(fields[5]);
            data.adjClose = data.close;  // Use close as adjusted close
        }
        
        // Basic validation
        if (data.open <= 0 || data.high <= 0 || data.low <= 0 || data.close <= 0) {
            return false;
        }
        
        // Sanity check: high >= low
        if (data.high < data.low) {
            return false;
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::string CSVParser::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n\"");
    if (start == std::string::npos) {
        return "";
    }
    size_t end = str.find_last_not_of(" \t\r\n\"");
    return str.substr(start, end - start + 1);
}

bool CSVParser::validateHeader(const std::vector<std::string>& header) {
    if (header.size() < 6) {
        return false;
    }
    
    // Check for common header names (case-insensitive)
    std::string firstField = header[0];
    std::transform(firstField.begin(), firstField.end(), firstField.begin(), ::tolower);
    
    // If first field looks like a date, this might be data, not header
    if (firstField.find("date") == std::string::npos &&
        firstField.find("time") == std::string::npos) {
        // Check if it looks like a date (contains - or /)
        if (firstField.find('-') != std::string::npos ||
            firstField.find('/') != std::string::npos) {
            return false;  // Probably data, not header
        }
    }
    
    return true;
}

} // namespace StockSense
