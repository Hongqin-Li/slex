obj/slex: slex.cc debug.h
	mkdir -p obj/
	g++ slex.cc -o obj/slex

clean:
	rm -rf obj/

.PHONY: test clean
