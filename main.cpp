#include <QApplication>
#include <QGridLayout>
#include <QPushButton>
#include <QSpinBox>
#include <QWidget>

#include <iostream>
#include <memory>
#include <vector>
#include <stdio.h>

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>

// Please ALWAYS include cmath. Otherwise the Windows build breaks. No freaking idea why...
// It complains abs(float) functions being ambiguous
//This explains why: https://stackoverflow.com/a/5450536
/**
 * Its boils down to this: math.h is from C and was created over 10 years ago. In math.h, due to its primitive nature, the abs() function is "essentially" just for integer types and if you wanted to get the absolute value of a double, you had to use fabs().
When C++ was created it took math.h and made it cmath. cmath is essentially math.h but improved for C++. It improved things like having to distinguish between fabs() and abs, and just made abs() for both doubles and integer types.
In summary either:
Use  math.h and use abs() for integers, fabs() for doubles
or
use cmath and just have abs for everything (easier and recommended)

Hope this helps anyone who is having the same problem!
 */
#include <cmath>

#ifndef WIN32
#include <unistd.h>
#endif

#include "miniaudio/miniaudio.h"

#define DEVICE_FORMAT ma_format_f32
#define DEVICE_CHANNELS 2
#define DEVICE_SAMPLE_RATE 48000
#define MAX_AVG_SAMPLES 32

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
        static_cast<long long>(std::abs((audioInput[i]) * (0x7fffffff)));

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
  ma_result res = ma_waveform_init(sineWaveConfig, sineWave);

  return 0;
}

ma_result init_context(ma_context_config *pConfig, ma_context *pContext,
                       ma_uint32 backendCount, ma_backend backend) {
  ma_backend backends[]{backend};
  int res = ma_context_init(backends, backendCount, pConfig, pContext);

  return res;
}

bool is_device_available(ma_device* device, ma_device_config* config,
                         ma_context* context)
{
    return init_audio_device(device, config, context) == MA_SUCCESS?  true: false;
}

std::vector<ma_backend> getAudioBackends()
{
    std::array<ma_backend, MA_BACKEND_COUNT> backendArr;
    std::vector<ma_backend> backends{};
    size_t count = 0;
    ma_get_enabled_backends(backendArr.data(), backendArr.size(), &count);

    for (size_t i = 0; i < count; ++i) {
      backends.push_back(backendArr[i]);
    }

   return backends;
}

ma_bool32 enumerateCallback(ma_context* pContext, ma_device_type deviceType, const ma_device_info* pInfo, void* pUserData)
{
    auto* devices = static_cast<std::vector<std::string>*>(pUserData);
    devices->push_back(std::string{pInfo->name});
    return 0;
}

int main(int argc, char **argv) {

  std::array<ma_backend, MA_BACKEND_COUNT> backendArr;
  size_t count = 0;

  std::unique_ptr<ma_context> context = std::make_unique<ma_context>();
  std::unique_ptr<ma_context_config> contextConfig =
      std::make_unique<ma_context_config>();

  init_context(contextConfig.get(), context.get(), 1, ma_backend_pulseaudio);

   unsigned int a{0};

   //NOTE: I know the API is written in C, but I don't like this pointer busisness here...
   ma_uint32* pPlaybackDeviceCount;
   pPlaybackDeviceCount = &a;
   ma_device_info* ppCaptureDeviceInfos;
   ma_device_info info{};
   ma_uint32* pCaptureDeviceCount;
   pCaptureDeviceCount = &a;

   std::cout<<"Fetching devices"<<std::endl;
   ma_result res  = ma_context_get_devices(context.get(), nullptr, pPlaybackDeviceCount, &ppCaptureDeviceInfos, pCaptureDeviceCount);
   if(res == MA_SUCCESS)
   {
       std::cout<<"Fetched devices sucessfully"<<std::endl;
       std::cout<<"pPlaybackDeviceCount-->"<<*pCaptureDeviceCount<<std::endl;
       for(int i =0;i<*pCaptureDeviceCount; i++)
       {
           std::cout<<"device-->"<<ppCaptureDeviceInfos[i].name<<std::endl;
       }
   }
   else
   {
       std::cout<<"Something bad happended. ma_context_get_devices failed"<<std::endl;
       return -1;
   }

   std::cout<<"Available backends(resolved at compile time)"<<std::endl;
  for (auto backend: getAudioBackends()) {
    std::cout << ma_get_backend_name(backend) << std::endl;
  }

  std::unique_ptr<ma_waveform> sineWave = std::make_unique<ma_waveform>();
  std::unique_ptr<ma_waveform_config> sineWaveConfig =
      std::make_unique<ma_waveform_config>();

  std::unique_ptr<ma_device_config> config =
      std::make_unique<ma_device_config>();
  init_audio_device_config(config.get());

  std::unique_ptr<ma_device> device = std::make_unique<ma_device>();

  ma_device_uninit(device.get());

  return 0;
}
