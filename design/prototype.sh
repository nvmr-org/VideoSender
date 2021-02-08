#!/bin/bash

width=1280
height=720
framerate=24
host=224.1.1.3
port=8230

gst-launch-1.0 v4l2src \
  ! video/x-h264,width=$width,height=$height,framerate=$framerate/1 \
  ! rtph264pay config-interval=1 pt=96 \
  ! udpsink sync=false host=$host port=$port auto-multicast=true
