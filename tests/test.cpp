#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

struct ExecutionEntry
{
    std::string clientOrderId{};
    std::string orderId{};
    std::string instrument{};
    std::string side{};
    std::string price{};
    std::string quantity{};
    std::string status{};
    std::string reason{};
    std::string timestamp{};
};

std::ostream &operator<<(std::ostream &os, const ExecutionEntry &entry)
{
    os << "{ clientOrderId: " << entry.clientOrderId
       << ", orderId: " << entry.orderId
       << ", instrument: " << entry.instrument
       << ", side: " << entry.side
       << ", status: " << entry.status
       << ", quantity: " << entry.quantity
       << ", price: " << entry.price
       << ", reason: " << entry.reason
       << ", timestamp: " << entry.timestamp
       << " }";
    return os;
}

std::vector<std::string> split(const std::string &str, char delimiter)
{
    std::vector<std::string> tokens{};
    std::stringstream ss{str};
    std::string token{};
    while (std::getline(ss, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

std::vector<ExecutionEntry> parseCSV(const std::string &filename)
{
    std::vector<ExecutionEntry> entries{};
    std::ifstream file{filename};

    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file " << filename << "\n";
        return entries;
    }

    std::string line{};
    bool isHeader = true;

    while (std::getline(file, line))
    {
        // Skip header line
        if (isHeader)
        {
            isHeader = false;
            continue;
        }

        std::vector<std::string> tokens = split(line, ',');

        // Some expected files omit the final empty timestamp column entirely.
        if (tokens.size() == 8)
        {
            tokens.push_back("");
        }

        if (tokens.size() != 9)
        {
            std::cerr << "Warning: Skipping line with " << tokens.size() << " fields (expected 9): " << line << "\n";
            continue;
        }

        ExecutionEntry entry{};
        entry.clientOrderId = tokens[0];
        entry.orderId = tokens[1];
        entry.instrument = tokens[2];
        entry.side = tokens[3];
        entry.status = tokens[4];
        entry.quantity = tokens[5];
        entry.price = tokens[6];
        entry.reason = tokens[7];
        entry.timestamp = tokens[8];

        entries.push_back(entry);
    }

    file.close();
    return entries;
}

std::string getComparisonGroup(const ExecutionEntry &entry)
{
    if (entry.status == "REJECTED")
    {
        return "REJECTED";
    }

    return entry.instrument;
}

// Group entries by comparison bucket, preserving order within each bucket.
std::map<std::string, std::vector<ExecutionEntry>> groupByInstrument(
    const std::vector<ExecutionEntry> &entries)
{
    std::map<std::string, std::vector<ExecutionEntry>> grouped{};
    for (const auto &entry : entries)
    {
        grouped[getComparisonGroup(entry)].push_back(entry);
    }
    return grouped;
}

// Compare two entries, ignoring orderId and timestamp
bool entriesMatch(const ExecutionEntry &actual, const ExecutionEntry &expected)
{
    if (actual.clientOrderId != expected.clientOrderId)
    {
        return false;
    }

    if (actual.status == "REJECTED" || expected.status == "REJECTED")
    {
        return actual.status == expected.status;
    }

    return actual.clientOrderId == expected.clientOrderId &&
           actual.instrument == expected.instrument &&
           actual.side == expected.side &&
           actual.status == expected.status &&
           actual.quantity == expected.quantity &&
           actual.price == expected.price &&
           actual.reason == expected.reason;
}

// Print difference between two entries
void printDifference(const std::string &instrument,
                     int entryIndex,
                     const ExecutionEntry &actual,
                     const ExecutionEntry &expected)
{
    std::cout << "\n  Instrument '" << instrument << "', entry " << (entryIndex + 1)
              << " mismatch";

    if (!expected.clientOrderId.empty())
    {
        std::cout << " (expected clientOrderId=" << expected.clientOrderId << ")";
    }

    std::cout << ":\n";
    std::cout << "    Expected: " << expected << "\n";
    std::cout << "    Actual:   " << actual << "\n";
    std::cout << "    Field differences:\n";

    if (actual.status == "REJECTED" || expected.status == "REJECTED")
    {
        if (actual.clientOrderId != expected.clientOrderId)
        {
            std::cout << "    clientOrderId: expected '" << expected.clientOrderId
                      << "', got '" << actual.clientOrderId << "'\n";
        }
        if (actual.status != expected.status)
        {
            std::cout << "    status: expected '" << expected.status
                      << "', got '" << actual.status << "'\n";
        }
        return;
    }

    if (actual.clientOrderId != expected.clientOrderId)
    {
        std::cout << "    clientOrderId: expected '" << expected.clientOrderId
                  << "', got '" << actual.clientOrderId << "'\n";
    }
    if (actual.instrument != expected.instrument)
    {
        std::cout << "    instrument: expected '" << expected.instrument
                  << "', got '" << actual.instrument << "'\n";
    }
    if (actual.side != expected.side)
    {
        std::cout << "    side: expected '" << expected.side
                  << "', got '" << actual.side << "'\n";
    }
    if (actual.status != expected.status)
    {
        std::cout << "    status: expected '" << expected.status
                  << "', got '" << actual.status << "'\n";
    }
    if (actual.quantity != expected.quantity)
    {
        std::cout << "    quantity: expected '" << expected.quantity
                  << "', got '" << actual.quantity << "'\n";
    }
    if (actual.price != expected.price)
    {
        std::cout << "    price: expected '" << expected.price
                  << "', got '" << actual.price << "'\n";
    }
    if (actual.reason != expected.reason)
    {
        std::cout << "    reason: expected '" << expected.reason
                  << "', got '" << actual.reason << "'\n";
    }
}

bool validateCSV(const std::vector<ExecutionEntry> &actualEntries,
                 const std::vector<ExecutionEntry> &expectedEntries)
{
    const int MAX_MISMATCHES = 10;

    // Group by instrument
    auto actualGrouped = groupByInstrument(actualEntries);
    auto expectedGrouped = groupByInstrument(expectedEntries);

    // Check if same instruments are present
    if (actualGrouped.size() != expectedGrouped.size())
    {
        std::cout << "  Number of instrument types differ: expected "
                  << expectedGrouped.size() << ", got " << actualGrouped.size() << "\n";
        return false;
    }

    int mismatchCount = 0;
    bool hasError = false;

    // Compare entries within each instrument
    for (const auto &[instrument, expectedInstrumentEntries] : expectedGrouped)
    {
        if (actualGrouped.find(instrument) == actualGrouped.end())
        {
            std::cout << "  Missing instrument in actual output: " << instrument << "\n";
            hasError = true;
            continue;
        }

        const auto &actualInstrumentEntries = actualGrouped[instrument];

        if (actualInstrumentEntries.size() != expectedInstrumentEntries.size())
        {
            std::cout << "  Instrument '" << instrument << "': expected "
                      << expectedInstrumentEntries.size() << " entries, got "
                      << actualInstrumentEntries.size() << "\n";
            hasError = true;
            continue;
        }

        // Compare entries in order within instrument
        for (size_t i = 0; i < expectedInstrumentEntries.size(); ++i)
        {
            if (!entriesMatch(actualInstrumentEntries[i], expectedInstrumentEntries[i]))
            {
                if (mismatchCount < MAX_MISMATCHES)
                {
                    printDifference(instrument, i, actualInstrumentEntries[i], expectedInstrumentEntries[i]);
                }
                mismatchCount++;
                hasError = true;
            }
        }
    }

    if (mismatchCount > MAX_MISMATCHES)
    {
        std::cout << "  ... and " << (mismatchCount - MAX_MISMATCHES) << " more mismatches\n";
    }

    return !hasError;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <actual_csv> <expected_csv>\n";
        return 1;
    }

    std::string actualFile = argv[1];
    std::string expectedFile = argv[2];

    std::cout << "Comparing CSV files:\n";
    std::cout << "  Actual:   " << actualFile << "\n";
    std::cout << "  Expected: " << expectedFile << "\n\n";

    // Parse both files
    auto actualEntries = parseCSV(actualFile);
    auto expectedEntries = parseCSV(expectedFile);

    if (actualEntries.empty())
    {
        std::cerr << "Error: Failed to parse actual file or file is empty\n";
        return 1;
    }

    if (expectedEntries.empty())
    {
        std::cerr << "Error: Failed to parse expected file or file is empty\n";
        return 1;
    }

    std::cout << "Parsed entries:\n";
    std::cout << "  Actual:   " << actualEntries.size() << " entries\n";
    std::cout << "  Expected: " << expectedEntries.size() << " entries\n\n";

    // Validate
    bool passed = validateCSV(actualEntries, expectedEntries);

    if (passed)
    {
        std::cout << "✓ TEST PASSED\n";
        return 0;
    }
    else
    {
        std::cout << "✗ TEST FAILED\n";
        return 1;
    }
}
