#include <vector>
#include <iostream>
#include <random>
#include <thread>
#include <mutex>
#include <functional>
#include "Sorts.h"
#include "SortHandler.h"
#include "Constants.h"

using namespace std;

visual_sort::visual_sort(){};
visual_sort::~visual_sort(){};
void visual_sort::setup(Observable<int>* list, int size, semfunction lockfun, semfunction unlockfun)
{
    _quit = false;
    _lock = lockfun;
    _unlock = unlockfun;
    _done = false;
    _list = list;
    _size = size;
    _started = false;
}

void visual_sort::lock()
{
    _lock();
}

void visual_sort::unlock()
{
    _unlock();
}

void visual_sort::stop()
{
    _quit = true;
}

void visual_sort::_exit()
{
    unlock();
}

void visual_sort::run_sort(void* par)
{
    _started = true;
    sort_handle* parent = (sort_handle*)par;
    
    _run_sort(par);
    
    for(int i=1; cond(i<_size); i++)
    {
    
lock();
_list[i-1].hold();
_list[i].touch();
unlock();

    }
    
lock();
_list[_size - 1].hold();
    _done = true;
unlock();

}

bubble_sort::bubble_sort(){};
bubble_sort::~bubble_sort(){};

void bubble_sort::_run_sort(void* par)
{
    sort_handle* parent = (sort_handle*)par;
    bool swapped, done = false;
    Observable<int> tmp;
    int i;

    for(i=_size-2; cond(i>=0 && !done); i--)
    {
lock();
_list[i + 1].unhold();
_list[i + 2].hold();
unlock();

        swapped = false;
        for(int j=0; cond(j<=i); j++)
        {
        
lock();
            if(_list[j] > _list[j+1])
            {
                swapped = true;
                tmp = move(_list[j]);
                _list[j] = move(_list[j+1]);
                _list[j+1] = move(tmp);
            }

unlock();

        }
        if(!swapped) done = true;
parent -> add_cycle();
    }
}

bogo_sort::bogo_sort(){};
bogo_sort::~bogo_sort(){};
void bogo_sort::_run_sort(void* par)
{
    sort_handle* parent = (sort_handle*)par;
    bool done = false;
    int swaps, left, right;
    Observable<int>tmp;
    while(cond(!done))
    {
        swaps = rand()%_size+_size;
        
        done = true;
        for(int i=0; cond(i<_size-1 && done); i++)
        {
        
lock();
            
            if(_list[i] > _list[i+1])
                done = false;
                
unlock();

        }
        for(int i=0; cond(i<swaps); i++)
        {
lock();
            
            left = rand()%_size;
            right = rand()%_size;
            
            tmp = _list[left];
            _list[left] = _list[right];
            _list[right] = tmp;

unlock();
        }
parent -> add_cycle();
    }
}

merge_sort::merge_sort(){};
merge_sort::~merge_sort(){};
void merge_sort::_run_sort(void* par)
{
    _sort_section(0, _size, par);
}

void merge_sort::_sort_section(int start, int size, void* par)
{
    sort_handle* parent = (sort_handle*)par;
    if(size == 1)
    {
        //scout << "Merging 1 item: " << start << endl;
lock();
_list[start].tmpHold();
parent -> add_cycle();
unlock();
        return;   
    }

    int right = start+size;
    int middle = start+size/2;
    
    //cout << "Splitting " << start << ": " << size << " into " << start << ": " << middle-start << " / " << middle << ": " << right-middle << endl;
    _sort_section(start, middle-start, parent);
    _sort_section(middle, right-middle, parent);
    
    Observable<int>* merged = new Observable<int>[size];
    int m = 0;
    int left = start;
    right = middle;
    
    while(cond(left < middle && right < start+size))
    {
lock();
        if(_list[left] > _list[right])
        {
            merged[m++] = _list[right++];
        }
        else
        {
            merged[m++] = _list[left++];
        }
unlock();
    }
    
    while(cond(left < middle))
    {
lock();
        merged[m++] = _list[left++];
unlock();
     }
    
    while(cond(right < start+size))
    {
lock();
        merged[m++] = _list[right++]; 
unlock();
    }
    
    for(int i=start, j=0; cond(j<size); i++, j++)
    {
lock();
        _list[i] = merged[j];
unlock();
    }
    
parent -> add_cycle();
    delete[] merged;   
}

