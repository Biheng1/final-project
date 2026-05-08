#include "CSVParser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

// Opens the CSV file at 'filename', skips the header row, and builds
// a PriceHistory linked list from each valid data row.
// Filters rows to only include dates between 2000-01-01 and 2020-01-01.
// Returns a heap-allocated PriceHistory* (caller owns it).
// Returns nullptr if the file cannot be opened.
PriceHistory* CSVParser::loadHistory(const string& filename) {
    ifstream file(filename);
    
    // Check if file opened successfully
    if (!file.is_open()) {
        return nullptr;
    }
    
    PriceHistory* history = new PriceHistory();
    string line;
    
    // Skip the header row
    if (!getline(file, line)) {
        file.close();
        return history; // Return empty history if no header
    }
    
    const string START_DATE = "2000-01-01";
    const string END_DATE = "2020-01-01";
    
    // Read each data row
    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        vector<string> fields;
        
        // Parse CSV fields (separated by commas)
        while (getline(ss, token, ',')) {
            // Trim leading/trailing whitespace
            size_t start = token.find_first_not_of(" \t");
            size_t end = token.find_last_not_of(" \t");
            if (start != string::npos) {
                token = token.substr(start, end - start + 1);
            } else {
                token = "";
            }
            fields.push_back(token);
        }
        
        // Expected format: Date, Open, High, Low, Close, Adj Close, Volume
        // We need at least 7 fields
        if (fields.size() < 7) {
            continue; // Skip invalid rows
        }
        
        try {
            string date = fields[0];           // Date (YYYY-MM-DD)
            
            // Filter rows outside the required date window
            if (!dateInRange(date, START_DATE, END_DATE)) {
                continue;
            }
            
            double open = stod(fields[1]);     // Open price
            double high = stod(fields[2]);     // High price
            double low = stod(fields[3]);      // Low price
            double close = stod(fields[4]);    // Close price
            // fields[5] is Adj Close (ignored)
            long volume = stol(fields[6]);     // Volume
            
            history->append(date, open, high, low, close, volume);
        } catch (const exception& e) {
            // Skip rows that cannot be parsed
            continue;
        }
    }
    
    file.close();
    return history;
}

// Returns true if 'date' falls within [startDate, endDate] (inclusive).
// Works on "YYYY-MM-DD" strings: lexicographic == chronological.
bool CSVParser::dateInRange(const string& date, const string& start, const string& end) {
    return date >= start && date <= end;
}

// Extracts the 4-digit year from "YYYY-MM-DD" and returns it as an int.
int CSVParser::extractYear(const string& date) {
    // Format is "YYYY-MM-DD", so year is at positions 0-3
    if (date.length() < 4) {
        return 0;
    }
    return stoi(date.substr(0, 4));
}

// Extracts the 2-digit month from "YYYY-MM-DD" and returns it as an int.
int CSVParser::extractMonth(const string& date) {
    // Format is "YYYY-MM-DD", so month is at positions 5-6
    if (date.length() < 7) {
        return 0;
    }
    return stoi(date.substr(5, 2));
}
