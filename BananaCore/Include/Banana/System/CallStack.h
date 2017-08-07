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

#include <string>
#include <vector>
#include <sstream>

#ifdef __GNUC__

#include <stdio.h>
#include <execinfo.h>
#include <cxxabi.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <iostream>

#define MAX_DEPTH 32
#endif

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace StackTrace
{
/** Call-stack entry datastructure. */
struct Entry
{
    /** Default constructor that clears all fields. */
    Entry() : line(0) {}

    std::string file;     ///< filename
    size_t      line;     ///< line number
    std::string function; ///< name of function or method

    /** Serialize entry into a text string. */
    std::string to_string() const
    {
        std::ostringstream os;
        //        os << file << " (" << line << "): " << function;
        os << file << ": " << function;
        return os.str();
    }

    std::string to_string(size_t i) const
    {
        std::ostringstream os;
        os << i << "    " << file << ": " << function;
        return os.str();
    }
};

/** Stack-trace base class, for retrieving the current call-stack. */
class CallStack
{
public:
    /** Stack-trace consructor.
       \param num_discard - number of stack entries to discard at the top. */
#ifdef __GNUC__
    CallStack(const int num_discard = 0)
    {
        using namespace abi;

        // retrieve call-stack
        void* trace[MAX_DEPTH];
        int   stack_depth = backtrace(trace, MAX_DEPTH);

        for(int i = num_discard + 1; i < stack_depth; i++)
        {
            Dl_info dlinfo;

            if(!dladdr(trace[i], &dlinfo))
            {
                break;
            }

            const char* symname = dlinfo.dli_sname;


            int   status;
            char* demangled = abi::__cxa_demangle(symname, NULL, 0, &status);

            if(status == 0 && demangled)
            {
                symname = demangled;
            }

            //printf("entry: %s, %s\n", dlinfo.dli_fname, symname);

            // store entry to stack
            if(dlinfo.dli_fname && symname)
            {
                Entry e;

                std::string str(dlinfo.dli_fname);
                std::string file_name = str.substr(str.find_last_of("/") + 1);
                e.file = file_name;
                //            e.file     = dlinfo.dli_fname;
                e.line     = 0; // unsupported
                e.function = symname;
                stack.push_back(e);
            }
            else
            {
                break;  // skip last entries below main
            }

            if(demangled)
            {
                free(demangled);
            }
        }
    }

#else
    CallStack(const int num_discard = 0) {}
#endif

    virtual ~CallStack() noexcept {}

    /** Serializes the entire call-stack into a text string. */
    std::string to_string() const
    {
        std::ostringstream os;

        for(size_t i = 0; i < stack.size(); i++)
        {
            os << stack[i].to_string(i) << std::endl;
        }

        return os.str();
    }

    /** Call stack. */
    std::vector<Entry> stack;
};
}   // namespace stacktrace

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana