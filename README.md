# tee-benchmark
Benchmarking hashing, encryption, decryption, disk I/Os, and communication speeds for TEEs.

## Installation 
Execute the following commands:
```bash
cmake .
make
```

## Benchmarking hashing, encryption, and decryption
Run without any arguments to run all the benchmarks.
```bash
./tee_benchmark
```

The arguments are as follows:
- `-b`: Specify size of *b*locks (in bits) to hash or encrypt.
- `-w`: Specify number of seconds to *w*armup for.
- `-r`: Specify number of seconds to *r*un for.
- `-h`: Whether to benchmark *h*ashing.
- `-e`: Whether to benchmark *e*ncryption.
- `-d`: Whether to benchmark *d*ecryption.
Default values for the arguments can be found at the top of `main.cpp`.

An example of running the benchmark for 60 seconds with 30 seconds of warmup, only benchmarking hashing: 
```bash
./tee_benchmark -w 30 -r 60 -h
```

## Benchmarking disk I/Os
Go to the `file` directory, run the `install.sh` script to install sysbench, then run `read_run.sh` or `write_run.sh` to get the read/write numbers.
Note that to simulate disk read/writes (and avoid cache), DIRECTIO is turned on for reads, and every write is followed by a fsync.