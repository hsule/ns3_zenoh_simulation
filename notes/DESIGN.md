# Congestion-aware Flow Management for V2X over Zenoh

## Core Problem

Zenoh is flow-blind: it always routes via the shortest path (fewest hops), completely ignoring link capacity and delay. In experiments:

- Exp1 (capacity bottleneck): 2 Mbps data pushed through a 1 Mbps link → ~50% drop rate, delay grows from 100 ms to 10-12 s.
- Exp2 (delay difference): 100 Mbps / 10 ms links vs 100 Mbps / 1 ms links → Zenoh still picks the short-but-slow path → ~58% drop rate, delay up to 14-16 s.
- Correction: What initially appeared to be a "path change" was actually the long-fast connection not being fully established yet. Zenoh never dynamically switches paths on its own.

In V2X, this is unacceptable: remote driving control streams need low latency and high reliability, while video monitoring streams need sustained throughput. Both coexist on shared links with fluctuating bandwidth.

## Why Not Existing Approaches?

### RSVP
- Supports per-flow end-to-end QoS reservation — ideal in theory.
- Not viable for V2X: vehicles move frequently, RSVP paths/states become invalid; per-hop state storage doesn't scale.
- Conclusion: A bandwidth allocation mechanism *on top of Zenoh's pub-sub model* can compensate for RSVP's limitations in dynamic environments.

### SDN
- Controller-based centralized decisions for bandwidth allocation, shortest path calculation, and congestion routing.
- Drawbacks: single point of failure; controller information may be delayed; doesn't align with Zenoh's decentralized architecture.

### QSCCP (Class-Based Differentiated Scheduling)
- 3-level scheduling: SP (strict priority) → WFQ (weighted fair queuing) → WDRR (weighted deficit round robin).
- Token-Bucket Rate Limiters (TBRL) prevent greedy high-priority flows from starving others.
- Insight: The scheduling hierarchy is relevant — my system needs similar protection where control packets can preempt data flows when bandwidth is fully saturated.

## Why Zenoh

- High throughput: ~34 Gbps in brokered mode.
- Low latency: 16 us P2P (best among Kafka, MQTT, Cyclone DDS).
- Distributed architecture: no single point of failure, natural fit for V2X edge/core deployment.
- Missing piece: Zenoh's congestion control only offers `Block` and `Drop` — no adaptive bitrate, no congestion-aware routing. This is what my work addresses.

## My Approach

### Dual-Tree Architecture
- Control tree (`trees`): uses original link weights, stays on the shortest path. Bandwidth is dynamically protected for control/real-time traffic via the adaptive `cap_limit`.
- Data tree (`data_trees`): uses separate `data_link_weights` that are dynamically adjusted based on congestion. Data flows can be detoured away from congested links.
- Key benefit: Control traffic is never affected by data congestion — complete isolation between control plane and data plane routing.

### Congestion Response Loop
1. Detection: Each router locally monitors per-link bandwidth usage via Prometheus metrics. Per-priority statistics (`PriorityStats`) distinguish high-priority (Control + RealTime) from data traffic.
2. Weight Update: When data throughput exceeds `cap_limit = link_capacity - high_priority_throughput - buffer`, set `data_link_weight = 65535` (blocked). The dynamic cap_limit automatically reserves bandwidth for high-priority traffic.
3. Recomputation: Broadcast updated data weights via linkstate → all nodes recompute data trees via Bellman-Ford.
4. Forwarding Decision:
   - Existing flows: pinned to their current path (ignore new tree) to prevent route flapping.
   - New flows: use the new data tree to bypass congested links.

### Flow Pinning Strategy
- Active flows are "pinned" to their upstream interface to prevent route oscillation.
- Pinning cascades upstream via `active_flows` field in linkstate messages.
- Unpin timing: Flow TTL / soft state with `last_active_timestamp`. Periodic cleanup (every 1s, TTL = 5s) removes stale pins, allowing natural rerouting to resume.
- Unsubscribe: When a subscriber disconnects, the flow naturally expires and gets unpinned.

### Path Recovery & Restoration
- When data throughput drops below 20% of `cap_limit`, restore default weight (100).
- Low threshold in `update_data_link_weights` prevents oscillation between block and recover states.

### QoS
- Dual-tree architecture ensures control traffic routes are never affected by data congestion.
- Dynamic `cap_limit` reroutes data traffic before the link saturates, preserving bandwidth for high-priority flows.

## Advantages Over Related Work

| Aspect | SDN | RSVP | My Method |
|--------|-----|------|-----------|
| Bandwidth | Centralized allocation (possibly delayed) | Per-flow reservation | Local monitoring + congestion-triggered linkstate update to neighbors |
| Delay | Controller computes shortest path | End-to-end reservation | Pre-connection delay detection → adjust link weight (proposal) |
| Reliability | Single point of failure | Per-hop state, breaks on mobility | Decentralized, no single point of failure |
| Congestion routing | Controller's decision | N/A | Distributed: each node reacts locally, propagates via linkstate |
| Stability | Depends on controller | Path breaks on handover | Flow pinning prevents oscillation; proactive avoidance |
| Control/Data isolation | Possible but requires controller config | Separate reservations | Dual-tree: inherent architectural separation |

## Scenario Proposal

The congestion-aware routing (bandwidth-aware flow management) operates in the backbone where:
- Link capacities are known and relatively stable.
- Multiple paths exist between edge and core.
- The V2X application layer (vehicles → RSU → core) connects through 5G NR with URLLC/eMBB slicing.

This maps to the ns-3 / 5G-LENA simulation setup: containers running Zenoh connected via TAP interfaces through Ghost Nodes with CSMA/VXLAN links to 5G UE nodes.

## TODOs

1. Adaptive bitrate: Zenoh doesn't support it. This should be a main contribution — feeding congestion signals back to publishers so they can adjust video encoding bitrate (similar to the 3-level adaptive rate control algorithm from the railway remote driving paper).

2. Guarantee algorithm: Need to design an algorithm that ensures resource allocation is correct for both control data and streaming, meeting end-to-end latency requirements.

3. Close-loop traffic: Must guarantee that control loop traffic (e.g., remote driving commands) meets its latency requirements even under heavy video streaming load.

4. Segment routing: If the original path becomes invalid (e.g., link failure), insert a segment routing header to forward to the nearest valid intersection. This is an alternative/complementary approach to full tree recomputation — need to evaluate whether it's faster for transient failures.

5. Delay-aware routing: Currently only bandwidth is considered. Proposal: pre-connection delay detection and incorporate measured delay into link weight calculation. Not implemented yet.

6. Distributed algorithm comparison: What existing distributed algorithms address similar problems? What are their limitations that my approach can improve upon?

7. Emergency threshold: The 2 Mbps threshold and 20% recovery threshold need formal justification or empirical tuning. How do these values generalize across different topologies and traffic patterns?