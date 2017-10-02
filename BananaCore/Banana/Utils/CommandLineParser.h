//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//  Copyright (c) 2017 by
//       __      _     _         _____
//    /\ \ \__ _| |__ (_) __ _  /__   \_ __ _   _  ___  _ __   __ _
//   /  \/ / _` | '_ \| |/ _` |   / /\/ '__| | | |/ _ \| '_ \ / _` |
//  / /\  / (_| | | | | | (_| |  / /  | |  | |_| | (_) | | | | (_| |
//  \_\ \/ \__, |_| |_|_|\__,_|  \/   |_|   \__,_|\___/|_| |_|\__, |
//         |___/                                              |___/
//
//  <nghiatruong.vn@gmail.com>
//  All rights reserved.
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <Banana/Setup.h>

#include <map>
#include <algorithm>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class CommandLineParser : public std::map<String, String>
{
public:
    CommandLineParser() = default;
    CommandLineParser(int argc, char** argv) { init(argc, argv); }

    void init(int argc, char** argv)
    {
        insert(std::make_pair("Program", argv[0]));

        for(int arg = 1; arg < argc; ++arg) {
            String argstr(argv[arg]);

            if(argstr.find('=') == String::npos) {
                fprintf(stderr, "Invalid parameter input: variable assignment must be '-name=value' or 'name=value' without spacing\n");
                exit(EXIT_FAILURE);
            }

            const String tag(argstr.substr(0, argstr.find('=')));
            const String val(argstr.substr(tag.size() + 1));

            if(find(tag) == end())
                insert(std::make_pair(tag, val));
            else
                (*this)[tag] = val;
        }
    }

    const String& getProgramName() { return (*this)[String("Program")]; }

    bool getString(const String& tag, String& value) const
    {
        auto it = find(tag);

        if(it == end())
            return false;

        value = it->second;
        return true;
    }

    bool getInt(const String& tag, Int& value) const
    {
        auto it = find(tag);

        if(it == end())
            return false;

        value = static_cast<Int>(atoi(it->second.c_str()));
        return true;
    }

    template<class RealType>
    bool getReal(const String& tag, RealType& value) const
    {
        auto it = find(tag);

        if(it == end())
            return false;

        value = static_cast<RealType>(atof(it->second.c_str()));
        return true;
    }

    template<Int N, class RealType>
    bool getVector(const String& tag, VecX<N, RealType>& vec) const
    {
        auto it = find(tag);

        if(it == end())
            return false;

        const String& valStr   = it->second;
        size_t        commaPos = 0;
        for(Int i = 0; i < N - 1; ++i) {
            size_t nextCommaPos = valStr.find(',', commaPos);
            if(nextCommaPos == String::npos)
                return false;

            vec[i]   = static_cast<RealType>(atof(valStr.substr(commaPos, nextCommaPos - commaPos).c_str()));
            commaPos = nextCommaPos + 1;
        }
        vec[N - 1] = static_cast<RealType>(atof(valStr.substr(commaPos, valStr.size() - commaPos).c_str()));

        return true;
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana