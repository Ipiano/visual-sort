#include "args.h"

#include "algorithms/sorting/bogo_sort.hpp"
#include "algorithms/sorting/bubble_sort.hpp"
#include "algorithms/sorting/bucket_sort.hpp"
#include "algorithms/sorting/heap_sort.hpp"
#include "algorithms/sorting/merge_sort.hpp"
#include "algorithms/sorting/quick_sort.hpp"
#include "rendering/color_strategy.hpp"
#include "rendering/draw_strategy.hpp"
#include "rendering/render_items.hpp"
#include "rendering/tone_strategy.hpp"
#include "sort_visualizer.h"

#include <boost/program_options.hpp>

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <iostream>
#include <numeric>
#include <random>

namespace po = boost::program_options;

using string_list     = std::vector<std::string>;
using Item            = SortVisualizer::Item;
using sort_function   = SortVisualizer::sort_function;
using render_function = SortVisualizer::draw_function;

using algorithm_choice = std::tuple<string_list, sort_function, rendering::ColorStrategyChoices>;

struct RenderOptions
{
    rendering::DrawStrategyChoices draw_strategy   = rendering::DrawStrategyChoices::min_value;
    rendering::ColorStrategyChoices color_strategy = rendering::ColorStrategyChoices::min_value;
    rendering::ToneStrategyChoices tone_strategy   = rendering::ToneStrategyChoices::min_value;
};

enum class Dataset
{
    unique,
    uniform,
    normal,
    random
};

SortVisualizer::draw_function makeDrawFunction(RenderOptions options);

const static std::initializer_list<algorithm_choice> ALGORITHMS {
    {string_list {"b", "bubble", "bubblesort"},
     [](std::vector<Item>& items, const std::atomic_bool& cancelled) { algorithms::sorting::bubble_sort(items.begin(), items.end()); },
     rendering::ColorStrategyChoices::touches},

    {string_list {"m", "merge", "mergesort"},
     [](std::vector<Item>& items, const std::atomic_bool& cancelled) { algorithms::sorting::merge_sort(items.begin(), items.end()); },
     rendering::ColorStrategyChoices::touches},

    {string_list {"bb", "bogo", "bogosort"},
     [](std::vector<Item>& items, const std::atomic_bool& cancelled)
     { algorithms::sorting::bogo_sort(items.begin(), items.end(), cancelled); },
     rendering::ColorStrategyChoices::touches},

    {string_list {"std", "stl", "std::sort"},
     [](std::vector<Item>& items, const std::atomic_bool& cancelled) { std::sort(items.begin(), items.end()); },
     rendering::ColorStrategyChoices::touches},

    {string_list {"stable", "std::stable_sort"},
     [](std::vector<Item>& items, const std::atomic_bool& cancelled) { std::stable_sort(items.begin(), items.end()); },
     rendering::ColorStrategyChoices::touches},

    {string_list {"r", "radix"},
     [](std::vector<Item>& items, const std::atomic_bool& cancelled)
     { algorithms::sorting::bucket_sort(items.begin(), items.end(), algorithms::sorting::bucket::traits<Item::underlying_type>()); },
     rendering::ColorStrategyChoices::touches},

    {string_list {"q", "quick", "quicksort"},
     [](std::vector<Item>& items, const std::atomic_bool& cancelled)
     { algorithms::sorting::quick_sort(items.begin(), items.end(), algorithms::sorting::bucket::traits<Item::underlying_type>()); },
     rendering::ColorStrategyChoices::touches},

    {string_list {"h", "heap", "heapsort"},
     [](std::vector<Item>& items, const std::atomic_bool& cancelled) { algorithms::sorting::heap_sort(items.begin(), items.end()); },
     rendering::ColorStrategyChoices::heap},

    // placeholder with special name that is used to pick the random algorithm
    {string_list {"randomize"}, [](std::vector<Item>& items, const std::atomic_bool& cancelled) {},
     rendering::ColorStrategyChoices::touches},
};

bool is_random_algo(const algorithm_choice& choice)
{
    return !std::get<0>(choice).empty() && std::get<0>(choice)[0] == "randomize";
}

