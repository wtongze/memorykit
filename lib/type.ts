import { MEMORY_TYPE } from './const';

export interface ProcessIdentifier {
  pid: number;
  name: string;
}

export interface BasicProcessInfo {
  pid: number;
  ppid: number | null;
  name: string | null;
}

export interface ProcessInfo extends BasicProcessInfo {
  baseAddr: number;
  handle?: number;
}

export type MemoryType = keyof typeof MEMORY_TYPE;
