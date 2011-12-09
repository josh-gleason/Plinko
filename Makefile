# Semi-Generic Makefile by Joshua Gleason
default: all

####################### DEFINE LOCAL DIRECTORIES ###############################

# directory locations
BUILD_DIR     = build
SRC_DIR       = src
BINARY_DIR    = bin

CORE          = core
OPENGL        = opengl
STRUCTURES    = structures
MATH          = math
MODELLOADER   = model_loader

# File extensions should use a uniform format throught the project

# Valid C++ source extensions are cpp/cc/cxx/c++/C
CPP_SRC_EXT   = cpp
# Valid C++ header extentions are h/hpp/hh/hxx/h++/H
CPP_HDR_EXT   = h

##################### ADD FILES TO THESE VARIABLES #############################

# Names of files which are HEADER ONLY (don't include file extension in name)
HEADER_FILES  = $(MATH)/mat \
                $(MATH)/vec \
                $(OPENGL)/GL

# Names of files that need compiled (assumes each has a HEADER AND SOURCE, don't
# include file extension in name)
OBJECT_NAMES  = $(CORE)/parser \
                $(STRUCTURES)/Parameters \
                $(STRUCTURES)/Camera \
                $(STRUCTURES)/Light \
                $(STRUCTURES)/Object \
                $(STRUCTURES)/Physics \
                $(STRUCTURES)/Scoreboard \
                $(STRUCTURES)/HUD \
                $(STRUCTURES)/Winner \
                $(MATH)/triangulation \
                $(OPENGL)/display \
                $(OPENGL)/keyboard \
                $(OPENGL)/mouse \
                $(OPENGL)/shader_processing \
                $(OPENGL)/timer \
                $(OPENGL)/reshape \
                $(OPENGL)/menu \
                $(MODELLOADER)/list \
                $(MODELLOADER)/string_extra \
                $(MODELLOADER)/obj_parser \
                $(MODELLOADER)/print_data \
                $(MODELLOADER)/objLoader

######################### SETTINGS VARIABLES ###################################

EXE_NAME         = main

# tells makefile to have -I flags to all local header directories (TRUE/FALSE)
ADD_HEADERS      = TRUE

# OS Specific settings
ifeq ($(shell uname),Linux)
  BOOST_LIBS     = -lboost_system -lboost_filesystem -lboost_program_options
  BOOST_INCLUDE  =
  OPENGL_LIBS    = -lglut -lGLU -lGL -lGLEW
  OPENGL_INCLUDE =
  BULLET_INCLUDE = `pkg-config bullet --cflags`
  BULLET_LIBS    = -L/home/ee/gleason/local/lib `pkg-config bullet --libs`
  ASSIMP_INCLUDE = `pkg-config assimp --cflags`
  ASSIMP_LIBS    = -L/home/ee/gleason/local/lib `pkg-config assimp --libs`
else  # Apple
  BOOST_LIBS     = -L/opt/local/lib -lboost_system -lboost_filesystem \
                   -lboost_program_options
  BOOST_INCLUDE  = -I/opt/local/include
  OPENGL_LIBS    = -framework OpenGL -framework GLUT
  OPENGL_INCLUDE =
  BULLET_INCLUDE = -I/opt/local/include/bullet
  BULLET_LIBS    = -L/opt/local/lib -lLinearMath -lBulletCollision -lBulletDynamics
  ASSIMP_INCLUDE = -I/opt/local/include/assimp
  ASSIMP_LIBS    = -L/opt/local/lib -lassimp
endif

LIBS          = $(OPENGL_LIBS) $(BOOST_LIBS) $(BULLET_LIBS) $(ASSIMP_LIBS)
XTRA_INCLUDE  = $(OPENGL_INCLUDE) $(BOOST_INCLUDE) $(BULLET_INCLUDE) \
                $(ASSIMP_INCLUDE)
FLAGS         = -g -Wall

CC            = g++

######################### THIRD PARTY LIBRARIES ################################

# builds a binary for compiling shaders (just for testing)
SHD_NM = data/shaders/compile_test/compileshader

# names of extra commands to be executed
ALL_EXTRA    = ${SHD_NM} 
PHONY_EXTRA  =

## add any third party build commands here

# build the shader compiler and put it in it's source directory
${SHD_NM}:${SHD_NM}.${CPP_SRC_EXT} Makefile
	${CC} -o ${SHD_NM} ${SHD_NM}.${CPP_SRC_EXT} ${OPENGL_LIBS} ${FLAGS}

########### DO NOT MODIFY BELOW THIS POINT(unless you want to) #################

all: ${ALL_EXTRA} ${EXE_NAME}

${EXE_NAME}: $(BINARY_DIR)/${EXE_NAME}

clean:
	rm -rf build

HEADERS = ${patsubst %,%.${CPP_HDR_EXT},${HEADER_FILES}}
HEADERS += ${patsubst %,%.${CPP_HDR_EXT},${OBJECT_NAMES}}
OBJECT_FILES = ${OBJECT_NAMES} ${EXE_NAME}
OBJECTS = ${patsubst %,${BUILD_DIR}/%.o,${OBJECT_FILES}}

ifneq ($(ADD_HEADERS),TRUE)
  INCLUDE_DIRS = ${XTRA_INCLUDE}
else
  INCLUDE_DIRS = ${sort ${patsubst %,-I${SRC_DIR}/%,${dir ${HEADERS}}} \
                 ${patsubst %,-I${SRC_DIR}/%,${dir ${OBJECT_NAMES}}}} \
                 ${XTRA_INCLUDE}
endif

########################### BUILD EXECUTABLE ###################################

${BINARY_DIR}/${EXE_NAME}: ${OBJECTS} ${patsubst %,${SRC_DIR}/%,${HEADERS}} \
    Makefile
	@mkdir -p ${BINARY_DIR}
	${CC} -o $@ ${OBJECTS} ${FLAGS} ${INCLUDE_DIRS} ${LIBS}

######################## BUILD THE OBJECT FILES ################################

${OBJECTS}:${BUILD_DIR}/%.o:${SRC_DIR}/%.${CPP_SRC_EXT} \
    ${patsubst %,${SRC_DIR}/%,${HEADERS}} Makefile
	@mkdir -p ${dir $@}
	${CC} -c $< -o $@ ${FLAGS} ${INCLUDE_DIRS}

.PHONY:all ${EXE_NAME} clean doxygen ${PHONY_EXTRA}

