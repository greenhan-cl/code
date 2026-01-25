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
        const QString HTTP_URl = "http://127/0.0.1";

        QNetworkAccessManager httpClient;

    };

}

#endif // NETWORKCLIENT_H
