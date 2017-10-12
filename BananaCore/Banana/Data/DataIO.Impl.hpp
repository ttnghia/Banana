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
