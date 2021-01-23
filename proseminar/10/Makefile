all: pi matrix matrix-multinode pi-multinode

%-multinode: %-multinode.chpl
	chpl $^

pi: pi.chpl
	chpl pi.chpl

matrix: matrix.chpl
	chpl matrix.chpl

.PHONY: clean
clean:
	rm -f pi matrix *.log
