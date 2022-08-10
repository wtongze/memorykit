# memorykit

[![Test](https://github.com/wtongze/memorykit/actions/workflows/test.yaml/badge.svg)](https://github.com/wtongze/memorykit/actions/workflows/test.yaml)

[Documentation](https://memorykit.netlify.app)

A cross-platform JavaScript native library for wrangling virtual memory

## Example

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

## Install

```bash
npm i -g node-gyp prebuildify # For compiling
npm i memorykit
```

## Supported System

- Windows
- Linux
- MacOS*

**Due to operating system policy, wrangling memory of a process that uses hardened runtime is not supported and you must run your code as `root`.*

## Special Thanks
<https://github.com/Rob--/memoryjs> for providing an implementation example on Windows.
