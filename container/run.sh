#!/bin/bash
cd "$(dirname "$0")"
docker-compose run --rm ns3-sim bash
