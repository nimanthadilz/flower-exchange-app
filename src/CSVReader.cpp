#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

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
        std::string line{};
        std::getline(ordersFile, line);
        if (line.empty() && ordersFile.eof())
        {
            return lineArray;
        }
        std::string::size_type pos{};
        while ((pos = line.find(',')) != std::string::npos)
        {
            lineArray.push_back(line.substr(0, pos));
            line.erase(0, pos + 1);
        }
        lineArray.push_back(line);
    }
    return lineArray;
}
