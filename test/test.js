const memorykit = require('../lib/binding');
const assert = require('chai').assert;
const spawn = require('child_process').spawn;
const process = require('process');

describe('getProcesses', function () {
  it('Exports such function', function () {
    assert.exists(memorykit.getProcesses);
  });
  it('Return list of processes', function () {
    const list = memorykit.getProcesses();
    assert.isAbove(list.length, 0);
  });
});

describe('Process', function () {
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
  it('Has getBaseAddr', function () {
    console.log(proc.baseAddr);
    assert.exists(proc.baseAddr);
  });

  it('baseAddr has type BigInt', function () {
    assert.isTrue(typeof proc.baseAddr === 'bigint');
  });
});

describe('Process.readInt', function () {
  it('Able to read', function (done) {
    const testProc = spawn('./build/Release/test');
    if (testProc.pid) {
      let proc = new memorykit.Process(testProc.pid);
      testProc.stdout.on('data', function (e) {
        const content = e.toString().split(' ');
        const addr = BigInt(content[0]);
        const target = parseInt(content[1]);

        const value = proc.readInt(addr);
        testProc.kill();
        if (target === value) {
          done();
        } else {
          done(new Error("Value doesn't match"));
        }
      });
    } else {
      done(new Error('Fail to spawn test program'));
    }
  });
  it('Able to write', function (done) {
    const testProc = spawn('./build/Release/test');
    if (testProc.pid) {
      let proc = new memorykit.Process(testProc.pid);
      let ready = false;
      const target = -1;
      testProc.stdout.on('data', function (e) {
        const content = e.toString().split(' ');
        const addr = BigInt(content[0]);
        if (!ready) {
          proc.writeInt(addr, target);
          ready = true;
        } else {
          const curr = parseInt(content[1]);
          testProc.kill();
          if (curr - 1 === target) {
            done();
          } else {
            done(new Error("Value doesn't match"));
          }
        }
      });
    } else {
      done(new Error('Fail to spawn test program'));
    }
  });
});

describe('Process.readString', function () {
  it('Able to read', function (done) {
    const testProc = spawn('./build/Release/test');
    if (testProc.pid) {
      let proc = new memorykit.Process(testProc.pid);
      testProc.stdout.on('data', function (e) {
        const content = e.toString().split(' ');
        const addr = BigInt(content[2]);
        const target = content[3];

        const value = proc.readString(addr);
        testProc.kill();
        if (target === value) {
          done();
        } else {
          console.log(target, value);
          done(new Error("Value doesn't match"));
        }
      });
    } else {
      done(new Error('Fail to spawn test program'));
    }
  });
});
