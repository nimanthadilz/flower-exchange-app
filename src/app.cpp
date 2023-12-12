#include <iostream>

#include "CSVReader.h"
#include "Order.h"

int main(int argc, char const *argv[])
{
    CSVReader csvReader{"../sample_files/orders_5.csv"};

    std::vector<std::string> order = csvReader.readOrder();
    while (order.size())
    {
        std::cout << order[0] << " " << order[1] << " " << order[2] << '\n';
        order = csvReader.readOrder();
    }

    return 0;
}
