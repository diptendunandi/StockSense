#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <iomanip>

#include "StockData.h"
#include "CSVParser.h"
#include "DataCleaner.h"
#include "FeatureEngine.h"
#include "LinearRegression.h"
#include "BaselineModel.h"
#include "Evaluator.h"
#include "Predictor.h"
#include "Display.h"

using namespace StockSense;

/**
 * @brief Print usage information
 */
void printUsage(const char* programName) {
    std::cout << "\nUsage: " << programName << " [OPTIONS]\n\n";
    std::cout << "Options:\n";
    std::cout << "  -f, --file <path>     Input CSV file (default: data/AAPL.csv)\n";
    std::cout << "  -o, --output <path>   Output predictions CSV (default: output/predictions.csv)\n";
    std::cout << "  -d, --days <N>        Number of future days to predict (default: 5)\n";
    std::cout << "  -s, --split <ratio>   Train/test split ratio (default: 0.8)\n";
    std::cout << "  -m, --model <type>    Model type: 'lr' (Linear Regression) or 'ma' (Moving Average)\n";
    std::cout << "                        (default: lr)\n";
    std::cout << "  -v, --verbose         Enable verbose output\n";
    std::cout << "  -h, --help            Show this help message\n";
    std::cout << "\nExamples:\n";
    std::cout << "  " << programName << " --file data/AAPL.csv --days 7\n";
    std::cout << "  " << programName << " -f data/MSFT.csv -d 10 -v\n";
    std::cout << "  " << programName << " --file data/TSLA.csv --split 0.75\n\n";
}

/**
 * @brief Parse command line arguments
 */
Config parseArgs(int argc, char* argv[]) {
    Config config;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            exit(0);
        }
        else if ((arg == "-f" || arg == "--file") && i + 1 < argc) {
            config.inputFile = argv[++i];
        }
        else if ((arg == "-o" || arg == "--output") && i + 1 < argc) {
            config.outputFile = argv[++i];
        }
        else if ((arg == "-d" || arg == "--days") && i + 1 < argc) {
            config.predictionDays = std::stoi(argv[++i]);
        }
        else if ((arg == "-s" || arg == "--split") && i + 1 < argc) {
            config.trainTestSplit = std::stod(argv[++i]);
        }
        else if ((arg == "-m" || arg == "--model") && i + 1 < argc) {
            config.modelType = argv[++i];
        }
        else if (arg == "-v" || arg == "--verbose") {
            config.verbose = true;
        }
        else if (arg[0] == '-') {
            std::cerr << "Unknown option: " << arg << "\n";
            printUsage(argv[0]);
            exit(1);
        }
    }
    
    // Validate configuration
    if (config.trainTestSplit <= 0.0 || config.trainTestSplit >= 1.0) {
        std::cerr << "Error: Split ratio must be between 0 and 1\n";
        exit(1);
    }
    
    if (config.predictionDays < 1 || config.predictionDays > 365) {
        std::cerr << "Error: Prediction days must be between 1 and 365\n";
        exit(1);
    }
    
    return config;
}

/**
 * @brief Export predictions to CSV
 */
void exportResults(const std::string& filename, 
                   const std::vector<PredictionResult>& results,
                   const std::vector<EnhancedStockData>& data) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        Display::printError("Could not create output file: " + filename);
        return;
    }
    
    // Write header
    file << "Date,Actual_Close,Predicted_Close,Error,Error_Percent,SMA5,SMA20\n";
    
    // Find matching enhanced data for each result
    for (const auto& result : results) {
        file << result.date << ","
             << std::fixed << std::setprecision(2)
             << result.actualClose << ","
             << result.predictedClose << ","
             << result.error << ","
             << result.errorPercent;
        
        // Find SMA values for this date
        for (const auto& d : data) {
            if (d.raw.date == result.date) {
                file << "," << d.sma5 << "," << d.sma20;
                break;
            }
        }
        file << "\n";
    }
    
    file.close();
}

/**
 * @brief Main entry point
 */
int main(int argc, char* argv[]) {
    try {
        // Parse command line arguments
        Config config = parseArgs(argc, argv);
        
        Display::printProgress("Starting StockSense...");
        
        // Step 1: Parse CSV
        Display::printProgress("Loading data from " + config.inputFile);
        CSVParser parser(config.inputFile);
        std::vector<StockData> rawData = parser.parse();
        
        if (rawData.empty()) {
            Display::printError("No data loaded from file");
            return 1;
        }
        
        Display::printSuccess("Loaded " + std::to_string(rawData.size()) + " rows");
        
        // Step 2: Clean data
        Display::printProgress("Cleaning data...");
        DataCleaner cleaner;
        std::vector<StockData> cleanedData = cleaner.clean(rawData);
        
        auto stats = cleaner.getStats();
        if (config.verbose) {
            Display::printCleaningStats(
                stats.originalCount, 
                stats.cleanedCount,
                stats.missingValuesFilled,
                stats.outliersRemoved);
        }
        Display::printSuccess("Cleaned data: " + std::to_string(cleanedData.size()) + " rows");
        
        // Step 3: Feature engineering
        Display::printProgress("Computing features...");
        FeatureEngine featureEngine(config.smaShortWindow, config.smaLongWindow);
        std::vector<EnhancedStockData> enhancedData = featureEngine.computeFeatures(cleanedData);
        
        auto completeData = featureEngine.getCompleteRows(enhancedData);
        Display::printSuccess("Features computed: " + std::to_string(completeData.size()) + 
                              " rows with complete features");
        
        // Step 4: Train and evaluate
        Display::printProgress("Training models...");
        Predictor predictor(config);
        std::vector<PredictionResult> testResults = predictor.run(enhancedData);
        Display::printSuccess("Models trained and evaluated");
        
        // Step 5: Generate future predictions
        Display::printProgress("Generating " + std::to_string(config.predictionDays) + 
                               "-day forecast...");
        std::vector<PredictionResult> futurePredictions = 
            predictor.predictFuture(enhancedData, config.predictionDays);
        
        // Step 6: Export results
        Display::printProgress("Exporting results to " + config.outputFile);
        exportResults(config.outputFile, testResults, enhancedData);
        Display::printSuccess("Results exported");
        
        // Step 7: Display dashboard
        Display::printDashboard(
            config,
            predictor.getTrainDateRange(),
            predictor.getTestDateRange(),
            predictor.getBaselineMetrics(),
            predictor.getLRMetrics(),
            futurePredictions);
        
        // Show coefficients if verbose
        if (config.verbose) {
            auto coefficients = predictor.getCoefficients();
            auto featureNames = featureEngine.getFeatureNames();
            Display::printCoefficients(coefficients, featureNames);
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        Display::printError(std::string("Error: ") + e.what());
        return 1;
    }
}
