#include "args.h"

#include "algorithms/sorting/bubble_sort.hpp"
#include "algorithms/sorting/merge_sort.hpp"
#include "rendering/render_items.h"

#include <boost/program_options.hpp>

#include <algorithm>
#include <iostream>
#include <numeric>
#include <random>

enum class Visualization
{
    // Slope from bottom left to upper right
    SLOPE_BARS,

    // Slope, but it's shifted up and mirrored across
    // the X axis
    SLOPE_MIRRORED,

    // Slope, but only the top of each bar is drawn
    SLOPE_POINTS,

    // Placeholder for max value
    MAX_VALUE
};

namespace po = boost::program_options;

using string_list   = std::vector<std::string>;
using Item          = SortVisualizer::Item;
using sort_function = SortVisualizer::sort_function;

const static std::initializer_list<std::tuple<string_list, sort_function>> ALGORITHMS {
    {string_list {"b", "bubble", "bubblesort"},
     [](std::vector<Item>& items, const std::atomic_bool& cancelled) { algorithms::sorting::bubble_sort(items.begin(), items.end()); }},

    {string_list {"m", "merge", "mergesort"},
     [](std::vector<Item>& items, const std::atomic_bool& cancelled) { algorithms::sorting::merge_sort(items.begin(), items.end()); }},

    {string_list {"s", "std", "stl"},
     [](std::vector<Item>& items, const std::atomic_bool& cancelled) { std::sort(items.begin(), items.end()); }}};

// const std::array<std::tuple<string_list, std::unique_ptr<sorting::visual_sort>>, 6> ALGORITHMS {{
//     {string_list {{"bb", "bogo", "bogosort"}}, std::make_unique<sorting::bogo_sort>()},
//     {string_list {{"b", "bubble", "bubblesort"}}, std::make_unique<sorting::bubble_sort>()},
//     {string_list {{"q", "quick", "quicksort"}}, std::make_unique<sorting::quick_sort>()},
//     {string_list {{"h", "heap", "heapsort"}}, std::make_unique<sorting::heap_sort>()},
//     {string_list {{"m", "merge", "mergesort"}}, std::make_unique<sorting::merge_sort>()},
//     {string_list {{"r", "radix", "radixsort"}}, std::make_unique<sorting::radix_sort>()},
// }};

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

std::istream& operator>>(std::istream& in, Visualization& v)
{
    std::string token;
    in >> token;
    try
    {
        const auto token_value = std::stoul(token);
        if (token_value >= static_cast<unsigned long>(Visualization::MAX_VALUE))
        {
            throw std::exception();
        }

        v = static_cast<Visualization>(token_value);
    }
    catch (const std::exception&)
    {
        in.setstate(std::ios_base::failbit);
    }
    return in;
}

void validate(boost::any& v, const std::vector<std::string>& values, sort_function* dispatch_unused, int unused)
{
    po::validators::check_first_occurrence(v);
    const std::string& s = po::validators::get_single_string(values);

    for (const auto& algo : ALGORITHMS)
    {
        if (std::find(std::get<0>(algo).begin(), std::get<0>(algo).end(), s) != std::get<0>(algo).end())
        {
            v = std::get<1>(algo);
            return;
        }
    }
    throw po::invalid_option_value("Unknown algorithm option: " + s);
}

SortVisualizer::draw_function makeDrawFunction(Visualization which)
{
    switch (which)
    {
    case Visualization::SLOPE_MIRRORED:
        return [](const std::vector<Item>& items, int max_item, glut::Coordinate viewport_origin, glut::Size viewport_size)
        {
            rendering::renderItems(items.begin(), items.end(),
                                   rendering::LeftToRightMirrorStrategy(viewport_origin, viewport_size, max_item, items.size()));
        };

    case Visualization::SLOPE_POINTS:
        return [](const std::vector<Item>& items, int max_item, glut::Coordinate viewport_origin, glut::Size viewport_size)
        {
            rendering::renderItems(items.begin(), items.end(),
                                   rendering::LeftToRightDotStrategy(viewport_origin, viewport_size, max_item, items.size()));
        };

    case Visualization::SLOPE_BARS:
    default:
        return [](const std::vector<Item>& items, int max_item, glut::Coordinate viewport_origin, glut::Size viewport_size)
        {
            rendering::renderItems(items.begin(), items.end(),
                                   rendering::LeftToRightSlopeStrategy(viewport_origin, viewport_size, max_item, items.size()));
        };
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

std::vector<int> nonUniqueDataset(std::size_t count, std::mt19937& reng)
{
    std::uniform_int_distribution<int> dist(0, static_cast<int>(count));

    std::vector<int> items(count, 0);
    std::transform(items.begin(), items.end(), items.begin(), [&](int) { return dist(reng); });

    return items;
}

std::function<std::vector<int>()> makeDatasetFactory(std::size_t number_of_items, bool unique)
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

    if (unique)
    {
        result.m_factory = &uniqueDataset;
    }
    else
    {
        result.m_factory = &nonUniqueDataset;
    }
    return result;
}

ProgramArgs parse_args(int argc, char** argv)
{
    ProgramArgs result;

    po::options_description options("Options");

    std::size_t set_size;
    bool unique_values;
    Visualization visual_choice;

    // clang-format off
    options.add_options()
        ("help", "Show this help")

        ("algo,a,algorithm",
            po::value<sort_function>(&result.sort_function)
                ->default_value(
                    std::get<1>(*ALGORITHMS.begin()),
                    std::get<0>(*ALGORITHMS.begin())[0]),
            "Sort algorithm")
        ("visual,v",
            po::value<Visualization>(&visual_choice)
                ->default_value(
                    Visualization::SLOPE_BARS,
                    std::to_string(static_cast<unsigned long>(Visualization::SLOPE_BARS))),
            "Visualization (0-2)")

        (",n",
            po::value<size_t>(&set_size)->default_value(100),
            "Number of items to sort")

        ("steps,s",
            po::value<size_t>(&result.steps_between_draws)->default_value(10),
            "Number of steps to execute between screen updates")

        ("unique,u",
            po::value<bool>(&unique_values)->default_value(true),
            "Prohibit duplicate values in the dataset")
    ;
    // clang-format on

    po::variables_map vm;
    try
    {
        po::store(po::parse_command_line(argc, argv, options), vm);
        po::notify(vm);

        if (vm.count("help") > 0)
        {
            showHelp(*argv, options);
        }
    }
    catch (const po::error& e)
    {
        std::cout << e.what() << "\n\n";
        showHelp(*argv, options);
    }

    result.data_set_factory = makeDatasetFactory(set_size, unique_values);
    result.draw_function    = makeDrawFunction(visual_choice);

    return result;
}
