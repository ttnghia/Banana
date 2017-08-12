// Copyright 2009,2011 Philip Wallstedt
//
// This software is covered by the following FreeBSD-style license:
//
// Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following
// conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
//    disclaimer in the documentation and/or other materials provided with the distribution.
//
// This software is provided "as is" and any express or implied warranties, including, but not limited to, the implied warranties
// of merchantability and fitness for a particular purpose are disclaimed.  In no event shall any authors or contributors be
// liable for any direct, indirect, incidental, special, exemplary, or consequential damages (including, but not limited to,
// procurement of substitute goods or services; loss of use, data, or profits; or business interruption) however caused and on any
// theory of liability, whether in contract, strict liability, or tort (including negligence or otherwise) arising in any way out
// of the use of this software, even if advised of the possibility of such damage.

#ifndef UTIL_H
#define UTIL_H

#include<exception>
#include<limits>
#include<iomanip>
#include<fstream>
#include<map>
#include<sstream>
#include <iostream>

#include <Olicado/olicaDeclarations.h>

using namespace std;


struct earlyExit: public std::exception
{
    const char* c;
    earlyExit(const char* a): c(a) {}
    const char* what()const throw()
    {
        return c;
    }
};

inline void crash(string s)
{
    s += ":---Early Termination---:\n";
    cerr << s << endl;
    cout << s << endl;
}



// custom stream operators for tab and non-flushed newline
template<class charT, class traits>basic_ostream<charT, traits>& tab(
    basic_ostream<charT, traits>& os)
{
    os << '\t';
    return os;
}
template<class charT, class traits>basic_ostream<charT, traits>& nwl(
    basic_ostream<charT, traits>& os)
{
    os << '\n';
    return os;
}

template<typename T>string toStr(const T& val) // universal conversion to string
{
    ostringstream osval;
    osval.precision(14);
    osval << val;
    return osval.str();
}

//////// command-line args /////////////

struct argRec
{
    const string val;
    mutable unsigned Naccess;
    argRec(const string& a): val(a)
    {
        Naccess = 0;
    }
};

class CLargMap: public map<const string, const argRec>
{
    template<typename T>friend T comLineArg(const string& tag, T val);
public:
    void init(int argc, char** argv)
    {
        for(int arg = 1; arg < argc; ++arg)
        {
            string argstr(argv[arg]);

            for(unsigned j = 0; j < argstr.size(); ++j)if(argstr[j] == '~')
                {
                    argstr[j] = ' ';
                }

            if(argstr.find('=') == string::npos)
            {
                throw earlyExit("invalid stdin - did you forget to put '~' between vector components?");
            }

            const string tag(argstr.substr(0, argstr.find('=')));   // exception if = not found
            const string val(argstr.substr(tag.size() + 1));

            if(find(tag) == end())
            {
                insert(make_pair(tag, argRec(val)));
            }
            else
            {
                throw earlyExit("duplicate command line tag encountered");
            }
        }
    }
    void reportUnusedTags()
    {
        for(CLargMap::const_iterator it = begin(); it != end(); ++it)
        {
            if(it->second.Naccess < 1)
            {
                cerr << "Warning, unused command line argument: " << it->first << " = " << it->second.val
                     << endl;
            }
        }
    }
};
extern CLargMap globalArgMap;



template<typename T>
T comLineArg(const string& tag, T val)
{
    CLargMap::const_iterator it = globalArgMap.find(tag);

    if(it == globalArgMap.end())
    {
        //report.param(tag,toStr(val));
        return val; // tag not found, return default val
    }

    istringstream sval(it->second.val);
    sval >> val;
    ++it->second.Naccess;      // increment the access counter
    //report.param(tag,toStr(val));
    return val;                // tag found, return command line val
}


//////// tag=val reports /////////////

