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
    int _max;
    bool _running = false;
    semaphore _sem1;
    semaphore _sem2;
    
    static void sort_lock(void* ths);
    static void sort_unlock(void* ths);
    static void handle_lock(sort_handle* ths);
    static void handle_unlock(sort_handle* ths);
    
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
    void reset(visual_sort* sort, int items, int max, bool force = false);
    void draw(int width, int height, int x=0, int y=0);
    void add_cycle(){_cycles++;};
};

#endif
