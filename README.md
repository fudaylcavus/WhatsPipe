# WhatsPipe
> Developers: [*Fudayl Çavuş*](https://www.linkedin.com/in/fudaylcavus/) | [*Mahmut Sacit Meydanal*](https://www.linkedin.com/in/mahmut-sacit-meydanal-110271231/) 

![whatspipelogo](https://user-images.githubusercontent.com/23340400/201096396-44077b66-d844-42d7-992a-7a07e9cb8ed3.png)



### Used
 * Pipes
 * Signal and Signal Handling
 * Threads
 * Exec And Fork
 * Child Processes

_No external libraries are used_


### Features

#### Client
* can change target anytime
* can get notification if target user is offline
* can get notification whenever target goes offline
* can get notification even when the sender is someone else
* can store messages (on memory)
* can track all unread messages
* has custom and responsive design

#### Server
* can handle multiple clients at a time
* can handle the change of user status
* can trigger client whenever it's needed (by signals)
* can handle duplicate names

### Demonstration

![](https://i.ibb.co/GMhMxxL/ezgif-1-d00a3866da.gif)

![](https://i.ibb.co/kx306pp/ezgif-1-5d255f7000-1.gif)


### How to run on your machine
 **UNIX BASED OS REQUIRED** 

Open terminal on cloned folder then do the following steps;
#### Client-Side Compilation
`g++ -I./include -o client src/client.cpp src/screen.cpp src/events.cpp`
#### Server-Side Compilation
`g++ -I./include -o server src/server.cpp`

###### then run `./server` on one terminal, and `./client` on 2 seperate terminals, voila! You are ready to go!

### Special Thanks To [*Muhammet Cinsdikici*](https://www.linkedin.com/in/muhammed-cinsdikici-a6575a20/) for his contributions.
