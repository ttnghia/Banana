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
template<class T>
void ParticleSerialization::setFixedAttribute(const String& attrName, T value)
{
    __BNN_ASSERT(m_FixedAttributes.find(attrName) != m_FixedAttributes.end());
    auto& attr = m_FixedAttributes[attrName];
    __BNN_ASSERT(sizeof(T) == attr->typeSize());
    attr->buffer.setData(value);
    attr->bReady = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
void ParticleSerialization::setFixedAttribute(const String& attrName, const Vector<T>& values)
{
    __BNN_ASSERT(m_FixedAttributes.find(attrName) != m_FixedAttributes.end());
    auto& attr = m_FixedAttributes[attrName];
    __BNN_ASSERT(values.size() == static_cast<size_t>(attr->count) && sizeof(T) == attr->typeSize());
    attr->buffer.setData(values, false);
    attr->bReady = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
void ParticleSerialization::setFixedAttribute(const String& attrName, T* values)
{
    __BNN_ASSERT(m_FixedAttributes.find(attrName) != m_FixedAttributes.end());
    auto& attr = m_FixedAttributes[attrName];
    attr->buffer.setData((const unsigned char*)values, attr->count * attr->typeSize());
    attr->bReady = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class T>
void ParticleSerialization::setFixedAttribute(const String& attrName, const VecX<N, T>& value)
{
    __BNN_ASSERT(m_FixedAttributes.find(attrName) != m_FixedAttributes.end());
    auto& attr = m_FixedAttributes[attrName];
    __BNN_ASSERT(N == attr->count && sizeof(T) == attr->typeSize());
    attr->buffer.setData((const unsigned char*)glm::value_ptr(value), sizeof(T) * N);
    attr->bReady = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
void ParticleSerialization::setParticleAttribute(const String& attrName, const Vector<T>& values)
{
    __BNN_ASSERT(m_ParticleAttributes.find(attrName) != m_ParticleAttributes.end() && values.size() == static_cast<size_t>(m_nParticles));
    auto& attr = m_ParticleAttributes[attrName];
    __BNN_ASSERT(values.size() == m_nParticles * attr->count);
    if(attr->type != TypeCompressedReal) {
        __BNN_ASSERT(sizeof(T) == attr->typeSize());
        attr->buffer.setData(values, false);
    }
    else{
        ParticleHelpers::compress(values, attr->buffer, false);
    }
    attr->bReady = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
void ParticleSerialization::setParticleAttribute(const String& attrName, const Vector<Vector<T> >& values)
{
    __BNN_ASSERT(m_ParticleAttributes.find(attrName) != m_ParticleAttributes.end() && values.size() == static_cast<size_t>(m_nParticles));
    auto& attr = m_ParticleAttributes[attrName];
    __BNN_ASSERT(attr->type == TypeVectorInt || attr->type == TypeVectorUInt || attr->type == TypeVectorFloat);

    __BNN_ASSERT(sizeof(T) == attr->typeSize());
    attr->buffer.setData(values, false);
    attr->bReady = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class T>
void ParticleSerialization::setParticleAttribute(const String& attrName, const Vector<VecX<N, T> >& values)
{
    __BNN_ASSERT(m_ParticleAttributes.find(attrName) != m_ParticleAttributes.end() && values.size() == static_cast<size_t>(m_nParticles));
    auto& attr = m_ParticleAttributes[attrName];
    __BNN_ASSERT(N == attr->count);
    if(attr->type != TypeCompressedReal) {
        __BNN_ASSERT(sizeof(T) == attr->typeSize());
        attr->buffer.setData(values, false);
    }
    else{
        ParticleHelpers::compress(values, attr->buffer, false);
    }
    attr->bReady = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
bool ParticleSerialization::getFixedAttribute(const String& attrName, T& value)
{
    if(m_FixedAttributes.find(attrName) == m_FixedAttributes.end())
        return false;
    auto& attr = m_FixedAttributes[attrName];
    __BNN_ASSERT(sizeof(T) == attr->typeSize());
    attr->buffer.getData(value);
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
bool ParticleSerialization::getFixedAttribute(const String& attrName, T* values)
{
    if(m_FixedAttributes.find(attrName) == m_FixedAttributes.end())
        return false;
    auto& attr = m_FixedAttributes[attrName];
    attr->buffer.getData((unsigned char*)values, attr->count * attr->typeSize());
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
bool ParticleSerialization::getFixedAttribute(const String& attrName, Vector<T>& values)
{
    if(m_FixedAttributes.find(attrName) == m_FixedAttributes.end())
        return false;
    auto& attr = m_FixedAttributes[attrName];
    __BNN_ASSERT(sizeof(T) == attr->typeSize());
    values.resize(attr->count);
    attr->buffer.getData(values, static_cast<UInt>(attr->count));
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class T>
bool ParticleSerialization::getFixedAttribute(const String& attrName, VecX<N, T>& value)
{
    if(m_FixedAttributes.find(attrName) == m_FixedAttributes.end())
        return false;
    auto& attr = m_FixedAttributes[attrName];
    __BNN_ASSERT(N == attr->count && sizeof(T) == attr->typeSize());
    attr->buffer.getData((unsigned char*)glm::value_ptr(value), sizeof(T) * N);
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
bool ParticleSerialization::getParticleAttribute(const String& attrName, T* values)
{
    if(m_ParticleAttributes.find(attrName) == m_ParticleAttributes.end())
        return false;
    auto& attr = m_ParticleAttributes[attrName];

    if(attr->type != TypeCompressedReal) {
        attr->buffer.getData((unsigned char*)values, m_nParticles * attr->typeSize() * attr->count);
    }
    else{
        __BNN_ASSERT(sizeof(T) == attr->typeSize());
        if(attr->count == 1) {
            Vector<T> tmp;
            ParticleHelpers::decompress(tmp, attr->buffer, m_nParticles);
            memcpy(values, tmp.data(), attr->typeSize() * tmp.size());
        }
        else{
            if(attr->count == 1) {
                Vector<Vec2<T> > tmp;
                ParticleHelpers::decompress(tmp, attr->buffer, m_nParticles);
                memcpy(values, tmp.data(), attr->typeSize() * tmp.size());
            }
            else{
                Vector<Vec3<T> > tmp;
                ParticleHelpers::decompress(tmp, attr->buffer, m_nParticles);
                memcpy(values, tmp.data(), m_nParticles * attr->typeSize() * attr->count);
            }
        }
    }
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
bool ParticleSerialization::getParticleAttribute(const String& attrName, Vector<T>& values)
{
    if(m_ParticleAttributes.find(attrName) == m_ParticleAttributes.end())
        return false;
    auto& attr = m_ParticleAttributes[attrName];
    __BNN_ASSERT(sizeof(T) == attr->typeSize());

    values.resize(m_nParticles);
    if(attr->type != TypeCompressedReal) {
        attr->buffer.getData(values, 0, m_nParticles);
    }
    else{
        ParticleHelpers::decompress(values, attr->buffer, m_nParticles);
    }
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<class T>
bool ParticleSerialization::getParticleAttribute(const String& attrName, Vector<Vector<T> >& values)
{
    if(m_ParticleAttributes.find(attrName) == m_ParticleAttributes.end())
        return false;
    auto& attr = m_ParticleAttributes[attrName];
    __BNN_ASSERT(sizeof(T) == attr->typeSize());

    values.resize(m_nParticles);
    if(attr->type != TypeCompressedReal) {
        attr->buffer.getData(values, 0, m_nParticles);
    }
    else{
        ParticleHelpers::decompress(values, attr->buffer, m_nParticles);
    }
    return true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class T>
bool ParticleSerialization::getParticleAttribute(const String& attrName, Vector<VecX<N, T> >& values)
{
    if(m_ParticleAttributes.find(attrName) == m_ParticleAttributes.end())
        return false;
    auto& attr = m_ParticleAttributes[attrName];
    __BNN_ASSERT(N == attr->count && sizeof(T) == attr->typeSize());

    values.resize(m_nParticles);
    if(attr->type != TypeCompressedReal) {
        attr->buffer.getData(values, 0, m_nParticles);
    }
    else{
        ParticleHelpers::decompress(values, attr->buffer, m_nParticles);
    }

    return true;
}