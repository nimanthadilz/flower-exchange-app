#pragma once
#include <string>

enum class Instrument
{
    ROSE,
    LAVENDER,
    LOTUS,
    TULIP,
    ORCHID,
};

enum class Side
{
    BUY,
    SELL
};

class Order
{
private:
    std::string m_clientOrderId{};
    Instrument m_instrument{};
    Side m_side{};
    double m_price{};
    int m_quantity{};

public:
    Order(std::string clientOrderId, Instrument instrument, Side side, double price, int quantity);
    std::string getClientOrderId() { return m_clientOrderId; };
    Instrument getInstrument() { return m_instrument; };
    Side getSide() { return m_side; };
    double getPrice() { return m_price; };
    int getQuantity() { return m_quantity; };

    void setClientOrderId(std::string clientOrderId) { m_clientOrderId = clientOrderId; };
    void setInstrument(Instrument instrument) { m_instrument = instrument; };
    void setSide(Side side) { m_side = side; };
    void setPrice(double price) { m_price = price; };
    void setQuantity(int quantity) { m_quantity = quantity; };

    friend std::ostream &operator<<(std::ostream &os, const Order &order);
};

std::string_view getInstrument(Instrument instrument);

std::string_view getSide(Side side);