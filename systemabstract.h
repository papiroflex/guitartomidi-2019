#ifndef SYSTEMABSTRACT_H
#define SYSTEMABSTRACT_H
#include <QObject>

class SystemAbstract : public QObject
{
    Q_OBJECT
public:
    explicit SystemAbstract(QObject *parent = nullptr);
    virtual void SetStatus(std::string text)=0;
protected:
    static SystemAbstract *Sys;

public:
//    explicit SystemAbstract(){Sys=this;}
    static SystemAbstract& SysI(){return *SystemAbstract::Sys;}
signals:

public slots:
};

#endif // SYSTEMABSTRACT_H
