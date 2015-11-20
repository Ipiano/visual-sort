#include <vector>
#include <iostream>
#include <random>
#include <thread>
#include <mutex>
#include <cmath>
#include <functional>
#include <algorithm>
#include "Sorts.h"
#include "SortHandler.h"
#include "Constants.h"

using namespace std;
#define while(a) while(a) if(_quit){ kill(); break;} else
#define for(a) for(a) if (_quit){ kill(); break;} else

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

void visual_sort::kill(){};

void visual_sort::wait()
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


void visual_sort::run_sort(void* par, semfunction force)
{
    _started = true;
    sort_handle* parent = (sort_handle*)par;
    
    _run_sort(par);
    int i;
    for(i=1; i<_size; i++)
    {
    
wait();
_list[i-1].hold();
_list[i].touch();
unlock();

    }

wait();
_list[_size - 1].hold();
force();
wait();
    _done = true;
force();

}

bubble_sort::bubble_sort(){};
bubble_sort::~bubble_sort(){};

void bubble_sort::_run_sort(void* par)
{
    bsort(par);
}

void bubble_sort::bsort(void* par)
{
    sort_handle* parent = (sort_handle*)par;
    bool swapped, done = false;
    Observable<int> tmp;
    int i;

    for(i=_size-2; i>=0 && !done; i--)
    {
        wait();
        if (i < _size - 3)
            _list[i + 3].unhold();
        if (i < _size-2)
            _list[i + 2].hold();
        unlock();

        swapped = false;
        int j;
        for(j=0; j<=i; j++)
        {
        
wait();
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
    bsort(par);
}
void bogo_sort::bsort(void* par)
{
    sort_handle* parent = (sort_handle*)par;
    bool done = false;
    int swaps, left, right;
    Observable<int>tmp;
    while(!done)
    {
        swaps = rand()%_size+_size;
        
        done = true;
        int i;
        for(i=0; i<_size-1 && done; i++)
        {
            wait();
            
            if(_list[i] > _list[i+1])
                done = false;
                
            unlock();
        }
        for(int i=0; i<swaps && !done; i++)
        {
            wait();
            
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
merge_sort::~merge_sort()
{
    delete[] _merged;
};
void merge_sort::kill()
{
    delete[] _merged;
    _merged = nullptr;
}

void merge_sort::_run_sort(void* par)
{
    _merged = new Observable<int>[_size];
    msort(0, _size, par);
    delete[] _merged;
    _merged = nullptr;
}

void merge_sort::msort(int start, int size, void* par)
{
    if (_quit)return;
    sort_handle* parent = (sort_handle*)par;
    if(size == 1)
    {
        //scout << "Merging 1 item: " << start << endl;
wait();
_list[start].tmpHold();
parent -> add_cycle();
unlock();
        return;   
    }

    int right = start+size;
    int middle = start+size/2;
    
    //cout << "Splitting " << start << ": " << size << " into " << start << ": " << middle-start << " / " << middle << ": " << right-middle << endl;
    msort(start, middle-start, parent);
    msort(middle, right-middle, parent);
    
    int m = 0;
    int left = start;
    right = middle;
    
    while(left < middle && right < start+size)
    {
wait();
        if(_list[left] > _list[right])
        {
            _merged[m++] = _list[right++];
        }
        else
        {
            _merged[m++] = _list[left++];
        }
unlock();
    }
    
    while(left < middle)
    {
wait();
        _merged[m++] = _list[left++];
unlock();
     }
    
    while(right < start+size)
    {
wait();
        _merged[m++] = _list[right++]; 
unlock();
    }
    
    int j = 0;
    int i;
    for (i = start; j < size; i++)
    {
wait();
        _list[i] = _merged[j];
unlock();
        j++;
    }
    
parent -> add_cycle();
}

radix_sort::radix_sort(){};
radix_sort::~radix_sort()
{
    delete[] _copy;
}

void radix_sort::kill()
{
    delete[] _copy;
    _copy = nullptr;
}

int radix_sort::getBucket(Observable<int> num, int rad)
{
    int ret;
    string word = to_string(num);
    if (rad <= word.size())
        ret = word[word.size() - rad] - '0';
    else
        ret = 0;

    return ret;
}

void radix_sort::_run_sort(void* par)
{
    rsort(par);
}

void radix_sort::rsort(void* par)
{
    sort_handle* parent = (sort_handle*)par;
    _copy = new Observable<int>[_size];
    bool done = false;
    int radix = 1;
    int buckets[10];
    int num;
    int maxRadix = -1;
    bool foundEnd = false;
    while (!done)
    {
        for (int i = 0; i < 10; i++)
        {
            buckets[i] = 0;
        }

        for (int i = 0; i < _size; i++)
        {
            wait();
            num = getBucket(_list[i], radix);
            buckets[num]++;
            _copy[i] = _list[i];
            if (!foundEnd)
            {
                int n = to_string(_list[i]).size();
                if (n > maxRadix)
                    maxRadix = n;
            }
            unlock();
        }
        foundEnd = true;

        int lastNum = buckets[0];
        int tmp = 0;
        buckets[0] = 0;
        for (int i = 1; i < 10; i++)
        {
            tmp = buckets[i];
            buckets[i] = buckets[i - 1] + lastNum;
            lastNum = tmp;
        }

        for (int i = 0; i < _size; i++)
        {
            wait();
            num = getBucket(_copy[i], radix);

            _list[buckets[num]++] = _copy[i];
            unlock();
        }
        radix++;
        if (radix > maxRadix) done = true;
        parent->add_cycle();
    }
}

quick_sort::quick_sort(){};
quick_sort::~quick_sort(){};

void quick_sort::_run_sort(void* par)
{
    qsort(par, 0, _size-1);
}

template <class T> 
T median(T a, T b, T c)
{
    if ((a < b && a > c) || (a < c && a > b))
        return a;
    if ((b < a && b > c) || (b < c && b > a))
        return b;
    if ((c < a && c > b) || (c < b && c > a))
        return c;
}

void quick_sort::getPivot(int& pivot, int& pivleft, int& pivright, int left, int right)
{
    if (pivleft == -1)
    {
        int mid = (left + right) / 2;
        pivot = median(_list[mid - 1].rawVal(), _list[mid].rawVal(), _list[mid + 1].rawVal());

        if (_list[mid - 1] == pivot) pivleft = pivright = mid - 1;
        else if (_list[mid + 1] == pivot) pivleft = pivright = mid + 1;
        else pivleft = pivright = mid;
    }
    else
    {
        int newmid = (left + right) / 2;
        int sames = pivright - pivleft+1;
        int newLeft = newmid - sames / 2;
        if (newLeft < 0) cout << "Error" << endl;
        int i;
        for (i = pivleft; i <= pivright; i++)
        {
            swap(_list[i], _list[newLeft++]);
        }
        pivleft = newmid - (sames-1);
        pivright = newmid + (sames-1);
    }
}

void quick_sort::qsort(void* par, int left, int right)
{
    int pivot;
    int origleft = left, origright = right;
    int pivleft = -1, pivright = -1;
    int mid;
    Observable<int> low = _list[left], high = _list[right];

    if (right - left < 3)
    {
        wait();
        if (right - left == 2)
        {
            if (_list[right] < _list[left])
            {
                swap(_list[right], _list[left]);
            }
        }
        unlock();
        return;
    }

    getPivot(pivot, pivleft, pivright, left, right);

    mid = (pivleft + pivright) / 2;
    _list[mid].hold();
    while (left < pivleft && right > pivright)
    {
        while (_list[left] <= pivot && left <= pivleft)
        {
            wait();
            if (_list[left] > high)
                high = _list[left];
            if (_list[left] < low)
                low = _list[left];
            
            if (left == pivleft)
            {
                _list[mid].unhold();
                getPivot(pivot, pivleft, pivright, left, right);
                mid = (pivleft + pivright) / 2;
                _list[mid].hold();
            }

            if (_list[left] == pivot)
                swap(_list[left], _list[pivleft--]);
            
                left++;

            unlock();
        }

        while (_list[right] >= pivot && right >= pivright)
        {
            wait();
            if (_list[right] > high)
                high = _list[right];
            if (_list[right] < low)
                low = _list[right];

            if (right == pivright)
            {
                _list[mid].unhold();
                getPivot(pivot, pivleft, pivright, left, right);
                mid = (pivleft + pivright) / 2;
                _list[mid].hold();
            }

            if (_list[right] == pivot)
                swap(_list[right], _list[pivright++]);
            
            right--;

            unlock();
        }

        swap(_list[left], _list[right]);
    }
    _list[mid].unhold();

    if (high == low) return;
    qsort(par, origleft, pivleft-1);
    qsort(par, pivright+1, origright);
}