#ifndef _OBSERVABLE_H_
#define _OBSERVABLE_H_

#include <string>
template <class T>
class Observable
{
private:
    T _T;
    static const int CMP = 3;
    static const int HLD = 0;
    static const int THLD = 1;
    static const int TCH = 2;
    static const int NNN = 4;
    int color;

    void _setColor(int col, bool overwrite = false)
    {
        if (overwrite||col<color)
            color = col;
    };
public:
    void hold(){ _setColor(HLD); };
    void touch(){ _setColor(TCH); };
    void tmpHold(){ _setColor(THLD); };
    void unhold(){ _setColor(NNN, true); };
    void getRGB(float& r, float& g, float& b)
    {
        switch (color)
        {
        case CMP:
            r = 0.0;
            g = 0.0;
            b = 1.0;
            color = NNN;
            break;
        case THLD:
        case HLD:
            r = 0.0;
            g = 1.0;
            b = 0.0;
            if (color == THLD)color = NNN;
            break;
        case TCH:
            r = 1.0;
            g = 0.0;
            b = 0.0;
            color = NNN;
            break;
        case NNN:
        default:
            r = 1.0;
            g = 1.0;
            b = 1.0;
        }
    }

    Observable(){ color = NNN; };
    ~Observable(){};

    Observable(T obj){ color = NNN; _T = obj; };

    T rawVal(){ return _T; };
    Observable<T>& operator =(Observable<T>& right)
    {
        _T = right._T;
        _setColor(TCH);
        right._setColor(CMP);
        return *this;
    }

    Observable<T>& operator =(T right)
    {
        _T = right;
        _setColor(TCH);
        return *this;
    }

    Observable<T> operator +(Observable<T>& right)
    {
        Observable<T> ret = *this;
        ret._T += right._T;
        _setColor(TCH);
        right._setColor(TCH);
        ret._setColor(TCH);
        return ret;
    }

    Observable<T>& operator +=(Observable<T>& right)
    {
        _T += right._T;
        _setColor(TCH);
        right._setColor(TCH);
        return *this;
    }

    //Post
    Observable<T> operator ++(int)
    {
        Observable<T> ret = *this;
        _T++;
        _setColor(TCH);
        return ret;
    }

    //Pre
    Observable<T>& operator ++()
    {
        _T++;
        _setColor(TCH);
        return *this;
    }

    Observable<T> operator -(Observable<T>& right)
    {
        Observable<T> ret = *this;
        ret._T = -right._T;
        _setColor(TCH);
        ret._setColor(TCH);
        right._setColor(TCH);
    }

    Observable<T> operator -()
    {
        Observable<T> ret = *this;
        ret._T = -_T;
        _setColor(TCH);
        ret._setColor(TCH);
        return ret;
    }

    Observable<T>& operator -=(Observable<T>& right)
    {
        _T -= right._T;
        _setColor(TCH);
        right._setColor(TCH);
        return *this;
    }

    //Post
    Observable<T> operator --(int)
    {
        Observable<T> ret = *this;
        _T--;
        _setColor(TCH);
        ret._setColor(TCH);
        return ret;
    }

    //Pre
    Observable<T>& operator --()
    {
        _T--;
        _setColor(TCH);
        return *this;
    }

    Observable<T> operator *(Observable<T>& right)
    {
        Observable<T> ret = *this;
        ret._T *= right._T;
        _setColor(TCH);
        ret._setColor(TCH);
        right._setColor(TCH);
        return ret;
    }
    Observable<T>& operator *=(Observable<T>& right)
    {
        _T *= right._T;
        _setColor(TCH);
        _setColor(TCH);
        return *this;
    }

    Observable<T> operator /(Observable<T>& right)
    {
        Observable<T> ret = *this;
        ret._T /= right._T;
        _setColor(TCH);
        ret._setColor(TCH);
        right._setColor(TCH);
        return ret;
    }
    Observable<T>& operator /=(Observable<T>& right)
    {
        _T /= right._T;
        _setColor(TCH);
        right._setColor(TCH);
        return *this;
    }

