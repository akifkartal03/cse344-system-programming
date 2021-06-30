### Implement one server multiple client paradigm with Socket Programming.
### Critical Points

- Clients will send a database query(i.e "Select * From Table") and server will respond the result.
- When a new connection arrives, server will delagate that task to a thread from thread pool.
- Threads will get result by considering reader-writer problem(write queries will be privileged).
- Server will be a deamon process.
- All IPC will be provided by POSIX monitors(condition variable with mutex).

