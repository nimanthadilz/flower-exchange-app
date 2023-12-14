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

    CSVReader csvReader{argv[1]};
    Exchange flowerExchange;

    std::ofstream executionRecordsFile{argv[2]};

    if (!executionRecordsFile)
    {
        std::cerr << "Unable to open output file\n";
        return 1;
    }

    executionRecordsFile << "orderId,clientOrderId,instrument,side,status,quantity,price,reason,transactionTime\n";

    std::thread processThread(processOrders, std::ref(flowerExchange.getRoseOrdersQueue()),
                              std::ref(flowerExchange.getExecutionRecordQueue()),
                              std::ref(flowerExchange.getRoseOrderBook()));

    std::thread writeExecutionRecordsThread(writeExecutionRecords, std::ref(flowerExchange.getExecutionRecordQueue()),
                                            std::ref(executionRecordsFile));

    std::vector<std::string> order = csvReader.readOrder();
    while (order.size())
    {
        flowerExchange.receiveOrder(order);
        order = csvReader.readOrder();
    }
    flowerExchange.getRoseOrdersQueue().setDone();
    processThread.join();
    flowerExchange.getExecutionRecordQueue().setDone();
    writeExecutionRecordsThread.join();

    std::cout << "main thread finished" << '\n';
    return 0;
}
