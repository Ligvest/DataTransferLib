#include "dataclient.h"
#include <stdlib.h>
#include <QHostAddress>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

DataClient::DataClient(const QString& ipToConnect, quint16 port, QObject* parent)
    : QObject(parent), dataStream_(&socket_) {
    connect(&socket_, &QTcpSocket::readyRead, this, &DataClient::slotReadyToRead);
    connect(&socket_, &QTcpSocket::connected, this, &DataClient::slotConnected);
    connect(&socket_, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this,
            &DataClient::slotError);
    socket_.connectToHost(ipToConnect, port, QIODevice::ReadWrite);
}

void DataClient::slotConnected() {
    auto host = socket_.peerAddress();
    auto port = socket_.peerPort();
    std::cout << "You are connected to " << host.toString().toStdString() << ":" << port
              << std::endl;
}

void DataClient::slotReadyToRead() {
    while (true) {
        if (blockSize_ == 0) {
            if (socket_.bytesAvailable() < sizeof(quint16)) {
                break;
            }
            dataStream_ >> blockSize_;
        }

        if (socket_.bytesAvailable() < blockSize_) {
            break;
        }

        dataStream_ >> data_;

        emit signalDataWasProcessed(data_);

        data_.clear();
        blockSize_ = 0;
    }
}

void DataClient::slotError(QAbstractSocket::SocketError err) { std::cout << "error!" << std::endl; }

void DataClient::sendData(void* data, quint16 dataSize) {
    // Convert data to QByteArray
    QByteArray dataAsBytes;
    dataAsBytes.resize(dataSize);
    memcpy(dataAsBytes.data(), data, dataSize);

    // Convert data to stream data
    QByteArray convertedData;
    QDataStream convertor(&convertedData, QIODevice::ReadWrite);
    convertor << quint16(0) << dataAsBytes;
    convertor.device()->seek(0);
    convertor << (quint16)(convertedData.size() - sizeof(quint16));

    auto written = socket_.write(convertedData, convertedData.size());
    if (written < 0) {
        std::cerr << "Error when writing into socket" << std::endl;
    }
}
