
#if 0

#ifndef QT5MEDIAPLAYERCONTROLGTM_H
#define QT5MEDIAPLAYERCONTROLGTM_H
#include <QMediaPlayerControl>

class qt5MediaPlayerControlgtm : public QMediaPlayerControl
{
public:
    qt5MediaPlayerControlgtm();
    virtual QMediaContent media() const override {};
    virtual QMediaPlayer::MediaStatus mediaStatus() const override {};
    virtual QIODevice *mediaStream() const override {};
    virtual void pause() override {};
    virtual void play(){};



};

#endif // QT5MEDIAPLAYERCONTROLGTM_H

#endif
