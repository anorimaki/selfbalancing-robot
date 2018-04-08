import { NetConfig } from './net-config';
import { BssConfig } from './bss-config';

export interface SoftApConfig {
	enabled: string;
	bss: BssConfig;
	network: NetConfig;
	channel: number,
	mac: string;
}
