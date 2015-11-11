#ifndef _SORT_HANDLE_
#define _SORT_HANDLE_

#include "Sorts.h"
#include "semaphore.h"
#include <vector>
#include <thread>

class sort_handle
{
protected:
    visual_sort* _sort = nullptr;
    int* _list = nullptr;
    int _size;
    int _cycles;
    int _max;
    bool _running = false;
    std::vector<int> _held;
    std::vector<int> _touched;
    std::thread* _sortt = nullptr;
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

    void touch(int n){_touched.push_back(n);};
    void hold(int n){_held.push_back(n);};
    void animate();
    void reset(bool force = false);
    bool complete(){return _sort -> finished();};
    void reset(visual_sort* sort, int items, int max, bool force = false);
    void draw(int width, int height, int x=0, int y=0);
    void add_cycle(){_cycles++;};
    void clear_held(){_held.clear();}
};

#endif
