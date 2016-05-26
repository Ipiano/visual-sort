#ifndef _SORT_HANDLE_
#define _SORT_HANDLE_

#include "Sorts.h"
#include "semaphore.h"
#include "Observable.h"
#include <vector>
#include <thread>
#include <iostream>

class sort_handle
{
protected:
    visual_sort* _sort = nullptr;
    Observable<int>* _list = nullptr;
    int _size;

    int _cycles;

    bool _running = false;

    int _max;
    bool _ordered = true;
    int _loops_per_draw = 1;
    int _currLoops = 0;

    bool _sort_locked;
    semaphore _sem1;
    semaphore _sem2;
    
    static void wait_for_handle(void* ths);
    static void unlock_handle(void* ths, bool force);
    static void unlock_sort(sort_handle* ths, bool forcelock = false);
    static void wait_for_sort(sort_handle* ths, bool forcewait = false);
    void _wait(unsigned long long clocks){ for (int i = 0; i < clocks; i++)int n=i; };
public:
    enum class draw_mode
    {
        LINE, RECT
    };

    sort_handle();
    ~sort_handle();

    void animate();
    void reset(bool force = false);
    bool complete(){return _sort -> finished();};
    void reset(visual_sort* sort, int items, int max, int loops = 1, bool ordered = true, bool force = false);
    void draw(int width, int height, int x=0, int y=0);
    void add_cycle(){_cycles++;};
};

#endif
