openGl = -lGLU -lGL -lglut
tiny = MyFunctions/parserxml/tinystr.cpp MyFunctions/parserxml/tinyxml.cpp MyFunctions/parserxml/tinyxmlerror.cpp MyFunctions/parserxml/tinyxmlparser.cpp

all: 
	g++ main.cpp $(tiny) -o trabalhocg $(openGl)

clean:
	-rm trabalhocg
