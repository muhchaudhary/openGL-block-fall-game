CXX = g++
CXXFLAGS =  -Wall -MMD -lglut -lGLU -lGL 
OBJECTS = triangle.o GLwindow.o Cell.o block.o font.o
DEPENDS = ${OBJECTS:.o=.d}
EXEC = triangle

${EXEC} : ${OBJECTS}
	${CXX} ${CXXFLAGS} ${OBJECTS} -o ${EXEC}

-include ${DEPENDS}

.PHONY : clean
clean :
	rm ${DEPENDS} ${OBJECTS} ${EXEC}
