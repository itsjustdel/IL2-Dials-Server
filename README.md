<a href="https://docs.microsoft.com/en-us/dotnet/csharp/"><img src="https://img.shields.io/badge/%20-Winforms-purple.svg"/></a>
<a href="https://docs.microsoft.com/en-us/dotnet/csharp/"><img src="https://img.shields.io/badge/%20-C%2B%2B-blue.svg"/></a>

![VFEX3z](https://user-images.githubusercontent.com/45520351/151053750-5e95be44-932c-4e19-a274-a53da8d04cfa.png)

# IL2-Dials-Server
Reverse engineers the flight sim IL-2 Sturmovik and creates a server to send data over UDP. 

This project had a steep learning curve, nearly every aspect of the server side of this project was new to me, I had never programmed in C++, I had never written network code at socket level, and I had never reverse engineered or hacked a game before. Therefore, this project took time, and certainly a few missteps were taken. I had decided to go with C++ because I was going to need to manipulate memory and write externally to other running programs and having a language that was more powerful in this regard would help me, however in choosing Winforms as a GUI I ran in to a multitude of complexities (C++/CLI - another conversation) and in fact, if I had realised how big a task adding a GUI was at all, I would have put less pressure on myself to have a working product as quickly as I had initially hoped.



What I did not know, is that I would need to be able to read and write assembly code (and even compiled x86/x64) to do this. 

## IL-2 Dials Client Github
https://github.com/itsjustdel/IL2-Dials-Client

## Release
https://dellywelly.itch.io/il-2-dials-server


