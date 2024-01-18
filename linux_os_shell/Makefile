program: main.o ourls.o cmd_file.o ourps.o ourtree.o ourhistory.o ourhelp.o
	gcc main.o ourls.o cmd_file.o ourps.o ourtree.o ourhistory.o ourhelp.o -o program -lm

main.o: main.c
	gcc -c -o main.o -I./header main.c

ourls.o: ourls.c
	gcc -c -o ourls.o -I./header ourls.c

cmd_file.o: cmd_file.c
	gcc -c -o cmd_file.o -I./header cmd_file.c

ourps.o: ourps.c
	gcc -c -o ourps.o -I./header ourps.c

ourtree.o: ourtree.c
	gcc -c -o ourtree.o -I./header ourtree.c

ourhistory.o: ourhistory.c
	gcc -c -o ourhistory.o -I./header ourhistory.c

ourhelp.o: ourhelp.c
	gcc -c -o ourhelp.o -I./header ourhelp.c

# Add any additional object files and rules as needed

.PHONY: clean clean_o test test_cd

clean:
	rm -f main.o ourls.o cmd_file.o ourps.o ourtree.o ourhistory.o ourhelp.o program

clean_o:
	rm -f main.o ourls.o cmd_file.o ourps.o ourtree.o ourhistory.o ourhelp.o
