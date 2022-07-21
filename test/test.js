const memorykit = require('../lib/memorykit');
const assert = require('chai').assert;
const spawn = require('child_process').spawn;
const process = require('process');
const os = require('os');

const testProgramPath = './build/Release/test';

function getAddr(str) {
  return BigInt(os.platform() === 'win32' ? '0x' + str : str);
}

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
    current.release();
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

  it('Has getBaseAddr', function () {
    const proc = new memorykit.Process(process.pid);
    assert(proc.baseAddr > -1);
    assert.exists(proc.baseAddr);
    proc.release();
  });

  it('baseAddr has type BigInt', function () {
    const proc = new memorykit.Process(process.pid);
    assert.isTrue(typeof proc.baseAddr === 'bigint');
    proc.release();
  });

  it('Has hasReleased', function () {
    const proc = new memorykit.Process(process.pid);
    assert(!proc.hasReleased());
    proc.release();
    assert(proc.hasReleased());
  });
});

describe('INT', function () {
  it('Able to read', function (done) {
    const testProc = spawn(testProgramPath);
    if (testProc.pid) {
      let proc = new memorykit.Process(testProc.pid);
      testProc.stdout.on('data', function (e) {
        const content = e.toString().split(' ');
        const addr = getAddr(content[0]);
        const target = parseInt(content[1]);

        const value = proc.readInt(addr);
        testProc.kill();
        proc.release();
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
    const testProc = spawn(testProgramPath);
    if (testProc.pid) {
      let proc = new memorykit.Process(testProc.pid);
      let ready = false;
      const target = -1;
      testProc.stdout.on('data', function (e) {
        const content = e.toString().split(' ');
        const addr = getAddr(content[0]);
        if (!ready) {
          proc.writeInt(addr, target);
          ready = true;
        } else {
          const curr = parseInt(content[1]);
          testProc.kill();
          proc.release();
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

describe('STRING', function () {
  it('Able to read', function (done) {
    const testProc = spawn(testProgramPath);
    if (testProc.pid) {
      let proc = new memorykit.Process(testProc.pid);
      testProc.stdout.on('data', function (e) {
        const content = e.toString().split(' ');
        const addr = getAddr(content[2]);
        const target = content[3];

        const value = proc.readString(addr);
        testProc.kill();
        proc.release();
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

  it('Able to write', function (done) {
    const testProc = spawn(testProgramPath);
    if (testProc.pid) {
      let proc = new memorykit.Process(testProc.pid);
      let ready = false;
      const target = 'dcba';
      testProc.stdout.on('data', function (e) {
        const content = e.toString().split(' ');
        const addr = getAddr(content[2]);
        if (!ready) {
          proc.writeString(addr, target);
          ready = true;
        } else {
          const curr = content[3];
          testProc.kill();
          proc.release();
          if (curr === target) {
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
