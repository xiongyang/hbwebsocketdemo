#include <QCoreApplication>


#include <string>
#include <iostream>
#include "hbwebsocketauth.h"
#include "zbwebsocket.h"

int main(int argc, char** argv)
{
    QCoreApplication a(argc, argv);


    zbwebsocketauth demo(nullptr, "" , "");

    return a.exec();


}

int main1(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if(argc != 3)
    {
        std::cout << "Useage: app api_key api_Secret" << std::endl;
        return 0;
    }
    std::string gAccessKey = argv[1];
    std::string gSecretKey = argv[2];
    std::cout  << "Test Auth Using " << gAccessKey << "  " << gSecretKey << std::endl;

    hbwebsocketauth demo(nullptr,gAccessKey, gSecretKey);

    return a.exec();
}
