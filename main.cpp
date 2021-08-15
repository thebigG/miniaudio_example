#include "miniaudio/miniaudio.h"
#include <QApplication>
#include <QWidget>
#include <memory.h>

#include <stdio.h>

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    // In playback mode copy data to pOutput. In capture mode read data from pInput. In full-duplex mode, both
    // pOutput and pInput will be valid and you can move data from pInput into pOutput. Never process more than
    // frameCount frames.

    printf("sample..\n");


}

void init_audio_device_config(ma_device_config* config)
{
    *config = ma_device_config_init(ma_device_type_playback);
    config->playback.format   = ma_format_f32;   // Set to ma_format_unknown to use the device's native format.
    config->playback.channels = 2;               // Set to 0 to use the device's native channel count.
    config->sampleRate        = 48000;           // Set to 0 to use the device's native sample rate.
    config->dataCallback      = data_callback;   // This function will be called when miniaudio needs more data.

}

int init_audio_device(ma_device* device, ma_device_config* config)
{
//    ma_device device;
    if (ma_device_init(NULL, config, device) != MA_SUCCESS) {
        return -1;  // Failed to initialize the device.
    }
//    config.pUserData         = device.pUserData;   // Can be accessed from the device object (device.pUserData).
    ma_device_start(device);     // The device is sleeping by default so you'll need to start it manually.
    // Do something here. Probably your program's main loop.

    return 0;
}

void quit()
{
    printf("quit...");
}

int main(int argc, char** argv)
{
    std::unique_ptr<ma_device_config> config = std::make_unique<ma_device_config>();
    init_audio_device_config(config.get());

    std::unique_ptr<ma_device> device = std::make_unique<ma_device>();
    init_audio_device(device.get(), config.get());

    config->pUserData = device->pUserData;

    QApplication a(argc, argv);
    QWidget main{};
    main.show();
    a.exec();

    ma_device_uninit(device.get());

    quit();
   return 0;
}
