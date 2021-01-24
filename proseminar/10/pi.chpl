use Random;
use Math;
use Time;

config const numRuns = 1e9:int;
config const numPUs = 8;

var timer:Timer;


writeln("Running on ", here.numPUs(), " threads");
timer.start();

var inside = 0;
coforall t in 1..numPUs with (+ reduce inside) {
  var actualNumRuns = numRuns/numPUs;
  var randStream = new RandomStream(real);

  for i in 1..actualNumRuns {
    var x = randStream.getNext();
    var y = randStream.getNext();
    if (x*x+y*y < 1){
      inside+=1;
    }
  }
}

var pi = inside:real*4/numRuns:real;
timer.stop();

writeln("Approximation over ", numRuns, " steps took ", timer.elapsed(), " seconds, approximation of pi is ", pi);
