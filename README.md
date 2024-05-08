## Musicophile

TCP/IP Music streaming server and client
=========================================


```Designed and developed MUSICOPHILE, a multi-threaded client-server music streaming platform, emphasizing concurrent streaming, user authentication, and playback controls. Implemented features such as play, pause, resume, and stop, while managing client connections and network disruptions effectively.

bash
To build server:
make dir build : mkdir build_s
step into build : cd build_s
cmake ..
make musicServer 

To build client:
make dir build : mkdir build_s
step into build : cd build_s
cmake ..
make musicClient 

To run server:
run musicServer : : ./musicServer
The server will wait for clients to connect and then handle incoming song requests.

To run client:
run musicClient : : ./musicClient
The music client will ask for port no to connect to. After that, an attempt to connect is made. If connection is established, 
client will be redirected to user authenication page and after successful authentication the client will be redirected to Musicophile platform, then depending on the genre the client
can request for a song to be played. Music will then be streamed from the server to client. Once a song is playing, you can pause, play,resume,paly forward, backward, change the music 

```

