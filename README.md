# ns3_zenoh_simulation

This project provides an integrated simulation environment combining the **ns-3** network simulator and **Zenoh** for testing and evaluation.

## Usage

1. Add a topology

   Create your own topology under `script/topology/` (the folder name will be the `<experiment_name>`).  
   Check the `test/` topology as an example. 

   Each topology should include:
   - `NETWORK_CONFIG.json5` (defines network configuration, e.g., nodes, links, bandwidth, delay).
   - `zenoh.cc` (ns-3 simulation file runs the simulation).

2. Build Zenoh
    ```bash
    ./script/build_cross_zenoh.sh
    ```
3. Build ns-3 for a specific experiment
    ```bash
    ./script/build_ns3.sh <experiment_name>
    ```
4. Run zenoh nodes
    ```bash
    ./script/run_zenoh.sh
    ```
5. Run ns-3
    ```bash
    ./script/run_ns3.sh
    ```