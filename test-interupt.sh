#!/bin/sh
GPIO_PATH="/sys/devices/platform/pinctrl/gpiochip2/gpio/gpio68"
COUNT=0

echo "=== GPIO68 Interrupt Test ==="
echo "Current config:"
echo "  direction: $(cat $GPIO_PATH/direction)"
echo "  edge:      $(cat $GPIO_PATH/edge)"
echo "  value:     $(cat $GPIO_PATH/value)"
echo ""

echo "Monitoring for edges (Ctrl+C to stop)..."
LAST=$(cat $GPIO_PATH/value)

while true; do
    # 使用 poll 等待变化（需要 dd 或 read）
    dd if=$GPIO_PATH/value bs=1 count=1 2>/dev/null | grep -q .
    CURRENT=$(cat $GPIO_PATH/value)
    
    if [ "$CURRENT" != "$LAST" ]; then
        COUNT=$((COUNT + 1))
        echo "$(date '+%H:%M:%S.%3N') Interrupt #$COUNT: $LAST -> $CURRENT"
        LAST=$CURRENT
    fi
done
