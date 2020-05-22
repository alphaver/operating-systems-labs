#pragma once
#ifndef POBEGOS_LAB_5_PIPEMESSAGE_H

    #define POBEGOS_LAB_5_PIPEMESSAGE_H

    #include <utility>
    #include "Employee.h" 

    enum class PipeMessageType
    {
        GET_EMPLOYEE,
        WRITE_EMPLOYEE,
        SEND_EMPLOYEE,
        RELEASE,
        SUCCESS,
        FAILURE,
        EXIT
    };

    struct PipeMessage
    {
        PipeMessageType type;
        byte buffer[sizeof(Employee)];
    };

#endif // POBEGOS_LAB_5_PIPEMESSAGE_H
