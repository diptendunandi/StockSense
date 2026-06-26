<div align="center">

# StockSense

### A High-Performance Stock Price Prediction Engine

[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg?style=flat&logo=cplusplus)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.14%2B-064F8C?style=flat&logo=cmake)](https://cmake.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20macOS%20%7C%20Windows-lightgrey.svg)]()

*A robust C++17 stock price prediction engine implementing Linear Regression from scratch using the Normal Equation. Zero external ML dependencies.*

[Features](#features) • [Installation](#installation) • [Usage](#usage) • [Architecture](#architecture) • [Documentation](#documentation)

---

</div>

## Overview

**StockSense** is a command-line stock price prediction tool built entirely in modern C++. Unlike typical ML projects that rely on Python libraries, this project implements all mathematical operations from the ground up—demonstrating deep understanding of both the underlying algorithms and systems programming.

### Why StockSense?

- **Educational Value**: Understand ML internals by seeing how Linear Regression actually works
- **No Black Boxes**: Every matrix operation, every calculation is transparent and accessible
- **Production-Ready Code**: Clean architecture, comprehensive error handling, and thorough testing
- **Performance**: Native C++ execution without interpreter overhead

---

## Features

<table>
<tr>
<td width="50%">

### Data Processing
- Custom CSV parser for Yahoo Finance format
- Forward-fill imputation for missing values
- Outlier detection & removal
- Zero-volume day filtering

</td>
<td width="50%">

### Machine Learning
- Linear Regression via Normal Equation
- Moving Average baseline model
- Time-aware train/test splitting
- Multiple evaluation metrics

</td>
</tr>
<tr>
<td width="50%">

### Feature Engineering
- SMA-5 & SMA-20 (Moving Averages)
- Daily Returns calculation
- Price Range (Volatility)
- Volume Change tracking

</td>
<td width="50%">

### Evaluation Metrics
- RMSE (Root Mean Square Error)
- MAE (Mean Absolute Error)
- MAPE (Mean Absolute Percentage Error)
- Model comparison reports

</td>
</tr>
</table>

---

## Installation

### Prerequisites

| Requirement | Version |
|-------------|---------|
| C++ Compiler | GCC 7+, Clang 5+, or MSVC 2017+ |
| CMake | 3.14 or higher |
| Make | Any recent version |

### Build from Source

```bash
# Clone the repository
git clone https://github.com/diptendunandi/StockSense.git
cd StockSense

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make -j$(nproc)

# Run tests to verify installation
./stocksense_tests

# Run the application
./bin/stocksense --file ../data/aapl_stock_data.csv --days 5
```

---

## Usage

### Command Line Interface

```bash
./stocksense [OPTIONS]
```

| Option | Description | Default |
|--------|-------------|---------|
| `-f, --file <path>` | Input CSV file path | `data/aapl_stock_data.csv` |
| `-o, --output <path>` | Output predictions file | `output/predictions.csv` |
| `-d, --days <N>` | Days to predict | `5` |
| `-s, --split <ratio>` | Train/test split ratio | `0.8` |
| `-m, --model <type>` | Model: `lr` (Linear Regression) or `ma` (Moving Average) | `lr` |
| `-v, --verbose` | Enable detailed output | `false` |
| `-h, --help` | Display help message | - |

### Examples

```bash
# Basic prediction with Apple stock data
./stocksense --file data/aapl_stock_data.csv --days 7

# Verbose output with custom split ratio
./stocksense -f data/MSFT.csv -d 10 -s 0.75 -v

# Compare with Moving Average baseline
./stocksense --file data/GOOGL.csv --model ma
```

### Sample Output

```
════════════════════════════════════════════════════════════
              StockSense - Stock Price Predictor            
════════════════════════════════════════════════════════════
 Dataset:        data/aapl_stock_data.csv
 Training Set:   2023-01-03 to 2023-04-14
 Testing Set:    2023-04-17 to 2023-05-26
 Features Used:  SMA5, SMA20, DailyReturn, PriceRange, VolumeChange
────────────────────────────────────────────────────────────
 MODEL COMPARISON:

 Moving Average Baseline:
   RMSE: $3.45  │  MAE: $2.89  │  MAPE: 4.1%

 Linear Regression:
   RMSE: $2.21  │  MAE: $1.76  │  MAPE: 2.8%

 ✓ Linear Regression outperforms baseline by 32% (MAPE)
────────────────────────────────────────────────────────────
 NEXT 5-DAY FORECAST:
   Day 1: $176.12  │  Day 2: $176.45  │  Day 3: $176.78
   Day 4: $177.01  │  Day 5: $177.25
════════════════════════════════════════════════════════════
```

---

## Architecture

### Project Structure

```
StockSense/
├── CMakeLists.txt              # Build configuration
├── README.md                   # Documentation
├── data/
│   └── aapl_stock_data.csv     # Sample stock data
├── output/
│   └── predictions.csv         # Generated predictions
├── include/
│   ├── StockData.h             # Core data structures
│   ├── CSVParser.h             # File I/O operations
│   ├── DataCleaner.h           # Preprocessing pipeline
│   ├── FeatureEngine.h         # Technical indicators
│   ├── Matrix.h                # Linear algebra operations
│   ├── LinearRegression.h      # ML model implementation
│   ├── BaselineModel.h         # Baseline comparison
│   ├── Evaluator.h             # Metrics calculation
│   ├── Predictor.h             # Pipeline orchestrator
│   └── Display.h               # Console formatting
├── src/
│   ├── main.cpp                # Entry point & CLI
│   └── *.cpp                   # Implementation files
└── tests/
    └── test_main.cpp           # Unit test suite
```

### Data Flow Pipeline

```
┌─────────────┐    ┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│  CSV Input  │───▶│  Cleaning   │───▶│  Feature    │───▶│   Train/    │
│   Parser    │    │  Pipeline   │    │  Engineering│    │   Test Split│
└─────────────┘    └─────────────┘    └─────────────┘    └─────────────┘
                                                                │
                   ┌─────────────┐    ┌─────────────┐           │
                   │  Prediction │◀───│   Model     │◀──────────┘
                   │   Output    │    │  Training   │
                   └─────────────┘    └─────────────┘
```

---

## Documentation

### Technical Implementation

#### Linear Regression: The Normal Equation

The model computes optimal weights using the closed-form solution:

```
θ = (XᵀX)⁻¹Xᵀy
```

| Symbol | Description |
|--------|-------------|
| `X` | Feature matrix with bias column |
| `y` | Target vector (closing prices) |
| `θ` | Learned coefficients |

This approach provides an exact solution without iterative optimization, making it computationally efficient for datasets of moderate size.

#### Feature Engineering

| Feature | Formula | Purpose |
|---------|---------|---------|
| **SMA-5** | `mean(close[t-4:t])` | Short-term trend smoothing |
| **SMA-20** | `mean(close[t-19:t])` | Long-term trend identification |
| **Daily Return** | `(close - prev_close) / prev_close × 100` | Momentum indicator |
| **Price Range** | `high - low` | Intraday volatility measure |
| **Volume Change** | `(volume - prev_volume) / prev_volume × 100` | Trading activity signal |

#### Time-Series Best Practices

> **Critical**: This project uses chronological train/test splitting. Random shuffling is avoided as it causes **data leakage**—where the model inadvertently trains on future information.

```
Timeline:  ──────[Training Data]──────│──────[Test Data]──────▶
                                      ↑
                               Split Point (80%)
```

---

## Data Sources

### Obtaining Stock Data

Download historical stock data in Yahoo Finance CSV format:

**Option 1: Yahoo Finance**
1. Visit [finance.yahoo.com](https://finance.yahoo.com)
2. Search for a stock (e.g., AAPL)
3. Go to "Historical Data"
4. Download CSV

**Option 2: Kaggle Datasets**
- Search for "stock data" on [kaggle.com/datasets](https://www.kaggle.com/datasets)

### Expected CSV Format

```csv
Date,Open,High,Low,Close,Adj Close,Volume
2023-01-03,130.28,130.90,124.17,125.07,124.62,112117500
2023-01-04,126.89,128.66,125.08,126.36,125.91,89113600
```

---

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request. For major changes, please open an issue first to discuss what you would like to change.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

---

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## Acknowledgments

- Inspired by the need to understand ML fundamentals beyond high-level APIs
- Built as a demonstration of modern C++ capabilities
- Thanks to the open-source community for continuous inspiration

---

<div align="center">

**[⬆ Back to Top](#stocksense)**

Made with dedication by [Diptendu Nandi](https://github.com/diptendunandi)

</div>
