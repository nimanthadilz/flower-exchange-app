#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "Order.h"

class CSVReader
{
private:
    std::string m_filename;
    std::ifstream ordersFile;

public:
    CSVReader(std::string filename);
    std::vector<std::string> readOrder();
};