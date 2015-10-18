CC  = gcc
CXX = g++



SRC = ./src
INC = ./include

OBJ  = tomoVA_model.o Eqsta.o VelModel.o tx.o
EXEC = 	getEq4Depth getEq4Profile \
		Profile Section \
		SelectEqCircle SelectEqCircle_ISC \
		pickDat pickDat4dt pickSta pickDat4Order \
		GenerateGMT GenerateGMT4checkerboard \
		dtTest \
		LiaolingTransform \
		gndk \
		getAverageModel1D getModel1D\
		getRaypath41D

CFLAGS = -Wall -Ofast -lm -I${INC}
CXXFLAGS = -Wall -Ofast -lm -I${INC}


VPATH=${SRC}:${INC}

all: ${EXEC}

${OBJ}: tomoVA_model.h Eqsta.h VelModel.h tx.h

################################################################################
getEq4Depth: getEq4Depth.c
	${CC} -o getEq4Depth ${SRC}/getEq4Depth.c ${CFLAGS}

getEq4Profile: getEq4Profile.c
	${CC} -o getEq4Profile ${SRC}/getEq4Profile.c ${CFLAGS}

################################################################################
Profile: Profile.c tomoVA_model.h tomoVA_model.o
	${CC} -o Profile ${SRC}/Profile.c tomoVA_model.o ${CFLAGS}

Section: Section.c tomoVA_model.h tomoVA_model.o
	${CC} -o Section ${SRC}/Section.c tomoVA_model.o ${CFLAGS}

################################################################################
SelectEqCircle: SelectEqCircle.c
	${CC} -o SelectEqCircle ${SRC}/SelectEqCircle.c ${CFLAGS}

SelectEqCircle_ISC:SelectEqCircle_ISC.c
	${CC} -o SelectEqCircle_ISC ${SRC}/SelectEqCircle_ISC.c ${CFLAGS}

################################################################################
pickDat: pickDat.cpp Eqsta.h Eqsta.o
	${CXX} -o pickDat ${SRC}/pickDat.cpp Eqsta.o ${CXXFLAGS}

pickDat4dt: pickDat4dt.cpp Eqsta.h Eqsta.o
	${CXX} -o pickDat4dt ${SRC}/pickDat4dt.cpp Eqsta.o ${CXXFLAGS}

pickDat4Order: pickDat4Order.cpp Eqsta.h Eqsta.o
	${CXX} -o pickDat4Order ${SRC}/pickDat4Order.cpp Eqsta.o ${CXXFLAGS}

pickSta: pickSta.cpp Eqsta.h Eqsta.o
	${CXX} -o pickSta ${SRC}/pickSta.cpp Eqsta.o ${CXXFLAGS}

################################################################################
GenerateGMT: GenerateGMT.cpp Eqsta.h Eqsta.o
	${CXX} -o GenerateGMT ${SRC}/GenerateGMT.cpp Eqsta.o ${CXXFLAGS}

GenerateGMT4checkerboard: GenerateGMT4checkerboard.c
	${CC} -o GenerateGMT4checkerboard ${SRC}/GenerateGMT4checkerboard.c ${CFLAGS}

################################################################################
dtTest: dtTest.cpp Eqsta.h Eqsta.o
	${CXX} -o dtTest ${SRC}/dtTest.cpp Eqsta.o ${CXXFLAGS}

################################################################################
LiaolingTransform: LiaolingTransform.cpp Eqsta.h Eqsta.o
	${CXX} -o LiaolingTransform ${SRC}/LiaolingTransform.cpp Eqsta.o ${CXXFLAGS}

################################################################################
gndk: gndk.c
	${CC} -o gndk ${SRC}/gndk.c ${CFLAGS}

################################################################################
getAverageModel1D: getAverageModel1D.cpp VelModel.h VelModel.o
	${CXX} -o getAverageModel1D ${SRC}/getAverageModel1D.cpp VelModel.o ${CXXFLAGS}

getModel1D: getModel1D.cpp VelModel.h VelModel.o
	${CXX} -o getModel1D ${SRC}/getModel1D.cpp VelModel.o ${CXXFLAGS}
################################################################################
getRaypath41D: getRaypath41D.cpp tx.h tx.o VelModel.h VelModel.o
	${CXX} -o getRaypath41D ${SRC}/getRaypath41D.cpp tx.o VelModel.o ${CXXFLAGS}

clean:
	rm ${EXEC} ${OBJ} -rf

cleanexe:
	rm ${EXEC} -rf

install:
	mv ${EXEC} ./bin