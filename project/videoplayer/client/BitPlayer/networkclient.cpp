#include "networkclient.h"
#include "util.h"
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>


namespace network{

    NetworkClient::NetworkClient()
        : QObject{nullptr}
    {

    }

    void NetworkClient::hello()
    {
        // 1. 构造请求体 body
        QJsonObject reqBody;
        // 2. 发送请求
        QNetworkRequest httpReq;
        httpReq.setUrl(HTTP_URL + "/hello");
        httpReq.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=utf8");
        QJsonDocument document(reqBody);
        QNetworkReply* httpResp = httpClient.post(httpReq, document.toJson());
        // 3. 异步处理响应
        connect(httpResp, &QNetworkReply::finished, this, [=]() {
            // 判定Http层⾯是否出错
            if(httpResp->error() != QNetworkReply::NoError){
                LOG()<<httpResp->errorString();
                httpResp->deleteLater();
                return;
            }
            // 获取到响应的body
            QByteArray respBody = httpResp->readAll();
            // 针对body反序列化
            QJsonDocument jsonDoc = QJsonDocument::fromJson(respBody);
            if(jsonDoc.isNull()){
                LOG()<<"解析 JSON ⽂件失败! JSON ⽂件格式有错误!";
                httpResp->deleteLater();
                return;
            }
            // 判定业务上的逻辑是否正确
            QJsonObject respObj = jsonDoc.object();
            if(0 != respObj["errorCode"].toInt()){
                LOG()<<respObj["errorMsg"].toString();
                httpResp->deleteLater();
                return;
            }
            // 解析响应数据
            QJsonObject resoBody = respObj["data"].toObject();
            LOG()<<resoBody["hello"].toString();
            httpResp->deleteLater();
        });
    }

}//end network
