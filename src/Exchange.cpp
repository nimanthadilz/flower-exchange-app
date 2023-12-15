#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <thread>
#include <utility>
#include <vector>

#include "Exchange.h"
#include "ExecutionRecord.h"
#include "Order.h"

void Exchange::receiveOrder(std::vector<std::string> order)
{
    if (!validateOrder(order))
        return;
    Order newOrder = createOrder(order);
    m_queueMap[order[1]]->push(newOrder);
}

void Exchange::init(std::ofstream &executionRecordsFile)
{
    for (auto instrument : m_instruments)
    {
        // initialize queue
        m_queueMap[instrument] = std::make_unique<BlockingQueue<Order>>();

        // initialize order book
        m_orderBookMap[instrument] = std::make_unique<OrderBook>();

        // start thread
        m_threadMap[instrument] = std::thread(&Exchange::processOrders, this, instrument);
    }
    m_executionRecordQueuePtr = std::make_unique<BlockingQueue<ExecutionRecord>>();
    m_threadMap["executionRecordsWriterThread"] = std::thread(&Exchange::writeExecutionRecords, this,
                                                              std::ref(executionRecordsFile));
}

void Exchange::setOrderProducerDone()
{
    for (auto instrument : m_instruments)
    {
        m_queueMap[instrument]->setDone();
        m_threadMap[instrument].join();
    }
    m_executionRecordQueuePtr->setDone();
    m_threadMap["executionRecordsWriterThread"].join();
}

void Exchange::addInstrument(std::string instrument)
{
    m_instruments.push_back(instrument);
}

Order Exchange::createOrder(std::vector<std::string> order)
{
    std::string clientOrderId{order[0]};

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

    return Order{clientOrderId, order[1], side, price, quantity};
}

bool Exchange::isInstrumentValid(std::string instrument)
{
    if (m_queueMap.find(instrument) != m_queueMap.end())
    {
        return true;
    }
    return false;
}

void Exchange::processOrders(std::string instrument)
{
    while (true)
    {
        std::optional<Order> orderOpt = m_queueMap[instrument]->pop();
        if (orderOpt.has_value())
        {
            Order order = orderOpt.value();
            std::cout << "Processing order " << order.getClientOrderId() << '\n';

            std::vector<ExecutionRecord> executionRecordsList = m_orderBookMap[instrument]->processOrder(order);

            for (auto executionRecord : executionRecordsList)
            {
                m_executionRecordQueuePtr->push(executionRecord);
            }
        }
        else
            return;
    }
}

void Exchange::writeExecutionRecords(std::ofstream &file)
{
    while (true)
    {
        std::optional<ExecutionRecord> executionRecordOpt = m_executionRecordQueuePtr->pop();

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
                     << executionRecord.getInstrument() << ','
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

bool Exchange::validateOrder(std::vector<std::string> order)
{
    if (order.size() < 5)
    {
        std::cout << "Invalid order: expected field count: 5, received field count: " << order.size() << '\n';
        return false;
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
        m_executionRecordQueuePtr->push(ExecutionRecord(Order::getNextOrderId(), orderMap, "Invalid client order id"));
        return false;
    }

    if (!isInstrumentValid(order[1]))
    {
        std::cout << "Invalid order: client order id: '" << order[0] << "' invalid instrument: " << order[1] << '\n';
        m_executionRecordQueuePtr->push(ExecutionRecord(Order::getNextOrderId(), orderMap, "Invalid instrument"));
        return false;
    }

    if (order[2] != "1" && order[2] != "2")
    {
        std::cout << "Invalid order: order id: '" << order[0] << "' invalid side: " << order[2] << '\n';
        m_executionRecordQueuePtr->push(ExecutionRecord(Order::getNextOrderId(), orderMap, "Invalid side"));
        return false;
    }

    try
    {
        int quantity = std::stoi(order[3]);
        if (quantity < 10 || quantity > 1000 || quantity % 10 != 0)
        {
            std::cout << "Invalid order: order id: '" << order[0] << "' invalid quantity: " << order[3] << '\n';
            m_executionRecordQueuePtr->push(ExecutionRecord(Order::getNextOrderId(), orderMap, "Invalid quantity"));
            return false;
        }
    }
    catch (std::invalid_argument &e)
    {
        std::cout << "Invalid order: order id: '" << order[0] << "' invalid quantity: " << order[3] << '\n';
        m_executionRecordQueuePtr->push(ExecutionRecord(Order::getNextOrderId(), orderMap, "Invalid quantity"));
        return false;
    }
    catch (std::out_of_range &e)
    {
        std::cout << "Invalid order: order id: '" << order[0] << "' invalid quantity: " << order[3] << '\n';
        m_executionRecordQueuePtr->push(ExecutionRecord(Order::getNextOrderId(), orderMap, "Invalid quantity"));
        return false;
    }

    try
    {
        double price = std::stod(order[4]);
        if (price < 0)
        {
            std::cout << "Invalid order: order id: '" << order[0] << "' invalid price: " << order[4] << '\n';
            m_executionRecordQueuePtr->push(ExecutionRecord(Order::getNextOrderId(), orderMap, "Invalid price"));
            return false;
        }
    }
    catch (std::invalid_argument &e)
    {
        std::cout << "Invalid order: order id: '" << order[0] << "' invalid price: " << order[4] << '\n';
        m_executionRecordQueuePtr->push(ExecutionRecord(Order::getNextOrderId(), orderMap, "Invalid price"));
        return false;
    }
    catch (std::out_of_range &e)
    {
        std::cout << "Invalid order: order id: '" << order[0] << "' invalid price: " << order[4] << '\n';
        m_executionRecordQueuePtr->push(ExecutionRecord(Order::getNextOrderId(), orderMap, "Invalid price"));
        return false;
    }

    return true;
}
