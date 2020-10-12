slex: slex.cc
	g++ slex.cc -o slex

clean:
	rm -f slex *.pdf *.dot

.PHONY: clean
