#include "philipshue.h"

#include <QEventLoop>
#include <QColor>
#include <QDebug>
#include <QColorDialog>

PhilipsHue::PhilipsHue(QObject *parent) : QObject(parent)
{
    //when "POST" is used and this operate is needed
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
}

/*********************************************
*this function is used to operate the light
*base on the light number
**********************************************/
void PhilipsHue::lightsStatus(const int &number, const bool &status)
{
    QUrl url(createUrl("lights",number,"state"));
    QJsonObject objecet;
    QJsonDocument doc;
    objecet.insert("on",status);
    doc.setObject(objecet);
    jsonData=doc.toJson();
    sendOrder(url,"put",jsonData);

}

void PhilipsHue::lightsColor(const int &number, const float *color)
{
    QUrl url(createUrl("lights",number,"state"));
    QJsonArray array;
    QJsonObject objecet;
    QJsonDocument doc;
    array.insert(0,color[0]);
    array.insert(1,color[1]);
    QJsonValue value(array);
    objecet.insert("xy",value);
    doc.setObject(objecet);
    jsonData=doc.toJson();
    sendOrder(url,"put",jsonData);
}

void PhilipsHue::lightsBri(const int &number, const int &bri)
{
    QUrl url(createUrl("lights",number,"state"));
    QJsonArray array;
    QJsonObject objecet;
    QJsonDocument doc;
    objecet.insert("bri",bri);
    doc.setObject(objecet);
    jsonData=doc.toJson();
    sendOrder(url,"put",jsonData);

}

void PhilipsHue::lightsDraw(const int &number,const bool &draw)
{
    QJsonObject objecet,objecet1;
    QJsonDocument doc,doc1;
    request.setUrl(createUrl("lights",number,"state"));
    //open the light
    objecet.insert("on",true);
    doc.setObject(objecet);
    jsonData=doc.toJson();
    reply = manager->put(request,jsonData);
    //send the order about the light bri
    request.setUrl(createUrl("lights",number,"state"));
    if(draw==true)
    {
        for(int i=0;i<90;i+=5)
        {
            objecet1.insert("bri",i);
            doc1.setObject(objecet1);
            jsonData=doc1.toJson();
           //send the json to the bridge
            reply = manager->put(request,jsonData);

        }
    }
}

void PhilipsHue::lightsCt(const int &number, const int &ct)
{
    QUrl url(createUrl("lights",number,"state"));
    QJsonObject objecet;
    QJsonDocument doc;
    objecet.insert("ct",ct);
    doc.setObject(objecet);
    jsonData=doc.toJson();
    sendOrder(url,"put",jsonData);
}

void PhilipsHue::lightsCt(const int &number, const int &ct, const int &bri)
{
    QUrl url(createUrl("lights",number,"state"));
    QJsonObject objecet;
    QJsonDocument doc;
    objecet.insert("ct",ct);
    objecet.insert("bri",bri);
    doc.setObject(objecet);
    jsonData=doc.toJson();
    sendOrder(url,"put",jsonData);
}

void PhilipsHue::lightsCt(const int &number, const bool &status, const int &ct, const int &bri)
{
    QUrl url(createUrl("lights",number,"state"));
    QJsonObject objecet;
    QJsonDocument doc;
    objecet.insert("on",status);
    objecet.insert("ct",ct);
    objecet.insert("bri",bri);
    doc.setObject(objecet);
    jsonData=doc.toJson();
    sendOrder(url,"put",jsonData);
}
/*****************************************************************/

/*********************************************
*this function is used to operate the light
*base on the light number
**********************************************/
int PhilipsHue::createGroup(const int *groupMembers)
{
    QJsonArray array;
    QJsonObject objecet;
    QJsonDocument doc;
    int id;

    //set the request url
    request.setUrl(createUrl("groups"));
    //create the json data
    for(int i=0;i<sizeof(groupMembers)/sizeof(groupMembers[0]);i++)
    {
        array.insert(i,QString::number(groupMembers[i]));
    }
    QJsonValue value(array);
    objecet.insert("lights",value);
    objecet.insert("name","test");
    objecet.insert("type","LightGroup");
    doc.setObject(objecet);
    jsonData=doc.toJson();
    //send the json data
    reply=manager->post(request,jsonData);
    //go into the evenloop until the reply finished
    QEventLoop *eventLoop=new QEventLoop;
    connect(manager,&QNetworkAccessManager::finished, eventLoop,&QEventLoop::quit);
    eventLoop->exec();
    QByteArray receivedData = reply->readAll();
    qDebug()<<"dataReceived=" << receivedData;
    //get the group id
    id=readReceivedData(receivedData);
    return id;
}

