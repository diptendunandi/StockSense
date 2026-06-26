# StockSense: CSV-Based Stock Price Predictor

A **C++17** stock price prediction engine that implements **Linear Regression from scratch** using the Normal Equation. No external ML libraries required.

## Features

- **CSV Parser**: Custom parser for Yahoo Finance format stock data
- **Data Cleaning**: Forward-fill imputation, outlier detection, zero-volume filtering
- **Feature Engineering**: SMA-5, SMA-20, Daily Returns, Price Range, Volume Change
- **Linear Regression**: Implemented from scratch using the Normal Equation
- **Baseline Model**: Moving Average for model comparison
- **Evaluation Metrics**: RMSE, MAE, MAPE
- **Time-Aware Split**: Proper train/test split for time-series data

## Project Structure

```
StockSense/
├── CMakeLists.txt           # Build configuration
├── README.md                # This file
├── data/
│   └── AAPL.csv            # Sample Apple stock data
├── output/
│   └── predictions.csv     # Generated predictions
├── include/
│   ├── StockData.h         # Data structures
│   ├── CSVParser.h         # CSV file parser
│   ├── DataCleaner.h       # Data preprocessing
│   ├── FeatureEngine.h     # Technical indicators
│   ├── Matrix.h            # Matrix operations
│   ├── LinearRegression.h  # ML model
│   ├── BaselineModel.h     # Baseline comparison
│   ├── Evaluator.h         # Metrics calculation
│   ├── Predictor.h         # Pipeline orchestrator
│   └── Display.h           # Console output
├── src/
│   ├── main.cpp            # Entry point + CLI
│   ├── CSVParser.cpp
│   ├── DataCleaner.cpp
│   ├── FeatureEngine.cpp
│   ├── Matrix.cpp
│   ├── LinearRegression.cpp
│   ├── BaselineModel.cpp
│   ├── Evaluator.cpp
│   ├── Predictor.cpp
│   └── Display.cpp
└── tests/
    └── test_main.cpp       # Unit tests
```

## Building

### Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.14+

### Build Instructions

```bash
# Create build directory
mkdir build && cd build

# Configure
cmake ..

# Build
make -j$(nproc)

# Run tests
./stocksense_tests

# Run main program
./bin/stocksense --file ../data/AAPL.csv --days 5
```

## Usage

```bash
./stocksense [OPTIONS]

Options:
  -f, --file <path>     Input CSV file (default: data/AAPL.csv)
  -o, --output <path>   Output predictions CSV (default: output/predictions.csv)
  -d, --days <N>        Number of future days to predict (default: 5)
  -s, --split <ratio>   Train/test split ratio (default: 0.8)
  -m, --model <type>    Model type: 'lr' or 'ma' (default: lr)
  -v, --verbose         Enable verbose output
  -h, --help            Show help message

Examples:
  ./stocksense --file data/AAPL.csv --days 7
  ./stocksense -f data/MSFT.csv -d 10 -v
```

## Sample Output

```
========================================================
         StockSense - Stock Price Predictor           
========================================================
 Dataset:        data/AAPL.csv
 Training Set:   2023-01-03 to 2023-04-14
 Testing Set:    2023-04-17 to 2023-05-26
 Features Used:  SMA5, SMA20, DailyReturn,
                 PriceRange, VolumeChange
--------------------------------------------------------
 MODEL COMPARISON:

 Moving Average Baseline:
   RMSE: $3.45  |  MAE: $2.89  |  MAPE: 4.1%

 Linear Regression:
   RMSE: $2.21  |  MAE: $1.76  |  MAPE: 2.8%

 [OK] Linear Regression wins by 32% (MAPE)
--------------------------------------------------------
 NEXT 5-DAY PREDICTION:
   Day 1: $176.12  |  Day 2: $176.45
   Day 3: $176.78  |  Day 4: $177.01
   Day 5: $177.25
========================================================
```

## Technical Details

### Linear Regression Implementation

The model uses the **Normal Equation** (closed-form solution):

```
θ = (XᵀX)⁻¹Xᵀy
```

Where:
- `X` = Feature matrix with bias column
- `y` = Target vector (closing prices)
- `θ` = Learned coefficients

### Features

| Feature | Description | Interview Explanation |
|---------|-------------|----------------------|
| SMA-5 | 5-day Simple Moving Average | "Smooths short-term noise" |
| SMA-20 | 20-day Simple Moving Average | "Captures longer trends" |
| Daily Return | `(close - prev_close) / prev_close * 100` | "Daily momentum" |
| Price Range | `high - low` | "Intraday volatility" |
| Volume Change | `(vol - prev_vol) / prev_vol * 100` | "Trading activity change" |

### Time-Aware Train/Test Split

**Critical for time-series data**: Training data comes BEFORE test data chronologically. Random shuffling would cause **data leakage**.

## Interview Talking Points

1. **Why C++ instead of Python?**
   > "I wanted to understand the math behind ML by implementing it myself. C++ forces you to handle memory and matrix operations manually."

2. **Why no external libraries?**
   > "I implemented the Normal Equation from scratch to show I understand HOW linear regression works, not just how to call sklearn.fit()."

3. **How do you handle time-series data?**
   > "Time-based split, not random shuffle. Shuffling causes data leakage where the model trains on future data."

4. **How do you know the model works?**
   > "I compare against a Moving Average baseline and measure RMSE, MAE, and MAPE. My model beat baseline by 32% on MAPE."

## Getting Stock Data

Download from [Kaggle](https://www.kaggle.com/datasets):
- Search "AAPL stock" or "Apple stock data"
- Download CSV in Yahoo Finance format:
  ```
  Date,Open,High,Low,Close,Adj Close,Volume
  ```

## License

MIT License - Feel free to use for learning and interviews!

## Author

Built as a portfolio project demonstrating:
- C++17 programming
- Machine Learning fundamentals
- Data preprocessing
- Clean code architecture
