
#include "ExecutionRecord.h"
#include "Order.h"

ExecutionRecord::ExecutionRecord(std::string orderId, std::string clientOrderId, Instrument instrument,
                                 Side side, Status status, int quantity, double price, std::string reason)
    : m_orderId{orderId}, m_clientOrderId{clientOrderId}, m_instrument{instrument}, m_side{side}, m_status{status},
      m_quantity{quantity}, m_price{price}, m_reason{reason} {}

std::string_view getStatus(Status status)
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