#include <vector>

#include "ExecutionRecord.h"
#include "Order.h"
#include "OrderBook.h"

void OrderBook::addOrderToList(std::list<Order> &ordersList, Order order, Side side)
{
    if (side == Side::BUY)
    {
        // insert order in the list in descending order
        auto it = ordersList.begin();
        while (it != ordersList.end() && it->getPrice() >= order.getPrice())
        {
            it++;
        }
        ordersList.insert(it, order);
    }
    else
    {
        // insert order in the list in ascending order
        auto it = ordersList.begin();
        while (it != ordersList.end() && it->getPrice() <= order.getPrice())
        {
            it++;
        }
        ordersList.insert(it, order);
    }
}

std::vector<ExecutionRecord> OrderBook::processOrder(Order order)
{
    std::vector<ExecutionRecord> executionRecordsList;
    Side side = order.getSide();
    double price = order.getPrice();

    if (side == Side::BUY)
    {
        if (m_sellOrdersList.size() == 0 || price < m_sellOrdersList.front().getPrice())
        {
            addOrderToList(m_buyOrdersList, order, side);
            executionRecordsList.push_back(ExecutionRecord{"XXX", order.getClientOrderId(), order.getInstrument(),
                                                           order.getSide(), Status::NEW, order.getQuantity(),
                                                           order.getPrice()});
            return executionRecordsList;
        }

        // while there are sell orders and the price of the buy order is greater than or equal to the price of the
        // sell order, execute the buy order against the sell order
        while (m_sellOrdersList.size() > 0 && price >= m_sellOrdersList.front().getPrice())
        {
            Order sellOrder = m_sellOrdersList.front();
            m_sellOrdersList.pop_front();

            int quantity = std::min(order.getQuantity(), sellOrder.getQuantity());
            order.setQuantity(order.getQuantity() - quantity);
            sellOrder.setQuantity(sellOrder.getQuantity() - quantity);

            ExecutionRecord executionRecord{"XXX", order.getClientOrderId(), order.getInstrument(), order.getSide(),
                                            Status::FILL, quantity, sellOrder.getPrice()};
            if (order.getQuantity() > 0)
            {
                executionRecord.setStatus(Status::PFILL);
            }
            executionRecordsList.push_back(executionRecord);

            executionRecord.setClientOrderId(sellOrder.getClientOrderId());
            executionRecord.setSide(sellOrder.getSide());
            executionRecord.setPrice(sellOrder.getPrice());
            executionRecord.setStatus(sellOrder.getQuantity() > 0 ? Status::PFILL : Status::FILL);
            executionRecordsList.push_back(executionRecord);

            if (sellOrder.getQuantity() > 0)
            {
                m_sellOrdersList.push_front(sellOrder);
            }
        }
        // if the buy order has not been completely filled, add it to the buy orders list
        if (order.getQuantity() > 0)
        {
            addOrderToList(m_buyOrdersList, order, side);
        }
    }
    else
    {
        if (m_buyOrdersList.size() == 0 || price > m_buyOrdersList.front().getPrice())
        {
            addOrderToList(m_sellOrdersList, order, side);
            executionRecordsList.push_back(ExecutionRecord{"XXX", order.getClientOrderId(), order.getInstrument(),
                                                           order.getSide(), Status::NEW, order.getQuantity(),
                                                           order.getPrice()});
            return executionRecordsList;
        }
        // while there are buy orders and the price of the sell order is less than or equal to the price of the
        // buy order, execute the sell order against the buy order
        while (m_buyOrdersList.size() > 0 && price <= m_buyOrdersList.front().getPrice())
        {
            Order buyOrder = m_buyOrdersList.front();
            m_buyOrdersList.pop_front();

            int quantity = std::min(order.getQuantity(), buyOrder.getQuantity());
            order.setQuantity(order.getQuantity() - quantity);
            buyOrder.setQuantity(buyOrder.getQuantity() - quantity);

            ExecutionRecord executionRecord{"XXX", order.getClientOrderId(), order.getInstrument(), order.getSide(),
                                            Status::FILL, quantity, buyOrder.getPrice()};
            if (order.getQuantity() > 0)
            {
                executionRecord.setStatus(Status::PFILL);
            }
            executionRecordsList.push_back(executionRecord);

            executionRecord.setClientOrderId(buyOrder.getClientOrderId());
            executionRecord.setSide(buyOrder.getSide());
            executionRecord.setPrice(buyOrder.getPrice());
            executionRecord.setStatus(buyOrder.getQuantity() > 0 ? Status::PFILL : Status::FILL);
            executionRecordsList.push_back(executionRecord);

            if (buyOrder.getQuantity() > 0)
            {
                m_buyOrdersList.push_front(buyOrder);
            }
        }
        // if the sell order has not been completely filled, add it to the sell orders list
        if (order.getQuantity() > 0)
        {
            addOrderToList(m_sellOrdersList, order, side);
        }
    }
    return executionRecordsList;
}
