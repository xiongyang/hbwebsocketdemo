#ifndef HBWEBSOCKETAUTH_H
#define HBWEBSOCKETAUTH_H

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

class hbwebsocketauth : public QObject
{
    Q_OBJECT
public:
    hbwebsocketauth(QObject *parent = nullptr, std::string key = "", std::string secret= "")
        : QObject(parent) ,gAccessKey(key) , gSecretKey(secret)
    {
        accountFreeSock = new QWebSocket(QString(), QWebSocketProtocol::VersionLatest,this);
        connect(accountFreeSock,&QWebSocket::connected, this,  &hbwebsocketauth::onWsAccountFreeConnected);;
        connect(accountFreeSock,&QWebSocket::disconnected , this, &hbwebsocketauth::onWsAccountFreeDisconnected);
        connect(accountFreeSock,&QWebSocket::textMessageReceived , this, &hbwebsocketauth::onWsAccountFreeBinaryMessageReceived);


        QTimer::singleShot(100, [=](){
            //QUrl url("wss://api.huobi.pro/ws/v1");
            QUrl url("wss://api.huobi.pro/ws/v2");
            accountFreeSock->open(url);
            std::cout << "Do Connect " << std::endl;
        });

    }

    virtual ~hbwebsocketauth() {}

    void onWsAccountFreeConnected()
    {
         std::cout << "onWsAccountFreeConnected " << std::endl;


         std::map<QString, QString> ps;

         ps["accessKey"] = gAccessKey.c_str();
         ps["signatureMethod"] = "HmacSHA256";
         ps["signatureVersion"] = "2.1";
         std::string dateStrConvented = currentDateTime();
        // ps["timestamp"] = dateStrConvented.c_str();
        ps["timestamp"] = convertDateTimeOrg(dateStrConvented).c_str();



         QString apikey;
         for(auto & each : ps)
         {
             apikey.append(each.first + "=" + each.second + "&");
         }
         // remove last &
         apikey.resize(apikey.size()-1);

         QByteArray  calcString;
         calcString.append("GET\napi.huobi.pro\n/ws/v2\n");
         calcString.append(apikey);




         QByteArray sign = QMessageAuthenticationCode::hash(calcString, gSecretKey.c_str(), QCryptographicHash::Sha256).toBase64();
         //QByteArray signstr = sign.toPercentEncoding();
       //  QString parms = apikey  + "&Signature=" + signstr;

         std::cout << "String to Sing :" << calcString.toStdString() << std::endl;
         std::cout << "auth Sign      :" << sign.toStdString() << std::endl;

         QJsonObject params;
         params.insert("authType","api");
         params.insert("accessKey",  ps["accessKey"]);
         params.insert("signatureMethod", ps["signatureMethod"]);
         params.insert("signatureVersion", ps["signatureVersion"]);
         params.insert("timestamp", convertDateTimeOrg(dateStrConvented).c_str());
         params.insert("signature", QString(sign));

         
         QJsonObject json;
         json.insert("action", "req");
         json.insert("ch", "auth");
         json.insert("params", params);
         QJsonDocument jsonDoc;
         jsonDoc.setObject(json);
         QByteArray ba = jsonDoc.toJson(QJsonDocument::Compact);
//         QByteArray ba = jsonDoc.toJson();
         QString jsonStr(ba);

     //    QString jsonStrFormat;
     //    jsonStrFormat = "{\"op\":\"auth\",\"AccessKeyId\":\"%1\",\"SignatureMethod\":\"HmacSHA256\",\"SignatureVersion\":\"2\",\"Timestamp\":\"%2\",\"Signature\":\"%3\"}";
     //    QString jsonStr = jsonStrFormat.arg(ps["AccessKeyId"], convertDateTimeOrg(dateStrConvented).c_str(), QString(sign)); //
         std::cout <<  "OEEHuobiNew accountFreeSock DoAuth " << jsonStr.toStdString() << std::endl;
         accountFreeSock->sendTextMessage(jsonStr);
    }
    void onWsAccountFreeDisconnected()
    {
        std::cout << "onWsAccountFreeDisconnected " << std::endl;
    }
    void onWsAccountFreeBinaryMessageReceived(const QString& message)
    {
   //     QByteArray baUnpack;
   //     bool bResult = gzipDecompress(message, baUnpack);
   //     if (!bResult)
   //     {
   //         std::cout << "unPack Error Receive Data " << message.toStdString() << std::endl;
   //         return;
   //     }
        std::cout  << "onWsAccountFreeBinaryMessageReceived recv " << message.toStdString() << std::endl;

        QJsonParseError jsonError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(message.toLatin1(), &jsonError);
        if(jsonError.error != QJsonParseError::NoError) return;

        if(!jsonDoc.isObject()) return;

        QJsonObject jsonObj = jsonDoc.object();
        if(jsonObj.contains("ping"))
        {
            processHeartbeat(jsonObj, accountFreeSock);
            return;
        }
        else
        {

        }
    }


    void processHeartbeat(QJsonObject &jsonObj, QWebSocket* sock)
    {
        QJsonObject jsonPong;
        jsonPong.insert("pong", jsonObj.value("ping"));
        QJsonDocument docPong(jsonPong);
        std::cout << "Send Pong for Sock:" << sock << std::endl;
        sock->sendTextMessage(QString(docPong.toJson(QJsonDocument::Compact)));
    }


    const std::string currentDateTime()
    {
        time_t     now = time(0);
        struct tm  tstruct;
        char       buf[80];
        tstruct = *gmtime(&now);
        // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
        // for more information about date/time format

        //    2018-01-06T14%3A52%3A59
        strftime(buf, sizeof(buf), "%Y-%m-%dT%H%%3A%M%%3A%S", &tstruct);

        return buf;
    }

    std::string convertDateTimeOrg(const std::string& time)
    {
      std::string ret(time);
      boost::algorithm::replace_all(ret,"%3A",":");
      return ret;
    }


    bool gzipDecompress(const QByteArray & in , QByteArray& out) {
        std::stringstream ss_decomp;
        boost::iostreams::filtering_istream stream;
        stream.push(boost::iostreams::gzip_decompressor());
        //in.push(ss_comp);     //从字符流中解压
        stream.push(boost::iostreams::array_source(in.constData(), in.count()));     //从文件中解压
        boost::iostreams::copy(stream, ss_decomp);
        out.append(ss_decomp.str().c_str() , (ss_decomp.str().size()));
        return true;
    }


    QWebSocket*  accountFreeSock;


     std::string gAccessKey;
     std::string gSecretKey;


};



#endif // HBWEBSOCKETAUTH_H
