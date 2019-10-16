#include "dataserver.h"
#include <iostream>

DataServer::DataServer(quint16 port, QObject* parent) : QObject(parent) {
    server_ = new QTcpServer(this);

    connect(server_, &QTcpServer::newConnection, this, &DataServer::slotNewConnection);

    if (!server_->listen(QHostAddress::Any, port)) {
        qDebug() << "Server is not started";
    } else {
        qDebug() << "Server is started";
    }
}

void DataServer::slotNewConnection() {
    std::cerr << "Established new connection" << std::endl;
    socket_ = server_->nextPendingConnection();
    connect(socket_, &QTcpSocket::readyRead, this, &DataServer::slotReadyToRead);
    connect(socket_, &QTcpSocket::disconnected, this, &DataServer::slotClientDisconnected);
    dataStream_.setDevice(socket_);
}

void DataServer::slotReadyToRead() {
    while (true) {
        if (blockSize_ == 0) {
            if (socket_->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            dataStream_ >> blockSize_;
        }

        if (socket_->bytesAvailable() < blockSize_) {
            break;
        }

        dataStream_ >> data_;

        emit signalDataWasProcessed(data_);

        data_.clear();
        blockSize_ = 0;
    }
}

void DataServer::slotClientDisconnected() { socket_->close(); }

void DataServer::sendData(void* data, quint16 dataSize) {
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

    auto written = socket_->write(convertedData, convertedData.size());
    if (written < 0) {
        std::cerr << "Error when writing into socket" << std::endl;
    }
}
