#ifndef MXCWEBSOCKETAUTH_H
#define MXCWEBSOCKETAUTH_H

#include <QObject>
#include <QWebSocket>
#include <QTimer>
#include <QByteArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QString>
#include <QMessageAuthenticationCode>
#include <QNetworkAccessManager>
#include <QNetworkReply>


#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <sstream>
#include <map>



class mxcwebsocketauth : public QObject
{
    Q_OBJECT
public:
    mxcwebsocketauth(QObject *parent = nullptr, std::string key = "", std::string secret= "")
        : QObject(parent) ,gAccessKey(key) , gSecretKey(secret)
    {

        dataSock = new QWebSocket(QString(), QWebSocketProtocol::VersionLatest,this);
        connect(dataSock,&QWebSocket::connected, this,  &mxcwebsocketauth::onWsAccountFreeConnected);
        connect(dataSock,&QWebSocket::disconnected , this, &mxcwebsocketauth::onWsAccountFreeDisconnected);
        connect(dataSock,&QWebSocket::binaryMessageReceived , this, &mxcwebsocketauth::onWsAccountFreeBinaryMessageReceived);
        connect(dataSock,&QWebSocket::textMessageReceived , this, &mxcwebsocketauth::onTextMessageReceived);


        QTimer::singleShot(100, [=](){
            this->doTest();
        });

        qnam = new QNetworkAccessManager(this);
        connect(qnam, &QNetworkAccessManager::finished, this, &mxcwebsocketauth::replyFinished);


//        QTimer* timer =  new QTimer(this);
//        connect(timer, &QTimer::timeout, this, &mxcwebsocketauth::doQuery);
//        timer->start(300);
    }

    virtual ~mxcwebsocketauth() {}

    void doQuery()
    {
        getRequest();
    }


    void replyFinished(QNetworkReply *reply)
    {
        if(reply->error())
        {
            std::cout << "reply error " << std::endl;
        }
        else {
            std::cout << "reply " << reply->readAll().toStdString() << "\n";
        }
    }

    void doTest()
    {
        QTimer::singleShot(100, [=](){
            QUrl url("wss://wbs.mxc.io/socket.io/?EIO=3&transport=websocket");
            dataSock->open(url);
            std::cout << "Do Connect " << url.toString().toStdString() <<  std::endl;
        });
    }

    void onWsAccountFreeConnected()
    {
        std::cout << "onWsAccountFreeConnected " << std::endl;
        {
            QJsonObject json;
            json.insert("symbol", "BTC_USDT");
            QJsonDocument jsondoc;
            jsondoc.setObject(json);
            QByteArray ba = jsondoc.toJson(QJsonDocument::Compact);
            QString jsonstr(ba);
            dataSock->sendTextMessage(jsonstr);
        }
    }
    void onWsAccountFreeDisconnected()
    {
        std::cout << "onWsAccountFreeDisconnected " << std::endl;
    }
    void onWsAccountFreeBinaryMessageReceived(const QByteArray &message)
    {
        std::cout << "onWsAccountFreeBinaryMessageReceived " << message.toStdString();
    }

    void onTextMessageReceived(const QString& msg)
    {
        std::cout << "onTextMessageReceived:" << msg.toStdString()  << std::endl;
    }

    QNetworkReply* getRequest()
    {

        std::map<QString, QString> ps;

        QString rs("http://api.zb.cn/data/v1/depth?market=%1&size=50");
        QUrl url = rs.arg("ltc_usdt");
        QNetworkRequest request(url);


        request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/39.0.2171.71 Safari/537.36");
        //request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

        QNetworkReply* reply = qnam->get(request);

        //    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
        //            this, SLOT(requestError(QNetworkReply::NetworkError)));
        //     qDebug() << "after Request connect " ;
        // connect(reply, &QNetworkReply::finished, [reply,this](){this->getFinished(reply);});
        // connect(reply, &QNetworkReply::error, this, &BtcOrder::requestError);
        return reply;
    }


    QWebSocket*  dataSock;
    std::string gAccessKey;
    std::string gSecretKey;
    QNetworkAccessManager* qnam;

};



#endif // MXCWEBSOCKETAUTH_H
