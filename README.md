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

We will use Ubuntu 20.04 LTS since that is the only version supported by [CCF](https://github.com/microsoft/CCF) at the moment and we may one day use some of their code.
The VM name and admin username are arbitrary.

We will select VMs that use the same hardware to compare between confidential and non-confidential VMs.
The set of supported sizes for Confidential VMs can be found [here](https://learn.microsoft.com/en-us/azure/confidential-computing/virtual-machine-solutions-amd).
The regions and zones are selected based on VM size availability.

```bash
RG=<myResourceGroup>
az vm create \
  --resource-group $RG \
  --name tee_benchmark \
  --admin-username azureuser \
  --generate-ssh-keys \
  --public-ip-sku Standard \
  --location swedencentral \
  --size Standard_D8as_v5 \
  --image Canonical:0001-com-ubuntu-server-focal:20_04-lts:latest 
```

If launching a Confidential VM, replace the last 3 lines above with:
```bash
  --location northeurope \
  --zone 2 \
  --size Standard_DC8as_v5 \
  --image Canonical:0001-com-ubuntu-confidential-vm-focal:20_04-lts-cvm:latest \
  --security-type ConfidentialVM \
  --os-disk-security-encryption-type VMGuestStateOnly
```

Once launched, write down the public IP of the VM. I will assume your VM's IP is <IP>. We can now SSH into the VM:
```bash
IP=<IP>
ssh azureuser@$IP
```

Now you can clone this Github project on the VM and run it from there.