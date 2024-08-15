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

### ParallelFor

Implementation of Parallel For has been added, which will allow you to perform calculations for a large amount of data by spreading the load across different threads.

![image](https://github.com/user-attachments/assets/ee7608c4-7caa-4260-96c1-cb93ca2a1309)

## Class
### Threaded actor component

![image](https://github.com/user-attachments/assets/55a604fb-072f-42ff-872f-9068113bc7de)

On Begin, play creates its own thread that does not affect the game thread in any way. To execute logic in this thread, you need to use a special event. The name of the stream is assigned the name Actor.

![image](https://github.com/user-attachments/assets/92a2c0ed-36e7-4e53-9ecb-ba252a023849)
![image](https://github.com/user-attachments/assets/ce67c40c-c3f9-49c5-946b-7dc771f0dd25)

### Atomic container

New nodes have been created that allow you to create absolutely any atomics variables. They automatically pull up the variable type when you connect it.
It also has Queue, a thread safe linked list.

![image](https://github.com/user-attachments/assets/0f38f2d3-884f-48fa-a7ba-8cd44b56b117)

### Thread pool

allows you to create a pool of a certain number of threads, and then add tasks that will be executed on these threads depending on their load.

![image](https://github.com/user-attachments/assets/d734b799-0b4a-4b5e-852f-ad9c0e243406)

