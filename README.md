# Universal-Walking-Simulator
## Firstly, this isn't universal, there are many bugs on each season. I hope to add more versions in the future. <br>This support S3.5-S7.40

# PLANS

To add an auto pattern finder, this is in development, it's easy to make, it's just time consuming.
Possibly add abilities, this will require a bit more patterns unfortunately.


# CURRENT ISSUES

S8+ I am unable to call ProcessEvent for some reason.
Not using beacons causes no ReplicationDriver.
S5-S6 fix is very scuffed (I just reimplemented NotifyControlMessage).

S5+ movement is broken by default, because ServerAcknowledgePossession is stripped for some reason.
My fix was to set the AcknowledgedPawn. But there are more issues, ClientVeryShortAdjustment gets repeatedly called whenever a client goes out of sync.
The head does not rotate up and down, making the mesh look very weird at certain points.

# CONTRIBUTING

Use ->Function if you are calling a function instead of using FindObject.
Test your code.

# CREDITS

Some of this code is from or inspired by <a href="https://github.com/kem0x/raider3.5">Raider by kem0x</a>.
