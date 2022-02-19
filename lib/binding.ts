import path from 'path';
import process from 'process';
import os from 'os';
// @ts-ignore
import nodeGypBuild from 'node-gyp-build';
import { BasicProcessInfo, ProcessIdentifier, MemoryType } from './type';
import { MEMORY_TYPE, CONST } from './const';

const addon = nodeGypBuild(path.resolve(__dirname, '../../'));

function checkSupportedOS() {
  if (!CONST.SUPPORTED_PLATFORM.includes(os.platform())) {
    throw new Error(`MemoryKit currently doesn't support ${os.platform()}`);
  }
}

function getProcesses(): BasicProcessInfo[] {
  checkSupportedOS();
  return addon.getProcesses();
}

class Process {
  private handle?: number;

  constructor(id: ProcessIdentifier) {
    checkSupportedOS();
    //
  }

  readMemory(address: number, type: MemoryType) {}
  writeMemory(address: number, type: MemoryType, value: any) {}

  close() {}
}

export default {
  getProcesses,
  Process,
  MEMORY_TYPE
};
