#include "SortHandler.h"
#include "Globals.h"
#include "Constants.h"
#include <random>
#include <GL/freeglut.h>
#include <iostream>
#include <string>
#include <thread>
#include <functional>

using namespace std;

sort_handle::sort_handle(){};
sort_handle::~sort_handle()
{
    delete[] _list;
}

void sort_handle::animate()
{

    if(!_running && _sortt == nullptr)
    {
        _running = true;
        _sortt = new thread(&visual_sort::run_sort, _sort, (void*)this);
        _sortt->detach();
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
            delete _sortt;
            _sortt = nullptr;
            handle_unlock(this);
        }
        semfunction lockfun = bind(sort_handle::sort_lock, (void*)this);
        semfunction unlockfun = bind(sort_handle::sort_unlock, (void*)this);
        
        _running = false;
        _cycles = 0;
        delete[] _list;
        _list = nullptr;
        _list = new int[_size];

        for(int i=0; i<_size; i++)
        {
            _list[i] = rand() % _max;
        }

        _sort -> setup(_list, _size, lockfun, unlockfun);
    }
}

void sort_handle::reset(visual_sort* sort, int items, int max, bool force)
{
    _sort = sort;
    _size = items;
    _max = max;

    reset();
}

void sort_handle::draw(int width, int height, int x, int y)
{
if(_running)
    handle_lock(this);

    for(int i=0; i<_size; i++)
    {
        glColor3fv( FREE );
        for(unsigned int j=0; j<_held.size(); j++)
        {
            if(i == _held[j])
            {
                glColor3fv( HOLD );
            }
        }
        for(unsigned int j=0; j<_touched.size(); j++)
        {
            if(i == _touched[j])
            {
                glColor3fv( ACCESS );
            }
        }
        glBegin( GL_POLYGON );
            glVertex2f( i*((double)width/_size) + x, y );
            glVertex2f( i*((double)width/_size) + x, height * (_list[i]/(double)_max) + y);
            glVertex2f( (i+1)*((double)width/_size) + x, height * (_list[i]/(double)_max) + y);
            glVertex2f( (i+1)*((double)width/_size) + x, y );
        glEnd();
    
        glFlush();
    }
    string text = "Cycles: " + to_string(_cycles);

    glColor3fv( TEXT );
    glRasterPos2i( x, y+height-13 );
    for(unsigned int i=0; i<text.size(); i++)
        glutBitmapCharacter( GLUT_BITMAP_8_BY_13, text[i] );

    _touched.clear();
    
if(_running)
    handle_unlock(this);
}

void sort_handle::handle_lock(sort_handle* ths)
{
    ((sort_handle*)ths)->_sem1.notify();
}

void sort_handle::handle_unlock(sort_handle* ths)
{
    ((sort_handle*)ths)->_sem2.wait();
}

void sort_handle::sort_lock(void* ths)
{
    ((sort_handle*)ths)->_sem2.notify();
}

void sort_handle::sort_unlock(void* ths)
{
    ((sort_handle*)ths)->_sem1.wait();
}
