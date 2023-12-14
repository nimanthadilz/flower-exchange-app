#include <iostream>
#include <string>

#include "Order.h"

int Order::m_current_id{0};

Order::Order(std::string clientOrderId, Instrument instrument, Side side, double price, int quantity)
    : m_clientOrderId{clientOrderId}, m_instrument{instrument}, m_side{side}, m_price{price}, m_quantity{quantity}
{
    m_orderId = getNextOrderId();
}

std::ostream &operator<<(std::ostream &os, const Order &order)
{
    os << "clientOrderId: " << order.m_clientOrderId << ", instrument: " << getInstrument(order.m_instrument)
       << ", side: " << getSide(order.m_side) << ", price: " << order.m_price << ", quantity: " << order.m_quantity;
    return os;
}

std::string_view getInstrument(Instrument instrument)
{
    switch (instrument)
    {
    case Instrument::ROSE:
        return "ROSE";
    case Instrument::LAVENDER:
        return "LAVENDER";
    case Instrument::LOTUS:
        return "LOTUS";
    case Instrument::TULIP:
        return "TULIP";
    case Instrument::ORCHID:
        return "ORCHID";
    default:
        return "UNKNOWN";
    }
}

std::string_view getSide(Side side)
{
    switch (side)
    {
    case Side::BUY:
        return "BUY";
    case Side::SELL:
        return "SELL";
    default:
        return "UNKNOWN";
    }
}