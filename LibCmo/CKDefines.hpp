#pragma once

#include <string>
#include <vector>

namespace LibCmo {

    using CKINT = int32_t;
    using CK_ID = uint32_t;
    using CKDWORD = uint32_t;
    using CKWORD = uint16_t;
    using CKBYTE = uint8_t;
    using CKBOOL = int32_t;
    using CKMUTSTRING = char*;
    using CKSTRING = const char*;

    using XString = std::string;
    using XBitArray = std::vector<bool>;
    template<typename T>
    using XArray = std::vector<T>;
    using XIntArray = std::vector<int32_t>;
    template<typename T>
    using XClassArray = std::vector<T>;
    //using CKObjectArray = std::vector<CKObject*>;


    struct CKGUID {
        union {
            struct {
                CKDWORD d1, d2;
            };
            CKDWORD d[2];
        };
        CKGUID(CKDWORD gd1 = 0, CKDWORD gd2 = 0) { d[0] = gd1; d[1] = gd2; }
    };


}
