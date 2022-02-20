const path = require('path');
const nodeGypBuild = require('node-gyp-build');
const nativeAddon = nodeGypBuild(path.resolve(__dirname, '../'));
const CONST = require('./const');

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

module.exports = {
  getProcesses,
  ...CONST,
};
