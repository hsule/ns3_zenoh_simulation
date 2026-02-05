# ns3_zenoh_simulation

This project provides an integrated simulation environment combining the **ns-3** network simulator and **Zenoh** pub/sub middleware. It uses TAP bridges to connect ns-3's simulated network links with Docker containers running Zenoh routers, enabling evaluation of Zenoh routing behavior across configurable network topologies (bandwidth, delay, capacity).

## Prerequisites

- Linux (Ubuntu 22.04 recommended)
- Docker & Docker Compose
- `sudo` privileges (required for TAP interface and bridge setup)
- `tmux`

All other dependencies (GCC 12, CMake, Rust, Python 3, ns-3 libraries, etc.) are included in the provided Docker container. You can also install them locally and run without the container -- refer to the `container/Dockerfile` for the full list of dependencies.

## Quick Start

### 1. Clone with submodules

```bash
git clone --recurse-submodules https://github.com/hsule/ns3_zenoh_simulation.git
cd ns3_zenoh_simulation
```

### 2. Build and enter the development container

```bash
./container/build.sh
./container/run.sh
```

This launches a privileged Docker container with all build tools pre-installed. The project directory is mounted at `/workspace`. All subsequent commands should be run **inside this container**.

### 3. Build Zenoh

Cross-compile Zenoh to produce statically-linked binaries (`zenohd`, `z_pub`, `z_sub`, etc.):

```bash
./script/build_cross_zenoh.sh
```

Binaries are output to `zenoh/target/x86_64-unknown-linux-musl/fast/`.

### 4. Define a topology (or use an existing one)

Each experiment lives under `script/topology/<experiment_name>/` and contains two files:

- **`NETWORK_CONFIG.json5`** -- defines nodes (pub/sub/router), Zenoh IDs, listen endpoints, and links between nodes.
- **`zenoh.cc`** -- ns-3 C++ simulation that creates CSMA channels with specified bandwidth/delay and attaches TAP bridges to connect to the Docker containers.

See `script/topology/test/` for a simple 7-node example.

### 5. Build ns-3 for the experiment

```bash
./script/build_ns3.sh <experiment_name>
```

This copies the topology's `zenoh.cc` and `NETWORK_CONFIG.json5` to the correct locations and builds ns-3.

### 6. Launch Zenoh nodes

```bash
./script/run_zenoh.sh
```

This reads `NETWORK_CONFIG.json5` and for each node:
- Starts a Docker container
- Creates TAP interfaces and veth bridges
- Launches `zenohd` (router), `z_pub` (publisher), or `z_sub` (subscriber) based on the node's role

Logs are saved to `zenohd-auto-deploy/experiment_data/<experiment_name>/<timestamp>/`.

### 7. Run the ns-3 simulation

```bash
./script/run_ns3.sh
```

This runs the ns-3 real-time simulation, which imposes the configured bandwidth/delay constraints on the links between Zenoh containers via TAP bridges.

## Project Structure

```
.
├── container/                  # Dev container (Dockerfile, docker-compose)
├── ns-3-dev/                   # ns-3 submodule (network simulator)
├── zenoh/                      # Zenoh submodule (pub/sub middleware, Rust)
├── zenohd-auto-deploy/         # Submodule: automated Zenoh container deployment
│   ├── launch_routers.py       #   Main orchestration script
│   └── experiment_data/        #   Output logs per experiment run
└── script/
    ├── build_cross_zenoh.sh    # Build Zenoh (cross-compile, static linking)
    ├── build_ns3.sh            # Build ns-3 for a given experiment
    ├── run_zenoh.sh            # Launch Zenoh Docker containers
    ├── run_ns3.sh              # Run ns-3 simulation
    └── topology/               # Experiment definitions
        ├── test/               #   Simple 7-node topology
        └── twopath/            #   Two-path routing topology
```

## NETWORK_CONFIG.json5 Format

```json5
{
    experiment: "my_experiment",
    docker_image: { tag: "eclipse/zenoh:1.4.0", clean_first: false },
    volume: "../zenoh",
    nodes: {
        "0": {
            role: "pub",           // "pub", "sub", or omit for router
            zid: { set: true, value: "194320990a90d745a86034516feef351" },
            listen_endpoints: [ "tcp/10.0.9.9:8000" ],
        },
        // ... more nodes
    },
    links: [
        { a: "0", a_idx: 0, b: "1", b_idx: 2, cap: 100 },
        // a/b: node IDs, a_idx/b_idx: index into listen_endpoints, cap: Mbps
    ]
}
```