#include <atomic>
#include <fstream>
#include <functional>
#include <iostream>
#include <thread>

#include "BlockingQueue.h"
#include "CSVReader.h"
#include "Exchange.h"
#include "ExecutionRecord.h"
#include "Order.h"

int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " <input_file> <output_file>" << '\n';
        return 1;
    }

    // Open input file
    std::ifstream ordersFile{argv[1]};
    if (!ordersFile)
    {
        std::cerr << "Unable to open input file\n";
        return 1;
    }
    CSVReader csvReader{};

    // Open output file
    std::ofstream executionRecordsFile{argv[2]};
    if (!executionRecordsFile)
    {
        std::cerr << "Unable to open output file\n";
        return 1;
    }
    // write header of the output file
    executionRecordsFile << "orderId,clientOrderId,instrument,side,status,quantity,price,reason,transactionTime\n";

    Exchange flowerExchange;
    flowerExchange.initThreads(executionRecordsFile);

    // Read orders from file
    std::vector<std::string> order = csvReader.readOrder(ordersFile); // skip header
    order = csvReader.readOrder(ordersFile);
    while (order.size())
    {
        flowerExchange.receiveOrder(order);
        order = csvReader.readOrder(ordersFile);
    }

    ordersFile.close();
    flowerExchange.setOrderProducerDone(); // signal to threads that no more orders will be received
    executionRecordsFile.close();

    std::cout << "Finished processing." << '\n';
    return 0;
}
