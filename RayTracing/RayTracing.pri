INCLUDEPATH += $$PWD/Include

CONFIG += c++14

CONFIG(debug, debug|release) {
    message("Ray Tracing -- Debug")
}else {
    message("Ray Tracing -- Release")
}

macx {
    CUDA_DIR = /Developer/NVIDIA/CUDA-8.0
    CUDA_SDK = /Developer/NVIDIA/CUDA-8.0/samples
    OPTIX_DIR = /Developer/OptiX

    QMAKE_LIBDIR += $$CUDA_DIR/lib
#    QMAKE_LIBDIR += $$CUDA_SDK/common/lib
    LIBS += -loptix
}

win32 {
    CUDA_DIR = "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0"
    CUDA_SDK = "C:\ProgramData\NVIDIA Corporation\CUDA Samples\v8.0"
    OPTIX_DIR = "C:\ProgramData\NVIDIA Corporation\OptiX SDK 4.1.0"

    QMAKE_LIBDIR += $$CUDA_DIR\lib\x64
#    QMAKE_LIBDIR += $$CUDA_SDK\common\lib\x64 # This is covered by Qt
    LIBS += -loptix.1
}

INCLUDEPATH += $$CUDA_DIR/include
INCLUDEPATH += $$CUDA_SDK/common/inc

INCLUDEPATH += $$OPTIX_DIR/include
INCLUDEPATH += $$OPTIX_DIR/SDK

QMAKE_LIBDIR += $$OPTIX_DIR/lib64
LIBS += -lcudart -lcudadevrt

# -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
CUDA_SOURCES += $$files($$_PRO_FILE_PWD_/CUDA/*.cu, true)

# nvcc flags (ptxas option verbose is always useful)
# add the PTX flags to compile optix files
NVCCFLAGS = --ptxas-options=-v -ptx

#set our ptx directory
PTX_DIR = $$_PRO_FILE_PWD_/CUDA/PTX
message(ptx output directory: $$PTX_DIR)

# join the includes in a line
# We need quotations for windows because windows is dumb!
CUDA_INC = $$join(INCLUDEPATH,'" -I"','-I"','"')

optix_compiler.input = CUDA_SOURCES

for(file, $$list($$CUDA_SOURCES)) {
    message(optix input is $$file)
}

optix_compiler.output = $$PTX_DIR/${QMAKE_FILE_BASE}.cu.ptx

# Tweak arch according to your hw's compute capability
# For optix you can only have one architechture when using the PTX flags when using the -ptx flag you dont want to have the -c flag for compiling
optix_compiler.commands = \"$$CUDA_DIR/bin/nvcc\" -ccbin \"C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\" -m64 -gencode arch=compute_52,code=sm_52 \
                          $$NVCCFLAGS $$CUDA_INC \"$$_PRO_FILE_PWD_/${QMAKE_FILE_NAME}\" -o \"$$_PRO_FILE_PWD_/${QMAKE_FILE_OUT}\"

#Declare that we wnat to do this before compiling the C++ code
optix_compiler.CONFIG = target_predeps

#now declare that we dont want to link these files with gcc
optix_compiler.CONFIG += no_link

optix_compiler.dependency_type = TYPE_C

# Tell Qt that we want add our optix compiler
QMAKE_EXTRA_COMPILERS += optix_compiler