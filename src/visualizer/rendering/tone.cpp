#include "tone.h"

#include <cmath>

namespace rendering::audio
{
#ifdef WITH_SFML

ToneGenerator::ToneGenerator()
{
    const auto sample_per_wave  = SAMPLE_RATE / FREQ;
    const auto delta_per_sample = 2 * PI / sample_per_wave;

    {
        double t = 0;
        for (auto& s : m_samples)
        {
            s = 1000 * std::sin(t);
            t += delta_per_sample;
        }
    }

    m_buffer.loadFromSamples(m_samples.data(), m_samples.size(), 1, m_samples.size());

    m_sound.setLoop(true);
    m_sound.setBuffer(m_buffer);
}

void ToneGenerator::start()
{
    m_sound.play();
}

void ToneGenerator::set_pitch(float pitch)
{
    m_sound.setPitch(pitch);
}

void ToneGenerator::stop()
{
    m_sound.stop();
}

#else
ToneGenerator::ToneGenerator() = default;

void ToneGenerator::start()
{
}

void ToneGenerator::set_pitch(float)
{
}

void ToneGenerator::stop()
{
}
#endif
ToneGenerator g_tone;
}