bool is_bogosort_algo(const algorithm_choice& choice)
{
    return !std::get<0>(choice).empty() && std::get<0>(choice)[0] == "bb";
}

void showHelp(const char* arg0, const po::options_description& options)
{
    std::cout << options << "\n\nSort Algorithms:\n";
    for (const auto& algo : ALGORITHMS)
    {
        std::string names;
        for (const auto& name : std::get<0>(algo))
        {
            names += name + "/";
        }
        names.pop_back();
        std::cout << "\t" << names << "\n";
    }

    std::cout << "\nExample:\n\t" << arg0 << " --algo=bubble --visual 2 --unique=no\n" << std::endl;
    exit(0);
}

// ADL handling
namespace rendering
{
std::istream& operator>>(std::istream& in, rendering::DrawStrategyChoices& v)
{
    std::string token;
    in >> token;
    try
    {
        const auto token_value = std::stoul(token);
        if (token_value >= static_cast<unsigned long>(rendering::DrawStrategyChoices::max_value))
        {
            throw std::exception();
        }

        v = static_cast<rendering::DrawStrategyChoices>(token_value);
    }
    catch (const std::exception&)
    {
        in.setstate(std::ios_base::failbit);
    }
    return in;
}
}

void validate(boost::any& v, const std::vector<std::string>& values, algorithm_choice* dispatch_unused, int unused)
{
    po::validators::check_first_occurrence(v);
    const std::string& s = po::validators::get_single_string(values);

    for (const auto& algo : ALGORITHMS)
    {
        if (std::find(std::get<0>(algo).begin(), std::get<0>(algo).end(), s) != std::get<0>(algo).end())
        {
            v = algo;
            return;
        }
    }
    throw po::invalid_option_value("Unknown algorithm option: " + s);
}

void validate(boost::any& v, const std::vector<std::string>& values, Dataset* dispatch_unused, int unused)
{
    po::validators::check_first_occurrence(v);
    const std::string& s = po::validators::get_single_string(values);

    if (s == "unique")
    {
        v = Dataset::unique;
    }
    else if (s == "uniform")
    {
        v = Dataset::uniform;
    }
    else if (s == "normal")
    {
        v = Dataset::normal;
    }
    else if (s == "random")
    {
        v = Dataset::random;
    }
    else
    {
        throw po::invalid_option_value("Unknown dataset option: " + s);
    }
}

std::vector<int> uniqueDataset(std::size_t count, std::mt19937& reng)
{
    std::vector<int> items(count, 0);
    for (int i = 0; i < count; ++i)
    {
        items[i] = i;
    }

    std::shuffle(items.begin(), items.end(), reng);
    return items;
}

std::vector<int> uniformDataset(std::size_t count, std::mt19937& reng)
{
    std::uniform_int_distribution<int> dist(0, static_cast<int>(count));

    std::vector<int> items(count, 0);
    std::transform(items.begin(), items.end(), items.begin(), [&](int) { return dist(reng); });

    return items;
}

std::vector<int> normalDataset(std::size_t count, std::mt19937& reng)
{
    std::normal_distribution<double> dist(1000, 200);

    std::vector<int> items(count, 0);
    std::transform(items.begin(), items.end(), items.begin(), [&](int) { return static_cast<int>(dist(reng)); });

    return items;
}

std::vector<int> randomDataset(std::size_t count, std::mt19937& reng)
{
    std::uniform_int_distribution<int> choice(0, 2);
    switch (Dataset(choice(reng)))
    {
    case Dataset::unique:
        return uniqueDataset(count, reng);
    case Dataset::uniform:
        return uniformDataset(count, reng);
    case Dataset::normal:
        return normalDataset(count, reng);
    default:
        return {};
    }
}

std::function<std::vector<int>()> makeDatasetFactory(std::size_t number_of_items, Dataset data_choice)
{
    struct factory
    {
        std::function<std::vector<int>(std::size_t, std::mt19937&)> m_factory;
        std::mt19937 m_reng =
            std::mt19937 {static_cast<std::mt19937::result_type>(std::chrono::system_clock::now().time_since_epoch().count())};
        std::size_t m_count = 0;

        std::vector<int> operator()() { return m_factory(m_count, m_reng); }
    };

    factory result;
    result.m_count = number_of_items;

    switch (data_choice)
    {
    case Dataset::unique:
        result.m_factory = &uniqueDataset;
        break;
    case Dataset::uniform:
        result.m_factory = &uniformDataset;
        break;
    case Dataset::normal:
        result.m_factory = &normalDataset;
        break;
    case Dataset::random:
        result.m_factory = &randomDataset;
    }
    return result;
}

