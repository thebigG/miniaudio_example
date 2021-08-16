#include "miniaudio/miniaudio.h"
#include <QApplication>
#include <QWidget>
#include <memory.h>

#include <stdio.h>

/**
 * Not working properly at the moment.
 * @brief calc_peak_amplitude
 * @param pOutput
 * @param pInput
 * @param frameCount
 * @return
 */
float calc_peak_amplitude(void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    //    printf("sample..\n");

    //    printf("frame count:%d\n", frameCount);

        float maxValue = 0;
        float maxAmplitude = 0x7fffffff;
        float captureValue = 0;
        float minAmplitude = 0;
        uint32_t current_sample_value = 0;

        const float * audioInput = static_cast<const float*>(pInput);

        for (unsigned int i = 0; i < frameCount; i++) {

          current_sample_value =
              static_cast<uint32_t>(std::abs((audioInput[i]) * (0x7fffffff)));
          maxValue =
              current_sample_value > maxValue ? current_sample_value : maxValue;

    //      printf("raw value-->%f\n", *(static_cast<const float*>(pInput)) * (0x7fffffff));
          if(audioInput[i]>0)
          {
              printf("raw sample-->%f\n", audioInput[i]);
          }
    //      if(current_sample_value>0)
    //      printf("current_sample_value-->%d\n", current_sample_value);

        }

        // Calculate the volume of the sound coming from the device.

    //    printf("maxValue:%f\n", maxValue);

        maxValue = std::min(maxValue, maxAmplitude);
        captureValue = static_cast<uint32_t>(maxValue) / maxAmplitude;


        // When we say "deviceLevel", what we really mean is Peak Amplitude.
        float deviceLevel = captureValue - minAmplitude;

    //    printf("deviceLevel:%f\n", deviceLevel);
    //    printf("deviceLevel:%f\n", deviceLevel);
}

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    // In playback mode copy data to pOutput. In capture mode read data from pInput. In full-duplex mode, both
    // pOutput and pInput will be valid and you can move data from pInput into pOutput. Never process more than
    // frameCount frames.
}

void init_audio_device_config(ma_device_config* config)
{
    *config = ma_device_config_init(ma_device_type_capture);
    config->playback.format   = ma_format_f32;   // Set to ma_format_unknown to use the device's native format.
    config->playback.channels = 2;               // Set to 0 to use the device's native channel count.
    config->sampleRate        = 48000;           // Set to 0 to use the device's native sample rate.
    config->dataCallback      = data_callback;   // This function will be called when miniaudio needs more data.

}

int init_audio_device(ma_device* device, ma_device_config* config)
{
    if (ma_device_init(NULL, config, device) != MA_SUCCESS) {
        return -1;  // Failed to initialize the device.
    }

    ma_result res =  ma_device_start(device);     // The device is sleeping by default so you'll need to start it manually.
    return res;
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

    config->pUserData = device->pUserData; // Can be accessed from the device object (device.pUserData).

    QApplication a(argc, argv);
    QWidget main{};
    main.show();
    a.exec();

    ma_device_uninit(device.get());

    quit();
   return 0;
}
