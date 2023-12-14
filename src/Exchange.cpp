#include <fstream>
#include <iomanip>
#include <iostream>
#include <thread>

#include "Exchange.h"
#include "ExecutionRecord.h"
#include "Order.h"

Exchange::Exchange()
{
}

void Exchange::receiveOrder(std::vector<std::string> order)
{
    // validations
    if (order.size() < 5)
    {
        std::cout << "Invalid order: expected field count: 5, received field count: " << order.size() << '\n';
        return;
    }

    std::unordered_map<std::string, std::string> orderMap = {
        {"clientOrderId", order[0]},
        {"instrument", order[1]},
        {"side", order[2]},
        {"quantity", order[3]},
        {"price", order[4]}};

    if (order[0].empty())
    {
        std::cout << "Invalid order: client order id is empty\n";
        m_executionRecordQueue.push(ExecutionRecord(Order::getNextOrderId(), orderMap, "Invalid client order id"));
        return;
    }

    if (!isInstrumentValid(order[1]))
    {
        std::cout << "Invalid order: client order id: '" << order[0] << "' invalid instrument: " << order[1] << '\n';
        m_executionRecordQueue.push(ExecutionRecord(Order::getNextOrderId(), orderMap, "Invalid instrument"));
        return;
    }

    if (order[2] != "1" && order[2] != "2")
    {
        std::cout << "Invalid order: order id: '" << order[0] << "' invalid side: " << order[2] << '\n';
        m_executionRecordQueue.push(ExecutionRecord(Order::getNextOrderId(), orderMap, "Invalid side"));
        return;
    }

    try
    {
        int quantity = std::stoi(order[3]);
        if (quantity < 10 || quantity > 1000 || quantity % 10 != 0)
        {
            std::cout << "Invalid order: order id: '" << order[0] << "' invalid quantity: " << order[3] << '\n';
            m_executionRecordQueue.push(ExecutionRecord(Order::getNextOrderId(), orderMap, "Invalid quantity"));
            return;
        }
    }
    catch (std::invalid_argument &e)
    {
        std::cout << "Invalid order: order id: '" << order[0] << "' invalid quantity: " << order[3] << '\n';
        m_executionRecordQueue.push(ExecutionRecord(Order::getNextOrderId(), orderMap, "Invalid quantity"));
        return;
    }
    catch (std::out_of_range &e)
    {
        std::cout << "Invalid order: order id: '" << order[0] << "' invalid quantity: " << order[3] << '\n';
        m_executionRecordQueue.push(ExecutionRecord(Order::getNextOrderId(), orderMap, "Invalid quantity"));
        return;
    }

    try
    {
        double price = std::stod(order[4]);
        if (price < 0)
        {
            std::cout << "Invalid order: order id: '" << order[0] << "' invalid price: " << order[4] << '\n';
            m_executionRecordQueue.push(ExecutionRecord(Order::getNextOrderId(), orderMap, "Invalid price"));
            return;
        }
    }
    catch (std::invalid_argument &e)
    {
        std::cout << "Invalid order: order id: '" << order[0] << "' invalid price: " << order[4] << '\n';
        m_executionRecordQueue.push(ExecutionRecord(Order::getNextOrderId(), orderMap, "Invalid price"));
        return;
    }
    catch (std::out_of_range &e)
    {
        std::cout << "Invalid order: order id: '" << order[0] << "' invalid price: " << order[4] << '\n';
        m_executionRecordQueue.push(ExecutionRecord(Order::getNextOrderId(), orderMap, "Invalid price"));
        return;
    }

    Order newOrder = createOrder(order);

    switch (newOrder.getInstrument())
    {
    case Instrument::ROSE:
        m_roseOrdersQueue.push(newOrder);
        break;
    default:
        break;
    }
}

Order Exchange::createOrder(std::vector<std::string> order)
{
    std::string clientOrderId = order[0];

    Instrument instrument;
    if (order[1] == "Rose")
    {
        instrument = Instrument::ROSE;
    }
    else if (order[1] == "Lavender")
    {
        instrument = Instrument::LAVENDER;
    }
    else if (order[1] == "Lotus")
    {
        instrument = Instrument::LOTUS;
    }
    else if (order[1] == "Tulip")
    {
        instrument = Instrument::TULIP;
    }
    else
    {
        instrument = Instrument::ORCHID;
    }

    Side side;
    if (order[2] == "1")
    {
        side = Side::BUY;
    }
    else
    {
        side = Side::SELL;
    }

    int quantity = std::stoi(order[3]);
    double price = std::stod(order[4]);

    return Order{clientOrderId, instrument, side, price, quantity};
}

bool Exchange::isInstrumentValid(std::string instrument)
{
    if (instrument == "Rose" || instrument == "Lavender" || instrument == "Lotus" || instrument == "Tulip" ||
        instrument == "Orchid")
    {
        return true;
    }
    return false;
}

void processOrders(BlockingQueue<Order> &ordersQueue, BlockingQueue<ExecutionRecord> &executionRecordQueue,
                   OrderBook &orderBook)
{
    while (true)
    {
        std::optional<Order> orderOpt = ordersQueue.pop();
        if (orderOpt.has_value())
        {
            Order order = orderOpt.value();
            std::cout << "Processing order " << order.getClientOrderId() << '\n';

            std::vector<ExecutionRecord> executionRecordsList = orderBook.processOrder(order);

            for (auto executionRecord : executionRecordsList)
            {
                executionRecordQueue.push(executionRecord);
            }
        }
        else
            return;
    }
}

void writeExecutionRecords(BlockingQueue<ExecutionRecord> &executionRecordQueue, std::ofstream &file)
{
    while (true)
    {
        std::optional<ExecutionRecord> executionRecordOpt = executionRecordQueue.pop();

        if (executionRecordOpt.has_value())
        {
            ExecutionRecord executionRecord = executionRecordOpt.value();

            if (executionRecord.getStatus() == Status::REJECTED)
            {
                std::unordered_map<std::string, std::string> invalidValues = executionRecord.getInvalidValues();
                file << "ord" << executionRecord.getOrderId() << ','
                     << invalidValues["clientOrderId"] << ','
                     << invalidValues["instrument"] << ','
                     << invalidValues["side"] << ','
                     << getStatus(executionRecord.getStatus()) << ','
                     << invalidValues["quantity"] << ','
                     << invalidValues["price"] << ','
                     << executionRecord.getReason() << ','
                     << executionRecord.getTransactionTime() << '\n';
            }
            else
            {
                file << "ord" << executionRecord.getOrderId() << ','
                     << executionRecord.getClientOrderId() << ','
                     << getInstrument(executionRecord.getInstrument()) << ','
                     << getSide(executionRecord.getSide()) << ','
                     << getStatus(executionRecord.getStatus()) << ','
                     << executionRecord.getQuantity() << ','
                     << std::fixed << std::setprecision(2)
                     << executionRecord.getPrice() << ','
                     << "" << ','
                     << executionRecord.getTransactionTime() << '\n';
            }
            std::cout << "Wrote execution record for order " << executionRecord.getClientOrderId() << '\n';
        }
        else
            return;
    }
}
