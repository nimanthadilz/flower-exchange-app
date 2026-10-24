#pragma once

#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "BlockingQueue.h"
#include "ExecutionRecord.h"
#include "Order.h"
#include "OrderBook.h"

class Exchange {
private:
  BlockingQueue<ExecutionRecord> m_executionRecordQueue;
  std::unordered_map<std::string, std::thread> m_threadMap;
  std::unordered_map<std::string, BlockingQueue<Order>> m_queueMap;
  std::unordered_map<std::string, OrderBook> m_orderBookMap;
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
