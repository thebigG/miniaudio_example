#include "audiomanager.h"

AudioManager::AudioManager() : volume{0} {}

AudioManager::AudioManager(float initialVoulme) : volume{0} {}

void AudioManager::setVolume(float newVolume) { volume = newVolume; }
float AudioManager::getVolume() { return volume; }

/**
 *Still thinking about this...
 * Profiling Algorithm(WIP):
 * 1. Set volume to 1(MAX)
 * 2. Start audio playback
 * 3. Store ZERO base
 * 4. Stop audio playback
 * 5. Set Volume to 0(MIN)
 * 6. Start audio playback
 * 7. Store MAX base(this will be the new MAX relative to our new ZERO)
 * 8. Stop audio playback
 * 9. Lerp, maybe?? Not sure yet...
 */
void AudioManager::profile() {}
