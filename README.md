# prf

<a href="http://en.wikipedia.org/wiki/For_No_Reason_at_All_in_C">
  <img src="./data/Parlophfornoreason.jpg" height="260" width="266" alt="For Nor Reason At All In C" title="Well, C is kind of fun, actually."/>
</a>

## Play Random File

This is a C project to give me some practice with the file system interface.

1. Given a list of directories in a search path, or a glob pattern that results in such a list, find a random file with an acceptable extension that hasn't been played recently..
2. Play the file in an associated player.
3. Kill the player when the file is done.

The search path, list of file extensions, and player associations are all configurable from the command line or a file at standard location $HOME/.prfrc
