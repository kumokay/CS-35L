//Week 7 Homework

First thing I did was to update the Makefile to include the link to the lpthread
I also modified the makefile for it to output 1-test.ppm, and modified the for loop
to check for 1, 2, 4, and 8.

Now I ran 'make clean check' to see what would happen...
At this point, I kept thinking that my program froze when really it was just being
really slow. After realizing my mistake, I got this output.

rm -f *.o *.ppm *.tmp srt srt.tgz
gcc -std=gnu99 -g -O2 -Wall -Wextra -Wno-unused-parameter -lpthread   -c -o main.o main.c
gcc -std=gnu99 -g -O2 -Wall -Wextra -Wno-unused-parameter -lpthread   -c -o raymath.o raymath.c
gcc -std=gnu99 -g -O2 -Wall -Wextra -Wno-unused-parameter -lpthread   -c -o shaders.o shaders.c
gcc -std=gnu99 -g -O2 -Wall -Wextra -Wno-unused-parameter -lpthread -o srt main.o raymath.o shaders.o -lm
time ./srt 1-test.ppm >1-test.ppm.tmp

real    0m48.874s
user    0m48.876s
sys     0m0.002s
mv 1-test.ppm.tmp 1-test.ppm
time ./srt 2-test.ppm >2-test.ppm.tmp
./srt: Multithreading is not supported yet.

real    0m0.002s
user    0m0.000s
sys     0m0.001s
make: *** [2-test.ppm] Error 1

It seems to loop and work just fine. Now I actually need to work on the multithreading
in main.c

Things I noticed:
0. TA gave me a hint that I might want to multithread the drawing in main by rows
1. Some globals I need to have(that were otherwise local or didn't exist before)
	nthreads(get rid of the 1 thread check in main, I need this in my multithreaded func too)
	scene (I noticed that scene was used in the drawing code which I should put in the multithreaded func)
	scaled_color (I want the multithread func to load things into the buffer scaled_color
		and then print it at the end)
2. I found where I wanted to break up the code into a mulththreading func and I copy pasted.
3. I found it more wise to load everything into the scaled_color buffer before printing
	it so I did just that. In order to do that I had to update it within the multithread func.
4. I thought that I didn't really need to wait for the threads to complete before printing
	since it was already processed before printing, but I added the join commands just to be safe.
5. Once I partitioned everything and successfully allocated everything, I needed to know how
	to partition the tasks into my threads...I was given many suggestions, getting one to work
	was the actual challenge.
	
Problems:
-I spelled some stuff wrong and that took an embarassing amount of time to fix
-I made pixel_color 3 dimensional argument in gamma() when it should've stayed 1 dimension
-I had several failed partitioning methods...
-I finally got a really simple row-splitting (px) threading partitioning working
	(as TA recommended me to do...)
-I had the makefile test 2,4,8-test.ppm against 1.ppm which was basically the original code...
	since I couldn't find baseline.ppm in my directory. The picture seemed fine
-(cd srt && make clean check) 2>&1 | diff -u - make-log.txt: this check took some time
	to run, but at this point, I already got used to waiting...
	As stated, the diff only printed out minor timing differences but also this:
-         diff -u 1-test.ppm $file || exit; \
-       done
+          diff -u 1-test.ppm $file || exit; \
+        done
	...I ignored this line that diff spat out because it doesn't make a difference at all.

When I was finished with multithreading the program, I ran make clean check again.
I got this output...also shown in make-log.txt:

rm -f *.o *.ppm *.tmp srt srt.tgz
gcc -std=gnu99 -g -O2 -Wall -Wextra -Wno-unused-parameter -lpthread   -c -o main.o main.c
gcc -std=gnu99 -g -O2 -Wall -Wextra -Wno-unused-parameter -lpthread   -c -o raymath.o raymath.c
gcc -std=gnu99 -g -O2 -Wall -Wextra -Wno-unused-parameter -lpthread   -c -o shaders.o shaders.c
gcc -std=gnu99 -g -O2 -Wall -Wextra -Wno-unused-parameter -lpthread -o srt main.o raymath.o shaders.o -lm
time ./srt 1-test.ppm >1-test.ppm.tmp

real    0m48.599s
user    0m48.611s
sys     0m0.003s
mv 1-test.ppm.tmp 1-test.ppm
time ./srt 2-test.ppm >2-test.ppm.tmp

real    0m24.832s
user    0m49.342s
sys     0m0.001s
mv 2-test.ppm.tmp 2-test.ppm
time ./srt 4-test.ppm >4-test.ppm.tmp

real    0m12.673s
user    0m49.618s
sys     0m0.001s
mv 4-test.ppm.tmp 4-test.ppm
time ./srt 8-test.ppm >8-test.ppm.tmp

real    0m6.787s
user    0m51.682s
sys     0m0.003s
mv 8-test.ppm.tmp 8-test.ppm
for file in 1-test.ppm 2-test.ppm 4-test.ppm 8-test.ppm; do \
          diff -u 1-test.ppm $file || exit; \
        done

Multithreading greatly improved the performance of raytracing. We see that this
improvement is limited by Amdahl's law. However, this does not hinder the potential
that multithreading has on improving performance.