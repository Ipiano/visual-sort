#ifndef _CONST_H_
#define _CONST_H_
#include <functional>

const int ESCAPE = 27;
const int ITEMS = 4;
const int MAXVAL = 500;
const float FREE[] = {1,1,1};
const float ACCESS[] = {1,0,0};
const float HOLD[] = {0,1,0};
const float TEXT[] = {0.5, 0.5, 0.5};
const float BLACK[] = { 0, 0, 0};
typedef unsigned char byte;
typedef std::function<void()> semfunction;


#endif
