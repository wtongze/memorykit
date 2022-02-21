const memorykit = require('../lib/binding');
const process = require('process');
const assert = require('chai').assert;

describe('getProcesses()', function () {
  it('Exports such function', function () {
    assert.exists(memorykit.getProcesses);
  });
  it('Return list of processes', function () {
    const list = memorykit.getProcesses();
    assert.isAbove(list.length, 0);
  });
});

describe('class Process', function () {
  it('Exports such class', function () {
    assert.exists(memorykit.Process);
  });

  it('Create Process by PID', function () {
    const current = new memorykit.Process(process.pid);
    assert.exists(current);
  });

  it("Can't create Process by string", function () {
    assert.throw(function () {
      new memorykit.Process('aaaa');
    });
  });

  it("Can't create Process if PID not found", function () {
    assert.throw(function () {
      new memorykit.Process(-1024);
    });
  });

  const proc = new memorykit.Process(process.pid);
  it('Has getBaseAddr()', function () {
    assert.exists(proc.baseAddr);
  })
});

// const code = new memorykit.Process(12689);
// console.log(code, code.native);
// console.log(code.native.getBaseAddr());
// console.log(code.handle, code.baseAddr);
// code.readMemory(code.baseAddr + 0x100, memorykit.TYPE.INT);
