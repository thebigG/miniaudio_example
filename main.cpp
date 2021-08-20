#include "miniaudio/miniaudio.h"
#include <QApplication>
#include <QWidget>
#include <memory.h>

#include <stdio.h>

#define DEVICE_FORMAT       ma_format_f32
#define DEVICE_CHANNELS     2
#define DEVICE_SAMPLE_RATE  48000

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
        float maxValue = 0;
        float maxAmplitude = 0x7fffffff;
        float captureValue = 0;
        float minAmplitude = 0;
        uint32_t current_sample_value = 0;

        const float * audioInput = static_cast<const float*>(pInput);

        int clipped_sample = 0;

        std::vector<int> clips{};

        int negative_count = 0;
        int positive_count = 0;

        for (unsigned int i = 0; i < frameCount; i++) {

          current_sample_value =
              static_cast<uint32_t>(std::abs((audioInput[i]) * (0x7fffffff)));
          maxValue =
              current_sample_value > maxValue ? current_sample_value : maxValue;

          clipped_sample = ma_clip_f32(audioInput[i]);

          if(clipped_sample == -1)
          {
              negative_count += 1;
          }
          if(clipped_sample == 1)
          {
              positive_count += 1;
          }

        }
        if(positive_count>negative_count)
        printf("+1\n");
        else
            printf("-1\n");

        printf("pos:%d, negative:%d\n", positive_count, negative_count);

        // Calculate the volume of the sound coming from the device.

        maxValue = std::min(maxValue, maxAmplitude);
        captureValue = static_cast<uint32_t>(maxValue) / maxAmplitude;

        // When we say "deviceLevel", what we really mean is Peak Amplitude.
        float deviceLevel = captureValue - minAmplitude;

        return deviceLevel;

}

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
  printf("audioInput:%f\n", calc_peak_amplitude(pDevice, pInput, frameCount));

}

void init_audio_device_config(ma_device_config* config)
{
    *config = ma_device_config_init(ma_device_type_capture);
    config->playback.format   = ma_format_f32;   // Set to ma_format_unknown to use the device's native format.
    config->playback.channels = 2;               // Set to 0 to use the device's native channel count.
    config->sampleRate        = 0;           // Set to 0 to use the device's native sample rate.
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

int init_sin_wave_config(ma_waveform* sineWave,ma_waveform_config* sineWaveConfig )
{
     ma_device_config deviceConfig;
     ma_device device;

     *sineWaveConfig = ma_waveform_config_init(DEVICE_FORMAT, DEVICE_CHANNELS, DEVICE_SAMPLE_RATE, ma_waveform_type_sine, 0.2, 220);
     int res = ma_waveform_init(sineWaveConfig, sineWave);

     printf("%d\n", res);
     return 0;
}
void quit()
{
    printf("quit...");
}

int main(int argc, char** argv)
{
    std::unique_ptr<ma_waveform> sineWave = std::make_unique<ma_waveform>();
    std::unique_ptr<ma_waveform_config> sineWaveConfig = std::make_unique<ma_waveform_config>();

//    init_sin_wave_config(sineWave.get(), sineWaveConfig.get());
    std::unique_ptr<ma_device_config> config = std::make_unique<ma_device_config>();
    init_audio_device_config(config.get());

    std::unique_ptr<ma_device> device = std::make_unique<ma_device>();

//    config->pUserData = sineWave.get(); // Can be accessed from the device object (device.pUserData).

    init_audio_device(device.get(), config.get());

    QApplication a(argc, argv);
    QWidget main{};
    main.show();
    a.exec();

    ma_device_uninit(device.get());

    quit();
   return 0;
}
