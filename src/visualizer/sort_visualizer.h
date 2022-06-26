#pragma once

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

        constexpr static type NONE    = 0;
        constexpr static type COMPARE = 1;
        constexpr static type MOVE    = 1 << 1;
    };

    // Item that's begin visualized during a sort. It encapsulates another
    // item and updates the SortVisualizer that created it any time it is
    // touched
    class Item
    {
      public:
        Item(SortVisualizer& visualizer, int value) : m_value(value), m_visual(visualizer), m_touches(Touch::NONE) { }

        Item(const Item&) = delete;
        Item& operator=(const Item&) = delete;

        Item(Item&& other);
        Item& operator=(Item&& other);

        bool operator<(const Item& other) const;

        operator int() const { return m_value; }

        // Get the set of touches that have occurred since they were
        // last reset, then reset them.
        Touch::type getAndClearTouches() const;

      private:
        void addTouch(Touch::type type) const { m_touches |= type; }

        int m_value;

        SortVisualizer& m_visual;
        mutable Touch::type m_touches;
    };

    // Typedef for a sort algorithm. If possible, it should check the
    // cancel flag at appropriate points in the algorithm and exit early
    // if it is cleared.
    using sort_function = std::function<void(std::vector<Item>& items, const std::atomic_bool& cancel_flag)>;
    using draw_function = std::function<void(const std::vector<Item>& items)>;

    SortVisualizer(std::size_t moves_per_draw);

    // Cancels any ongoing sort
    ~SortVisualizer();

    // Sorts a copy of the list of numbers with a specific sort
    // algorithm
    void start(const std::vector<int>& values, sort_function sort_fn, draw_function draw_fn);

    // Stops blocking the sort thread and waits for it to run to
    // completion. If the algorithm can be cancelled early, it will be.
    void cancel();

    // Return true if the thread is blocked waiting to draw
    bool readyToDraw();

    // Draw the current sort on the OGL canvas. Returns immediately
    // if there's nothing to draw
    void draw();

    // Get total number of moves so far
    std::size_t totalMoves() { return m_total_moves; }

    // Get total number of compares so far
    std::size_t totalCompares() { return m_total_compares; }

  private:
    // Handlers for events to increment counters and potentially block
    // the sort thread
    void onCompare();
    void onMove();
    void checkDraw();
    bool needDraw();
    void waitForDraw();

    const std::size_t m_moves_per_draw;

    // The function that should be invoked to draw the items in onDraw
    draw_function m_draw_fn;

    // Number of actions that have happened
    std::atomic<std::size_t> m_compares_since_draw;
    std::atomic<std::size_t> m_moves_since_draw;

    std::atomic<std::size_t> m_total_compares;
    std::atomic<std::size_t> m_total_moves;

    // Modified by the sort thread, drawn by the render thread; so be
    // careful about threadsafety
    std::vector<Item> m_items;

    // The thread doing sorts and flags to communicate with it
    std::thread m_thread;
    std::atomic_bool m_cancel_flag;
    std::atomic_bool m_complete_flag;

    // Used to block/unblock the sort thread when it's time to draw
    std::mutex m_ready_draw_mutex;
    std::atomic_bool m_ready_draw;
    std::condition_variable m_wait_draw;
};
