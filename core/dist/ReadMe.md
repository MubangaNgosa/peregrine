# Distributed Peregrine System

This README provides instructions for compiling and running the Distributed Peregrine System. This system extends the capabilities of Peregrine, a pattern-aware graph analysis software, allowing it to operate across multiple nodes in a distributed environment.

## System Requirements

- Linux-based operating system (Ubuntu, CentOS, etc.)
- g++ compiler (supporting C++20 standard)
- ZeroMQ library
- Boost Serialization library (optional, for advanced serialization needs)

## Installation

### Step 1: Install ZeroMQ

ZeroMQ is required for communication between nodes. Install it using your system's package manager. For example, on Ubuntu:

```bash
sudo apt-get update
sudo apt-get install libzmq3-dev

```
## Installation Step 2: Clone the Repository

Clone the project repository to your local machine.

```bash
git clone https://github.com/MubangaNgosa/peregrine.git
cd peregrine
```
##Compilation

Navigate to the project directory and use the provided Makefile to compile the program.

```bash
cd core/dist
make
```
This will compile both the jobPool and worker executables.
## Running the System
Starting the JobPool (Master Node)

To start the jobPool, run the following command:

```bash

./jobPool
```
This will initialize the master node and wait for worker nodes to connect.

Starting Worker Nodes

Open a new terminal for each worker node and run:

```bash
./worker
```
This will start the worker node, which will connect to the jobPool and wait for jobs.
Observing the System

Once the jobPool and at least one worker are running, you should observe the following:

    The jobPool will distribute job packets to available worker nodes.
    Each worker node will receive a job packet, process it, and send a response back to the jobPool.
    The jobPool and worker terminals will display messages indicating the status of job processing.

Troubleshooting

    Ensure that ZeroMQ is installed correctly and is accessible by the system.
    Check for any compilation errors and resolve them by installing missing dependencies or adjusting the environment settings.
    If the jobPool and worker nodes are not communicating, verify that the network settings and firewall configurations allow for local socket communication.