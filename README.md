# Blueprint multi-threading
Congratulations. This plugin is designed to use multi-threading in blueprints. I do not claim that this is the best solution, but the plugin can grow and develop with you. I would appreciate your feedback.

Below you will find the functionality.

## Nodes
### Async nodes
Latent nodes have their own regular counterpart so that they can be called in functions.
As you can see, there are 2 types of nodes. 1 has only one output (input), and two outputs (outputs). In the 1st case, it is the launch of logic in the game thread, which is needed to call changes from another thread. In the case of two outputs: one runs the logic in another thread, after this code is executed, the logic in the game thread will be called. Such a node is needed to do some heavy calculations in another thread, and to remove this load from the main thread, and then apply the necessary changes to the game thread.

![image](https://github.com/user-attachments/assets/51fea96c-4a70-4ff0-8b60-64edaafebce7)

### Development/test nodes
The main purpose of these nodes is testing. 
Get thread - displays the name of the thread on which the node was executed (and prints to the log).
Sleep thread - puts the thread to sleep, allows you to simulate a heavy load on the thread, so you can check whether it will affect the game thread.

![image](https://github.com/user-attachments/assets/f49ca58f-3e79-4a33-a969-2004384081cc)

## Class
### Threaded actor component
![image](https://github.com/user-attachments/assets/55a604fb-072f-42ff-872f-9068113bc7de)

On Begin, play creates its own thread that does not affect the game thread in any way. To execute logic in this thread, you need to use a special event. The name of the stream is assigned the name Actor.

![image](https://github.com/user-attachments/assets/92a2c0ed-36e7-4e53-9ecb-ba252a023849)
![image](https://github.com/user-attachments/assets/ce67c40c-c3f9-49c5-946b-7dc771f0dd25)

It also contains a container with Atomic variables for safe data modification.

![image](https://github.com/user-attachments/assets/f5d49535-8e48-4847-aa7c-4435a35fbb85)
