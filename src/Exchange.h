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
    // BlockingQueue<Order> m_roseOrdersQueue;
    std::unique_ptr<BlockingQueue<ExecutionRecord>> m_executionRecordQueuePtr;
    // OrderBook m_roseOrderBook;
    std::unordered_map<std::string, std::thread> threadsMap;
    std::unordered_map<std::string, std::unique_ptr<BlockingQueue<Order>>> queueMap;
    std::unordered_map<std::string, std::unique_ptr<OrderBook>> orderBookMap;
    std::vector<std::string> m_instruments;

    Order createOrder(std::vector<std::string> order);
    bool isInstrumentValid(std::string instrument);
    void processOrders(std::string instrument);
    void writeExecutionRecords(std::ofstream &file);

public:
    void receiveOrder(std::vector<std::string> order);
    // BlockingQueue<Order> &getRoseOrdersQueue() { return m_roseOrdersQueue; };
    // BlockingQueue<ExecutionRecord> &getExecutionRecordQueue() { return m_executionRecordQueue; };
    // OrderBook &getRoseOrderBook() { return m_roseOrderBook; };

    void init(std::ofstream &ExecutionRecordsFile);
    void setOrderProducerDone();
    void addInstrument(std::string instrument);
};

void processOrders(BlockingQueue<Order> &ordersQueue, BlockingQueue<ExecutionRecord> &executionRecordQueue,
                   OrderBook &orderBook);
void writeExecutionRecords(BlockingQueue<ExecutionRecord> &executionRecordQueue, std::ofstream &file);