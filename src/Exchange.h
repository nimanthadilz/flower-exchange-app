#pragma once

#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "BlockingQueue.h"
#include "ExecutionRecord.h"
#include "Order.h"
#include "OrderBook.h"

class Exchange
{
private:
    std::unique_ptr<BlockingQueue<ExecutionRecord>> m_executionRecordQueuePtr;
    std::unordered_map<std::string, std::thread> m_threadMap;
    std::unordered_map<std::string, std::unique_ptr<BlockingQueue<Order>>> m_queueMap;
    std::unordered_map<std::string, std::unique_ptr<OrderBook>> m_orderBookMap;
    std::vector<std::string> m_instruments;

    Order createOrder(std::vector<std::string> order);
    bool isInstrumentValid(std::string instrument);
    void processOrders(std::string instrument);
    void writeExecutionRecords(std::ofstream &file);
    bool validateOrder(std::vector<std::string> order);

public:
    void receiveOrder(std::vector<std::string> order);
    void init(std::ofstream &ExecutionRecordsFile);
    void setOrderProducerDone();
    void addInstrument(std::string instrument);
};