class paramReport
{
    ofstream reportFile;
    map<string, string>inLoop, exLoop;
    string reportName;
public:
    void init()
    {
        reportName = string(comLineArg("testName", string("run"))) + string(".report");
    }
    template<typename T>void    param(const string& a, const T& b)
    {
        exLoop[a] = toStr(b);
    }
    template<typename T>void progress(const string& a, const T& b)
    {
        inLoop[a] = toStr(b);
    }
    void writeProgress()
    {
        reportFile.open(reportName.c_str(), std::ofstream::out | std::ofstream::app);
        reportFile.precision(6);

        for(multimap<string, string>::iterator it = inLoop.begin(); it != inLoop.end(); ++it)
        {
            reportFile << it->first << '=' << it->second << nwl;
        }

        reportFile.close();
    }
    void writeAll()
    {
        reportFile.open(reportName.c_str(), std::ofstream::out | std::ofstream::app);
        reportFile.precision(14);

        for(multimap<string, string>::iterator it = exLoop.begin(); it != exLoop.end(); ++it)
        {
            reportFile << it->first << '=' << it->second << nwl;
        }

        for(multimap<string, string>::iterator it = inLoop.begin(); it != inLoop.end(); ++it)
        {
            reportFile << it->first << '=' << it->second << nwl;
        }

        reportFile.close();
    }
};

extern paramReport report;







const Real pi = 3.1415926535897932384626433832795028841971694;
const Real machEps = numeric_limits<Real>::epsilon();
const Real oneEps = 1. - machEps;
const Real tiny = numeric_limits<Real>::min();
const Real huge = numeric_limits<Real>::max();

template<typename T>inline T sgn(T a)
{
    return(a < 0. ? -1. : 1.);
}
template<bool>struct Assert;
template<>struct Assert<true> {}; // Assert<(1==2)>(); <-- will not compile



struct timeTag
{
    const Real sec;
    timeTag(const Real tm): sec(tm) {}
};
inline std::ostream& operator<<(std::ostream& os, const timeTag tr)
{
    const int w = 4;
    const Real& t = tr.sec;

    if(t > 3600.)
    {
        os << setw(w) << t / 3600. << " hr  ";
        return os;
    }

    if(t > 60.  )
    {
        os << setw(w) << t / 60.  << " min ";
        return os;
    }

    os << setw(w) << t      << " sec ";
    return os;
}

//////// managed arrays /////////////

// operations required of all arrays in an arrayGroup
struct arraySC
{
    virtual int  size() = 0;
    virtual void resize(int) = 0;
    virtual ~arraySC() {}
};

// each array of type T uses the operation from its underlying vector
template<typename T>struct managedArray: public arraySC, public vector<T>
{
    int size()
    {
        return vector<T>::size();
    }
    void resize(int s)
    {
        vector<T>::resize(s);
    }
};

// a heterogeneous group of arrays, accessed via virtual functions
class arrayGroup
{
    int eachSize;
    vector<arraySC*>ag;
public:
    arrayGroup()
    {
        eachSize = 0;
    }
    void resizeAll(const int s)
    {
        eachSize = s;

        for(unsigned i = 0; i < ag.size(); ++i)
        {
            ag[i]->resize(s);
        }
    }
    int size()
    {
        return eachSize;
    }
    void addArray(arraySC* p)
    {
        ag.push_back(p);
        p->resize(eachSize);
    }
};

// two global array groups; arrays may be added anytime, from anywhere
extern arrayGroup globalPartArrays;
extern arrayGroup globalNodeArrays;
// convenience classes for defining arrays that belong to a managed group
template<typename T>struct partArray: public managedArray<T>
{
    partArray()
    {
        globalPartArrays.addArray(this);
    }
};
template<typename T>struct nodeArray: public managedArray<T>
{
    nodeArray()
    {
        globalNodeArrays.addArray(this);
    }
};
// convenience functions for array group sizes
inline int Npart()
{
    return globalPartArrays.size();
}
inline int Nnode()
{
    return globalNodeArrays.size();
}






#endif
