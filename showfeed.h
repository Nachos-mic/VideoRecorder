#ifndef SHOWFEED_H
#define SHOWFEED_H
#include <QCamera>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

class ShowFeed: public QObject{
    Q_OBJECT

public:
    ShowFeed();
    void useCamera();

};

#endif // SHOWFEED_H
