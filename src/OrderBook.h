#pragma once

#include <list>
#include <vector>

#include "ExecutionRecord.h"
#include "Order.h"

class OrderBook
{
private:
    std::list<Order> m_buyOrdersList;
    std::list<Order> m_sellOrdersList;

    void addOrderToList(std::list<Order> &ordersList, Order order, Side side);

public:
    std::vector<ExecutionRecord> processOrder(Order order);
};