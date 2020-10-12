slex: slex.c
	cc slex.c -o slex

clean:
	rm -f slex *.pdf *.dot

.PHONY: clean
