const path = require('path');
const nodeGypBuild = require('node-gyp-build');
const nativeAddon = nodeGypBuild(path.resolve(__dirname, '../'));

/**
 * Enum for supported types.
 * @readonly
 * @private
 * @enum {string}
 */
const TYPE = {
  BYTE: 'BYTE',
  INT: 'INT',
  INT32: 'INT32',
  UINT32: 'UINT32',
  INT64: 'INT64',
  UINT64: 'UINT64',
  SHORT: 'SHORT',
  LONG: 'LONG',
  FLOAT: 'FLOAT',
  DOUBLE: 'DOUBLE',
  POINTER: 'POINTER',
  BOOL: 'BOOL',
  DWORD: 'DWORD',
  STRING: 'STRING',
};

/**
 * @typedef {number | BigInt} Addr Address
 */

/**
 * @typedef {Object} BasicProcessInfo Basic Process Info
 * @property {number} pid PID
 * @property {number | undefined} ppid PPID
 * @property {string} name Name
 */

/**
 * Get basic information about all the processes that currently running
 * *Note: `root` is required on Linux and MacOS.*
 * @returns {Array<BasicProcessInfo>}
 */
function getProcesses() {
  return nativeAddon.getProcesses();
}

/**
 * Process class
 */
class Process {
  /**
   * For internal use only.
   */
  #nativeProcess;

  /**
   * Base Address of the Process
   * @readonly
   * @returns {BigInt} Base Address
   */
  get baseAddr() {
    return this.#nativeProcess.getBaseAddr();
  }

  /**
   * Create a new Process by PID
   * @param {number} pid PID
   */
  constructor(pid) {
    if (typeof pid === 'number') {
      if (
        getProcesses()
          .map((i) => i.pid)
          .includes(pid)
      ) {
        this.#nativeProcess = new nativeAddon.Process(pid);
      } else {
        throw new Error(`Process with PID ${pid} not found`);
      }
    } else {
      throw new Error('PID is expected to create Process');
    }
  }

  /**
   * Read `byte` from virtual memory at addr.
   * @param {Addr} addr Address
   * @returns {number} Value
   */
  readByte(addr) {
    return this.#nativeProcess.readMemory(addr, TYPE.BYTE);
  }

  /**
   * Read `int` from virtual memory at addr.
   * @param {Addr} addr Address
   * @returns {number} Value
   */
  readInt(addr) {
    return this.#nativeProcess.readMemory(addr, TYPE.INT);
  }

  /**
   * Read `int32` from virtual memory at addr.
   * @param {Addr} addr Address
   * @returns {number} Result
   */
  readInt32(addr) {
    return this.#nativeProcess.readMemory(addr, TYPE.INT32);
  }

  /**
   * Read `uint32` from virtual memory at addr.
   * @param {Addr} addr Address
   * @returns {number} Result
   */
  readUint32(addr) {
    return this.#nativeProcess.readMemory(addr, TYPE.UINT32);
  }

  /**
   * Read `int64` from virtual memory at addr.
   * @param {Addr} addr Address
   * @returns {BigInt} Result
   */
  readInt64(addr) {
    return this.#nativeProcess.readMemory(addr, TYPE.INT64);
  }

  /**
   * Read `uint64` from virtual memory at addr.
   * @param {Addr} addr Address
   * @returns {BigInt} Result
   */
  readUint64(addr) {
    return this.#nativeProcess.readMemory(addr, TYPE.UINT64);
  }

  /**
   * Read `short` from virtual memory at addr.
   * @param {Addr} addr Address
   * @returns {number} Result
   */
  readShort(addr) {
    return this.#nativeProcess.readMemory(addr, TYPE.SHORT);
  }

  /**
   * Read `long` from virtual memory at addr.
   * @param {Addr} addr Address
   * @returns {BigInt} Result
   */
  readLong(addr) {
    return this.#nativeProcess.readMemory(addr, TYPE.LONG);
  }

  /**
   * Read `float` from virtual memory at addr.
   * @param {Addr} addr Address
   * @returns {number} Result
   */
  readFloat(addr) {
    return this.#nativeProcess.readMemory(addr, TYPE.FLOAT);
  }

  /**
   * Read `double` from virtual memory at addr.
   * @param {Addr} addr Address
   * @returns {number} Result
   */
  readDouble(addr) {
    return this.#nativeProcess.readMemory(addr, TYPE.DOUBLE);
  }

  /**
   * Read pointer from virtual memory at addr.
   * @param {Addr} addr Address
   * @returns {BigInt} Result
   */
  readPointer(addr) {
    return this.#nativeProcess.readMemory(addr, TYPE.POINTER);
  }

  /**
   * Read `bool` from virtual memory at addr.
   * @param {Addr} addr Address
   * @returns {boolean} Result
   */
  readBool(addr) {
    return this.#nativeProcess.readMemory(addr, TYPE.BOOL);
  }

  /**
   * Read `DWORD` from virtual memory at addr.
   * @param {Addr} addr Address
   * @returns {BigInt} Result
   */
  readDword(addr) {
    return this.#nativeProcess.readMemory(addr, TYPE.DWORD);
  }

  /**
   * Read string from virtual memory at addr.
   * @param {Addr} addr Address
   * @returns {string} Value
   */
  readString(addr) {
    return this.#nativeProcess.readMemory(addr, TYPE.STRING);
  }

  /**
   * Write `int` to virtual memory at addr.
   * @param {BigInt} addr Address
   * @param {number} val Value
   */
  writeInt(addr, val) {
    this.#nativeProcess.writeMemory(addr, TYPE.INT, val);
  }

  /**
   * Write `string` to virtual memory at addr.
   * @param {BigInt} addr Address
   * @param {string} val Value
   */
   writeString(addr, val) {
    this.#nativeProcess.writeMemory(addr, TYPE.STRING, val);
  }

  /**
   * Release process handle / task port
   */
  release() {
    this.#nativeProcess.release();
  }

  /**
   * Check if process has released handle / task port
   * @returns {bool} Result
   */
  hasReleased() {
    return this.#nativeProcess.hasReleased();
  }
}

module.exports = {
  getProcesses,
  Process,
};
