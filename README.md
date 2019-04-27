# open62541_Server_Client_Example
Sample Server which simulate Temperature Data. Sample Client to receive that Data.
# Comile Client
gcc -std=c99 open62541.c Client.c -o Client
# Compile Server
gcc -std=c99 -D _BSD_SOURCE -pthread Server.c  open62541.c -o Server
