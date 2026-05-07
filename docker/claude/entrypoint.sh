#!/usr/bin/env bash
set -euo pipefail

mkdir -p /root/.claude
cd /workspace

echo "Workspace: $(pwd)"
echo "Claude config: /root/.claude"
echo "HTTP_PROXY=${HTTP_PROXY:-<unset>}"
echo "HTTPS_PROXY=${HTTPS_PROXY:-<unset>}"

exec "$@"