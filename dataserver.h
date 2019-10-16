#pragma once

#include <QByteArray>
#include <QDataStream>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class DataServer : public QObject {
    Q_OBJECT

   public:
    explicit DataServer(quint16 port, QObject* parent = nullptr);

    // Send functions
    void sendData(void* data, quint16 size);

   public slots:
    void slotNewConnection();
    void slotReadyToRead();
    void slotClientDisconnected();

   signals:
    // Receive signal
    void signalDataWasProcessed(QByteArray data);

   private:
    QByteArray data_;
    QTcpServer* server_ = nullptr;
    QTcpSocket* socket_ = nullptr;
    QDataStream dataStream_;
    quint16 blockSize_ = 0;
};
