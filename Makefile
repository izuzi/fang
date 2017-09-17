EXE = fang.out

all:
	g++ -o $(EXE) main.cpp -I../../zlab/comm -I../../zlab/zjson -L../../zlab/zjson -lzjson

clean:
	rm $(EXE)

test:
	./$(EXE) conf3.txt 0.5 324.36 3 180 60
