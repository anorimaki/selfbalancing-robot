export interface MpuCalibrationItem {
	x: number;
	y: number;
	z: number;
}

export interface MpuCalibrationData {
	accel: MpuCalibrationItem;
	gyro: MpuCalibrationItem;
}

export interface MpuSettings {
	pitchOffset: number;
}