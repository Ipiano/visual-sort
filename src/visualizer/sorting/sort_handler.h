#pragma once

#include "sorting/sorts.h"
#include "util/observable.h"
#include "util/semaphore.h"

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

namespace sorting
{
class sort_handle
{
  protected:
    visual_sort* _sort = nullptr;
    std::vector<util::Observable<int>> _list;
    int _size;

    int _cycles;

    bool _running = false;

    int _max;
    bool _ordered       = true;
    int _loops_per_draw = 1;
    int _currLoops      = 0;
    int _visual         = 0;
    bool _randomvis     = false;

    bool _sort_locked;
    util::semaphore _sem1;
    util::semaphore _sem2;

    static void wait_for_handle(void* ths);
    static void unlock_handle(void* ths, bool force);
    static void unlock_sort(sort_handle* ths, bool forcelock = false);
    static void wait_for_sort(sort_handle* ths, bool forcewait = false);
    void _wait(std::chrono::milliseconds time) { std::this_thread::sleep_for(time); };

  public:
    enum class draw_mode
    {
        LINE,
        RECT
    };

    sort_handle();
    ~sort_handle();

    void animate();
    void reset(bool force = false);
    bool complete() { return _sort->finished(); };
    void reset(visual_sort& sort, int items, int max, int loops = 1, bool ordered = true, bool force = false, int visual = 0);
    void draw(int width, int height, int x = 0, int y = 0);
    void add_cycle() { _cycles++; };
};
}
