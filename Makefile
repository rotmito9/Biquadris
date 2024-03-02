CXX = g++
CXXFLAGS = -std=c++20 -Wall -Werror=vla -MMD

GAME := biquadris

EXEC = ${GAME}

TEST_EXEC = ${GAME}_debug

OBJECTS = AbsDisplay.o Biquadris.o Block.o GameControl.o PlayerState.o Prompt.o subject.o TextDisplay.o X11Display.o window.o TrieNode.o Debuff.o main.o

all: ${EXEC}

format:
	clang-format -i *.[ch]

%.o : %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

DBG_OBJECTS = AbsDisplay.o.dbg Biquadris.o.dbg Block.o.dbg GameControl.o.dbg PlayerState.o.dbg Prompt.o.dbg subject.o.dbg TextDisplay.o.dbg X11Display.o.dbg window.o.dbg TrieNode.o.dbg Debuff.o.dbg main.o.dbg

%.o.dbg : %.cc
	$(CXX) $(CXXFLAGS) -DTEST -c $< -o $@

DEPENDS = ${OBJECTS:.o=.d}

all: ${EXEC}

# DO NOT FORGET THE TEXT FILES!!!
# and the design doc!!!
zip:
	rm -f *.zip; zip biquadris.zip ./*.txt ./*.pdf ./*.cc ./*.h ./Makefile

${EXEC}: ${OBJECTS}
	${CXX} ${CXXFLAGS} ${OBJECTS} -o ${EXEC} -lX11

debug: ${DBG_OBJECTS}
	${CXX} ${CXXFLAGS} -DTEST ${DBG_OBJECTS} -o ${TEST_EXEC} -lX11

clean:
	rm -f *.o ${EXEC} ${TEST_EXEC} *.d *.o.dbg *.zip

.PHONY: clean
# -include ${DEPENDS}
