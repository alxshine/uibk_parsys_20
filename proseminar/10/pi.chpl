use Random;
use Math;
use Time;

config const numRuns = 1e9:int;


var timer:Timer;

var randStream = new RandomStream(real);

timer.start();

var inside = 0;
forall i in 1..numRuns with (+ reduce inside) {
    var x = randStream.getNext();
    var y = randStream.getNext();
    if (x*x+y*y < 1){
        inside+=1;
    }
}

var pi = inside:real*4/numRuns:real;
timer.stop();

writeln("Approximation over ", numRuns, " steps took ", timer.elapsed(), " seconds, approximation of pi is ", pi);