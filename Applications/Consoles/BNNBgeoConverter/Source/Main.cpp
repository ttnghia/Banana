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
#include <Banana/Utils/FileHelpers.h>
#include <ParticleTools/ParticleHelpers.h>
#include <Partio.h>
#include <iostream>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void convert2BNN(const char* inputFile, const char* outputFile)
{
    ////////////////////////////////////////////////////////////////////////////////
    // read bgeo
    Partio::ParticlesData* particleData = Partio::read(inputFile);
    __BNN_ASSERT_MSG(particleData, "Could not open file for reading.");

    Partio::ParticleAttribute attrPosision, attrRadius;
    __BNN_ASSERT(particleData->attributeInfo("position", attrPosision));
    __BNN_ASSERT(particleData->attributeInfo("pradius", attrRadius));
    float particleRadius = particleData->data<float>(attrRadius, 0)[0];

    Vec_Vec3f positions(particleData->numParticles());
    for(int i = 0; i < particleData->numParticles(); ++i) {
        const float* ppos = particleData->data<float>(attrPosision, i);
        memcpy(glm::value_ptr(positions[i]), ppos, sizeof(Vec3f));
    }
    particleData->release();

    ////////////////////////////////////////////////////////////////////////////////
    // write Banana compressed data
    Banana::ParticleHelpers::compressAndSaveBinary(String(outputFile), positions, particleRadius);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void convert2Bgeo(const char* inputFile, const char* outputFile)
{
    ////////////////////////////////////////////////////////////////////////////////
    // read Banana compressed data
    Vec_Vec3f positions;
    float     particleRadius;
    UInt      nParticles = Banana::ParticleHelpers::loadBinaryAndDecompress(String(inputFile), positions, particleRadius);

    ////////////////////////////////////////////////////////////////////////////////
    // write bgeo data
    Partio::ParticlesDataMutable* particleData = Partio::create();
    Partio::ParticleAttribute     attrPosision = particleData->addAttribute("position", Partio::VECTOR, 3);
    Partio::ParticleAttribute     attrRadius   = particleData->addAttribute("pradius", Partio::FLOAT, 1);

    particleData->addParticles(static_cast<Int>(nParticles));

    for(UInt i = 0; i < nParticles; ++i) {
        float* pos    = particleData->dataWrite<float>(attrPosision, i);
        float* radius = particleData->dataWrite<float>(attrRadius, i);

        memcpy(pos, glm::value_ptr(positions[i]), sizeof(Vec3f));
        radius[0] = particleRadius;
    }

    Partio::write(outputFile, *particleData);
    particleData->release();
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
