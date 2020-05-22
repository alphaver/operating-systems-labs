#pragma once
#ifndef POBEGOS_LAB_5_CRITICALSECTION_H

    #define POBEGOS_LAB_5_CRITICALSECTION_H

    #include <Windows.h>

    class CriticalSection
    {
    private:
        CRITICAL_SECTION section;

        CriticalSection(const CriticalSection &) = default;
        CriticalSection(CriticalSection &&) = default;
        CriticalSection & operator= (const CriticalSection &) = default;
        CriticalSection & operator= (CriticalSection &&) = default;

    public:
        CriticalSection();
        virtual ~CriticalSection();

        void Enter();
        void Leave();
    };

#endif // POBEGOS_LAB_5_CRITICALSECTION_H