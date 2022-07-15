# "Universal" Walking Simulator
## Firstly, this isn't universal, there are many bugs on each season. I hope to add more versions in the future. <br>This support S3.5-S7.40

# PLANS

I want to add an auto pattern finder, this is in development, it's easy to make, it's just time consuming.<br>
Possibly add abilities, this will require a bit more patterns unfortunately.<br>
I need to do some more testing to add inventory.<br>
Building is easy, but requires inventory.<br>

# CURRENT ISSUES

S8+ I am unable to call ProcessEvent for some reason.<br>
Not using beacons causes no ReplicationDriver.<br>
S5-S6 fix is very scuffed (I just reimplemented NotifyControlMessage).<br>

S5+ movement is broken by default, because ServerAcknowledgePossession is stripped for some reason.<br>
My fix was to set the AcknowledgedPawn. But there are more issues, ClientVeryShortAdjustment gets repeatedly called whenever a client goes out of sync.<br>
The head does not rotate up and down, making the mesh look very weird at certain points.<br>

Code is everywhere.<br>
Console Colors are a 50/50.<br>

# CONTRIBUTING

In order to find a UFunction, please use Object->Function instead of FindObject("Function ..").<br>
Test your code.<br>

# CREDITS

If you are to use this base for your own project, please credit me and link the repository.

Some of this code is from or inspired by <a href="https://github.com/kem0x/raider3.5">Raider by kem0x</a>.
