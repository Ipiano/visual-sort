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
void visual_sort::setup(int* list, int size, semfunction lockfun, semfunction unlockfun)
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
    
    for(int i=0; cond(i<_size); i++)
    {
    
lock();
parent -> hold(i);
parent -> touch(i+1);
unlock();

    }
    
lock();
    _done = true;
unlock();

}

bubble_sort::bubble_sort(){};
bubble_sort::~bubble_sort(){};

void bubble_sort::_run_sort(void* par)
{
    sort_handle* parent = (sort_handle*)par;
    bool swapped, done = false;
    int tmp;
    int i;

    for(i=_size-2; cond(i>=0 && !done); i--)
    {
        swapped = false;
        for(int j=0; cond(j<=i); j++)
        {
        
lock();
parent -> clear_held();
parent -> hold(i+2);
parent -> touch(j);

            if(_list[j] > _list[j+1])
            {
                swapped = true;
                tmp = move(_list[j]);
                _list[j] = move(_list[j+1]);
                _list[j+1] = move(tmp);

parent -> touch(j+1);

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
    int swaps, left, right, tmp;
    while(cond(!done))
    {
        swaps = rand()%_size+_size;
        
        done = true;
        for(int i=0; cond(i<_size-1 && done); i++)
        {
        
lock();
            
            if(_list[i] > _list[i+1])
                done = false;
                
parent -> touch(i);
parent -> touch(i+1);
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
            
parent -> touch(left);
parent -> touch(right);
            
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
parent -> hold(start);
parent -> add_cycle();
unlock();
        return;   
    }

    int right = start+size;
    int middle = start+size/2;
    
    //cout << "Splitting " << start << ": " << size << " into " << start << ": " << middle-start << " / " << middle << ": " << right-middle << endl;
    _sort_section(start, middle-start, parent);
    _sort_section(middle, right-middle, parent);
    
lock();
parent -> clear_held();
unlock();
    
    int* merged = new int[size];
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
parent -> touch(left);
parent -> touch(right);
unlock();
    }
    
    while(cond(left < middle))
    {
lock();
parent -> touch(left);
parent -> touch(right);
        
        merged[m++] = _list[left++];
        
unlock();
     }
    
    while(cond(right < start+size))
    {
lock();
parent -> touch(left);
parent -> touch(right);
        
        merged[m++] = _list[right++];
        
unlock();
    }
    
    for(int i=start, j=0; cond(j<size); i++, j++)
    {
lock();
parent -> touch(i);
        _list[i] = merged[j];
unlock();
    }
    
parent -> add_cycle();
    delete[] merged;   
}

