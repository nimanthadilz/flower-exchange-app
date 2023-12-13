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
    CSVReader csvReader{"../sample_files/orders_5.csv"};
    Exchange flowerExchange;
    std::atomic<bool> isOrderProducerDone{false};
    std::atomic<bool> isOrderProcessingDone{false};

    std::ofstream executionRecordsFile{"../execution_records_5.csv"};

    if (!executionRecordsFile)
    {
        std::cerr << "Unable to open output file\n";
        return 1;
    }

    std::thread processThread(processOrders, std::ref(flowerExchange.getRoseOrdersQueue()),
                              std::ref(flowerExchange.getExecutionRecordQueue()));

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
