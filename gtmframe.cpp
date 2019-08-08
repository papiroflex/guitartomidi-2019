#include "gtmframe.h"
#include "ui_gtmframe.h"
#include "qt5audioinputinterface.h"
#include "../ObjectVisualiser/audiobuffvisualiser.h"

gtmFrame::gtmFrame(QWidget *parent,Qt5AudioInputInterface *i) :
    QFrame(parent),
    ui(new Ui::gtmFrame)
{
    interf=i;
    ui->setupUi(this);


    connect(ui->continue_2,SIGNAL(clicked()),this,SLOT(Play()));
    connect(ui->Pause,SIGNAL(clicked()),this,SLOT(Pause()));

    connect(ui->exit,SIGNAL(clicked()),this,SLOT(Exit()));
    connect(ui->StepButton,SIGNAL(clicked()),this,SLOT(DoOneStep()));
}

gtmFrame::~gtmFrame()
{
    delete ui;
}

void gtmFrame::SetStatus(std::string st)
{
    ui->statuslabel->setText(QString(st.data()));
};




void gtmFrame::Pause()
{
    std::cout<<"Application Paused"<<endl;
    interf->gfa->NewEventsPause=true;
    //interf->Pause();

};
void gtmFrame::Play()
{
    interf->Play();
};

void gtmFrame::Exit()
{
    QApplication::exit(0);
}


Qtg2mSystem::Qtg2mSystem(gtmFrame *mn):SystemAbstract ()
{
    MainWnd=mn;
};

void Qtg2mSystem::SetStatus(std::string text)
{
    MainWnd->SetStatus(text);
}
void gtmFrame::DoOneStep()
{
    interf->gfa->NewEventsPause=true;
    interf->DoOneStep();
};
