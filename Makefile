CXX = g++
CXXFLAGS = -lglut -lGLU -lGL 
OBJECTS = triangle.o GLwindow.o Cell.o
DEPENDS = ${OBJECTS:.o=.d}
EXEC = triangle

${EXEC} : ${OBJECTS}
	${CXX} ${CXXFLAGS} ${OBJECTS} -o ${EXEC}

clean :
	rm ${DEPENDS} ${OBJECTS} ${EXEC}

-include ${DEPENDS} # reads the .d files and reruns dependencies
