#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <QWidget>

class AudioManager {
private:
  float volume;

public:
  AudioManager();
  AudioManager(float initialVoulme);

  float getVolume();
  void setVolume(float);
  void profile();
};

#endif // AUDIOMANAGER_H
