# memorykit

[![Test](https://github.com/wtongze/memorykit/actions/workflows/test.yaml/badge.svg)](https://github.com/wtongze/memorykit/actions/workflows/test.yaml)

[Documentation](https://memorykit.netlify.app)

A cross-platform JavaScript native library for wrangling virtual memory

```JavaScript
const memoryKit = require('memorykit');

// List all processes
memoryKit.getProcesses();

// Select a processes by PID
const process = new memoryKit.Process(1234);

// Read INT at process base addr
process.readInt(process.baseAddr);

// Remember to release the process handle
process.release();
```
