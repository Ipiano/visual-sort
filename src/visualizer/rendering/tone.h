#pragma once

#ifdef WITH_SFML
#    include <SFML/Audio.hpp>
#    include <SFML/Audio/SoundBuffer.hpp>
#endif

#include <array>
#include <cstdint>

namespace rendering::audio
{
class ToneGenerator
{
  public:
    ToneGenerator();
    void start();
    void set_pitch(float);
    void stop();

#ifdef WITH_SFML
  private:
    constexpr static double PI = 3.141592653589793;

    // Audio sample rate
    constexpr static uint64_t SAMPLE_RATE = 44100;

    // Base tone frequency - C
    //constexpr static double FREQ = 256;
    constexpr static double FREQ = 125;

    // 1 second audio sample
    std::array<std::int16_t, SAMPLE_RATE> m_samples {{}};
    sf::SoundBuffer m_buffer;
    sf::Sound m_sound;
#endif
};
extern ToneGenerator g_tone;
}
