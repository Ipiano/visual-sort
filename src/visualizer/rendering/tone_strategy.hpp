#pragma once

#include "rendering/tone.h"
#include "sort_visualizer.h"

namespace rendering
{
enum class ToneStrategyChoices
{
    min_value,

    silent = min_value,
    proportional,

    max_value
};

class SilentToneStrategy
{
  public:
    explicit SilentToneStrategy(int max_item_value) {};
    void operator()(int item_value, SortVisualizer::Touch::type touches) const {};
};

class ProportionalToneStrategy
{
  public:
    explicit ProportionalToneStrategy(int max_item_value) : _max_item_value(max_item_value) { }

    void operator()(int item_value, SortVisualizer::Touch::type touches) const
    {
        const bool moved    = (touches & SortVisualizer::Touch::MOVE) != 0;
        const bool compared = (touches & SortVisualizer::Touch::COMPARE) != 0;

        if (moved)
        {
            audio::g_tone.set_pitch(static_cast<float>(item_value) / _max_item_value * 10);
        }
    };

  private:
    int _max_item_value;
};

}
