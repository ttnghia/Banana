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
template<class T>
void ParticleSerialization::setFixedAttribute(const String& attrName, T value)
{
    __BNN_REQUIRE(m_FixedAttributes.find(attrName) != m_FixedAttributes.end());
    auto& attr = m_FixedAttributes[attrName];
    __BNN_REQUIRE(sizeof(T) == attr->typeSize());
    attr->buffer.setData(value);
    attr->bReady = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
void ParticleSerialization::setFixedAttribute(const String& attrName, const Vector<T>& values)
{
    __BNN_REQUIRE(m_FixedAttributes.find(attrName) != m_FixedAttributes.end());
    auto& attr = m_FixedAttributes[attrName];
    __BNN_REQUIRE(values.size() == static_cast<size_t>(attr->count) && sizeof(T) == attr->typeSize());
    attr->buffer.setData(values, false);
    attr->bReady = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
void ParticleSerialization::setFixedAttribute(const String& attrName, T* values)
{
    __BNN_REQUIRE(m_FixedAttributes.find(attrName) != m_FixedAttributes.end());
    auto& attr = m_FixedAttributes[attrName];
    attr->buffer.setData((const void*)values, attr->count * attr->typeSize());
    attr->bReady = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class T>
void ParticleSerialization::setFixedAttribute(const String& attrName, const VecX<N, T>& value)
{
    __BNN_REQUIRE(m_FixedAttributes.find(attrName) != m_FixedAttributes.end());
    auto& attr = m_FixedAttributes[attrName];
    __BNN_REQUIRE(static_cast<UInt>(N) == attr->count && sizeof(T) == attr->typeSize());
    attr->buffer.setData((const void*)glm::value_ptr(value), sizeof(T) * N);
    attr->bReady = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class T>
void ParticleSerialization::setFixedAttribute(const String& attrName, const MatXxX<N, T>& value)
{
    __BNN_REQUIRE(m_FixedAttributes.find(attrName) != m_FixedAttributes.end());
    auto& attr = m_FixedAttributes[attrName];
    __BNN_REQUIRE(static_cast<UInt>(N * N) == attr->count && sizeof(T) == attr->typeSize());
    attr->buffer.setData((const void*)glm::value_ptr(value), sizeof(T) * N * N);
    attr->bReady = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
void ParticleSerialization::setParticleAttribute(const String& attrName, const Vector<T>& values)
{
    __BNN_REQUIRE(m_ParticleAttributes.find(attrName) != m_ParticleAttributes.end() && values.size() == static_cast<size_t>(m_nParticles));
    auto& attr = m_ParticleAttributes[attrName];
    __BNN_REQUIRE(static_cast<UInt>(values.size()) == m_nParticles * attr->count);
    if(attr->type != TypeCompressedReal) {
        __BNN_REQUIRE(sizeof(T) == attr->typeSize());
        attr->buffer.setData(values, false);
    } else {
        ParticleHelpers::compress(values, attr->buffer, false);
    }
    attr->bReady = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
void ParticleSerialization::setParticleAttribute(const String& attrName, const Vector<Vector<T> >& values)
{
    __BNN_REQUIRE(m_ParticleAttributes.find(attrName) != m_ParticleAttributes.end() && values.size() == static_cast<size_t>(m_nParticles));
    auto& attr = m_ParticleAttributes[attrName];
    __BNN_REQUIRE(attr->type == TypeVectorInt || attr->type == TypeVectorUInt || attr->type == TypeVectorReal);

    __BNN_REQUIRE(sizeof(T) == attr->typeSize());
    attr->buffer.setData(values, false);
    attr->bReady = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class T>
void ParticleSerialization::setParticleAttribute(const String& attrName, const Vector<VecX<N, T> >& values)
{
    __BNN_REQUIRE(m_ParticleAttributes.find(attrName) != m_ParticleAttributes.end() && values.size() == static_cast<size_t>(m_nParticles));
    auto& attr = m_ParticleAttributes[attrName];
    __BNN_REQUIRE(static_cast<UInt>(N) == attr->count);
    if(attr->type != TypeCompressedReal) {
        __BNN_REQUIRE(sizeof(T) == attr->typeSize());
        attr->buffer.setData(values, false);
    } else {
        ParticleHelpers::compress(values, attr->buffer, false);
    }
    attr->bReady = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class T>
void ParticleSerialization::setParticleAttribute(const String& attrName, const Vector<MatXxX<N, T> >& values)
{
    __BNN_REQUIRE(m_ParticleAttributes.find(attrName) != m_ParticleAttributes.end() && values.size() == static_cast<size_t>(m_nParticles));
    auto& attr = m_ParticleAttributes[attrName];
    __BNN_REQUIRE(static_cast<UInt>(N * N) == attr->count);
    if(attr->type != TypeCompressedReal) {
        __BNN_REQUIRE(sizeof(T) == attr->typeSize());
        attr->buffer.setData((const void*)values.data(), values.size() * sizeof(T) * N * N);
    } else {
        ParticleHelpers::compress(values, attr->buffer, false);
    }
    attr->bReady = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
bool ParticleSerialization::getFixedAttribute(const String& attrName, T& value)
{
    if(m_FixedAttributes.find(attrName) == m_FixedAttributes.end()) {
        return false;
    }
    auto& attr = m_FixedAttributes[attrName];
    __BNN_REQUIRE(sizeof(T) == attr->typeSize());
    attr->buffer.getData(value);
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
bool ParticleSerialization::getFixedAttribute(const String& attrName, T* values)
{
    if(m_FixedAttributes.find(attrName) == m_FixedAttributes.end()) {
        return false;
    }
    auto& attr = m_FixedAttributes[attrName];
    attr->buffer.getData((void*)values, attr->count * attr->typeSize());
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
bool ParticleSerialization::getFixedAttribute(const String& attrName, Vector<T>& values)
{
    if(m_FixedAttributes.find(attrName) == m_FixedAttributes.end()) {
        return false;
    }
    auto& attr = m_FixedAttributes[attrName];
    __BNN_REQUIRE(sizeof(T) == attr->typeSize());
    values.resize(attr->count);
    attr->buffer.getData(values, 0, attr->count);
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class T>
bool ParticleSerialization::getFixedAttribute(const String& attrName, VecX<N, T>& value)
{
    if(m_FixedAttributes.find(attrName) == m_FixedAttributes.end()) {
        return false;
    }
    auto& attr = m_FixedAttributes[attrName];
    __BNN_REQUIRE(N == attr->count && sizeof(T) == attr->typeSize());
    attr->buffer.getData((void*)glm::value_ptr(value), sizeof(T) * N);
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class T>
bool ParticleSerialization::getFixedAttribute(const String& attrName, MatXxX<N, T>& value)
{
    if(m_FixedAttributes.find(attrName) == m_FixedAttributes.end()) {
        return false;
    }
    auto& attr = m_FixedAttributes[attrName];
    __BNN_REQUIRE(N * N == attr->count && sizeof(T) == attr->typeSize());
    attr->buffer.getData((void*)glm::value_ptr(value), sizeof(T) * N * N);
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
bool ParticleSerialization::getParticleAttribute(const String& attrName, T* values)
{
    if(m_ParticleAttributes.find(attrName) == m_ParticleAttributes.end()) {
        return false;
    }
    auto& attr = m_ParticleAttributes[attrName];

    if(attr->type != TypeCompressedReal) {
        attr->buffer.getData((void*)values, m_nParticles * attr->typeSize() * attr->count);
    } else {
        __BNN_REQUIRE(sizeof(T) == attr->typeSize());
        if(attr->count == 1) {
            Vector<T> tmp;
            ParticleHelpers::decompress(tmp, attr->buffer, m_nParticles);
            memcpy(values, tmp.data(), attr->typeSize() * tmp.size());
        } else {
            if(attr->count == 2) {
                Vector<Vec2<T> > tmp;
                ParticleHelpers::decompress(tmp, attr->buffer, m_nParticles);
                memcpy(values, tmp.data(), attr->typeSize() * tmp.size());
            } else {
                if(attr->count == 3) {
                    Vector<Vec3<T> > tmp;
                    ParticleHelpers::decompress(tmp, attr->buffer, m_nParticles);
                    memcpy(values, tmp.data(), attr->typeSize() * tmp.size());
                } else {
                    Vector<Vec4<T> > tmp;
                    ParticleHelpers::decompress(tmp, attr->buffer, m_nParticles);
                    memcpy(values, tmp.data(), m_nParticles * attr->typeSize() * attr->count);
                }
            }
        }
    }
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
bool ParticleSerialization::getParticleAttribute(const String& attrName, Vector<T>& values)
{
    if(m_ParticleAttributes.find(attrName) == m_ParticleAttributes.end()) {
        return false;
    }
    auto& attr = m_ParticleAttributes[attrName];
    __BNN_REQUIRE(sizeof(T) == attr->typeSize());

    values.resize(m_nParticles);
    if(attr->type != TypeCompressedReal) {
        attr->buffer.getData(values, 0, m_nParticles);
    } else {
        ParticleHelpers::decompress(values, attr->buffer, m_nParticles);
    }
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
bool ParticleSerialization::getParticleAttribute(const String& attrName, Vector<Vector<T> >& values)
{
    if(m_ParticleAttributes.find(attrName) == m_ParticleAttributes.end()) {
        return false;
    }
    auto& attr = m_ParticleAttributes[attrName];
    __BNN_REQUIRE(sizeof(T) == attr->typeSize());

    values.resize(m_nParticles);
    if(attr->type != TypeCompressedReal) {
        attr->buffer.getData(values, 0, m_nParticles);
    } else {
        ParticleHelpers::decompress(values, attr->buffer, m_nParticles);
    }
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class T>
bool ParticleSerialization::getParticleAttribute(const String& attrName, Vector<VecX<N, T> >& values)
{
    if(m_ParticleAttributes.find(attrName) == m_ParticleAttributes.end()) {
        return false;
    }
    auto& attr = m_ParticleAttributes[attrName];
    __BNN_REQUIRE(N == attr->count && sizeof(T) == attr->typeSize());

    values.resize(m_nParticles);
    if(attr->type != TypeCompressedReal) {
        attr->buffer.getData(values, 0, m_nParticles);
    } else {
        ParticleHelpers::decompress(values, attr->buffer, m_nParticles);
    }

    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class T>
bool ParticleSerialization::getParticleAttribute(const String& attrName, Vector<MatXxX<N, T> >& values)
{
    if(m_ParticleAttributes.find(attrName) == m_ParticleAttributes.end()) {
        return false;
    }
    auto& attr = m_ParticleAttributes[attrName];
    __BNN_REQUIRE(N * N == attr->count && sizeof(T) == attr->typeSize());

    values.resize(m_nParticles);
    if(attr->type != TypeCompressedReal) {
        attr->buffer.getData((void*)values.data(), m_nParticles * sizeof(T) * N * N);
    } else {
        ParticleHelpers::decompress(values, attr->buffer, m_nParticles);
    }

    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
bool ParticleSerialization::getParticleAttributeCompressed(const String& attrName, Vec_UInt16& values, T& dMin, T& dMax)
{
    if(m_ParticleAttributes.find(attrName) == m_ParticleAttributes.end()) {
        return false;
    }
    auto& attr = m_ParticleAttributes[attrName];
    __BNN_REQUIRE(sizeof(T) == attr->typeSize());
    __BNN_REQUIRE(attr->type == TypeCompressedReal);
    values.resize(m_nParticles);

    ////////////////////////////////////////////////////////////////////////////////
    float  dMinf, dMaxf;
    UInt64 segmentStart = 0;
    UInt64 segmentSize;

    segmentSize = sizeof(float);
    memcpy(&dMinf, &attr->buffer.data()[segmentStart], segmentSize);
    segmentStart += segmentSize;
    memcpy(&dMaxf, &attr->buffer.data()[segmentStart], segmentSize);
    segmentStart += segmentSize;

    segmentSize = m_nParticles * sizeof(UInt16) * attr->count;
    __BNN_REQUIRE(segmentStart + segmentSize == attr->buffer.size());
    memcpy(values.data(), &attr->buffer.data()[segmentStart], segmentSize);

    dMin = static_cast<T>(dMinf);
    dMax = static_cast<T>(dMaxf);
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class T>
bool ParticleSerialization::getParticleAttributeCompressed(const String& attrName, Vec_UInt16& values, VecX<N, T>& dMin, VecX<N, T>& dMax)
{
    if(m_ParticleAttributes.find(attrName) == m_ParticleAttributes.end()) {
        return false;
    }
    auto& attr = m_ParticleAttributes[attrName];
    __BNN_REQUIRE(N == attr->count && sizeof(T) == attr->typeSize());
    __BNN_REQUIRE(attr->type == TypeCompressedReal);
    values.resize(m_nParticles * N);

    ////////////////////////////////////////////////////////////////////////////////
    VecX<N, float> dMinf, dMaxf;
    UInt64         segmentStart = 0;
    UInt64         segmentSize  = sizeof(float) * N;

    memcpy(&dMinf, &attr->buffer.data()[segmentStart], segmentSize);
    segmentStart += segmentSize;
    memcpy(&dMaxf, &attr->buffer.data()[segmentStart], segmentSize);
    segmentStart += segmentSize;

    segmentSize = m_nParticles * sizeof(UInt16) * N;
    __BNN_REQUIRE(segmentStart + segmentSize == attr->buffer.size());
    memcpy(values.data(), &attr->buffer.data()[segmentStart], segmentSize);

    for(Int i = 0; i < N; ++i) {
        dMin[i] = static_cast<T>(dMinf[i]);
        dMax[i] = static_cast<T>(dMaxf[i]);
    }
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
bool ParticleSerialization::getParticleAttributeCompressed(const String& attrName, Vec_VecUInt16& values, Vector<T>& dMin, Vector<T>& dMax)
{
    if(m_ParticleAttributes.find(attrName) == m_ParticleAttributes.end()) {
        return false;
    }
    auto& attr = m_ParticleAttributes[attrName];
    __BNN_REQUIRE(sizeof(T) == attr->typeSize());
    __BNN_REQUIRE(attr->type == TypeCompressedReal);
    values.resize(m_nParticles);
    dMin.resize(m_nParticles);
    dMax.resize(m_nParticles);

    ////////////////////////////////////////////////////////////////////////////////
    float  dMinf;
    float  dMaxf;
    UInt64 segmentStart = 0;
    UInt64 segmentSize;

    for(UInt i = 0; i < m_nParticles; ++i) {
        UInt iSize;
        segmentSize = sizeof(UInt);
        memcpy(&iSize, &attr->buffer.data()[segmentStart], segmentSize);
        segmentStart += segmentSize;

        segmentSize = sizeof(float);
        memcpy(&dMinf, &attr->buffer.data()[segmentStart], segmentSize);
        segmentStart += segmentSize;
        memcpy(&dMaxf, &attr->buffer.data()[segmentStart], segmentSize);
        segmentStart += segmentSize;

        dMin[i] = static_cast<T>(dMinf);
        dMax[i] = static_cast<T>(dMaxf);

        values[i].resize(iSize);
        segmentSize = iSize * sizeof(UInt16);
        __BNN_REQUIRE(segmentStart + segmentSize <= attr->buffer.size());
        memcpy(values[i].data(), &attr->buffer.data()[segmentStart], segmentSize);
    }

    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class T>
void Banana::ParticleSerialization::saveParticle(const String& fileName, const Vector<VecX<N, T> >& positions, T particleRadius, bool bCompress /*= true*/)
{
    ParticleSerialization particleWriter;
    particleWriter.addFixedAttribute<Real>("particle_radius", ParticleSerialization::TypeReal, 1);
    if(bCompress) {
        particleWriter.addParticleAttribute<Real>("position", ParticleSerialization::TypeCompressedReal, 3);
    } else {
        particleWriter.addParticleAttribute<Real>("position", ParticleSerialization::TypeReal, 3);
    }
    particleWriter.setNParticles(positions.size());
    particleWriter.setFixedAttribute("particle_radius", particleRadius);
    particleWriter.setParticleAttribute("position", positions);
    particleWriter.flushAsync(fileName);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class T>
bool Banana::ParticleSerialization::loadParticle(const String& fileName, Vector<VecX<N, T> >& positions, T particleRadius)
{
    ParticleSerialization particleReader;
    if(!particleReader.read(fileName)) {
        return false;
    }

    Real tmpRadius;
    __BNN_REQUIRE(particleReader.getFixedAttribute("particle_radius", tmpRadius));
    __BNN_REQUIRE_APPROX_NUMBERS(tmpRadius, particleRadius, MEpsilon);
    __BNN_REQUIRE(particleReader.getParticleAttribute("position", positions));

    return true;
}
