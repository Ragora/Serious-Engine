/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_THREADLOCALSTORAGE_H
#define SE_INCL_THREADLOCALSTORAGE_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#ifdef SINGLE_THREADED
#define THREADLOCAL(type, name, defval) type name = defval
#define EXTERNTHREADLOCAL(type, name) extern type name
#elif (defined _MSC_VER)
#define THREADLOCAL(type, name, defval) type __declspec(thread) name = defval
#define EXTERNTHREADLOCAL(type, name) extern type __declspec(thread) name
#else
#define THREADLOCAL(type, name, defval) CThreadLocalStorage<type> name(defval)
#define EXTERNTHREADLOCAL(type, name) extern CThreadLocalStorage<type> name

#include <Engine/Engine.h>

// !!! FIXME : There is a race condition if a thread is making space for
// !!! FIXME :  itself at the same time another thread is touching this class.
// !!! FIXME :  Generally, this won't be a problem, if you are careful about
// !!! FIXME :  new threads accessing an instance of CThreadLocalStorage for
// !!! FIXME :  the first time. I haven't added mutexes, since it would slow
// !!! FIXME :  the template class down. If you need it, either do some
// !!! FIXME :  external locking, or subclass CThreadLocalStorage to include
// !!! FIXME :  a locking mechanism.

// !!! FIXME: 15 years later: why didn't I just use a pthread_key?

ULONG ThreadLocalGetCurrentTID(void);

template <class T>
class CThreadLocalStorage
{
protected:

    typedef struct
    {
        ULONG tid;
        T data;
    } LocalElements;

    size_t FindThreadIndex(void)
    {
        ULONG tid = ThreadLocalGetCurrentTID();
        for (size_t i = 0; i < array_size; i++)
        {
            if (elements[i].tid == tid)
                return(i);
        } // for

        LocalElements *_elements = new LocalElements[array_size + 1];
        for (size_t i = 0; i < array_size; i++)
        {
            _elements[i].data = elements[i].data;
            _elements[i].tid = elements[i].tid;
        } // for

        delete[] elements;
        elements = _elements;
        elements[array_size].data = defval;
        elements[array_size].tid = tid;
        return(array_size++);
    } // FindThreadIndex

private:
    LocalElements *elements;
    size_t array_size;
    T defval;

public:
    CThreadLocalStorage(T _defval)
        : elements(NULL),
          array_size(0),
          defval(_defval)
    {
    } // Constructor


    ~CThreadLocalStorage(void)
    {
        delete[] elements;
    } // Destructor

    T &operator =(T val)
    {
        elements[FindThreadIndex()].data = val;
        return(val);
    } // operator =

    T &operator->(void)
    {
        return(elements[FindThreadIndex()].data);
    } // operator ->

    operator T &(void)
    {
        return(elements[FindThreadIndex()].data);
    } // operator T &
};


#if 0
// a test program.
static CThreadLocalStorage<int> tlocal(0);

void *other_thread(void *arg)
{
    tlocal = 10;

    while (true)
    {
        printf("2nd thread: %d.\n", (int) tlocal);
        sleep(1);
    }

    return(NULL);
}


int main(int argc, char **argv)
{
    pthread_t thread;

    tlocal = 5;

    pthread_create(&thread, NULL, other_thread, NULL);

    while (true)
    {
        printf("main thread: %d.\n", (int) tlocal);
        sleep(1);
    }

    return(0);
}
#endif

#endif  // !defined SINGLE_THREADED

#endif  // include-once blocker.


// end of ThreadLocalStorage.h ...

