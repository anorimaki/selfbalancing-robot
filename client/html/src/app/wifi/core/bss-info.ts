
export enum AuthMode {
    Open = 0,
    Wep,
    WpaPsk,
    Wpa2Psk,
    WpaWpa2Psk,
    Max
}

export interface BssInfo {
	ssid: string;
    channel: number;
    auth: AuthMode;
    rssi: number;
}
