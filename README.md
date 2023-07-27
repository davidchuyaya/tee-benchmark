# tee-benchmark
Benchmarking hashing, encryption, decryption, disk I/Os, and communication speeds for TEEs.

## Installation 
Execute the following commands:
```bash
./install.sh
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

## Benchmarking in the cloud
This section will assume that you want to use Azure, and already have an Azure account set up with the [Azure CLI](https://learn.microsoft.com/en-us/cli/azure/install-azure-cli) installed.
We will roughly follow the steps outlined in the [Azure tutorial](https://learn.microsoft.com/en-us/azure/virtual-machines/linux/quick-create-cli).

I will assume you have an existing resource group, or will create one by following [the tutorial](https://learn.microsoft.com/en-us/azure/azure-resource-manager/management/manage-resource-groups-portal).
I will assume your resource group's name is `<myResourceGroup>`.
We will set that value in the environment variable `$RG` for the remainder of this section:

```bash
RG=<myResourceGroup>
```

### Creating a Proximity Placement Group
Azure [recommends](https://learn.microsoft.com/en-us/azure/virtual-network/virtual-network-test-latency?tabs=windows#tips-and-best-practices-to-optimize-network-latency) launching VMs in the same proximity placement group (ppg) to reduce network latency.
We will create a ppg in the regions that we will launch the trusted and untrusted VMs used in [the following section](#creating-vms).

#### Untrusted PPG
```bash
az ppg create \
  --resource-group $RG \
  --name tee_benchmark_ppg \
  --location swedencentral \
  --intent-vm-sizes Standard_D8as_v5
```

#### Trusted PPG
```bash
az ppg create \
  --resource-group $RG \
  --name tee_benchmark_ppg_cvm \
  --location northeurope \
  --zone 2 \
  --intent-vm-sizes Standard_DC8as_v5
```


### Creating VMs
We will select VMs that use the same hardware to compare between confidential and non-confidential VMs.
The set of supported sizes for Confidential VMs can be found [here](https://learn.microsoft.com/en-us/azure/confidential-computing/virtual-machine-solutions-amd).
The following scripts use general compute VMs with 8 vCPUs.
The regions and zones are selected based on VM size availability.

We will use Ubuntu 20.04 LTS since that is the only version supported by [CCF](https://github.com/microsoft/CCF) at the moment and we may one day use some of their code.
The VM name and admin username are arbitrary.
Change the VM name accordingly if another VM with the same name already exists.

Create the correct VMs based on whether you're running the benchmark on [untrusted](#untrusted-vms) or [trusted](#confidential-vms) VMs.

#### Untrusted VMs
```bash
az vm create \
  --resource-group $RG \
  --name tee_benchmark_client \
  --admin-username azureuser \
  --generate-ssh-keys \
  --public-ip-sku Standard \
  --accelerated-networking \
  --ppg tee_benchmark_ppg \
  --location swedencentral \
  --size Standard_D8as_v5 \
  --image Canonical:0001-com-ubuntu-server-focal:20_04-lts:latest 
```

#### Confidential VMs
```bash
az vm create \
  --resource-group $RG \
  --name tee_benchmark_cvm \
  --admin-username azureuser \
  --generate-ssh-keys \
  --public-ip-sku Standard \
  --accelerated-networking \
  --ppg tee_benchmark_ppg_cvm \
  --location northeurope \
  --zone 2 \
  --size Standard_DC8as_v5 \
  --image Canonical:0001-com-ubuntu-confidential-vm-focal:20_04-lts-cvm:latest \
  --security-type ConfidentialVM \
  --os-disk-security-encryption-type VMGuestStateOnly \
  --enable-vtpm
```

### Using the VMs
Once launched, write down the public IP of the VM. I will assume your VM's IP is <IP>. We can now SSH into the VM:
```bash
IP=<IP>
ssh azureuser@$IP
```

Now you can clone this Github project on the VM and run it from there.

## Benchmarking network latency
On all VMs, execute the following after cloning this repo:
```bash
cloud/install.sh
```

We will assume that the public IP of the server is `<IP>`.
On the server, execute the following line:
```bash
IP=<IP>
sudo sockperf sr --tcp -i $IP -p 12345
```

On the client, execute the following line:
```bash
IP=<IP>
sockperf ping-pong -i $IP --tcp -m 350 -t 101 -p 12345 --full-rtt
```