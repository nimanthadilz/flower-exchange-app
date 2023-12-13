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
    std::string orderId{};
    std::string clientOrderId{};
    Instrument instrument{};
    Side side{};
    Status status{};
    int quantity{};
    double price{};
    std::string reason{};

public:
    ExecutionRecord(std::string orderId, std::string clientOrderId, Instrument instrument,
                    Side side, Status status, int quantity, double price, std::string reason = "");

    std::string getOrderId() const { return orderId; }
    std::string getClientOrderId() const { return clientOrderId; }
    Instrument getInstrument() const { return instrument; }
    Side getSide() const { return side; }
    Status getStatus() const { return status; }
    int getQuantity() const { return quantity; }
    double getPrice() const { return price; }
    std::string getReason() const { return reason; }
};

std::string_view getStatus(Status status);