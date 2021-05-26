#include <cmath>
#include <fstream>
#include <iostream>

class SineOscillator
{
    float frequency;
    float amplitude;
    float angle;
    float offset;

  public:
    SineOscillator(float freq, float amp, uint32_t sampleRate)
        : frequency(freq), amplitude(amp), angle(0), offset(static_cast<float>(2 * M_PI * frequency / sampleRate))
    {
    }

    float process()
    {
        auto sample = amplitude * std::sin(angle);
        angle += offset;
        return sample; // Asin(2pif/sr)
    }
};

#pragma pack(2)
class WaveFile
{
  private:
    std::ofstream stream;
    uint32_t size;
    uint16_t compression;
    uint16_t channels;
    uint32_t sampleRate;
    uint16_t bitDepth;

  public:
    WaveFile(const std::string &name, uint32_t size_, uint16_t compression_, uint16_t channels_, uint32_t sampleRate_,
             uint16_t bitDepth_)
        : stream(name), size(size_), compression(compression_), channels(channels_), sampleRate(sampleRate_),
          bitDepth(bitDepth_){};
    ~WaveFile()
    {
        if (stream.is_open())
            stream.close();
    }

    bool write()
    {
        if (!stream.is_open())
            return false;

        uint32_t duration = 4;
        SineOscillator sineOscillator(440, 0.5, sampleRate);
        SineOscillator sineOscillator2(880, 0.5, sampleRate);

        stream << "RIFF----WAVEfmt ";
        writeBinaryValue(size);
        writeBinaryValue(compression);
        writeBinaryValue(channels);
        writeBinaryValue(sampleRate);

        uint32_t byteRate = sampleRate * bitDepth / 8;
        uint16_t blockAlign = bitDepth / 8;

        writeBinaryValue(byteRate);
        writeBinaryValue(blockAlign);
        writeBinaryValue(bitDepth);
        stream << "data----";

        uint32_t preAudioPosition = static_cast<uint32_t>(stream.tellp());

        int16_t maxAmplitude = static_cast<int16_t>(std::pow(2, bitDepth - 1) - 1);
        for (uint32_t i = 0; i < sampleRate * duration; ++i)
        {
            float sample = sineOscillator.process();
            int16_t intSample = static_cast<int16_t>(sample * maxAmplitude);
            writeBinaryValue(intSample);
        }

        for (uint32_t i = 0; i < sampleRate * duration; ++i)
        {
            float sample = sineOscillator2.process();
            int16_t intSample = static_cast<int16_t>(sample * maxAmplitude);
            writeBinaryValue(intSample);
        }

        uint32_t postAudioPosition = static_cast<uint32_t>(stream.tellp());
        
        stream.seekp(preAudioPosition - 4);
        writeBinaryValue(postAudioPosition - preAudioPosition);

        stream.seekp(4, std::ios::beg);
        writeBinaryValue(postAudioPosition - 8);

        return true;
    }

    template <typename T> void writeBinaryValue(T value)
    {
        stream.write(reinterpret_cast<const char *>(&value), sizeof value);
    }
};

int main()
{
    WaveFile wf("sound.wav", 16, 1, 1, 44100, 16);
    std::cout << wf.write() << std::endl;

    return 0;
}
