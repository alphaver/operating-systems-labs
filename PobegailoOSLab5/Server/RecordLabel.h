#pragma once
#ifndef POBEGOS_LAB_5_RECORDLABEL_H

    #define POBEGOS_LAB_5_RECORDLABEL_H

    enum class RecordState
    {
        FREE = 0,
        READ = 1,
        WRITE = 2
    };
        
    struct RecordLabel
    {
        RecordState state;
        int numberOfThreads;
        std::uint32_t offset;
        RecordLabel(): state(RecordState::FREE), numberOfThreads(0), offset(0) {}
    };

#endif // POBEGOS_LAB_5_RECORDLABEL_H