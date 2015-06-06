#!/bin/sh
while true; do
  ./pidash &
  PID=$!
  inotifywait './pages.txt' -e modify
  kill $PID
done
