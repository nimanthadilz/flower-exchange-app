#include <string>
#include <iostream>

#include "Order.h"

Order::Order(std::string clientOrderId, std::string instrument, int side, double price, int quantity)
    : m_clientOrderId{clientOrderId}, m_instrument{instrument}, m_side{side}, m_price{price}, m_quantity{quantity}
{
}

std::ostream &operator<<(std::ostream &os, const Order &order)
{
    os << "clientOrderId: " << order.m_clientOrderId << ", instrument: " << order.m_instrument
        << ", side: " << order.m_side << ", price: " << order.m_price << ", quantity: " << order.m_quantity;
    return os;
}