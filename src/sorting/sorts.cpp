#include "sorts.h"

#include "constants.h"
#include "globals.h"
#include "sorting/sort_handler.h"

#include <GL/freeglut.h>

#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>
#include <vector>

using namespace std;

namespace sorting
{
// clang-format off
#define while(a)             \
    while (a) if (_quit)     \
    {                        \
        kill();              \
        break;               \
    }                        \
    else
#define for(a)             \
    for (a) if (_quit)     \
    {                      \
        kill();            \
        break;             \
    }                      \
    else
// clang-format on

visual_sort::visual_sort()  = default;
visual_sort::~visual_sort() = default;
void visual_sort::setup(util::Observable<int>* list, int size, semfunction lockfun, semfunction unlockfun)
{
    _quit    = false;
    _lock    = lockfun;
    _unlock  = unlockfun;
    _done    = false;
    _list    = list;
    _size    = size;
    _started = false;
}

void visual_sort::kill() {};

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
    _started            = true;
    sort_handle* parent = (sort_handle*)par;

    _run_sort(par);
    int i;
    for (i = 1; i < _size; i++)
    {

        wait();
        _list[i - 1].hold();
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

void visual_sort::draw(int& changes, int& compares, int width, int height, int _max, int x, int y, int method)
{
    float rgb[3];
    double left = 0, right;
    double top;
    double bottom               = y;
    util::Observable<int>* curr = _list;
    double wid                  = ((double)width / _size);

    for (int i = 0; i < _size; i++)
    {
        right = left + wid;
        top   = height * (curr->rawVal() / (double)_max) + y;

        changes += curr->changes();
        compares += curr->compares();
        curr->getRGB(rgb[0], rgb[1], rgb[2]);
        getColor(rgb, i);

        glColor3fv(rgb);

        glBegin(GL_POLYGON);
        switch (method)
        {
        case 1:
            glVertex2f(left, top - wid / 2);
            glVertex2f(left, top + wid / 2);
            glVertex2f(right, top + wid / 2);
            glVertex2f(right, top - wid / 2);
            break;
        case 2:
            glVertex2f(left, global::screen_height / 2 - top / 2);
            glVertex2f(left, global::screen_height / 2 + top / 2);
            glVertex2f(right, global::screen_height / 2 + top / 2);
            glVertex2f(right, global::screen_height / 2 - top / 2);
            break;
        case 0:
        default:
            glVertex2f(left, bottom);
            glVertex2f(left, top);
            glVertex2f(right, top);
            glVertex2f(right, bottom);
            break;
        }
        glEnd();

        left = right;
        curr++;
    }
    glFlush();
}

void bubble_sort::_run_sort(void* par)
{
    bsort(par);
}

void bubble_sort::bsort(void* par)
{
    sort_handle* parent = (sort_handle*)par;
    bool swapped, done = false;
    util::Observable<int> tmp;
    int i;

    for (i = _size - 2; i >= 0 && !done; i--)
    {
        wait();
        if (i < _size - 3)
            _list[i + 3].unhold();
        if (i < _size - 2)
            _list[i + 2].hold();
        unlock();

        swapped = false;
        int j;
        for (j = 0; j <= i; j++)
        {

            wait();
            if (_list[j] > _list[j + 1])
            {
                swapped      = true;
                tmp          = move(_list[j]);
                _list[j]     = move(_list[j + 1]);
                _list[j + 1] = move(tmp);
            }

            unlock();
        }
        if (!swapped)
            done = true;
        parent->add_cycle();
    }
}

void bogo_sort::_run_sort(void* par)
{
    bsort(par);
}
void bogo_sort::bsort(void* par)
{
    sort_handle* parent = (sort_handle*)par;
    bool done           = false;
    int swaps, left, right;
    util::Observable<int> tmp;
    while (!done)
    {
        swaps = rand() % _size + _size;

        done = true;
        int i;
        for (i = 0; i < _size - 1 && done; i++)
        {
            wait();

            if (_list[i] > _list[i + 1])
                done = false;

            unlock();
        }
        for (int i = 0; i < swaps && !done; i++)
        {
            wait();

            left  = rand() % _size;
            right = rand() % _size;

            tmp          = _list[left];
            _list[left]  = _list[right];
            _list[right] = tmp;

            unlock();
        }
        parent->add_cycle();
    }
}

void merge_sort::_run_sort(void* par)
{
    _merged.resize(_size);
    msort(0, _size, par);
}

void merge_sort::msort(int start, int size, void* par)
{
    if (_quit)
        return;
    sort_handle* parent = (sort_handle*)par;
    if (size == 1)
    {
        //scout << "Merging 1 item: " << start << endl;
        wait();
        _list[start].tmpHold();
        parent->add_cycle();
        unlock();
        return;
    }

    int right  = start + size;
    int middle = start + size / 2;

    //cout << "Splitting " << start << ": " << size << " into " << start << ": " << middle-start << " / " << middle << ": " << right-middle << endl;
    msort(start, middle - start, parent);
    msort(middle, right - middle, parent);

    int m    = 0;
    int left = start;
    right    = middle;

    while (left < middle && right < start + size)
    {
        wait();
        if (_list[left] > _list[right])
        {
            _merged[m++] = _list[right++];
        }
        else
        {
            _merged[m++] = _list[left++];
        }
        unlock();
    }

    while (left < middle)
    {
        wait();
        _merged[m++] = _list[left++];
        unlock();
    }

    while (right < start + size)
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

    parent->add_cycle();
}

int radix_sort::getBucket(util::Observable<int> num, int rad)
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
    _copy.resize(_size);
    bool done = false;
    int radix = 1;
    int buckets[10];
    int num;
    int maxRadix  = -1;
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
        int tmp     = 0;
        buckets[0]  = 0;
        for (int i = 1; i < 10; i++)
        {
            tmp        = buckets[i];
            buckets[i] = buckets[i - 1] + lastNum;
            lastNum    = tmp;
        }

        for (int i = 0; i < _size; i++)
        {
            wait();
            num = getBucket(_copy[i], radix);

            _list[buckets[num]++] = _copy[i];
            unlock();
        }
        radix++;
        if (radix > maxRadix)
            done = true;
        parent->add_cycle();
    }
}

