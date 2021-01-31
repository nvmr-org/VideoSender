# NVMR Video Sender

Reads video from the RPi, and transmits over the network.

Requirements:
* Change settings via master application so you don't need to remote in
* Registers with avahi/zeroconf so we can find it on the network

Configuration parameters:
* video-id: The ID of the video that this is sending out
* video-name: The name of the video that this is sending out
* device-uuid: The UUID of this device.  Not configurable; auto-generated on
 startup if it doesn't exist.  This is to allow for misconfiguration
 if two units have the same video-id
* ip-dest: the desination IP address of the video.  Should be a multicast address
* ip-port: the port to send the video to

Communication:
* Listen on port 9036 for websocket connections
* send JSON data back and forth

JSON data:
```
{
  command: "query", // Optional, command to do.
  configuration: {
    device-uuid:555-90123, // <-- Can only be received from device, cannot be set

    video: {
      id:5,
      name:foobar,
      width:1280,
      height:800,
      config-interval:96,
      pt:1,
      framerate:24
    },

    network: {
      udp-host:224.1.2.9,
      udp-port:8230
    }
  }
}
```

Available commands:
* query - query the current vidoe settings
* restart - restart this video sender application(exit with code 2)
* set - Update the current settings with the new settings that we are giving now
