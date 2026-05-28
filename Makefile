CXX = g++
CXXFLAGS =  -Wall -MMD -lglut -lGLU -lGL -lopenal
OBJECTS = blockfall.o GLwindow.o Cell.o block.o font.o audio.o
DEPENDS = ${OBJECTS:.o=.d}
EXEC = blockfall

${EXEC} : ${OBJECTS}
	${CXX} ${CXXFLAGS} ${OBJECTS} -o ${EXEC}

-include ${DEPENDS}

.PHONY : clean
clean :
	rm ${DEPENDS} ${OBJECTS} ${EXEC}
