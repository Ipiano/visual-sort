#ifndef _VIS_SORT_
#define _VIS_SORT_

#include <vector>
#include <mutex>
#include <functional>
#include "Constants.h"
#include "Observable.h"

class visual_sort
{
protected:
    Observable<int>* _list;
    int _size;
    bool _done = false;
    bool _started = false;
    bool _quit = false;
    semfunction _lock;
    semfunction _unlock;
    
    virtual void _run_sort(void* par) = 0;
    void _exit();
    void lock();
    void unlock();

public:
    visual_sort();
    virtual ~visual_sort();
    virtual void setup(Observable<int>* list, int size, semfunction lockfun, semfunction unlockfun);
    bool finished(){return _done;};
    void run_sort(void* par);
    bool started(){return _started;};
    void stop();
};

class bubble_sort : public visual_sort
{
protected:
    virtual void _run_sort(void* par);
public:
    bubble_sort();
    virtual ~bubble_sort();
    
};

class bogo_sort : public visual_sort
{
protected:

    void _run_sort(void* parent);
public:
    bogo_sort();
    virtual ~bogo_sort();
};

class merge_sort : public visual_sort
{
protected:

    void _run_sort(void* parent);
    void _sort_section(int start, int size, void* par);
public:
    merge_sort();
    virtual ~merge_sort();
};


#endif