void PhilipsHue::groupControl(const int &id, const bool &status, const float *color, const int &bri)
{
    QJsonArray array;
    QJsonObject objecet;
    QJsonDocument doc;
    //set the request
    request.setUrl(createUrl("groups",id,"action"));
    //create the Json data
    objecet.insert("on",status);
    array.insert(0,color[0]);
    array.insert(1,color[1]);
    QJsonValue value(array);
    objecet.insert("xy",value);
    objecet.insert("bri",bri);
    doc.setObject(objecet);
    jsonData=doc.toJson();
    //send the Json data
    reply=manager->put(request,jsonData);


}

void PhilipsHue::groupControl(const int &id, const bool &status, const int &ct, const int &bri)
{

    QJsonObject objecet;
    QJsonDocument doc;
    //set the request
    request.setUrl(createUrl("groups",id,"action"));
    //create the Json data
    objecet.insert("on",status);
    objecet.insert("ct",ct);
    objecet.insert("bri",bri);
    doc.setObject(objecet);
    jsonData=doc.toJson();
    //send the Json data
    reply=manager->put(request,jsonData);
}

void PhilipsHue::groupDraw(const int &id, const bool &draw)
{
    QJsonObject objecet,objecet1;
    QJsonDocument doc,doc1;
    QByteArray jsonData;
    request.setUrl(createUrl("groups",id,"action"));
    objecet.insert("on",true);
    doc.setObject(objecet);
    jsonData=doc.toJson();
    reply=manager->put(request,jsonData);
    if(draw==true){
        request.setUrl(createUrl("groups",id,"action"));
        for(int i=0;i<120;i=+2){
            objecet1.insert("bri",i);
            doc1.setObject(objecet1);
            jsonData=doc1.toJson();
            reply=manager->put(request,jsonData);
        }
    }
}

void PhilipsHue::groupColor(const int &id, const float *color)
{
    QJsonArray array;
    QJsonObject objecet;
    QJsonDocument doc;
    array.insert(0,color[0]);
    array.insert(1,color[1]);
    QJsonValue value(array);
    objecet.insert("xy",value);
    doc.setObject(objecet);
    jsonData=doc.toJson();
    request.setUrl(createUrl("groups",id,"action"));
    reply=manager->put(request,jsonData);
}

void PhilipsHue::groupStatus(const int &id, const bool &status)
{
    QJsonObject objecet;
    QJsonDocument doc;
    QByteArray jsonData;
    request.setUrl(createUrl("groups",id,"action"));
    objecet.insert("on",status);
    doc.setObject(objecet);
    jsonData=doc.toJson();
    reply=manager->put(request,jsonData);
}

void PhilipsHue::groupCt(const int &id, const int &ct)
{
    QJsonObject objecet;
    QJsonDocument doc;
    QByteArray jsonData;
    request.setUrl(createUrl("groups",id,"action"));
    objecet.insert("ct",ct);
    doc.setObject(objecet);
    jsonData=doc.toJson();
    reply=manager->put(request,jsonData);
}

void PhilipsHue::groupCt(const int &id, const int &ct, const int &bri)
{
    QJsonObject objecet;
    QJsonDocument doc;
    QByteArray jsonData;
    request.setUrl(createUrl("groups",id,"action"));
    objecet.insert("ct",ct);
    objecet.insert("bri",bri);
    doc.setObject(objecet);
    jsonData=doc.toJson();
    reply=manager->put(request,jsonData);
}

void PhilipsHue::groupBri(const int &id, const int &bri)
{
    QJsonObject objecet;
    QJsonDocument doc;
    QByteArray jsonData;
    request.setUrl(createUrl("groups",id,"action"));
    objecet.insert("bri",bri);
    doc.setObject(objecet);
    jsonData=doc.toJson();
    reply=manager->put(request,jsonData);
}

