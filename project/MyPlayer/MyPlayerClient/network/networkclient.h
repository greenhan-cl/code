#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>

namespace network{

    class NetworkClient : public QObject
    {
        Q_OBJECT
    public:
        NetworkClient();

        //test
        void hello();

    private:
        // ⽣成请求id
        static QString makeRequestId();

    private:
        const QString HTTP_URL = "http://127.0.0.1:8000";

        QNetworkAccessManager httpClient;

    };

}//end network

#endif // NETWORKCLIENT_H
