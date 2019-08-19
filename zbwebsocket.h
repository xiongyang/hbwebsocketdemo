#ifndef ZBWEBSOCKETAUTH_H
#define ZBWEBSOCKETAUTH_H

#include <QObject>
#include <QWebSocket>
#include <QTimer>
#include <QByteArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QString>
#include <QMessageAuthenticationCode>


#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <sstream>
#include <map>



class zbwebsocketauth : public QObject
{
    Q_OBJECT
public:
    zbwebsocketauth(QObject *parent = nullptr, std::string key = "", std::string secret= "")
        : QObject(parent) ,gAccessKey(key) , gSecretKey(secret)
    {

        dataSock = new QWebSocket(QString(), QWebSocketProtocol::VersionLatest,this);
        connect(dataSock,&QWebSocket::connected, this,  &zbwebsocketauth::onWsAccountFreeConnected);;
        connect(dataSock,&QWebSocket::disconnected , this, &zbwebsocketauth::onWsAccountFreeDisconnected);
        connect(dataSock,&QWebSocket::binaryMessageReceived , this, &zbwebsocketauth::onWsAccountFreeBinaryMessageReceived);
        connect(dataSock,&QWebSocket::textMessageReceived , this, &zbwebsocketauth::onTextMessageReceived);


        QTimer::singleShot(100, [=](){
		this->doTest();
        });


    }

    virtual ~zbwebsocketauth() {}

    void doTest()
    {
        QTimer::singleShot(100, [=](){
            QUrl url("wss://api.zb.cn/websocket");
            dataSock->open(url);
            std::cout << "Do Connect " << url.toString().toStdString() <<  std::endl;
        });
    }

    void onWsAccountFreeConnected()
    {
	    std::cout << "onWsAccountFreeConnected " << std::endl;
	    {
		    QJsonObject json;
		    json.insert("event", "addChannel");
		    json.insert("channel",  "ltcbtc_depth");
		    QJsonDocument jsondoc;
		    jsondoc.setObject(json);
		    QByteArray ba = jsondoc.toJson(QJsonDocument::Compact);
		    QString jsonstr(ba);
		    dataSock->sendTextMessage(jsonstr);
	    }

	    {
		    QJsonObject json;
		    json.insert("event", "addChannel");
		    json.insert("channel",  "ltcbtc_trades");
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

    QWebSocket*  dataSock;
    std::string gAccessKey;
    std::string gSecretKey;


};



#endif // zbwebsocketauth_H
