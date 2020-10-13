make
cat example.l | ./obj/slex > obj/g.dot
dot -Tpdf obj/g.dot -o obj/g.dot.pdf
# echo $1 | ./a.out | dot -Tpdf -o g.dot.pdf