    Observable<T> operator %(Observable<T>& right)
    {
        Observable<T> ret = *this;
        ret._T %= right._T;
        _setColor(TCH);
        right._setColor(TCH);
        ret._setColor(TCH);
        return ret;
    }
    Observable<T>& operator %=(Observable<T>& right)
    {
        _T %= right._T;
        _setColor(TCH);
        right._setColor(TCH);
        return *this;
    }

    /*
    Observable<T> operator &(Observable<T>& right);
    Observable<T>& operator &=(Observable<T>& right);

    Observable<T> operator |(Observable<T>& right);
    Observable<T>& operator |=(Observable<T>& right);

    Observable<T> operator ^(Observable<T>& right);
    Observable<T>& operator ^=(Observable<T>& right);

    Observable<T> operator <<(Observable<T>& right);
    Observable<T>& operator <<=(Observable<T>& right);

    Observable<T> operator >>(Observable<T>& right);
    Observable<T>& operator >>=(Observable<T>& right);
    */

    bool operator !()
    {
        _setColor(CMP);
        return !_T;
    }
    bool operator &&(Observable<T>& right)
    {
        _setColor(CMP);
        right._setColor(CMP);
        return _T && right._T;
    }
    bool operator ||(Observable<T>& right)
    {
        _setColor(CMP);
        right._setColor(CMP);
        return _T || right._T;
    }
    bool operator ==(Observable<T>& right)
    {
        _setColor(CMP);
        right._setColor(CMP);
        return _T == right._T;
    }
    bool operator !=(Observable<T>& right)
    {
        _setColor(CMP);
        right._setColor(CMP);
        return _T != right._T;
    }
    bool operator <(Observable<T>& right)
    {
        _setColor(CMP);
        right._setColor(CMP);
        return _T < right._T;
    }
    bool operator >(Observable<T>& right)
    {
        _setColor(CMP);
        right._setColor(CMP);
        return _T > right._T;
    }
    bool operator <=(Observable<T>& right)
    {
        _setColor(CMP);
        right._setColor(CMP);
        return _T <= right._T;
    }
    bool operator >=(Observable<T>& right)
    {
        _setColor(CMP);
        right._setColor(CMP);
        return _T >= right._T;
    }

    //
    bool operator &&(T& T_)
    {
        _setColor(CMP);
        return _T && T_;
    }
    bool operator ||(T& T_)
    {
        _setColor(CMP);
        return _T || T_;
    }
    bool operator ==(T& T_)
    {
        _setColor(CMP);
        return _T == T_;
    }
    bool operator !=(T& T_)
    {
        _setColor(CMP);
        return _T != T_;
    }
    bool operator <(T& T_)
    {
        _setColor(CMP);
        return _T < T_;
    }
    bool operator >(T& T_)
    {
        _setColor(CMP);
        return _T > T_;
    }
    bool operator <=(T& T_)
    {
        _setColor(CMP);
        return _T <= T_;
    }
    bool operator >=(T& T_)
    {
        _setColor(CMP);
        return _T >= T_;
    }


    //
    friend bool operator &&(T& T_, Observable<T>& _T)
    {
        _T._setColor(CMP);
        return _T._T && T_;
    }
    friend bool operator ||(T& T_, Observable<T>& _T)
    {
        _T._setColor(CMP);
        return _T._T || T_;
    }
    friend bool operator ==(T& T_, Observable<T>& _T)
    {
        _T._setColor(CMP);
        return _T._T == T_;
    }
    friend bool operator !=(T& T_, Observable<T>& _T)
    {
        _T._setColor(CMP);
        return _T._T != T_;
    }
    friend bool operator <(T& T_, Observable<T>& _T)
    {
        _T._setColor(CMP);
        return _T._T < T_;
    }
    friend bool operator >(T& T_, Observable<T>& _T)
    {
        _T._setColor(CMP);
        return _T._T > T_;
    }
    friend bool operator <=(T& T_, Observable<T>& _T)
    {
        _T._setColor(CMP);
        return _T._T <= T_;
    }
    friend bool operator >=(T& T_, Observable<T>& _T)
    {
        _T._setColor(CMP);
        return _T._T >= T_;
    }

    //Observable<T>& operator [](std::ptrdiff_t i);
    friend std::string to_string(Observable<T> obs)
    {
        obs._setColor(TCH);
        return to_string(obs._T);
    }
};
#endif