#include "sort_handler.h"

#include "constants.h"
#include "globals.h"

#include <GL/freeglut.h>

#include <algorithm>
#include <functional>
#include <iostream>
#include <random>
#include <string>
#include <thread>

using namespace std;

namespace sorting
{
sort_handle::sort_handle()  = default;
sort_handle::~sort_handle() = default;

void sort_handle::animate()
{
    semfunction forceunlock = bind(sort_handle::unlock_handle, (void*)this, true);
    if (!_running)
    {
        _running = true;
        thread sortt(&visual_sort::run_sort, _sort, (void*)this, forceunlock);
        sortt.detach();
    }
}

void sort_handle::reset(bool force)
{
    if (force || _sort->finished() || !_running)
    {
        _sort_locked = false;
        _currLoops   = 0;
        if (force)
        {
            wait_for_sort(this, true);
            _sort->stop();
            unlock_sort(this, true);
            wait_for_sort(this, true);
        }
        semfunction lockfun   = bind(sort_handle::wait_for_handle, (void*)this);
        semfunction unlockfun = bind(sort_handle::unlock_handle, (void*)this, false);

        _running = false;
        _cycles  = 0;

        _list.clear();
        _list.resize(_size);

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
            random_shuffle(_list.begin(), _list.end());
        }

        if (!force)
            if (_visual < 0)
                _randomvis = true;

        if (_randomvis)
            _visual = rand() % _sort->visual_count();

        _sort->setup(_list.data(), _size, lockfun, unlockfun);
        _wait(std::chrono::milliseconds(100));
    }
}

void sort_handle::reset(visual_sort& sort, int items, int max, int loops, bool ordered, bool force, int visual)
{
    _loops_per_draw = loops;
    _ordered        = ordered;
    _sort           = &sort;
    _size           = items;
    _max            = max;
    _visual         = visual;
    if (ordered)
    {
        _max = _size;
    }
    reset();
}

void drawText(const std::string& s, double x, double y)
{
    glPushMatrix();
    glTranslated(x, y, 0);
    glScalef(0.1, 0.1, 1);

    glColor3fv(TEXT);
    for (const auto c : s)
        glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, c);

    glPopMatrix();
}

void sort_handle::draw(int width, int height, int x, int y)
{
    wait_for_sort(this);

    int changes  = 0;
    int compares = 0;

    _sort->draw(changes, compares, width, height, _max, x, y, _visual);

    const auto text_left   = x;
    const auto text_top    = (y + height);
    const auto text_height = 15;

    drawText("Cycles: " + to_string(_cycles), text_left, text_top - text_height);
    drawText("Array Reads: " + to_string(compares), text_left, text_top - text_height * 2);
    drawText("Array Writes: " + to_string(changes), text_left, text_top - text_height * 3);
    drawText("Total Array Operations: " + to_string(changes + compares), text_left, text_top - text_height * 4);

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
        obj->_currLoops   = 0;
        obj->_sem2.notify();
    }
    else
        obj->_currLoops++;
}
}
