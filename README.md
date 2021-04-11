# multicore-support

From within the multicore-support directory, run InfOS using the ./build-and-run.sh script.

To modify the number of cores, modify the -smp argument within run.sh.

InfOS runs in unicore mode by default. To run in multicore mode, add the smp=yes argument to the runscript, like so:

./build-and-run.sh smp=yes

Open a new terminal and run vncviewer localhost to see the InfOS shell. From ehre, you can run programs in the user space. Try:

/usr/mandelbrot