void quick_sort::_run_sort(void* par)
{
    qsort(par, 0, _size - 1);
}

template <class T> T median(const T& a, const T& b, const T& c)
{
    return (a >= b && b >= c) ? c : ((b >= a && a >= c) ? a : c);
}

void quick_sort::getPivot(int& mid, int& pivleft, int& pivright, int left, int right)
{
    if (pivleft == -1)
    {
        mid = (left + right) / 2;
        int pivot;
        if (right - left > 5)
        {
            pivot = median(_list[mid - 1], _list[mid], _list[mid + 1]).rawVal();
            if (mid - 1 >= 0 && _list[mid - 1] == pivot)
                pivleft = pivright = mid = mid - 1;
            else if (_list[mid + 1] == pivot)
                pivleft = pivright = mid = mid + 1;
            else
                pivleft = pivright = mid;
        }
        else
            pivleft = pivright = mid;
    }
    else
    {
        mid       = (left + right) / 2;
        int sames = pivright - pivleft;
        int newLeft, newRight = newLeft = mid - sames / 2;
        if (newLeft < 0)
            cout << "Error" << endl;
        int i;
        for (i = pivleft; i <= pivright; i++)
        {
            swap(_list[i], _list[newRight++]);
        }
        pivleft  = newLeft;
        pivright = newRight - 1;
    }
    while (pivleft > left && _list[pivleft - 1] == _list[pivleft])
        pivleft--;
    while (pivright < right && _list[pivright + 1] == _list[pivright])
        pivright++;
}

