#ifndef DM836II_H
#define DM836II_H

#include <QObject>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <QDebug>

class DM836II : public QObject
{
    Q_OBJECT
public:

    explicit DM836II(const int &usb1,const int &id,QObject *parent = 0);
    //set the voice high or low
    void setVolume(const int&vol);
    int nowVolume();
    //set the song source where storge the song
    void setSongSource(const int&);
    int nowSource();
    //operate the song
    void next();
    void previous();
    void pause();
    void play();
    //play mode
    void setPalyMode(const int&);
    int nowPlayMode();
    //play the international broadcasting station
    void playRadioStation();
    //open or close the radio
    void setRadioStatus(const int&);
    int nowRadioStatus();

    //play sd/inter music/
    void playNow(const int &);
private:
    int fd_usb0;
    unsigned char data[64];




};

#endif // DM836II_H
