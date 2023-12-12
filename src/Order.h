#pragma once
#include <string>

class Order
{
private:
    std::string m_clientOrderId{};
    std::string m_instrument{}; // TODO: use enums
    int m_side{};
    double m_price{};
    int m_quantity{};

public:
    Order(std::string clientOrderId, std::string instrument, int side, double price, int quantity);
    std::string getClientOrderId() { return m_clientOrderId; };
    std::string getInstrument() { return m_instrument; };
    int getSide() { return m_side; };
    double getPrice() { return m_price; };
    int getQuantity() { return m_quantity; };

    friend std::ostream& operator<<(std::ostream& os, const Order& order);
};
