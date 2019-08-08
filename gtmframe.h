#ifndef GTMFRAME_H
#define GTMFRAME_H

#include <QFrame>
#include <QPushButton>
#include "qt5audioinputinterface.h"
#include "systemabstract.h"


namespace Ui {
class gtmFrame;
}
class Qtg2mSystem;
class gtmFrame : public QFrame
{
    Q_OBJECT
protected:

    Qt5AudioInputInterface *interf;
public:
    explicit gtmFrame(QWidget *parent = nullptr,Qt5AudioInputInterface *i=nullptr);
    ~gtmFrame();
    void SetStatus(std::string st);
    QPushButton *continuebt;
    QPushButton *Pausebt;
    QPushButton *exitbt;

private:
    Ui::gtmFrame *ui;
public slots:
    void Pause();
    void Play();
    void Exit();
    void DoOneStep();
};

class Qtg2mSystem:public SystemAbstract
{
    gtmFrame *MainWnd;
public:
    Qtg2mSystem(gtmFrame *mn);
    void SetStatus(std::string text)override;
};


#endif // GTMFRAME_H
