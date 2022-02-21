const memorykit = require('../lib/binding');
const assert = require('chai').assert;
const spawn = require('child_process').spawn;
const process = require('process');

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
  });

  it('baseAddr has type BigInt', function () {
    assert.isTrue(typeof proc.baseAddr === 'bigint');
  });

  it('Able to read int', function (done) {
    const testProc = spawn('./build/Release/test');
    if (testProc.pid) {
      let proc = new memorykit.Process(testProc.pid);
      testProc.stdout.on('data', function (e) {
        const content = e.toString().split(' ');
        testProc.kill();
        if (content[1] == proc.readMemory(BigInt(content[0]), 'INT')) {
          done();
        } else {
          done(new Error("Value doesn't match"));
        }
      });
    } else {
      done(new Error('Fail to spawn test program'));
    }
  });
});
