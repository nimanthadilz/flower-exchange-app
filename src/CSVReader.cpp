#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <limits>

#include "CSVReader.h"

CSVReader::CSVReader(std::string filename)
    : m_filename{filename}
{
    ordersFile.open(filename);

    if (!ordersFile)
    {
        throw std::invalid_argument("Could not open " + filename);
    }

    // skip the header line of the CSV
    ordersFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::vector<std::string> CSVReader::readOrder()
{
    std::vector<std::string> lineArray{};
    if (ordersFile)
    {
        std::string line;

        std::getline(ordersFile, line);

        int pos = 0;
        std::string delimiter = ",";
        while (pos < line.size())
        {
            pos = line.find(",");
            lineArray.push_back(line.substr(0, pos));
            line.erase(0, pos + 1);
        }

        return lineArray;
    }
    return lineArray;
}
