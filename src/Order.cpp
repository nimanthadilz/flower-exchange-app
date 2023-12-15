#include <iostream>
#include <string>

#include "Order.h"

int Order::m_current_id{0};

Order::Order(std::string clientOrderId, std::string instrument, Side side, double price, int quantity)
    : m_clientOrderId{clientOrderId}, m_instrument{instrument}, m_side{side}, m_price{price}, m_quantity{quantity}
{
    m_orderId = getNextOrderId();
}

std::ostream &operator<<(std::ostream &os, const Order &order)
{
    os << "clientOrderId: " << order.m_clientOrderId << ", instrument: " << order.m_instrument
       << ", side: " << getSide(order.m_side) << ", price: " << order.m_price << ", quantity: " << order.m_quantity;
    return os;
}

std::string getSide(Side side)
{
    switch (side)
    {
    case Side::BUY:
        return "1";
    case Side::SELL:
        return "2";
    default:
        return "UNKNOWN";
    }
}