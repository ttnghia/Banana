//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \     This file is part of Banana - a general programming framework    /
//    /                    Created: 2017 by Nghia Truong                     \
//    \                      <nghiatruong.vn@gmail.com>                      /
//    /                      https://ttnghia.github.io                       \
//    \                        All rights reserved.                          /
//    /                                                                      \
//    \______________________________________________________________________/
//                                  ___)( )(___
//                                 (((__) (__)))
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <Banana/Setup.h>

#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <cstdlib>
#include <sstream>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// DataBuffer class
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class DataBuffer
{
public:
    DataBuffer(size_t bufferSize = 0) { if(bufferSize > 0) { reserve(bufferSize); } }

    size_t size() const { return m_Buffer.size(); }
    void   resize(size_t bufferSize) { m_Buffer.resize(bufferSize); }
    void   reserve(size_t bufferSize) { m_Buffer.reserve(bufferSize); }
    void   clearBuffer() { m_Buffer.resize(0); }

    unsigned char*       data(size_t offset = 0) { return &m_Buffer.data()[offset]; }
    const unsigned char* data(size_t offset = 0) const { return &m_Buffer.data()[offset]; }

    Vector<UChar>&       buffer() { return m_Buffer; }
    const Vector<UChar>& buffer() const { return m_Buffer; }

    //////////////////////////////////////////////////////////////////////
    // set data (= clear + append)
    void setData(const DataBuffer& dataBuffer) { clearBuffer(); append(dataBuffer); }
    void setData(const void* dataPtr, size_t dataSize) { clearBuffer(); append(dataPtr, dataSize); }

    template<class T> void setData(T data) { clearBuffer(); append<T>(data); }
    template<class T> void setData(const Vector<T>& dvec, bool bWriteVectorSize = true) { clearBuffer(); append<T>(dvec, bWriteVectorSize); }
    template<class T> void setData(const Vector<Vector<T> >& dvec, bool bWriteVectorSize = true) { clearBuffer(); append<T>(dvec, bWriteVectorSize); }

    //////////////////////////////////////////////////////////////////////
    // append data
    void append(const DataBuffer& dataBuffer) { append((const void*)dataBuffer.data(), dataBuffer.size()); }
    void append(const void* dataPtr, size_t dataSize);

    template<class T> void        append(T value) { append((const void*)&value, sizeof(T)); }
    template<class T> void        append(const Vector<T>& dvec, bool bWriteVectorSize = true);
    template<class T> void        append(const Vector<Vector<T> >& dvec, bool bWriteVectorSize = true);
    template<Int N, class T> void append(const Vector<VecX<N, T> >& dvec, bool bWriteVectorSize = true);

    ////////////////////////////////////////////////////////////////////////////////
    // get data
    size_t getData(void* dataPtr, size_t dataSize, size_t startOffset = 0);

    template<class T> size_t        getData(T& value, size_t startOffset = 0) { return getData((void*)&value, sizeof(T), startOffset); }
    template<class T> size_t        getData(Vector<T>& dvec, size_t startOffset = 0, UInt vSize = 0);
    template<class T> size_t        getData(Vector<Vector<T> >& dvec, size_t startOffset = 0, UInt vSize = 0);
    template<Int N, class T> size_t getData(Vector<VecX<N, T> >& dvec, size_t startOffset = 0, UInt vSize = 0);

private:
    Vector<UChar> m_Buffer;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline void DataBuffer::append(const void* dataPtr, size_t dataSize)
{
    size_t lastSize = m_Buffer.size();
    resize(lastSize + dataSize);
    memcpy((void*)data(lastSize), dataPtr, dataSize);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline size_t DataBuffer::getData(void* dataPtr, size_t dataSize, size_t startOffset /*= 0*/)
{
    __BNN_REQUIRE(startOffset + dataSize <= m_Buffer.size());
    memcpy(dataPtr, (void*)data(startOffset), dataSize);
    return dataSize;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
void DataBuffer::append(const Vector<T>& dvec, bool bWriteVectorSize /*= true*/)
{
    // any vector data can begin with the number of vector elements
    if(bWriteVectorSize) {
        append<UInt>(static_cast<UInt>(dvec.size()));
    }
    append((const void*)dvec.data(), dvec.size() * sizeof(T));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
void DataBuffer::append(const Vector<Vector<T> >& dvec, bool bWriteVectorSize /*= true*/)
{
    if(bWriteVectorSize) {
        append<UInt>(static_cast<UInt>(dvec.size()));
    }

    for(size_t i = 0; i < dvec.size(); ++i) {
        append<T>(dvec[i], true);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class T>
void DataBuffer::append(const Vector<VecX<N, T> >& dvec, bool bWriteVectorSize /*= true*/)
{
    if(bWriteVectorSize) {
        append<UInt>(static_cast<UInt>(dvec.size()));
    }
    append((const void*)dvec.data(), dvec.size() * sizeof(T) * N);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
size_t DataBuffer::getData(Vector<T>& dvec, size_t startOffset /*= 0*/, UInt vSize /*= 0*/)
{
    size_t segmentStart = startOffset;
    UInt   nElements    = vSize;

    if(nElements == 0) {
        segmentStart += getData((void*)&nElements, sizeof(UInt), segmentStart);
    }

    dvec.resize(nElements);
    segmentStart += getData((void*)dvec.data(), sizeof(T) * nElements, segmentStart);
    return (segmentStart - startOffset);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
size_t DataBuffer::getData(Vector<Vector<T> >& dvec, size_t startOffset /*= 0*/, UInt vSize /*= 0*/)
{
    size_t segmentStart = startOffset;
    UInt   nElements    = vSize;

    if(nElements == 0) {
        segmentStart += getData((void*)&nElements, sizeof(UInt), segmentStart);
    }

    dvec.resize(nElements);
    for(UInt i = 0; i < nElements; ++i) {
        segmentStart += getData(dvec[i], segmentStart, 0);
    }

    return (segmentStart - startOffset);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class T>
size_t DataBuffer::getData(Vector<VecX<N, T> >& dvec, size_t startOffset /*= 0*/, UInt vSize /*= 0*/)
{
    size_t segmentStart = startOffset;
    UInt   nElements    = vSize;

    if(nElements == 0) {
        segmentStart += getData((void*)&nElements, sizeof(UInt), segmentStart);
    }

    dvec.resize(nElements);
    segmentStart += getData((void*)dvec.data(), sizeof(T) * N * nElements, segmentStart);
    return (segmentStart - startOffset);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana