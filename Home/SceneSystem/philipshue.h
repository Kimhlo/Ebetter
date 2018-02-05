#ifndef PHILIPSHUE_H
#define PHILIPSHUE_H

#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QByteArray>
#include <QDebug>
#include <QString>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
#include <QObject>

class PhilipsHue:public QObject
{
    Q_OBJECT
public:

    PhilipsHue(QObject *parent = 0);

    //operate the light
    void lightsStatus(const int& number,const bool& status);
    void lightsColor(const int& number,const float* color);
    void lightsBri(const int& number,const int& bri);
    void lightsDraw(const int& number, const bool &draw);
    void lightsCt(const int& number,const int &ct);
    void lightsCt(const int& number,const int &ct,const int& bri);
    void lightsCt(const int& number,const bool& status,const int &ct,const int& bri);
    //operate the group that at least two lights is included
    int createGroup(const int* groupMembers);
    void groupControl(const int& id,const bool& status,const float *color,const int &bri);
    void groupControl(const int& id,const bool& status,const int &ct,const int &bri);
    void groupDraw(const int &id,const bool &draw);
    void groupColor(const int &id,const float *color);
    void groupStatus(const int& id,const bool& status);
    void groupCt(const int& id,const int &ct);
    void groupCt(const int& id,const int &ct,const int& bri);
    void groupBri(const int& id,const int& bri);
    void getColorXY();
    float color[2]={0,0};
private:
    //create the url for operate the light
    QUrl createUrl(const QString& str1);
    QUrl createUrl(const QString& str1,const int& number);
    QUrl createUrl(const QString& str1,const int& number,const QString& str2);

    //send the Json order
    void sendOrder(const QUrl &url,const QString &method);
    void sendOrder(const QUrl &url, const QString &method,const QByteArray& data);

    //read the received data to get the status after send the message
    int readReceivedData(const QByteArray& data);

    //network operate class define
    QNetworkRequest request;
    QNetworkAccessManager *manager=new QNetworkAccessManager;
    QNetworkReply* reply;
    //this is the base url for the control,and his format is that "ip+white list";
    QString baseUrl="http://192.168.31.200/api/fJYTtPtpqG-AqpGsamzEG6mNk0edY06OHz9NayRs";

    //this variant is used to create Json order that send to the bridge
    QByteArray jsonData;


};

#endif // PHILIPSHUE_H
