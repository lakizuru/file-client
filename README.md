# file-client
A basic client program for the file server - client duo. Developed using C.

Please find the server program repository from [here](https://github.com/lakizuru/file-server)

## How to use
1. Clone this repository into your local Linux environment.
2. Clone the server program repository also.
3. Run server program as follows
```bash
./server
```
4. Run client program as follows.
```bash
./client <action> <IP address> <filename>
```
### Valid "action" keywords:
- push
- pull

### Valid files:
- Any file with file size from 10MB to 100MB
- File should be placed in relavent working directory. (For 'push', file should be in client program working directory. For 'pull', file should be in server program working directory.)

### Example:
```bash
./client push 127.0.0.1 example.mp4
```

### Building:
```bash
make clean client
```
