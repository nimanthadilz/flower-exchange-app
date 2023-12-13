#pragma once

#include <atomic>
#include <mutex>
#include <string>
#include <vector>

#include "BlockingQueue.h"
#include "ExecutionRecord.h"
#include "Order.h"

class Exchange
{
private:
    BlockingQueue<Order> roseOrdersQueue;
    BlockingQueue<ExecutionRecord> executionRecordQueue;
    Order createOrder(std::vector<std::string> order);

public:
    Exchange();
    void receiveOrder(std::vector<std::string> order);
    BlockingQueue<Order> &getRoseOrdersQueue() { return roseOrdersQueue; };
    BlockingQueue<ExecutionRecord> &getExecutionRecordQueue() { return executionRecordQueue; };
};

void processOrders(BlockingQueue<Order> &ordersQueue, BlockingQueue<ExecutionRecord> &executionRecordQueue);
void writeExecutionRecords(BlockingQueue<ExecutionRecord> &executionRecordQueue, std::ofstream &file);