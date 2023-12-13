#include <atomic>
#include <fstream>
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
    // TODO: do validations

    Order newOrder = createOrder(order);

    switch (newOrder.getInstrument())
    {
    case Instrument::ROSE:
        roseOrdersQueue.push(newOrder);
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

    double price = std::stod(order[3]);
    int quantity = std::stoi(order[4]);

    return Order{clientOrderId, instrument, side, price, quantity};
}

void processOrders(BlockingQueue<Order> &ordersQueue, BlockingQueue<ExecutionRecord> &executionRecordQueue)
{
    while (true)
    {
        std::optional<Order> orderOpt = ordersQueue.pop();
        if (orderOpt.has_value())
        {
            Order order = orderOpt.value();
            std::cout << "Processing order " << order.getClientOrderId() << '\n';

            // TODO: processing logic

            ExecutionRecord executionRecord{"1", order.getClientOrderId(), order.getInstrument(), order.getSide(),
                                            Status::NEW, order.getQuantity(), order.getPrice()};
            executionRecordQueue.push(executionRecord);
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
            file << "XXX,"
                 << executionRecord.getClientOrderId() << ','
                 << getInstrument(executionRecord.getInstrument()) << ','
                 << getSide(executionRecord.getSide()) << ','
                 << getStatus(executionRecord.getStatus()) << ','
                 << executionRecord.getQuantity() << ',' << executionRecord.getPrice() << '\n';
            std::cout << "Wrote execution record " << executionRecord.getClientOrderId() << '\n';
        }
        else
            return;
    }
}
