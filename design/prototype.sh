#!/bin/bash

width=1280
height=720
framerate=24
host=192.168.1.221
port=5248

gst-launch-1.0 v4l2src \
  ! video/x-h264,width=$width,height=$height,framerate=$framerate/1 \
  ! h264parse \
  ! rtph264pay config-interval=1 pt=96 \
  ! udpsink sync=false host=$host port=$port
