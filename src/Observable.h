#ifndef _OBSERVABLE_H_
#define _OBSERVABLE_H_

#include <string>
template <class T>
class Observable
{
private:
    // The actual value being observed
    T _T;

    static const int CMP = 3;
    static const int HLD = 0;
    static const int THLD = 1;
    static const int TCH = 2;
    static const int NNN = 4;

    // Stats about how much the value has been touched
    mutable int color;
    mutable int _compares = 0;
    mutable int _changes = 0;
    mutable bool _changed = false;

    void _setColor(int col, bool overwrite = false) const
    {
        if (col == CMP) _compares++;
        if (col == TCH) _changes++;
        if (overwrite || col < color)
        {
            color = col;
            _changed = true;
        }
    };
public:
    void hold() const { _setColor(HLD); };
    void touch() const { _setColor(TCH); };
    void tmpHold() const { _setColor(THLD); };
    void unhold() const { _setColor(NNN, true); };
    int compares() const { return _compares; _compares = 0; };
    int changes() const { return _changes; _changes = 0; };
    bool getRGB(float& r, float& g, float& b) const 
    {
        bool ret = _changed;
        _changed = false;
        switch (color)
        {
        case CMP:
            r = 0.0;
            g = 0.0;
            b = 1.0;
            _setColor(NNN, true);
            break;
        case THLD:
        case HLD:
            r = 0.0;
            g = 1.0;
            b = 0.0;
            if (color == THLD)_setColor(NNN, true);
            break;
        case TCH:
            r = 1.0;
            g = 0.0;
            b = 0.0;
            _setColor(NNN, true);
            break;
        case NNN:
        default:
            r = 1.0;
            g = 1.0;
            b = 1.0;
        }
        return ret;
    }

    Observable(){ color = NNN; };
    ~Observable(){};

    Observable(T obj){ color = NNN; _T = obj; };

    T rawVal() const { return _T; };
    
    Observable<T>& operator =(const Observable<T>& right)
    {
        _T = right._T;
        _setColor(TCH);
        right._setColor(CMP);
        return *this;
    }

    Observable<T>& operator =(const T& right)
    {
        _T = right;
        _setColor(TCH);
        return *this;
    }

    Observable<T> operator +(const Observable<T>& right) const
    {
        Observable<T> ret = *this;
        ret._T += right._T;
        _setColor(TCH);
        right._setColor(TCH);
        ret._setColor(TCH);
        return ret;
    }

    Observable<T>& operator +=(const Observable<T>& right)
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

    Observable<T> operator -(const Observable<T>& right) const
    {
        Observable<T> ret = *this;
        ret._T = -right._T;
        _setColor(TCH);
        ret._setColor(TCH);
        right._setColor(TCH);
    }

    Observable<T> operator -() const 
    {
        Observable<T> ret = *this;
        ret._T = -_T;
        _setColor(TCH);
        ret._setColor(TCH);
        return ret;
    }

    Observable<T>& operator -=(const Observable<T>& right)
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

    Observable<T> operator *(const Observable<T>& right) const
    {
        Observable<T> ret = *this;
        ret._T *= right._T;
        _setColor(TCH);
        ret._setColor(TCH);
        right._setColor(TCH);
        return ret;
    }
    Observable<T>& operator *=(const Observable<T>& right)
    {
        _T *= right._T;
        _setColor(TCH);
        _setColor(TCH);
        return *this;
    }

    Observable<T> operator /(const Observable<T>& right) const
    {
        Observable<T> ret = *this;
        ret._T /= right._T;
        _setColor(TCH);
        ret._setColor(TCH);
        right._setColor(TCH);
        return ret;
    }
    Observable<T>& operator /=(const Observable<T>& right)
    {
        _T /= right._T;
        _setColor(TCH);
        right._setColor(TCH);
        return *this;
    }

    Observable<T> operator %(const Observable<T>& right) const
    {
        Observable<T> ret = *this;
        ret._T %= right._T;
        _setColor(TCH);
        right._setColor(TCH);
        ret._setColor(TCH);
        return ret;
    }
    Observable<T>& operator %=(const Observable<T>& right)
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

    bool operator !() const
    {
        _setColor(CMP);
        return !_T;
    }
    bool operator &&(const Observable<T>& right) const
    {
        _setColor(CMP);
        right._setColor(CMP);
        return _T && right._T;
    }
    bool operator ||(const Observable<T>& right) const
    {
        _setColor(CMP);
        right._setColor(CMP);
        return _T || right._T;
    }
    bool operator ==(const Observable<T>& right) const
    {
        _setColor(CMP);
        right._setColor(CMP);
        return _T == right._T;
    }
    bool operator !=(const Observable<T>& right) const
    {
        _setColor(CMP);
        right._setColor(CMP);
        return _T != right._T;
    }
    bool operator <(const Observable<T>& right) const
    {
        _setColor(CMP);
        right._setColor(CMP);
        return _T < right._T;
    }
    bool operator >(const Observable<T>& right) const
    {
        _setColor(CMP);
        right._setColor(CMP);
        return _T > right._T;
    }
    bool operator <=(const Observable<T>& right) const
    {
        _setColor(CMP);
        right._setColor(CMP);
        return _T <= right._T;
    }
    bool operator >=(const Observable<T>& right) const
    {
        _setColor(CMP);
        right._setColor(CMP);
        return _T >= right._T;
    }

    //
    bool operator &&(const T& T_) const
    {
        _setColor(CMP);
        return _T && T_;
    }
    bool operator ||(const T& T_) const
    {
        _setColor(CMP);
        return _T || T_;
    }
    bool operator ==(const T& T_) const
    {
        _setColor(CMP);
        return _T == T_;
    }
    bool operator !=(const T& T_) const
    {
        _setColor(CMP);
        return _T != T_;
    }
    bool operator <(const T& T_) const
    {
        _setColor(CMP);
        return _T < T_;
    }
    bool operator >(const T& T_) const
    {
        _setColor(CMP);
        return _T > T_;
    }
    bool operator <=(const T& T_) const
    {
        _setColor(CMP);
        return _T <= T_;
    }
    bool operator >=(const T& T_) const
    {
        _setColor(CMP);
        return _T >= T_;
    }


    //
    friend bool operator &&(const T& T_, const Observable<T>& _T)
    {
        _T._setColor(CMP);
        return _T._T && T_;
    }
    friend bool operator ||(const T& T_, const Observable<T>& _T)
    {
        _T._setColor(CMP);
        return _T._T || T_;
    }
    friend bool operator ==(const T& T_, const Observable<T>& _T)
    {
        _T._setColor(CMP);
        return _T._T == T_;
    }
    friend bool operator !=(const T& T_, const Observable<T>& _T)
    {
        _T._setColor(CMP);
        return _T._T != T_;
    }
    friend bool operator <(const T& T_, const Observable<T>& _T)
    {
        _T._setColor(CMP);
        return _T._T < T_;
    }
    friend bool operator >(const T& T_, const Observable<T>& _T)
    {
        _T._setColor(CMP);
        return _T._T > T_;
    }
    friend bool operator <=(const T& T_, const Observable<T>& _T)
    {
        _T._setColor(CMP);
        return _T._T <= T_;
    }
    friend bool operator >=(const T& T_, const Observable<T>& _T)
    {
        _T._setColor(CMP);
        return _T._T >= T_;
    }

    //Observable<T>& operator [](std::ptrdiff_t i);
    friend std::string to_string(const Observable<T>& obs)
    {
        using std::to_string;
        obs._setColor(TCH);
        return to_string(obs._T);
    }

    friend std::ostream& operator << (std::ostream& out, const Observable<T>& obs)
    {
        obs._setColor(TCH);
        return out << obs.rawVal();
    }
};
#endif
