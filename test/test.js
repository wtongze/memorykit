const memorykit = require('../lib/binding');
const assert = require('chai').assert;
const { test } = require('mocha');

console.log(memorykit);

test('', () => {
  assert.equal;
});

// const code = new memorykit.Process(12689);
// console.log(code, code.native);
// console.log(code.native.getBaseAddr());
// console.log(code.handle, code.baseAddr);
// code.readMemory(code.baseAddr + 0x100, memorykit.TYPE.INT);
