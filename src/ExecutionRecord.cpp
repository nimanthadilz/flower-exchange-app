#include <atomic>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

#include "ExecutionRecord.h"
#include "Order.h"

void ExecutionRecord::setTransactionTime()
{
  // Get the current time point
  auto now = std::chrono::system_clock::now();

  // Convert the time point to a time_t object
  std::time_t currentTiem = std::chrono::system_clock::to_time_t(now);

  // Convert the time_t object to a tm struct
  std::tm *timeInfo = std::localtime(&currentTiem);

  // Get milliseconds
  auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

  // Format the time string
  std::ostringstream oss;
  oss << std::put_time(timeInfo, "%Y%m%d-%H%M%S") << '.' << std::setfill('0') << std::setw(3) << milliseconds.count();

  m_transactionTime = oss.str();
}

ExecutionRecord::ExecutionRecord(int orderId, std::string clientOrderId, std::string instrument,
                                 Side side, Status status, int quantity, double price, std::string reason)
    : m_orderId{orderId}, m_clientOrderId{clientOrderId}, m_instrument{instrument}, m_side{side}, m_status{status},
      m_quantity{quantity}, m_price{price}, m_reason{reason}
{
  setTransactionTime();
}

ExecutionRecord::ExecutionRecord(int order_id, std::unordered_map<std::string, std::string> invalidValues, std::string reason)
    : m_orderId{order_id}, m_invalidValues{invalidValues}, m_reason{reason}, m_status{Status::REJECTED}
{
  setTransactionTime();
}

std::string getStatus(Status status)
{
  switch (status)
  {
  case Status::NEW:
    return "NEW";
  case Status::PFILL:
    return "PFILL";
  case Status::FILL:
    return "FILL";
  case Status::REJECTED:
    return "REJECTED";
  default:
    return "UNKNOWN";
  }
}