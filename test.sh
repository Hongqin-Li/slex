make
echo -n "[1-8a-cz]*" | ./slex > g.dot
dot -Tpdf g.dot -o g.dot.pdf
# echo $1 | ./a.out | dot -Tpdf -o g.dot.pdf
