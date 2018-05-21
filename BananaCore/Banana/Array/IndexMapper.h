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
#include <Banana/Utils/STLHelpers.h>

#include <cassert>
#include <unordered_map>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace Banana
{
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class IndexType>
class IndexMapper
{
public:
    IndexMapper() = default;
    IndexType size() const { return m_Size; }
    void      clear() { m_idx_XDTo1D.clear(); m_Size = 0; }

    ////////////////////////////////////////////////////////////////////////////////
    void                         addUniqueKey(const VecX<N, IndexType>& key);
    template<class IntType> void addUniqueKey(IntType i, IntType j) { addUniqueKey(makeKey(i, j)); }
    template<class IntType> void addUniqueKey(IntType i, IntType j, IntType k) { addUniqueKey(makeKey(i, j, k)); }

    bool                         addKeyIfNotExist(const VecX<N, IndexType>& key);
    template<class IntType> bool addKeyIfNotExist(IntType i, IntType j) { return addKeyIfNotExist(makeKey(i, j)); }
    template<class IntType> bool addKeyIfNotExist(IntType i, IntType j, IntType k) { return addKeyIfNotExist(makeKey(i, j, k)); }

    IndexType                                  get1DIdx(const VecX<N, IndexType>& key);
    template<class IntType> IndexType          get1DIdx(IntType i, IntType j) { return get1DIdx(makeKey(i, j)); }
    template<class IntType> IndexType          get1DIdx(IntType i, IntType j, IntType k) { return get1DIdx(makeKey(i, j, k)); }

private:
    template<class IntType> VecX<N, IndexType> makeKey(IntType i, IntType j) const;
    template<class IntType> VecX<N, IndexType> makeKey(IntType i, IntType j, IntType k) const;

    ////////////////////////////////////////////////////////////////////////////////
    std::unordered_map<VecX<N, IndexType>, IndexType, STLHelpers::VecHash<N, IndexType> > m_idx_XDTo1D;
    IndexType                                                                             m_Size = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class IndexType>
IndexType IndexMapper<N, IndexType >::get1DIdx(const VecX<N, IndexType>& key)
{
#ifdef __BANANA_DEBUG__
    __BNN_REQUIRE(m_idx_XDTo1D.find(key) != m_idx_XDTo1D.end());
#endif

    return m_idx_XDTo1D[key];
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class IndexType>
template<class IntType>
VecX<N, IndexType> IndexMapper<N, IndexType >::makeKey(IntType i, IntType j) const
{
    static_assert(N == 2);
    return VecX<N, IndexType>(static_cast<IndexType>(i), static_cast<IndexType>(j));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class IndexType>
template<class IntType>
VecX<N, IndexType> IndexMapper<N, IndexType >::makeKey(IntType i, IntType j, IntType k) const
{
    static_assert(N == 3);
    return VecX<N, IndexType>(static_cast<IndexType>(i), static_cast<IndexType>(j), static_cast<IndexType>(k));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class IndexType>
void IndexMapper<N, IndexType >::addUniqueKey(const VecX<N, IndexType>& key)
{
#ifdef __BANANA_DEBUG__
    __BNN_REQUIRE(m_idx_XDTo1D.find(key) == m_idx_XDTo1D.end());
#endif

    m_idx_XDTo1D[key]    = m_Size;
    ++m_Size;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class IndexType>
bool IndexMapper<N, IndexType >::addKeyIfNotExist(const VecX<N, IndexType>& key)
{
    if(m_idx_XDTo1D.find(key) == m_idx_XDTo1D.end()) {
        m_idx_XDTo1D[key]    = m_Size;
        ++m_Size;
        return true;
    }
    return false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana