#pragma once

#include "constants.h"
#include "sort_visualizer.h"

namespace rendering
{

using rgb_t = std::array<float, 3>;

template <class CrtpType> struct ColorStrategy
{
  public:
    rgb_t operator()(const SortVisualizer::Item& item, SortVisualizer::Touch::type touches) const
    {
        return static_cast<const CrtpType&>(*this)(item, touches);
    }
};

struct DefaultColorStrategy : public ColorStrategy<DefaultColorStrategy>
{
  public:
    using ColorStrategy::ColorStrategy;

    rgb_t operator()(const SortVisualizer::Item& item, SortVisualizer::Touch::type touches) const
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
};

}
