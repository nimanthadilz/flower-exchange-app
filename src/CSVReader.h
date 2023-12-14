#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "Order.h"

class CSVReader
{

public:
    std::vector<std::string> readOrder(std::ifstream &ordersFile);
};