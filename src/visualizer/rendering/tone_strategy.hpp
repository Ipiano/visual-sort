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

// Provides 'type' typedef
template <ToneStrategyChoices strategy> struct tone_strategy_traits;

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
        const bool complete = (touches & SortVisualizer::Touch::COMPLETE) != 0;

        if (moved || complete)
        {
            audio::g_tone.set_pitch(static_cast<float>(item_value) / _max_item_value * 10);
        }
    };

  private:
    int _max_item_value;
};

template <> struct tone_strategy_traits<ToneStrategyChoices::silent>
{
    using type = SilentToneStrategy;
};

template <> struct tone_strategy_traits<ToneStrategyChoices::proportional>
{
    using type = ProportionalToneStrategy;
};

}
