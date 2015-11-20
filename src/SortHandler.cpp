#include "SortHandler.h"
#include "Globals.h"
#include "Constants.h"
#include <random>
#include <GL/freeglut.h>
#include <iostream>
#include <string>
#include <thread>
#include <algorithm>
#include <functional>

using namespace std;

sort_handle::sort_handle(){};
sort_handle::~sort_handle()
{
    delete[] _list;
}

void sort_handle::animate()
{
    semfunction forceunlock = bind(sort_handle::unlock_handle, (void*)this, true);
    if(!_running)
    {
        _running = true;
        thread sortt (&visual_sort::run_sort, _sort, (void*)this, forceunlock);
        sortt.detach();
    }
}

void sort_handle::reset(bool force)
{   
    if(force || _sort->finished() || !_running)
    {
        _sort_locked = false;
        _currLoops = 0;
        if(force)
        {
            wait_for_sort(this, true);
            _sort->stop();
            unlock_sort(this, true);
            wait_for_sort(this, true);
        }
        semfunction lockfun = bind(sort_handle::wait_for_handle, (void*)this);
        semfunction unlockfun = bind(sort_handle::unlock_handle, (void*)this, false);

        _running = false;
        _cycles = 0;
        delete[] _list;
        _list = nullptr;
        _list = new Observable<int>[_size];

        if (!_ordered)
        {
            for (int i = 0; i < _size; i++)
            {
                _list[i] = rand() % _max;
            }
        }
        else
        {
            for (int i = 0; i < _size; i++)
            {
                _list[i] = i;
            }
            random_shuffle(_list, _list + _size);
        }
        _sort -> setup(_list, _size, lockfun, unlockfun);
        _wait(100000000);
    }
}

void sort_handle::reset(visual_sort* sort, int items, int max, int loops, bool ordered, bool force)
{
    _loops_per_draw = loops;
    _ordered = ordered;
    _sort = sort;
    _size = items;
    _max = max;
    if (ordered)
    {
        _max = _size;
    }
    reset();
}

void sort_handle::draw(int width, int height, int x, int y)
{
wait_for_sort(this);

float rgb[3];
register double left=0, right;
double top;
register double bottom = y;
register Observable<int>* curr = _list;
register double wid = ((double)width / _size);
    for(int i=0; i<_size; i++)
    {
        right = left + wid;
        top = height * (curr->rawVal() / (double)_max) + y;

        curr->getRGB(rgb[0], rgb[1], rgb[2]);
        glColor3fv(rgb);
        
        glBegin( GL_POLYGON );
            glVertex2f( left , bottom );
            glVertex2f( left, top);
            glVertex2f( right, top);
            glVertex2f( right, bottom );
        glEnd();
    
        glFlush();
        left = right;
        curr++;
    }
    string text = "Cycles: " + to_string(_cycles);

    glColor3fv( TEXT );
    glRasterPos2i( x, y+height-13 );
    for(unsigned int i=0; i<text.size(); i++)
        glutBitmapCharacter( GLUT_BITMAP_8_BY_13, text[i] );

    
unlock_sort(this);
}

void sort_handle::unlock_sort(sort_handle* ths, bool forcelock)
{
    (ths)->_sem1.notify();
}

void sort_handle::wait_for_sort(sort_handle* ths, bool forcewait)
{
    if ((!ths->_sort->finished() && ths->_running) || forcewait)
    (ths)->_sem2.wait();
}

void sort_handle::wait_for_handle(void* ths)
{
    sort_handle* obj = ((sort_handle*)ths);
    if (!obj->_sort_locked)
        obj->_sem1.wait();
    obj->_sort_locked = true;

}

void sort_handle::unlock_handle(void* ths, bool force)
{
    sort_handle* obj = ((sort_handle*)ths);
    if (obj->_currLoops == obj->_loops_per_draw || force)
    {
        obj->_sort_locked = false;
        obj->_currLoops = 0;
        obj->_sem2.notify();
    }
    else obj->_currLoops++;

}
