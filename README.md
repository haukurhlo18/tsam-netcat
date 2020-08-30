# USH (Unsecure shell)

This project is an assignment in T-409-TSAM, which is built with C++.
The main goal of this project is get deeper understanding of how sockets work and how to send messages
through said sockets.

We were given an sample server which you cloud connect to with netcat and send messages to the server.
Using tcpdump you can see the traffic is in plain text.

Our assignment is to create an custom client (like netcat) in C++ where we can connect to our
custom server, same as with netcat.

Using the prefix `SYS ` you can execute system command on the server, which then should be relayed back to client.
Much like SSH, but because there is no encryption for the messages and / or no authentication for the client, therefore
I have dubbed this project USH (Unsecure shell).

## How to build

An make file has been supplied in this repo and this project uses some basic libraries so you should be able to compile
the binaries by running the make file.

In the project root:
```bash
# You can just run the make command, which will default to all section in the Makefile
make
# or be more specific
make all --file Makefile
```

## How to use

You will need to start by getting the server up and running. The binaries have been placed into the `bin` folder.

Starting the server
```bash
# Start the server on <port>, 5000 in this example
bin/server 5000
```

After which you can connect the client to the server, by running `client` program with some basic parameters 
like `netcat`.

```bash
# example: connect the client
bin/client localhost 5000
```

You will need to specify the hostname (first argument) and the port (second argument).
I would recommend running this on an trusted network, because of the unsecure nature of this program.

## client

By default this client will act much like netcat, i.e. send messages to the connected server, by typing in the message
and hitting Enter.
When `SYS ` is prepended to the message then the server will interperet the following message as an system command to be 
run and display the output of the command.

Great example commands to run:

- `SYS ls -lha`
- `SYS who`
- `SYS pwd`
- `SYS mkdir testfolder`
- `SYS ls -lha`
- `SYS rmdir testfolder`

## clean up

Everything this project compiles will be encapsulated within the project root, so after having fun you can just remove 
the project folder from your computer, which will remove everything.

## github

You can see more on the [github page](https://github.com/haukurhlo18/tsam-netcat).
