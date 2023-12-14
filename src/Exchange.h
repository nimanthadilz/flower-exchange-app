#pragma once

#include <atomic>
#include <mutex>
#include <string>
#include <vector>

#include "BlockingQueue.h"
#include "ExecutionRecord.h"
#include "Order.h"
#include "OrderBook.h"

class Exchange
{
private:
    BlockingQueue<Order> m_roseOrdersQueue;
    BlockingQueue<ExecutionRecord> m_executionRecordQueue;
    OrderBook m_roseOrderBook;
    std::unordered_map<std::string, std::thread> threadsMap;

    Order createOrder(std::vector<std::string> order);
    bool isInstrumentValid(std::string instrument);
    void processOrders(std::string_view instrument);
    void writeExecutionRecords(std::ofstream &file);

public:
    void receiveOrder(std::vector<std::string> order);
    BlockingQueue<Order> &getRoseOrdersQueue() { return m_roseOrdersQueue; };
    BlockingQueue<ExecutionRecord> &getExecutionRecordQueue() { return m_executionRecordQueue; };
    OrderBook &getRoseOrderBook() { return m_roseOrderBook; };

    void initThreads(std::ofstream &ExecutionRecordsFile);
    void setOrderProducerDone();
};

void processOrders(BlockingQueue<Order> &ordersQueue, BlockingQueue<ExecutionRecord> &executionRecordQueue,
                   OrderBook &orderBook);
void writeExecutionRecords(BlockingQueue<ExecutionRecord> &executionRecordQueue, std::ofstream &file);