void quick_sort::qsort(void* par, int left, int right)
{
    ((sort_handle*)par)->add_cycle();

    //cout << "Left: " << left << " : Right: " << right << endl;
    int pivot;
    int origleft = left, origright = right;
    int pivleft = -1, pivright = -1;
    int mid;
    util::Observable<int> low = _list[left], high = _list[right];

    if (right - left < 2)
    {
        wait();
        if (right - left == 1)
        {
            if (_list[right] < _list[left])
            {
                swap(_list[right], _list[left]);
            }
        }
        unlock();
        return;
    }

    /*
    int templeft = left;
    while (_list[templeft++] == _list[right] && templeft != right);
    if (templeft == right)return;
    */

    getPivot(mid, pivleft, pivright, left, right);
    if (right < pivright)
        right = pivright;
    if (left > pivleft)
        left = pivleft;
    pivot = _list[mid].rawVal();

    //cout << pivleft << "-" << pivright << " -> " << pivot << endl;

    _list[mid].hold();
    while (left < pivleft && right > pivright)
    {
        while (_list[left] <= pivot && left <= pivleft)
        {
            //cout << left << " -> " << _list[left] << " : " << pivleft << "-" << pivright << " -> " << pivot << " : " << right << " -> " << _list[right] << endl;
            wait();
            if (_list[left] > high)
                high = _list[left];
            if (_list[left] < low)
                low = _list[left];

            if (left == pivleft)
            {
                _list[mid].unhold();
                getPivot(mid, pivleft, pivright, left, right);
                pivot = _list[mid].rawVal();
                _list[mid].hold();
                if (left == pivleft)
                    break;
                left--;
            }

            if (_list[left] == pivot)
                swap(_list[left], _list[--pivleft]);
            else
                left++;

            unlock();
        }

        while (_list[right] >= pivot && right >= pivright)
        {
            //cout << left << " -> " << _list[left] << " : " << pivleft << "-" << pivright << " -> " << pivot << " : " << right << " -> " << _list[right] << endl;

            wait();
            if (_list[right] > high)
                high = _list[right];
            if (_list[right] < low)
                low = _list[right];

            if (right == pivright)
            {
                _list[mid].unhold();
                getPivot(mid, pivleft, pivright, left, right);
                pivot = _list[mid].rawVal();
                _list[mid].hold();

                if (right == pivright)
                    break;
                right++;
            }

            if (_list[right] == pivot)
                swap(_list[right], _list[++pivright]);
            else
                right--;

            unlock();
        }

        if (_list[left] != _list[right])
        {
            //if(left == pivleft){pivleft++; pivright++;}
            //if(right == pivright){pivleft--; pivright--;}
            swap(_list[left], _list[right]);
        }
        else if (_list[left] > pivot)
        {
            swap(_list[left], _list[pivright--]);
            pivleft--;
        }
        else if (_list[right] < pivot)
        {
            swap(_list[right], _list[pivleft++]);
            pivright++;
        }
    }
    _list[mid].unhold();

    if (high == low)
        return;
    //cout << "L -> ";
    qsort(par, origleft, pivleft);
    //cout << "R -> ";
    qsort(par, pivright, origright);
}

void heap_sort::_run_sort(void* par)
{
    hsort(par);
}

void heap_sort::hsort(void* par)
{
    end = _size - 1;
    util::Observable<int> tmp;

    build();

    _list[end].hold();
    while (end > 0)
    {
        ((sort_handle*)par)->add_cycle();
        wait();
        tmp        = _list[end];
        _list[end] = _list[0];
        _list[0]   = tmp;
        unlock();
        _list[end--].unhold();
        _list[end].hold();
        sift(0);
    }
}

void heap_sort::build()
{
    int start = _size - 1;
    //cout << "Build" << endl;
    while (start >= 0)
    {
        sift(start);
        start--;
    }
}

void heap_sort::sift(int start)
{
    int root = start;
    int child, swap;
    util::Observable<int> tmp;
    //cout << "Sift" << endl;
    _list[root].hold();
    while (root * 2 < end)
    {
        wait();
        child = root * 2;
        swap  = root;
        if (_list[swap] < _list[child])
            swap = child;
        if (child + 1 <= end && _list[swap] < _list[child + 1])
            swap = child + 1;
        if (swap == root)
        {
            _list[root].unhold();
            return;
        }
        else
        {
            tmp         = _list[root];
            _list[root] = _list[swap];
            _list[swap] = tmp;
            _list[root].unhold();
            root = swap;
            _list[root].hold();
        }
        unlock();
    }
    _list[root].unhold();
}

void heap_sort::getColor(float rgb[3], int item)
{
    if (item >= end)
        return;
    const float start[3] = {248.0 / 255, 255.0 / 255, 108.0 / 255};
    if (rgb[0] > 0.9 && rgb[1] > 0.9 && rgb[2] > 0.9)
    {
        int height = log(_size) + 1;
        int level  = log(item);
        rgb[0]     = start[0] - start[0] / height * level;
        rgb[1]     = start[1] - start[1] / height * level;
        rgb[2]     = start[2] - start[2] / height * level;
    }
}
}
