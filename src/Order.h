#pragma once
#include <string>

enum class Side
{
    BUY,
    SELL
};

class Order
{
private:
    static int m_current_id;
    int m_orderId{};
    std::string m_clientOrderId{};
    std::string m_instrument{};
    Side m_side{};
    double m_price{};
    int m_quantity{};

public:
    Order(std::string clientOrderId, std::string instrument, Side side, double price, int quantity);
    std::string getClientOrderId() { return m_clientOrderId; };
    int getOrderId() { return m_orderId; };
    std::string getInstrument() { return m_instrument; };
    Side getSide() { return m_side; };
    double getPrice() { return m_price; };
    int getQuantity() { return m_quantity; };

    void setClientOrderId(std::string clientOrderId) { m_clientOrderId = clientOrderId; };
    void setInstrument(std::string instrument) { m_instrument = instrument; };
    void setSide(Side side) { m_side = side; };
    void setPrice(double price) { m_price = price; };
    void setQuantity(int quantity) { m_quantity = quantity; };

    static int getNextOrderId() { return ++m_current_id; };
    friend std::ostream &operator<<(std::ostream &os, const Order &order);
};

std::string getSide(Side side);