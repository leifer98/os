# FOR OFRI READ BELOW THIS IS IMPORTANT:
when we do a makefile - we need to make sure TCP ipv4/6 and UDS stream compile with these flags:
-lssl -lcrypto
here is an example:
gcc UDS_stream.c -o UDSS -lssl -lcrypto
## TCP_IPV4 run instructions
Compile createFile file and wait for the loading proccess to finish.
Compile:
```
gcc TCP_ipv4.c -o TCP4
```
Run Server:
```
./TCP4 -s 9999 -p -q
```
Run Client:
```
./TCP4 -c 127.0.0.1 9999 -p sh h
```