void PhilipsHue::getColorXY()
{
    float colorXY[2]={0,0};
    QColorDialog colorRGB;
    QColor value=colorRGB.getRgba();
    int r=value.red();
    int g=value.green();
    int b=value.blue();
    float num_x=0.4124*r+0.3576*g+0.1805*b;
    float num_y=0.2126*r+0.7152*g+0.0722*b;
    float den=(0.4124+0.2126+0.0193)*r+(0.3576+0.7152+0.1192)*g+(0.1805+0.0722+0.9505)*b;
    color[0]=num_x/den;
    color[1]=num_y/den;
}

/*****************************************************************/

/*********************************************
*this function is used to create the url
**********************************************/
QUrl PhilipsHue::createUrl(const QString &str1)
{
    QString strUrl="";
    strUrl=baseUrl;
    //all the url format is that baseUrl+orderClass+number+operate class
    if(str1=="lights")
    {
        strUrl=strUrl.append("/lights");

    }else if(str1=="groups")
    {
        strUrl=strUrl.append("/groups");
    }else{
        qDebug()<<"url is invide";
    }
    QUrl url(strUrl);
    strUrl.clear();
    return url;

}

QUrl PhilipsHue::createUrl(const QString &str1, const int& number)
{
    QString strUrl="";
    strUrl=baseUrl;
    //all the url format is that baseUrl+orderClass+number+operate class
    if(str1=="lights")
    {
        strUrl=strUrl.append("/lights/"+QString::number(number));

    }else if(str1=="groups")
    {
        strUrl=strUrl.append("/groups/"+QString::number(number));
    }else{
        qDebug()<<"url is invide";
    }
    QUrl url(strUrl);
    return url;
}

QUrl PhilipsHue::createUrl(const QString &str1, const int &number, const QString &str2)
{
    //all the url format is that baseUrl+orderClass+number+operate class
    QString strUrl="";
    strUrl=baseUrl;
    if(str1=="lights")
    {
        strUrl=strUrl.append("/lights/"+QString::number(number));
        if(str2=="state")
        {
            strUrl=strUrl.append("/"+str2);
        }else{
            qDebug()<<"the order is invalid";
        }

    }else if(str1=="groups")
    {
        strUrl=strUrl.append("/groups/"+QString::number(number));
        if(str2=="action")
        {
            strUrl=strUrl.append("/"+str2);
        }else{
            qDebug()<<"the order is invalid";
        }
    }else{
        qDebug()<<"url is invalid";
    }
    QUrl url(strUrl);
    return url;
}

/*****************************************************************/

/*********************************************
*this function is send the data to the bridge
**********************************************/
void PhilipsHue::sendOrder(const QUrl &url, const QString &method)
{
    //set the request url
    request.setUrl(url);
    if(method=="get")
        reply = manager->get(request);
    else
        qDebug()<<"method error";
//    QEventLoop *eventLoop=new QEventLoop;
//    QObject::connect(manager,&QNetworkAccessManager::finished, eventLoop,&QEventLoop::quit);
//    eventLoop->exec();
//    // 获取响应信息
//    QByteArray bytes = reply->readAll();
//    qDebug()<<"dataReceived=" << bytes;
}

void PhilipsHue::sendOrder(const QUrl &url, const QString &method, const QByteArray &data)
{
    request.setUrl(url);
    if(method=="put")
        reply = manager->put(request,data);
    else if(method=="post")
        reply=manager->post(request,data);
    else
        qDebug()<<"method error";
//    QEventLoop *eventLoop=new QEventLoop;
//    QObject::connect(manager,&QNetworkAccessManager::finished, eventLoop,&QEventLoop::quit);
//    eventLoop->exec();
//    // 获取响应信息
//    QByteArray bytes = reply->readAll();
//    qDebug()<<"dataReceived=" << bytes;
}

int PhilipsHue::readReceivedData(const QByteArray &data)
{
    int groupID {1};
    QJsonParseError jsonError;
    QJsonDocument parseDoc=QJsonDocument::fromJson(data,&jsonError);
    if(parseDoc.isArray()){
        QJsonArray array=parseDoc.array();
        QJsonValue value1=array.at(0);
        QJsonObject object2=value1.toObject();
        if(object2.contains("success")){
            QJsonValue value2=object2.take("success");
            QJsonObject object3=value2.toObject();
            if(object3.contains("id")){
                QJsonValue value3=object3.take("id");
                groupID=value3.toString().toInt();
                return groupID;
            }
        }

    }

}

