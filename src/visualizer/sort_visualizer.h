#pragma once

#include "glut/window.h"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <thread>
#include <vector>

// Runs a sort algorithm, blocking the sort thread every few operations
// so that the screen can draw the update.
//
// When the thread is blocked, it is unblocked after the next call to
// draw() or cancel()
class SortVisualizer
{
  public:
    class Touch
    {
      public:
        using type = uint8_t;

        constexpr static type NONE     = 0;
        constexpr static type COMPARE  = 1;
        constexpr static type MOVE     = 1 << 1;
        constexpr static type COMPLETE = 1 << 2;
    };

    // Item that's begin visualized during a sort. It encapsulates another
    // item and updates the SortVisualizer that created it any time it is
    // touched
    class Item
    {
      public:
        using underlying_type = int;

        Item(SortVisualizer& visualizer, underlying_type value) : m_value(value), m_visual(visualizer) { }
        ~Item() = default;

        Item(const Item&)            = delete;
        Item& operator=(const Item&) = delete;

        Item(Item&& other) noexcept;
        Item& operator=(Item&& other) noexcept;

        bool operator<(const Item& other) const;

        operator underlying_type() const { return m_value; }

        // Get the set of touches that have occurred since they were
        // last reset, then reset them.
        Touch::type getAndClearTouches() const;

        void complete()
        {
            addTouch(Touch::COMPLETE);
            m_visual.onComplete();
        }

      private:
        void addTouch(Touch::type type) const { m_touches |= type; }
        void addMove() const
        {
            addTouch(Touch::MOVE);
            m_visual.onMove();
        };
        void addCompare() const
        {
            addTouch(Touch::COMPARE);
            m_visual.onCompare();
        };

        underlying_type m_value;

        SortVisualizer& m_visual; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members) - legitimate observer pattern
        mutable Touch::type m_touches {Touch::NONE};
    };

    // Typedef for a sort algorithm. If possible, it should check the
    // cancel flag at appropriate points in the algorithm and exit early
    // if it is cleared.
    using sort_function = std::function<void(std::vector<Item>& items, const std::atomic_bool& cancel_flag)>;
    using draw_function =
        std::function<void(const std::vector<Item>& items, int max_item, glut::Coordinate viewport_origin, glut::Size viewport_size)>;

    SortVisualizer(std::size_t moves_per_draw, bool audio_enabled = false);

    // Cancels any ongoing sort
    ~SortVisualizer();

    SortVisualizer(const SortVisualizer&)            = delete;
    SortVisualizer& operator=(const SortVisualizer&) = delete;

    SortVisualizer(SortVisualizer&&)            = delete;
    SortVisualizer& operator=(SortVisualizer&&) = delete;

    // Sorts a copy of the list of numbers with a specific sort
    // algorithm
    void start(const std::vector<int>& values, sort_function sort_fn, draw_function draw_fn);

    // Stops blocking the sort thread and waits for it to run to
    // completion. If the algorithm can be cancelled early, it will be.
    void cancel();

    // Return true if the thread is blocked waiting to draw
    [[nodiscard]] bool readyToDraw() const;

    // Return true after the sort operation has finished
    [[nodiscard]] bool sortCompleted() const;

    // Draw the current sort on the OGL canvas. Returns immediately
    // if there's nothing to draw
    void draw(glut::Coordinate viewport_origin, glut::Size viewport_size);

    // Get total number of moves so far
    [[nodiscard]] std::size_t totalMoves() const { return m_total_moves; }

    // Get total number of compares so far
    [[nodiscard]] std::size_t totalCompares() const { return m_total_compares; }

  private:
    // Handlers for events to increment counters and potentially block
    // the sort thread
    void onCompare();
    void onMove();
    void onComplete();
    void checkDraw();
    bool needDraw();
    void waitForDraw();

    const std::size_t m_moves_per_draw;

    // The function that should be invoked to draw the items in onDraw
    draw_function m_draw_fn;

    // Number of actions that have happened
    std::atomic<std::size_t> m_compares_since_draw {0};
    std::atomic<std::size_t> m_moves_since_draw {0};

    std::atomic<std::size_t> m_total_compares {0};
    std::atomic<std::size_t> m_total_moves {0};

    // Modified by the sort thread, drawn by the render thread; so be
    // careful about threadsafety
    std::vector<Item> m_items;

    // Set when a new sort starts based on the max item sorted
    int m_max_value = 0;

    // The thread doing sorts and flags to communicate with it
    std::thread m_thread;
    std::atomic_bool m_cancel_flag {false};
    std::atomic_bool m_complete_flag {false};

    // Used to block/unblock the sort thread when it's time to draw
    std::mutex m_ready_draw_mutex;
    std::atomic_bool m_ready_draw {false};
    std::condition_variable m_wait_draw;

    bool m_audio_enabled = false;
};
