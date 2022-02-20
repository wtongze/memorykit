const memorykit = require('../lib/binding');
const assert = require('chai').assert;
const { test } = require('mocha');

test('Return list of processes', () => {
  const list = memorykit.getProcesses();
  assert.isAbove(list.length, 0);
});

// const code = new memorykit.Process(12689);
// console.log(code, code.native);
// console.log(code.native.getBaseAddr());
// console.log(code.handle, code.baseAddr);
// code.readMemory(code.baseAddr + 0x100, memorykit.TYPE.INT);
