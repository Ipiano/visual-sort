#pragma once

#include "constants.h"
#include "sort_visualizer.h"

#include <cmath>

namespace rendering
{
enum class ColorStrategyChoices : std::uint8_t
{
    // Placeholder for the first valid value
    min_value = 0,

    touches = min_value,
    heap    = 1,

    // Place holder for one-past-the end of valid values
    max_value = 2
};

// Provides 'type' typedef
template <ColorStrategyChoices strategy> struct color_strategy_traits;

using rgb_t = std::array<float, 3>;
namespace color_strategy
{
rgb_t get_color_from_touches(SortVisualizer::Touch::type touches)
{
    const bool moved    = (touches & SortVisualizer::Touch::MOVE) != 0;
    const bool compared = (touches & SortVisualizer::Touch::COMPARE) != 0;
    const bool complete = (touches & SortVisualizer::Touch::COMPLETE) != 0;

    if (complete)
    {
        return constants::colors::COMPLETE;
    }

    if (moved)
    {
        return constants::colors::MOVE;
    }
    if (compared)
    {
        return constants::colors::COMPARE;
    }
    return constants::colors::FREE;
}

//! Translate HSV hue value (0-360) to RGB
//! See https://www.instructables.com/How-to-Make-Proper-Rainbow-and-Random-Colors-With-/
static rgb_t rainbow_hue(double hue)
{
    hue = std::fmod(hue, 360);

    constexpr static double PI = 3.14159265359;

    constexpr static double CHANNEL_WIDTH = 240;
    constexpr static double RED_MIN       = 240;
    constexpr static double RED_MAX       = 120;
    constexpr static double GREEN_MIN     = 0;
    constexpr static double BLUE_MIN      = 120;

    rgb_t result {{0, 0, 0}};

    if (hue >= RED_MIN && hue <= 360)
    {
        result[0] = static_cast<float>(std::sin(PI * (hue - RED_MIN) / CHANNEL_WIDTH));
    }

    if (hue >= 0 && hue <= RED_MAX)
    {
        result[0] = static_cast<float>(std::sin(PI * (hue + CHANNEL_WIDTH / 2) / CHANNEL_WIDTH));
    }

    if (hue >= GREEN_MIN && hue <= GREEN_MIN + CHANNEL_WIDTH)
    {
        result[1] = static_cast<float>(std::sin(PI * (hue - GREEN_MIN) / CHANNEL_WIDTH));
    }

    if (hue >= BLUE_MIN && hue <= BLUE_MIN + CHANNEL_WIDTH)
    {
        result[2] = static_cast<float>(std::sin(PI * (hue - BLUE_MIN) / CHANNEL_WIDTH));
    }

    return result;
}

}

template <class CrtpType> struct ColorStrategy // NOLINT(bugprone-crtp-constructor-accessibility) - legitimate CRTP pattern
{
  public:
    rgb_t operator()(std::size_t item_index, int item_value, SortVisualizer::Touch::type touches) const
    {
        return static_cast<const CrtpType&>(*this)(item_index, item_value, touches);
    }
};

struct DefaultColorStrategy : public ColorStrategy<DefaultColorStrategy>
{
  public:
    explicit DefaultColorStrategy(std::size_t total_items) { }
    rgb_t operator()(std::size_t /*item_index*/, int /*item_value*/, SortVisualizer::Touch::type touches) const
    {
        return color_strategy::get_color_from_touches(touches);
    }
};

struct HeapColorStrategy : public ColorStrategy<HeapColorStrategy>
{
  public:
    explicit HeapColorStrategy(std::size_t total_items) : _heap_depth(static_cast<int>(std::log2(static_cast<double>(total_items)) + 1.0))
    {
    }

    rgb_t operator()(std::size_t item_index, int /*item_value*/, SortVisualizer::Touch::type touches) const
    {
        if (touches != 0U)
        {
            return color_strategy::get_color_from_touches(touches);
        }

        const int item_depth = static_cast<int>(std::log2(static_cast<double>(item_index)));

        return color_strategy::rainbow_hue(static_cast<double>(item_depth) / static_cast<double>(_heap_depth) * 360.0);
    }

  private:
    int _heap_depth;
};

template <> struct color_strategy_traits<ColorStrategyChoices::touches>
{
    using type = DefaultColorStrategy;
};

template <> struct color_strategy_traits<ColorStrategyChoices::heap>
{
    using type = HeapColorStrategy;
};

}
