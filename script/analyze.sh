./script/analyze_data.py \
    --config script/topology/twopath/NETWORK_CONFIG.json5   \
    --sublog ./zenohd-auto-deploy/experiment_data/twopath/2025-10-20_08:43:48/zenohd_5.log   \
    --routerlog ./zenohd-auto-deploy/experiment_data/twopath/2025-10-20_08:43:48/zenohd_3.log   \
    --topic demo/example/zenoh-rs-pub   \
    --out delays_exp4.csv