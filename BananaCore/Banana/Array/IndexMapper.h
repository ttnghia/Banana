//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \    This file is part of Banana - a graphics programming framework    /
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
    void      clear() { m_idx_1DToXD.clear(); m_idx_XDTo1D.clear(); m_Size = 0; }

    ////////////////////////////////////////////////////////////////////////////////
    template<class IntType> void      addKey(const Vec2<IntType>& key) { addKey(key[0], key[1]); }
    template<class IntType> void      addKey(const Vec3<IntType>& key) { addKey(key[0], key[1], key[2]); }
    template<class IntType> IndexType get1DIdx(const Vec2<IntType>& key) { return get1DIdx(key[0], key[1]); }
    template<class IntType> IndexType get1DIdx(const Vec3<IntType>& key) { return get1DIdx(key[0], key[1], key[2]); }

    template<class IntType> void               addKey(IntType i, IntType j);
    template<class IntType> void               addKey(IntType i, IntType j, IntType k);
    template<class IntType> IndexType          get1DIdx(IntType i, IntType j) const;
    template<class IntType> IndexType          get1DIdx(IntType i, IntType j, IntType k) const;
    template<class IntType> VecX<N, IndexType> getXDIdx(IntType idx) const;

private:
    template<class IntType> VecX<N, IndexType> makeKey(IntType i, IntType j);
    template<class IntType> VecX<N, IndexType> makeKey(IntType i, IntType j, IntType k);

    ////////////////////////////////////////////////////////////////////////////////
    std::unordered_map<VecX<N, IndexType>, IndexType, STLHelpers::VecHash<N, IndexType> > m_idx_XDTo1D;
    std::unordered_map<IndexType, VecX<N, IndexType>, STLHelpers::VecHash<N, IndexType> > m_idx_1DToXD;
    IndexType                                                                             m_Size = 0;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Implementation
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class IntType>
template<Int N, class IndexType>
void IndexMapper<N, IndexType >::addKey(IntType i, IntType j)
{
    static_assert(N == 2);
    auto = makeKey(i, j);
#ifdef __BANANA_DEBUG__
    __BNN_ASSERT(m_idx_XDTo1D.find(key) == m_idx_XDTo1D.end());
#endif

    m_idx_XDTo1D[key]    = m_Size;
    m_idx_1DToXD[m_Size] = key;
    ////////////////////////////////////////////////////////////////////////////////
    ++m_Size;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class IntType>
template<Int N, class IndexType>
void IndexMapper<N, IndexType >::addKey(IntType i, IntType j, IntType k)
{
    static_assert(N == 3);
    auto key = makeKey(i, j, k);
#ifdef __BANANA_DEBUG__
    __BNN_ASSERT(m_idx_XDTo1D.find(key) == m_idx_XDTo1D.end());
#endif

    m_idx_XDTo1D[key]    = m_Size;
    m_idx_1DToXD[m_Size] = key;
    ////////////////////////////////////////////////////////////////////////////////
    ++m_Size;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class IntType>
template<Int N, class IndexType>
IndexType IndexMapper<N, IndexType >::get1DIdx(IntType i, IntType j) const
{
    static_assert(N == 2);
    auto key = makeKey(i, j);
 #ifdef __BANANA_DEBUG__
    __BNN_ASSERT(m_idx_XDTo1D.find(key) != m_idx_XDTo1D.end());
#endif

    return m_idx_XDTo1D[key];
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class IntType>
template<Int N, class IndexType>
IndexType IndexMapper<N, IndexType >::get1DIdx(IntType i, IntType j, IntType k) const
{
    static_assert(N == 3);
    auto key = makeKey(i, j, k);
 #ifdef __BANANA_DEBUG__
    __BNN_ASSERT(m_idx_XDTo1D.find(key) != m_idx_XDTo1D.end());
#endif

    return m_idx_XDTo1D[key];
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class IntType>
template<Int N, class IndexType>
VecX<N, IndexType> IndexMapper<N, IndexType >::getXDIdx(IntType idx) const
{
#ifdef __BANANA_DEBUG__
    __BNN_ASSERT(m_idx_1DToXD.find(idx) != m_idx_1DToXD.end());
#endif

    return m_idx_1DToXD[idx];
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class IntType>
template<Int N, class IndexType>
VecX<N, IndexType> IndexMapper<N, IndexType >::makeKey(IntType i, IntType j)
{
    static_assert(N == 2);
    return VecX<N, IndexType>(static_cast<IndexType>(i), static_cast<IndexType>(j));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class IntType>
template<Int N, class IndexType>
VecX<N, IndexType> IndexMapper<N, IndexType >::makeKey(IntType i, IntType j, IntType k)
{
    static_assert(N == 3);
    return VecX<N, IndexType>(static_cast<IndexType>(i), static_cast<IndexType>(j), static_cast<IndexType>(k));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace Banana