ProgramArgs parse_args(int argc, char** argv)
{
    ProgramArgs result;

    po::options_description options("Options");

    std::size_t set_size;
    rendering::DrawStrategyChoices visual_choice;

    algorithm_choice algo_choice;
    Dataset data_choice;

    // clang-format off
    options.add_options()
        ("help", "Show this help")

        ("algo,a,algorithm",
            po::value<algorithm_choice>(&algo_choice)
                ->default_value(
                    *ALGORITHMS.begin(),
                    std::get<0>(*ALGORITHMS.begin())[0]),
            "Sort algorithm")

        ("visual,v",
            po::value<rendering::DrawStrategyChoices>(&visual_choice)
                ->default_value(
                    rendering::DrawStrategyChoices::slope_bars,
                    std::to_string(static_cast<unsigned long>(rendering::DrawStrategyChoices::min_value))),
            "Visualization (0-2)")

#ifdef WITH_SFML
        ("audio,a", "Play audio related to visual")
#endif

        ("loop,l", "Repeatedly shuffle and sort")

        ("data,d", po::value<Dataset>(&data_choice)->default_value(Dataset::unique, "unique"), "Choose the distribution of data to sort [unique, uniform, normal, random]")

        (",n",
            po::value<size_t>(&set_size)->default_value(100),
            "Number of items to sort")

        ("steps,s",
            po::value<size_t>(&result.steps_between_draws)->default_value(10),
            "Number of steps to execute between screen updates")
    ;
    // clang-format on

    rendering::ToneStrategyChoices audio_choice = rendering::ToneStrategyChoices::silent;

    po::variables_map vm;
    try
    {
        po::store(po::parse_command_line(argc, argv, options), vm);
        po::notify(vm);

        if (vm.count("help") > 0)
        {
            showHelp(*argv, options);
        }

        if (vm.count("loop") > 0)
        {
            result.loop = true;
        }

        if (vm.count("audio") > 0)
        {
            audio_choice = rendering::ToneStrategyChoices::proportional;
        }
    }
    catch (const po::error& e)
    {
        std::cout << e.what() << "\n\n";
        showHelp(*argv, options);
    }

    result.data_set_factory = makeDatasetFactory(set_size, data_choice);
    result.audio_enabled    = (audio_choice != rendering::ToneStrategyChoices::silent);

    if (is_random_algo(algo_choice))
    {
        result.get_sort_and_draw_function = [visual_choice, audio_choice]
        {
            static thread_local std::mt19937_64 reng(std::chrono::system_clock::now().time_since_epoch().count());
            std::uniform_int_distribution<std::size_t> algo_dist(0, ALGORITHMS.size() - 1);

            std::size_t choice;
            do
            {
                choice = algo_dist(reng);

                // "random" isn't a real algo
                // bogosort takes too long
            } while (is_random_algo(*std::next(ALGORITHMS.begin(), choice)) || is_bogosort_algo(*std::next(ALGORITHMS.begin(), choice)));

            auto algo_choice = *std::next(ALGORITHMS.begin(), choice);

            return std::make_pair(std::get<1>(algo_choice),
                                  makeDrawFunction(RenderOptions {visual_choice, std::get<2>(algo_choice), audio_choice}));
        };
    }
    else
    {
        result.get_sort_and_draw_function = [visual_choice, audio_choice, algo_choice]
        {
            return std::make_pair(std::get<1>(algo_choice),
                                  makeDrawFunction(RenderOptions {visual_choice, std::get<2>(algo_choice), audio_choice}));
        };
    }

    return result;
}

template <rendering::DrawStrategyChoices min_value, rendering::DrawStrategyChoices max_value> struct select_draw_function;

template <rendering::DrawStrategyChoices draw_strategy, rendering::ColorStrategyChoices min_value,
          rendering::ColorStrategyChoices max_value>
struct select_color_strategy;

template <rendering::DrawStrategyChoices draw_strategy, rendering::ColorStrategyChoices color_strategy,
          rendering::ToneStrategyChoices min_value, rendering::ToneStrategyChoices max_value>
struct select_tone_strategy;

template <rendering::DrawStrategyChoices drawing_v, rendering::ColorStrategyChoices coloring_v, rendering::ToneStrategyChoices tone_v>
struct render_function_obj
{
    void operator()(const std::vector<Item>& items, int max_item, glut::Coordinate viewport_origin, glut::Size viewport_size)
    {
        rendering::renderItems(
            items.begin(), items.end(),
            typename rendering::draw_strategy_traits<drawing_v>::type(viewport_origin, viewport_size, max_item, items.size()),
            typename rendering::color_strategy_traits<coloring_v>::type(items.size()),
            typename rendering::tone_strategy_traits<tone_v>::type(max_item));
    }
};

template <rendering::DrawStrategyChoices draw_strategy, rendering::ColorStrategyChoices color_strategy>
struct select_tone_strategy<draw_strategy, color_strategy, rendering::ToneStrategyChoices::max_value,
                            rendering::ToneStrategyChoices::max_value>
{
    auto operator()(RenderOptions options)
    {
        return render_function(render_function_obj<draw_strategy, color_strategy, rendering::ToneStrategyChoices::min_value>());
    }
};

template <rendering::DrawStrategyChoices draw_strategy, rendering::ColorStrategyChoices color_strategy,
          rendering::ToneStrategyChoices min_value, rendering::ToneStrategyChoices max_value>
struct select_tone_strategy
{
    auto operator()(RenderOptions options)
    {
        if (min_value == options.tone_strategy)
        {
            return render_function(render_function_obj<draw_strategy, color_strategy, min_value>());
        }

        return select_tone_strategy<draw_strategy, color_strategy, rendering::ToneStrategyChoices(static_cast<int>(min_value) + 1),
                                    max_value>()(options);
    }
};

template <rendering::DrawStrategyChoices draw_strategy, rendering::ColorStrategyChoices max_value>
struct select_color_strategy<draw_strategy, max_value, max_value>
{
    auto operator()(RenderOptions options)
    {
        return select_tone_strategy<draw_strategy, rendering::ColorStrategyChoices::min_value, rendering::ToneStrategyChoices::min_value,
                                    rendering::ToneStrategyChoices::max_value>()(options);
    }
};

template <rendering::DrawStrategyChoices draw_strategy, rendering::ColorStrategyChoices min_value,
          rendering::ColorStrategyChoices max_value>
struct select_color_strategy
{
    auto operator()(RenderOptions options)
    {
        if (options.color_strategy == min_value)
        {
            return select_tone_strategy<draw_strategy, min_value, rendering::ToneStrategyChoices::min_value,
                                        rendering::ToneStrategyChoices::max_value>()(options);
        }

        return select_color_strategy<draw_strategy, rendering::ColorStrategyChoices(static_cast<int>(min_value) + 1), max_value>()(options);
    }
};

template <rendering::DrawStrategyChoices max_value> struct select_draw_function<max_value, max_value>
{
    auto operator()(RenderOptions options)
    {
        return select_color_strategy<rendering::DrawStrategyChoices::min_value, rendering::ColorStrategyChoices::min_value,
                                     rendering::ColorStrategyChoices::max_value>()(options);
    }
};

template <rendering::DrawStrategyChoices min_value, rendering::DrawStrategyChoices max_value> struct select_draw_function
{
    auto operator()(RenderOptions options)
    {
        if (options.draw_strategy == min_value)
        {
            return select_color_strategy<min_value, rendering::ColorStrategyChoices::min_value,
                                         rendering::ColorStrategyChoices::max_value>()(options);
        }

        return select_draw_function<rendering::DrawStrategyChoices(static_cast<int>(min_value) + 1), max_value>()(options);
    }
};

SortVisualizer::draw_function makeDrawFunction(RenderOptions options)
{
    return select_draw_function<rendering::DrawStrategyChoices::min_value, rendering::DrawStrategyChoices::max_value>()(options);
}
