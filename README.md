# threadless.io

## Concurrent I/O without threads

`threadless.io` will present multiple mechanisms to perform I/O operations concurrently without threads. Eventually, this will include the following:
* Event loops
 * `select`
 * `epoll`
 * `kqueue`
* Asynchronous callback-based I/O
* Psuedo-blocking I/O with coroutines
