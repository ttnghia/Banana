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

#include <Banana/Setup.h>
#include <Banana/Utils/NumberHelpers.h>
#include <ParticleTools/ParticleSerialization.h>
#include <Partio.h>
#include <iostream>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void convert2BNN(const char* inputFile, const char* outputFile)
{
    ////////////////////////////////////////////////////////////////////////////////
    // read bgeo
    Partio::ParticlesData* partioData = Partio::read(inputFile);
    __BNN_ASSERT_MSG(partioData, "Could not open file for reading.");
    const int nParticles = partioData->numParticles();

#if defined(_DEBUG) || defined(DEBUG)
    printf("Num. read particles: %s\n", Banana::NumberHelpers::formatWithCommas(nParticles).c_str());
#endif

    Banana::ParticleSerialization particleWriter;
    particleWriter.setNParticles(nParticles);

    for(int i = 0, iend = partioData->numFixedAttributes(); i < iend; ++i) {
        Partio::FixedAttribute fattr;
        partioData->fixedAttributeInfo(i, fattr);
#if defined(_DEBUG) || defined(DEBUG)
        printf("Read fixed attr: %s, count = %d\n", fattr.name.c_str(), fattr.count);
#endif

        if(fattr.type == Partio::INT) {
            particleWriter.addFixedAtribute(fattr.name, Banana::ParticleSerialization::TypeInt, Banana::ParticleSerialization::Size32b, fattr.count);
            const int* idata = partioData->fixedData<int>(fattr);
            particleWriter.setFixedAttribute(fattr.name, idata);
        }
        else if(fattr.type == Partio::FLOAT) {
            particleWriter.addFixedAtribute(fattr.name, Banana::ParticleSerialization::TypeReal, Banana::ParticleSerialization::Size32b, fattr.count);
            const float* fdata = partioData->fixedData<float>(fattr);
            particleWriter.setFixedAttribute(fattr.name, fdata);
        }
    }

    for(int i = 0, iend = partioData->numAttributes(); i < iend; ++i) {
        Partio::ParticleAttribute pattr;
        partioData->attributeInfo(i, pattr);
#if defined(_DEBUG) || defined(DEBUG)
        printf("Read particle attr: %s, count = %d\n", pattr.name.c_str(), pattr.count);
#endif

        if(pattr.type == Partio::INT) {
            particleWriter.addParticleAtribute(pattr.name, Banana::ParticleSerialization::TypeInt, Banana::ParticleSerialization::Size32b, pattr.count);
            Vector<int> idata(nParticles* pattr.count);
            size_t      elementSize = sizeof(int) * pattr.count;

            for(int j = 0; j < nParticles; ++j) {
                const void* src = partioData->data<void>(pattr, j);
                memcpy(&idata.data()[pattr.count * j], src, elementSize);
            }
            particleWriter.setParticleAttribute(pattr.name, idata);
        }
        else if(pattr.type == Partio::FLOAT || pattr.type == Partio::VECTOR) {
            particleWriter.addParticleAtribute(pattr.name, Banana::ParticleSerialization::TypeCompressedReal, Banana::ParticleSerialization::Size32b, pattr.count);
            if(pattr.count == 1) {
                Vector<float> fdata(nParticles);
                for(int j = 0; j < nParticles; ++j) {
                    const float* src = partioData->data<float>(pattr, j);
                    fdata[j] = src[0];
                }
                particleWriter.setParticleAttribute(pattr.name, fdata);
            }
            else if(pattr.count == 2) {
                Vector<Vec2f> fdata(nParticles);
                for(int j = 0; j < nParticles; ++j) {
                    const float* src = partioData->data<float>(pattr, j);
                    fdata[j] = Vec2f(src[0], src[1]);
                }
                particleWriter.setParticleAttribute(pattr.name, fdata);
            }
            else if(pattr.count == 3) {
                Vector<Vec3f> fdata(nParticles);
                for(int j = 0; j < nParticles; ++j) {
                    const float* src = partioData->data<float>(pattr, j);
                    fdata[j] = Vec3f(src[0], src[1], src[2]);
                }
                particleWriter.setParticleAttribute(pattr.name, fdata);
            }
        }
    }


    ////////////////////////////////////////////////////////////////////////////////
    // write Banana compressed data
    partioData->release();
    particleWriter.flush(outputFile);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void convert2Bgeo(const char* inputFile, const char* outputFile)
{
    ////////////////////////////////////////////////////////////////////////////////
    // read Banana compressed data
    Banana::ParticleSerialization particleReader;
    if(!particleReader.read(inputFile))
        return;
#if defined(_DEBUG) || defined(DEBUG)
    printf("Num. read particles: %s\n", Banana::NumberHelpers::formatWithCommas(particleReader.getNParticles()).c_str());
#endif
    Partio::ParticlesDataMutable* partioData = Partio::create();
    partioData->addParticles(static_cast<Int>(particleReader.getNParticles()));

    for(auto& kv : particleReader.getFixedAttributes()) {
#if defined(_DEBUG) || defined(DEBUG)
        printf("Read fixed attr: %s, count = %d\n", kv.first.c_str(), kv.second->count);
#endif
        Partio::FixedAttribute fattr;
        switch(kv.second->type) {
            case Banana::ParticleSerialization::TypeInt:
            {
                fattr = partioData->addFixedAttribute(kv.first.c_str(), Partio::INT, kv.second->count);
                int* idata = partioData->fixedDataWrite<int>(fattr);
                particleReader.getFixedAttribute(kv.first, idata);
            }
            break;

            case Banana::ParticleSerialization::TypeUInt:
            {
                fattr = partioData->addFixedAttribute(kv.first.c_str(), Partio::INT, kv.second->count);
                UInt* uidata = new UInt[kv.second->count];
                particleReader.getFixedAttribute(kv.first, uidata);

                int* idata = partioData->fixedDataWrite<int>(fattr);
                for(Int i = 0; i < kv.second->count; ++i)
                    idata[i] = static_cast<int>(uidata[i]);
            }
            break;

            case Banana::ParticleSerialization::TypeReal:
            {
                fattr = partioData->addFixedAttribute(kv.first.c_str(), Partio::FLOAT, kv.second->count);
                float* fdata = partioData->fixedDataWrite<float>(fattr);
                if(sizeof(kv.second->typeSize() == sizeof(float))) {
                    particleReader.getFixedAttribute(kv.first, fdata);
                }
                else{
                    double* ddata = new double[kv.second->count];
                    particleReader.getFixedAttribute(kv.first, ddata);
                    for(Int i = 0; i < kv.second->count; ++i)
                        fdata[i] = static_cast<float>(ddata[i]);
                }
            }
            break;

            default:
                __BNN_DIE_UNKNOWN_ERROR
        }
    }

    for(auto& kv : particleReader.getParticleAttributes()) {
#if defined(_DEBUG) || defined(DEBUG)
        printf("Read particle attr: %s, count = %d\n", kv.first.c_str(), kv.second->count);
#endif
        Partio::ParticleAttribute pattr;
        switch(kv.second->type) {
            case Banana::ParticleSerialization::TypeInt:
            {
                pattr = partioData->addAttribute(kv.first.c_str(), Partio::INT, kv.second->count);
                Vec_Int idata;
                particleReader.getParticleAttribute(kv.first, idata);
                for(UInt i = 0; i < particleReader.getNParticles(); ++i) {
                    int* pdata = partioData->dataWrite<int>(pattr, i);
                    pdata[0] = idata[i];
                }
            }
            break;

            case Banana::ParticleSerialization::TypeUInt:
            {
                pattr = partioData->addAttribute(kv.first.c_str(), Partio::INT, kv.second->count);
                Vec_UInt uidata;
                particleReader.getParticleAttribute(kv.first, uidata);
                for(UInt i = 0; i < particleReader.getNParticles(); ++i) {
                    int* pdata = partioData->dataWrite<int>(pattr, i);
                    pdata[0] = static_cast<int>(uidata[i]);
                }
            }
            break;

            case Banana::ParticleSerialization::TypeReal:
            case Banana::ParticleSerialization::TypeCompressedReal:
            {
                pattr = partioData->addAttribute(kv.first.c_str(), Partio::FLOAT, kv.second->count);
                if(sizeof(kv.second->typeSize() == sizeof(float))) {
                    Vec_Float fdata(particleReader.getNParticles() * kv.second->count);
                    particleReader.getParticleAttribute(kv.first, fdata.data());
                    for(UInt i = 0; i < particleReader.getNParticles(); ++i) {
                        float* pdata = partioData->dataWrite<float>(pattr, i);
                        for(Int j = 0; j < kv.second->count; ++j)
                            pdata[j] = fdata[i * kv.second->count + j];
                    }
                }
                else{
                    Vec_Double ddata(particleReader.getNParticles() * kv.second->count);
                    particleReader.getParticleAttribute(kv.first, ddata.data());
                    for(UInt i = 0; i < particleReader.getNParticles(); ++i) {
                        float* pdata = partioData->dataWrite<float>(pattr, i);
                        for(Int j = 0; j < kv.second->count; ++j)
                            pdata[j] = static_cast<float>(ddata[i * kv.second->count + j]);
                    }
                }
            }
            break;

            // Omit type vector
            default:
#if defined(_DEBUG) || defined(DEBUG)
                printf("....omit this attribute...\n");
#endif
                ;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // write bgeo data
    Partio::write(outputFile, *partioData);
    partioData->release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
static void printUsage(const char* programName)
{
    std::cerr << std::endl << "Usage: " << programName << " sourceFile dstFile" << std::endl;
    std::cerr << "The extension of the source/dest will be used to determine how the conversion is done." << std::endl;
    std::cerr << "Supported extensions are .cps and .bgeo" << std::endl << std::endl;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
int main(int argc, char* argv[])
{
    if(argc != 3) {
        printUsage(argv[0]);
        exit(EXIT_FAILURE);
    }

    if(Banana::FileHelpers::getExtension(String(argv[1])) == String("bgeo"))
        convert2BNN(argv[1], argv[2]);
    else
        convert2Bgeo(argv[1], argv[2]);

    return EXIT_SUCCESS;
}
