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

    Order createOrder(std::vector<std::string> order);

public:
    Exchange();
    void receiveOrder(std::vector<std::string> order);
    BlockingQueue<Order> &getRoseOrdersQueue() { return m_roseOrdersQueue; };
    BlockingQueue<ExecutionRecord> &getExecutionRecordQueue() { return m_executionRecordQueue; };
    OrderBook &getRoseOrderBook() { return m_roseOrderBook; };
};

void processOrders(BlockingQueue<Order> &ordersQueue, BlockingQueue<ExecutionRecord> &executionRecordQueue,
                   OrderBook &orderBook);
void writeExecutionRecords(BlockingQueue<ExecutionRecord> &executionRecordQueue, std::ofstream &file);