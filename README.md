# logbench
C++ logging library benchmark library.

### Alpha release 
The library is in alpha release.
Testing and feedback is greatly appreciated!

## Build, install

### Linux
~~~
git clone http://github.com/denesderhan/logbench
cd logbench && mkdir build && cd build
cmake ..
sudo make install
~~~

### Windows (Visual Studio)
Download, extract the project.
Open a Visual Studio Developer command prompt
~~~
cd [your_path_to]/logbench
mkdir build
cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja install
~~~

## Usage
Build a benchmark executable using the logbench library.
examples: http://github.com/denesderhan/loglib_benchmarks

Run the benchmark using the logbench executable:
~~~
root@debdev:~/bench# logbench fstlog
~~~
~~~
(TSC freq: 2.592 Ghz)
fstlog, -t 1 -l 10 -w 0 -d 0 -f txt -s lib -p 0 4 --buffer_size -1 --buffer_flush -1 --sink_flush -1,OK,
~~~
fstlog is the benchmark binary built with the logging library to be benchmarked.
logbench is the binary supplied by the logbench benchmarking library.
Run benchmark with custom parameters:
~~~
root@debdev:~/bench# logbench fstlog -o results -b logs -t 4 -l 1000 -d 2000 --latency --dropped
~~~
~~~
(TSC freq: 2.592 Ghz)
fstlog, -t 4 -l 1000 -w 0 -d 2000 -f txt -s lib -p 0 4 --buffer_size -1 --buffer_flush -1 --sink_flush -1 --latency  --dropped ,OK,
~~~
The two processes are communicating through interprocess memory,
logbench runs the fstlog benchmark, collects the results and writes to csv file.

## logbench options
~~~
logbench --help
SYNOPSIS
        logbench --version
        logbench --help
        logbench [<benchmark>] [-o <path>] [-b <path>] [-t <unsigned int>] [-l <unsigned int>] [-d
                 <millisec>] [-w <unsigned long>] [--buffersize <Kbyte>] [--bufferflush <millisec>]
                 [--sinkflush <millisec>] [-p <first core> <last core>] [-f <string>] [-s <string>]
                 [--latency] [--logself] [--noprio] [--dropped] [-c <millisec>]

OPTIONS
        --version   Prints the version.
        --help      Prints help.
        <benchmark> Path to benchmark executable or directory.
        -o, --outdir
                    Path to benchmark output (result) directory.

        -b, --benchdir
                    Path to log output (log) directory.

        -t, --threads
                    Number of parallel logging threads.

        <unsigned int>
                    [0 - 1024]

        -l, --lognum
                    Number of log calls per thread.

        <unsigned int>
                    [0 - 1'000'000]

        -d, --duration
                    Targeted duration of the individual benchmark runs in milliseconds.

        <millisec>  [0 - 3'600'000]
        -w, --work  Amount of work to do, preceding every log call.
        <unsigned long>
                    [0 - 1'000'000'000]

        --buffersize
                    Set memory used by the logging library in Kbytes per logging thread.

        <Kbyte>     [1 - 1'000'000]
        --bufferflush
                    Set logging library internal buffer flush interval in millieconds (0 means
                    infinity).

        <millisec>  [0 - 3'600'000]
        --sinkflush Set logging library sink flush interval in millieconds (0 means infinity)
        <millisec>  [0 - 3'600'000]
        -p, --pin   Pin logging threads to core range. ({-1,-1} means no pinning)
        <first core>
                    [0 - 1024] | -1

        <last core> [0 - 1024] | -1
        -f, --formatter
                    Type of formatter to use.

        <string>    [txt | bin]
        -s, --sink  Type of sink to use.
        <string>    [null | sort | unsort]
        --latency   Output latency data. (creates .csv file in result/[test_id] directory).
        --logself   Logging library self logging.
        --noprio    Do not set logging threads priority to high.
        --dropped   Count messages dropped by logging library.
        -c, --cooldown
                    Pause between benchmark runs in millisec (used only if benchmark path is a
                    directory).

        <millisec>  [0 - 3'600'000]
~~~
