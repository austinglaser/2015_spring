# 14 Jan 2015 - Concurrency in Java

"Computer Science isn't really about programming, any more than astronomy is about telescopes" - Dijkstra

# Threads

* Each thread is a sequential program
* You can...
..* Tell a thread what to do
..* Tell a thread hen to start
..* Wait for a thread to stop

## In Java

### Creating a thread in Java:
* "Runnable class"
* implement Runnable
* implement run() method
* Can then start() thread

### Syntax
* Single-use classes can be a pain
* Can use anonymous syntax
* Could be more trouble than it's worth
* Caller stuff:
..* t.start() async
..* t.join() waits for thread to finish

# Concurrency Problems
* **Mutual Exclusion**
..* Looked at earlier
* Producer/Consumer
* Reader/Writer

## Formalizing Problem
* Two types of formal properties
* Safety:
..* Nothing bad happens ever
* Liveness:
..* Something good happens eventually

##Producer/Consumer
* One actor produces the resource
* Other actor consumes it
* "Alice has pets, Bob feeds them"
