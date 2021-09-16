# Whisper Checker for Teamspeak 3

Detects whispers from clients which are in your current channel.

## How It Works

* The plugin checks the outbound speech packets of each client in 
your current channel and compares that to whether your client is 
aware of that client talking. If the client that is being checked 
shows outbound speech packets being sent and they have not been 
talking from the perspective of your client, then it is likely 
that they have been whispering.

* As it currently stands, the plugin is functional for it's intended 
purpose. There are quirks and such with it as I only really intended 
to use it personally but it's perfectly usable.


## Getting Started

### Dependencies

* This plugin does have dependencies which do make it windows 
specific although that can probably be pretty easily changed.
* Teamspeak 3 I guess.

### Installing

* After downloading from realease/building the plugin, it must be put in the Teamspeak
plugin folder which by default is located at:
    ``` 
    %APPDATA%\TS3Client\plugins 
    ```
* After adding it to the plugins folder, it can be enabled/disabled through the addons section of the Teamspeak options.


