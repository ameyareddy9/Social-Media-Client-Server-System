# Social-Media-Client-Server-System

A lightweight, terminal-based Client-Server architecture simulating core social media functionalities (like Instagram/Twitter). The system allows users to post updates, follow/unfollow other users, and generate a chronologically sorted news feed. 

It is built using a **C++ TCP Server** handling the backend logic and data storage, and a **PowerShell Interactive Client** for the user interface.

---

## Features
* **POST**: Publish a new post/tweet associated with a User ID.
* **FOLLOW**: Subscribe to another user's updates.
* **UNFOLLOW**: Unsubscribe from another user's updates.
* **FEED**: Fetch the 10 most recent posts from the user and the people they follow, sorted chronologically.

---

## Data Structures and Algorithms (DSA) Highlights

The backend is heavily optimized to process social media actions efficiently. Here are the core data structures and algorithms powering the server:

### 1. Hash Maps (std::unordered_map)
* **Posts Storage:** `unordered_map<int, vector<pair<int,int>>> tweets`
  * Maps a `userId` directly to a list of their posts. Each post is stored as a `(timestamp, tweetId)` pair.
  * *Why?* Allows **O(1)** average time complexity when fetching a specific user's complete post history.
* **Social Graph Storage:** `unordered_map<int, unordered_set<int>> following`
  * Maps a `userId` to a collection of users they follow.

### 2. Hash Sets (std::unordered_set)
* **Following List:** Inside the social graph, followees are stored in a Hash Set.
  * *Why?* Ensures that duplicate follows are ignored automatically and provides **O(1)** average time complexity for both `follow` (insertion) and `unfollow` (deletion) operations.

### 3. Min-Heap / Priority Queue (std::priority_queue)
* **News Feed Generation:** The `getNewsFeed` function uses a Min-Heap (configured with `greater<>`) to merge and sort the most recent posts from the user and their followees.
  * *The Algorithm:* As the server iterates through the posts of the user and their followees, it pushes them into the Min-Heap. If the heap size exceeds 10, the smallest (oldest) timestamp is popped off. 
  * *Why?* Sorting all posts would take `O(N log N)` time. By using a bounded Min-Heap of size K (where K = 10), the time complexity drops to **O(N log K)**. Since K is a constant (10), this effectively runs in **O(N)** time, making feed generation lightning-fast even with thousands of posts.

### 4. Concurrency Control (std::mutex)
* Thread-safety is implemented using a mutex lock (`lock_guard<mutex> lock(mtx)`). This ensures that if multiple clients attempt to post or alter the social graph simultaneously, the underlying data structures will not suffer from race conditions or memory corruption.

---

## How the Client and Server Work

### The Backend (C++ Server)
The server leverages **WinSock2** to open a TCP socket on port 8080. 
1. **Listening:** It binds to `127.0.0.1:8080` and enters an infinite loop, continuously listening for incoming connections.
2. **Processing:** When a connection is accepted, it reads the data packet into a buffer, parses the string into commands (`POST`, `FOLLOW`, etc.), and routes it to the `Instagram` class.
3. **Responding:** The server processes the DSA logic, converts the result into a string, sends it back over the socket, and then gracefully closes that specific client connection, ready for the next one.

### The Frontend (PowerShell Client)
The client provides a clean, interactive command-line interface (CLI).
1. **Interactive Loop:** It prompts the user for inputs using `Read-Host`.
2. **Ephemeral Connections:** For every command entered, the PowerShell script dynamically creates a new `TcpClient` connection to `127.0.0.1:8080`.
3. **Data Transmission:** It writes the command to the network stream, waits for the server's response, prints it to the console in green text, and immediately closes the socket. This ensures the client is lightweight and doesn't hold open connections unnecessarily.

---
