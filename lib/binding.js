const path = require('path');
const nodeGypBuild = require('node-gyp-build');
const nativeAddon = nodeGypBuild(path.resolve(__dirname, '../'));

/**
 * @typedef {Object} BasicProcessInfo
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
   * Read `int` from virtual memory at addr.
   * @param {BigInt} addr Address
   * @returns {number} Value
   */
  readInt(addr) {
    return this.#nativeProcess.readMemory(addr, 'INT');
  }

  /**
   * Write `int` to virtual memory at addr.
   * @param {BigInt} addr Address
   * @param {number} val Value
   */
  writeInt(addr, val) {
    this.#nativeProcess.writeMemory(addr, 'INT', val);
  }
}

module.exports = {
  getProcesses,
  Process,
};
