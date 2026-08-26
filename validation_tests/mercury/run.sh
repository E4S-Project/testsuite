#!/bin/bash
. ./setup.sh
set -x


# 1. Kill lingering server instances from prior crashed runs
pkill -9 -f hg_example_rpc_server 2>/dev/null || true

# 2. Dynamically auto-detect active network interface 
IFACE=$(awk '$2 == "00000000" {print $1; exit}' /proc/net/route 2>/dev/null)

# Fallback to first physical interface if no default route exists
if [ -z "$IFACE" ]; then
    IFACE=$(ls /sys/class/net 2>/dev/null | grep -v -E '^(lo|docker|veth|br-)' | head -n 1)
fi

# Export detected interface for Libfabric
if [ -n "$IFACE" ]; then
    export FI_TCP_IFACE="$IFACE"
    export FI_SOCKETS_IFACE="$IFACE"
fi

# 3. Clean up stale files
rm -f server_output.txt /tmp/hg-stock-*

# 4. Start server listening on all local interfaces with an OS-assigned ephemeral port (:0)
stdbuf -o0 ./build/src/hg_example_rpc_server "ofi+tcp://:0" > server_output.txt &
server_pid=$!
sleep 2 

# 5. Extract assigned address
server_address=$(grep -o '".*"' server_output.txt | tr -d '"')
echo "Server running at: $server_address"

if [ -z "$server_address" ]; then
    echo "Error: Server failed to start."
    cat server_output.txt
    exit 1
fi

# 6. Run client
./build/src/hg_example_rpc_client "$server_address" 
outid=$?

# 7. Cleanup
kill -9 $server_pid 2>/dev/null || true
rm -f /tmp/hg-stock-${server_pid}-*.txt 2>/dev/null
exit $outid
