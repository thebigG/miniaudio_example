#include <QApplication>
#include <QGridLayout>
#include <QPushButton>
#include <QSpinBox>
#include <QWidget>

#include <functional>
#include <iostream>
#include <memory.h>
#include <numeric>
#include <stdio.h>

#include "miniaudio/miniaudio.h"

#define DEVICE_FORMAT ma_format_f32
#define DEVICE_CHANNELS 2
#define DEVICE_SAMPLE_RATE 48000
#define MAX_AVG_SAMPLES 32

std::vector<int> values{};

/**
 * @brief calc_peak_amplitude
 * @param pOutput
 * @param pInput
 * @param frameCount
 * @return
 */
float calc_peak_amplitude(void *pOutput, const void *pInput,
                          ma_uint32 frameCount) {
  float maxValue = 0;
  float maxAmplitude = 0x7fffffff;
  float captureValue = 0;
  float minAmplitude = 0;
  uint32_t current_sample_value = 0;

  const float *audioInput = static_cast<const float *>(pInput);

  for (unsigned int i = 0; i < frameCount; i++) {
    current_sample_value =
        static_cast<uint32_t>(std::abs((audioInput[i]) * (0x7fffffff)));

    maxValue =
        current_sample_value > maxValue ? current_sample_value : maxValue;
  }

  // Calculate the volume of the sound coming from the device.

  maxValue = std::min(maxValue, maxAmplitude);
  captureValue = static_cast<uint32_t>(maxValue) / maxAmplitude;

  // When we say "deviceLevel", what we really mean is Peak Amplitude.
  float deviceLevel = captureValue - minAmplitude;

  return deviceLevel;
}

void data_callback(ma_device *pDevice, void *pOutput, const void *pInput,
                   ma_uint32 frameCount) {
    printf("audioInput:%f\n", calc_peak_amplitude(pDevice, pInput,
    frameCount));
}

void init_audio_device_config(ma_device_config *config) {
  *config = ma_device_config_init(ma_device_type_capture);
  config->capture.format = ma_format_f32; // Set to ma_format_unknown to use
                                           // the device's native format.
  config->capture.channels =
      2;                  // Set to 0 to use the device's native channel count.
  config->sampleRate = 0; // Set to 0 to use the device's native sample rate.
  config->dataCallback = data_callback; // This function will be called when
                                        // miniaudio needs more data.
}

int init_audio_device(ma_device *device, ma_device_config *config,
                      ma_context *context) {
  if (ma_device_init(context, config, device) != MA_SUCCESS) {
    return -1; // Failed to initialize the device.
  }

  ma_result res =
      ma_device_start(device); // The device is sleeping by default so you'll
                               // need to start it manually.
  return res;
}

int init_sin_wave_config(ma_waveform *sineWave,
                         ma_waveform_config *sineWaveConfig) {
  ma_device_config deviceConfig;
  ma_device device;

  *sineWaveConfig = ma_waveform_config_init(DEVICE_FORMAT, DEVICE_CHANNELS,
                                            DEVICE_SAMPLE_RATE,
                                            ma_waveform_type_sine, 0.2, 220);
  int res = ma_waveform_init(sineWaveConfig, sineWave);

  printf("%d\n", res);
  return 0;
}

ma_result init_context(ma_context_config *pConfig, ma_context *pContext,
                       ma_uint32 backendCount, ma_backend backend) {
  ma_backend backends[]{backend};
  ma_context_init(backends, backendCount, pConfig, pContext);

  return 0;
}

int main(int argc, char **argv) {

  std::array<ma_backend, MA_BACKEND_COUNT> backendArr;
  size_t count = 0;
  ma_get_enabled_backends(backendArr.data(), backendArr.size(), &count);

  std::unique_ptr<ma_context> context = std::make_unique<ma_context>();
  std::unique_ptr<ma_context_config> contextConfig =
      std::make_unique<ma_context_config>();

  init_context(contextConfig.get(), context.get(), 1, ma_backend_pulseaudio);

  for (size_t i = 0; i < count; ++i) {
    auto const backend = backendArr[i];
    std::cout << ma_get_backend_name(backend) << std::endl;
  }

  std::unique_ptr<ma_waveform> sineWave = std::make_unique<ma_waveform>();
  std::unique_ptr<ma_waveform_config> sineWaveConfig =
      std::make_unique<ma_waveform_config>();

  std::unique_ptr<ma_device_config> config =
      std::make_unique<ma_device_config>();
  init_audio_device_config(config.get());

  std::unique_ptr<ma_device> device = std::make_unique<ma_device>();

  init_audio_device(device.get(), config.get(), context.get());

  QApplication a(argc, argv);

  a.exec();

  ma_device_uninit(device.get());

  return 0;
}
