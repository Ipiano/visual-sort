#ifndef _VIS_SORT_
#define _VIS_SORT_

#include <vector>
#include <mutex>
#include <functional>
#include "Constants.h"
#include "Observable.h"

class visual_sort
{
private:
    bool _done = false;

protected:
    Observable<int>* _list;
    int _size;
    bool _started = false;
    bool _quit = false;
    semfunction _lock;
    semfunction _unlock;
    
    virtual void _run_sort(void* par) = 0;
    void wait();
    void unlock();
    virtual void kill();

public:
    visual_sort();
    virtual ~visual_sort();
    virtual void setup(Observable<int>* list, int size, semfunction lockfun, semfunction unlockfun);
    bool finished(){return _done;};
    void run_sort(void* par, semfunction force);
    bool started(){return _started;};
    void stop();
    void draw(int& changes, int& compares, int width, int height, int _max, int x, int y, int method = 0);
    int visual_count(){return 3;}
};

class bubble_sort : public visual_sort
{
private:
    void bsort(void* par);
protected:
    virtual void _run_sort(void* par);
public:
    bubble_sort();
    virtual ~bubble_sort();
    
};

class bogo_sort : public visual_sort
{
private:
    void bsort(void* parent);
protected:
    void _run_sort(void* parent);
public:
    bogo_sort();
    virtual ~bogo_sort();
};

class merge_sort : public visual_sort
{
private:
    Observable<int>* _merged = nullptr;
    void msort(int start, int size, void* par);

protected:
    void _run_sort(void* parent);
    virtual void kill();
public:
    merge_sort();
    virtual ~merge_sort();
};

class radix_sort : public visual_sort
{
private:
    Observable<int>* _copy = nullptr;
    int getBucket(Observable<int> num, int rad);
    void rsort(void* par);
protected:
    void _run_sort(void* par);
    virtual void kill();
public:
    radix_sort();
    virtual ~radix_sort();
};

class quick_sort : public visual_sort
{
private:
    void qsort(void* par, int left, int right);
    void getPivot(int& mid, int& pivleft, int& pivright, int left, int right);

protected:
    void _run_sort(void* par);

public:
    quick_sort();
    ~quick_sort();
};

#endif
