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

    if(!_running)
    {
        _running = true;
        thread sortt (&visual_sort::run_sort, _sort, (void*)this);
        sortt.detach();
    }
}

void sort_handle::reset(bool force)
{   
    if(force || !_running || (_running && _sort->finished()))
    {
        if(force)
        {
            handle_lock(this);
            _sort->stop();
            //handle_unlock(this);
        }
        semfunction lockfun = bind(sort_handle::sort_lock, (void*)this);
        semfunction unlockfun = bind(sort_handle::sort_unlock, (void*)this);
        
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
    }
}

void sort_handle::reset(visual_sort* sort, int items, int max, bool ordered, bool force)
{
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
handle_lock(this);

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

    

handle_unlock(this);
}

void sort_handle::handle_lock(sort_handle* ths)
{
    if (!ths->_sort->finished())
    (ths)->_sem1.notify();
}

void sort_handle::handle_unlock(sort_handle* ths)
{
    if (!ths->_sort->finished())
    (ths)->_sem2.wait();
}

void sort_handle::sort_lock(void* ths)
{
    ((sort_handle*)ths)->_sem2.notify();
}

void sort_handle::sort_unlock(void* ths)
{
    ((sort_handle*)ths)->_sem1.wait();
}
