#pragma once

#include <string>

#include "Order.h"

enum class Status
{
    NEW,
    PFILL,
    FILL,
    REJECTED
};

class ExecutionRecord
{
private:
    std::string m_orderId{};
    std::string m_clientOrderId{};
    Instrument m_instrument{};
    Side m_side{};
    Status m_status{};
    int m_quantity{};
    double m_price{};
    std::string m_reason{};

public:
    ExecutionRecord(std::string orderId, std::string clientOrderId, Instrument instrument,
                    Side side, Status status, int quantity, double price, std::string reason = "");

    std::string getOrderId() const { return m_orderId; }
    std::string getClientOrderId() const { return m_clientOrderId; }
    Instrument getInstrument() const { return m_instrument; }
    Side getSide() const { return m_side; }
    Status getStatus() const { return m_status; }
    int getQuantity() const { return m_quantity; }
    double getPrice() const { return m_price; }
    std::string getReason() const { return m_reason; }

    // setters for data members
    void setClientOrderId(std::string clientOrderId) { m_clientOrderId = clientOrderId; }
    void setInstrument(Instrument instrument) { m_instrument = instrument; }
    void setSide(Side side) { m_side = side; }
    void setStatus(Status status) { m_status = status; }
    void setQuantity(int quantity) { m_quantity = quantity; }
    void setPrice(double price) { m_price = price; }
    void setReason(std::string reason) { m_reason = reason; }
};

std::string_view getStatus(Status status);