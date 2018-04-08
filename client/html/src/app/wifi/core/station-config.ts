import { NetConfig } from './net-config';
import { BssConfig } from './bss-config';

export interface StationConfig {
	status: string;
	bss: BssConfig;
	network: NetConfig;
}
