#pragma once

#include <QByteArray>
#include <QDataStream>
#include <QTcpSocket>
#include <mutex>

class DataClient : public QObject {
    Q_OBJECT

   public:
    explicit DataClient(const QString& ipToConnect, quint16 port, QObject* parent = 0);

    // Send functions
    void sendData(void* data, quint16 dataSize);

   public slots:
    void slotConnected();
    void slotReadyToRead();
    void slotError(QAbstractSocket::SocketError err);

   signals:
    // Receive signal
    void signalDataWasProcessed(QByteArray data);

   private:
    std::mutex dataMx_;
    QByteArray data_;
    quint16 blockSize_ = 0;
    QTcpSocket socket_;
    QDataStream dataStream_;
};
