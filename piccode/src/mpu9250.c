#include "main.h"
#include "mpu9250.h"
#include "mpu_i2c.h"


//MPU9250 includes an AK8963 & MPU6500
#define AK8963_SECONDARY
#define AK89xx_SECONDARY
#define MPU6500

#define MPU_MAX_FIFO			1024

#define MPU_NUM_REG				118

#define MPU_REG_WHO_AM_I		0x75
#define MPU_REG_PWR_MGMT_1		0x6B
#define MPU_REG_PWR_MGMT_2		0x6C
#define MPU_REG_GYRO_CFG		0x1B
#define MPU_REG_ACCEL_CFG		0x1C
#define MPU_REG_LPF				0x1A
#define MPU_REG_RATE_DIV		0x19
#define MPU_REG_INT_ENABLE		0x38
#define MPU_REG_FIFO_EN			0x23
#define MPU_REG_USER_CTRL		0x6A
#define MPU_REG_INT_PIN_CFG		0x37
#define MPU_REG_ACCEL_CFG2		0x1D
#define MPU_REG_RAW_ACCEL		0x3B
#define MPU_REG_PRGM_START_H    0x70
#define MPU_REG_BANK_SEL        0x6D
#define MPU_REG_MEM_R_W         0x6F
#define MPU_REG_I2C_MST			0x24
#define MPU_REG_FIFO_COUNTH		0x72
#define MPU_REG_FIFO_COUNTL		0x73
#define MPU_REG_FIFO_R_W		0x74
#define MPU_REG_DPM_INT_STATUS	0x39
#define MPU_REG_INT_STATUS		0x3A
#ifdef AK89xx_SECONDARY
#define MPU_REG_S0_REG			0x26
#define MPU_REG_S0_ADDR			0x25
#define MPU_REG_S0_CTRL			0x27
#define MPU_REG_S1_DO			0x64
#define MPU_REG_S1_ADDR			0x28
#define MPU_REG_S1_REG			0x29
#define MPU_REG_S1_CTRL			0x2A
#define MPU_REG_I2C_DELAY_CTRL	0x67
#define MPU_REG_S4_CTRL			0x34
#endif

#define MPU_S_ADDR_I2C_READ		(0x80)
#define MPU_S_CTRL_SLAVE_EN		(0x80)

#define MPU_POWER_RESET			0x80
#define MPU_POWER_CLK_INTERNAL	0x00
#define MPU_POWER_CLK_PLL		0x01
#define MPU_POWER_SLEEP         (0x40)

#define MPU_POWER_STBY_XA         (0x20)
#define MPU_POWER_STBY_YA         (0x10)
#define MPU_POWER_STBY_ZA         (0x08)
#define MPU_POWER_STBY_XG         (0x04)
#define MPU_POWER_STBY_YG         (0x02)
#define MPU_POWER_STBY_ZG         (0x01)
#define MPU_POWER_STBY_XYZA       (MPU_POWER_STBY_XA | MPU_POWER_STBY_YA | MPU_POWER_STBY_ZA)
#define MPU_POWER_STBY_XYZG       (MPU_POWER_STBY_XG | MPU_POWER_STBY_YG | MPU_POWER_STBY_ZG)

#define MPU_INT_PIN_ACTL            (0x80)
#define MPU_INT_PIN_LATCH_EN        (0x20)
#define MPU_INT_PIN_ANY_RD_CLR      (0x10)
#define MPU_INT_PIN_BYPASS_EN       (0x02)

#define MPU_INT_DATA_RDY_EN     (0x01)
#define MPU_INT_DMP_INT_EN      (0x02)
#define MPU_INT_MOT_INT_EN      (0x40)

#define MPU_INT_STATUS_FIFO_OVERFLOW   (0x10)

#define MPU_USER_FIFO_EN         (0x40)
#define MPU_USER_DMP_EN          (0x80)
#define MPU_USER_FIFO_RST        (0x04)
#define MPU_USER_DMP_RST         (0x08)
#define MPU_USER_AUX_IF_EN       (0x20)

#define MPU_ACCEL_FIFO_SIZE_1024  (0x40)
#define MPU_ACCEL_FIFO_SIZE_2048  (0x80)
#define MPU_ACCEL_FIFO_SIZE_4096  (0xC0)

#if defined AK8975_SECONDARY
#define SUPPORTS_AK89xx_HIGH_SENS   (0x00)
#define AK89xx_FSR                  (9830)
#elif defined AK8963_SECONDARY
#define SUPPORTS_AK89xx_HIGH_SENS   (0x10)
#define AK89xx_FSR                  (4915)
#endif

#if defined(AK89xx_SECONDARY)

#define MPU_AKM_REG_WHOAMI      (0x00)

#define MPU_AKM_REG_ST1         (0x02)
#define MPU_AKM_REG_HXL         (0x03)
#define MPU_AKM_REG_ST2         (0x09)

#define MPU_AKM_REG_CNTL        (0x0A)
#define MPU_AKM_REG_ASTC        (0x0C)
#define MPU_AKM_REG_ASAX        (0x10)
#define MPU_AKM_REG_ASAY        (0x11)
#define MPU_AKM_REG_ASAZ        (0x12)

#define MPU_AKM_DATA_READY      (0x01)
#define MPU_AKM_DATA_OVERRUN    (0x02)
#define MPU_AKM_OVERFLOW        (0x80)
#define MPU_AKM_DATA_ERROR      (0x40)

#define MPU_AKM_BIT_SELF_TEST   (0x40)

#define MPU_AKM_POWER_DOWN          (0x00 | SUPPORTS_AK89xx_HIGH_SENS)
#define MPU_AKM_SINGLE_MEASUREMENT  (0x01 | SUPPORTS_AK89xx_HIGH_SENS)
#define MPU_AKM_FUSE_ROM_ACCESS     (0x0F | SUPPORTS_AK89xx_HIGH_SENS)
#define MPU_AKM_MODE_SELF_TEST      (0x08 | SUPPORTS_AK89xx_HIGH_SENS)

#define MPU_AKM_WHOAMI      (0x48)

#endif //AK89xx_SECONDARY

#define dpm_log_i(format, ...) printf(format, __VA_ARGS__)

struct 
{
	int1 dmp_on;
	int8 int_enable;
	int1 latched_int;
	int8 fifo_enable;
	int8 sensors;
	int16 sample_rate;
	int16 dmp_sample_rate;
	
#ifdef AK89xx_SECONDARY
    int8 compass_addr;
    int16 mag_sens_adj[3];
#endif	
} mpu;

static void set_int_enable( int1 enable );

#ifdef AK89xx_SECONDARY
static void setup_compass();
#define MAX_COMPASS_SAMPLE_RATE (100)
#endif

/**
 *  @brief      Initialize hardware.
 *  Initial configuration:\n
 *  Gyro FSR: +/- 2000DPS\n
 *  Accel FSR +/- 2G\n
 *  DLPF: 42Hz\n
 *  FIFO rate: 50Hz\n
 *  Clock source: Gyro PLL\n
 *  FIFO: Disabled.\n
 *  Data ready interrupt: Disabled, active low, unlatched.
 *  @param[in]  int_param   Platform-specific parameters to interrupt API.
 *  @return     0 if successful.
 */
void mpu_init()
{
	mpu_comm_init();
	
	mpu.dmp_on = 0;
	
    /* Reset device. */
    mpu_write( MPU_I2C_ADDR, MPU_REG_PWR_MGMT_1, MPU_POWER_RESET );
    delay_ms(100);

    /* Wake up chip. */
	mpu_write( MPU_I2C_ADDR, MPU_REG_PWR_MGMT_1, 0 );

#ifdef MPU6500
    /* MPU6500 shares 4kB of memory between the DMP and the FIFO. Since the
     * first 3kB are needed by the DMP, we'll use the last 1kB for the FIFO.
     */
	mpu_write( MPU_I2C_ADDR, MPU_REG_ACCEL_CFG2, MPU_ACCEL_FIFO_SIZE_1024 | 0x8 );
#endif
	
	/* Set to invalid values: Will be initialized in each function */
    mpu.sensors = 0xFF;
	mpu.fifo_enable = 0xFF;
    mpu.dmp_on = 0;
	mpu.sample_rate = 0xFFFF;
	mpu.dmp_sample_rate = 0xFFFF;

	mpu_set_gyro_fsr( MPU_FSR_2000DPS );
    mpu_set_accel_fsr( MPU_FSR_2G );
    mpu_set_lpf( 42 );
    mpu_set_sample_rate(50);
    mpu_configure_fifo(0);
	
#ifdef AK89xx_SECONDARY
    setup_compass();
    mpu_set_compass_sample_rate(10);
#else
    /* Already disabled by setup_compass. */
    mpu_set_bypass(0);
#endif
	
	mpu_set_sensors(0);
}


/**
 *  @brief      Set the gyro full-scale range.
 *  @param[in]  fsr Desired full-scale range.
 *  @return     0 if successful.
 */
void mpu_set_gyro_fsr( int8 fsr )
{
	mpu_write( MPU_I2C_ADDR, MPU_REG_GYRO_CFG, fsr );
}


/**
 *  @brief      Set the accel full-scale range.
 *  @param[in]  fsr Desired full-scale range.
 *  @return     0 if successful.
 */
void mpu_set_accel_fsr( int8 fsr )
{
	mpu_write( MPU_I2C_ADDR, MPU_REG_ACCEL_CFG, fsr );
}


/**
 *  @brief      Set digital low pass filter.
 *  The following LPF settings are supported: 188, 98, 42, 20, 10, 5.
 *  @param[in]  lpf Desired LPF setting.
 *  @return     0 if successful.
 */
#define MPU_FILTER_188HZ    1
#define MPU_FILTER_98HZ     2
#define MPU_FILTER_42HZ     3
#define MPU_FILTER_20HZ     4
#define MPU_FILTER_10HZ     5
#define MPU_FILTER_5HZ      6
void mpu_set_lpf( int8 lpf )
{
	int8 data;
	
	if (lpf >= 188)
        data = MPU_FILTER_188HZ;
    else if (lpf >= 98)
        data = MPU_FILTER_98HZ;
    else if (lpf >= 42)
        data = MPU_FILTER_42HZ;
    else if (lpf >= 20)
        data = MPU_FILTER_20HZ;
    else if (lpf >= 10)
        data = MPU_FILTER_10HZ;
    else
        data = MPU_FILTER_5HZ;
	
	mpu_write( MPU_I2C_ADDR, MPU_REG_LPF, data );
}

/**
 *  @brief      Set sampling rate.
 *  Sampling rate must be between 4Hz and 1kHz.
 *  @param[in]  rate    Desired sampling rate (Hz).
 *  @return     0 if successful.
 */
void mpu_set_sample_rate( int16 rate )
{
	if (mpu.dmp_on)
        return;
	
	if (rate < 4)
		rate = 4;
	else if (rate > 1000)
		rate = 1000;

	int8 data = (1000/rate) - 1;
	mpu_write( MPU_I2C_ADDR, MPU_REG_RATE_DIV, data );
	
	mpu.sample_rate = 1000 / (1 + data);
	
#ifdef AK89xx_SECONDARY
	mpu_set_compass_sample_rate( MAX_COMPASS_SAMPLE_RATE );
#endif	
	
	/* Automatically set LPF to 1/2 sampling rate. */
	mpu_set_lpf(mpu.sample_rate >> 1);
}

/**
 *  @brief      Select which sensors are pushed to FIFO.
 *  @e sensors can contain a combination of the following flags:
 *  \n INV_X_GYRO, INV_Y_GYRO, INV_Z_GYRO
 *  \n INV_XYZ_GYRO
 *  \n INV_XYZ_ACCEL
 *  @param[in]  sensors Mask of sensors to push to FIFO.
 *  @return     0 if successful.
 */
void mpu_configure_fifo(int8 sensors)
{
	if ( mpu.dmp_on )
		return;
	
	 /* Compass data isn't going into the FIFO. Stop trying. */
    sensors &= ~MPU_SENSOR_XYZ_COMPASS;
	
	mpu.fifo_enable = sensors & mpu.sensors;
	
	if ( sensors )
	{
		set_int_enable(1);
		mpu_reset_fifo();
	}
	else
		set_int_enable(0);
}

/**
 *  @brief      Enable/disable data ready interrupt.
 *  If the DMP is on, the DMP interrupt is enabled. Otherwise, the data ready
 *  interrupt is used.
 *  @param[in]  enable      1 to enable interrupt.
 *  @return     0 if successful.
 */
static void set_int_enable(unsigned char enable)
{
    int8 data = 0;
	if ( enable ) {
		data = mpu.dmp_on ? MPU_INT_DMP_INT_EN : MPU_INT_DATA_RDY_EN;
	}
	mpu_write( MPU_I2C_ADDR, MPU_REG_INT_ENABLE, data );
	mpu.int_enable = data;
}

/**
 *  @brief  Reset FIFO read/write pointers.
 *  @return 0 if successful.
 */
void mpu_reset_fifo()
{
	mpu_write( MPU_I2C_ADDR, MPU_REG_INT_ENABLE, 0 );
	mpu_write( MPU_I2C_ADDR, MPU_REG_FIFO_EN, 0 );
	mpu_write( MPU_I2C_ADDR, MPU_REG_USER_CTRL, 0 );
	
	if ( mpu.dmp_on ) {
		mpu_write( MPU_I2C_ADDR, MPU_REG_USER_CTRL,
					MPU_USER_FIFO_RST | MPU_USER_DMP_RST );
		delay_ms(50);

		int8 data = MPU_USER_DMP_EN | MPU_USER_FIFO_EN;
		if ( mpu.sensors & MPU_SENSOR_XYZ_COMPASS )
			data |= MPU_USER_AUX_IF_EN;
		mpu_write( MPU_I2C_ADDR, MPU_REG_USER_CTRL, data );
		
		mpu_write( MPU_I2C_ADDR, MPU_REG_INT_ENABLE, mpu.int_enable ?
							MPU_INT_DMP_INT_EN : 0 );
		
		mpu_write( MPU_I2C_ADDR, MPU_REG_FIFO_EN,  0 );
	}
	else {
		mpu_write( MPU_I2C_ADDR, MPU_REG_USER_CTRL, MPU_USER_FIFO_RST );
		
		int8 data = MPU_USER_FIFO_EN;
		if ( mpu.sensors & MPU_SENSOR_XYZ_COMPASS )
			data |= MPU_USER_AUX_IF_EN;
		mpu_write( MPU_I2C_ADDR, MPU_REG_USER_CTRL, data );
		
		mpu_write( MPU_I2C_ADDR, MPU_REG_INT_ENABLE, mpu.int_enable ?
							MPU_INT_DATA_RDY_EN : 0 );
		
		mpu_write( MPU_I2C_ADDR, MPU_REG_FIFO_EN, mpu.fifo_enable );
	}
}

/**
 *  @brief      Turn specific sensors on/off.
 *  @e sensors can contain a combination of the following flags:
 *  \n INV_X_GYRO, INV_Y_GYRO, INV_Z_GYRO
 *  \n INV_XYZ_GYRO
 *  \n INV_XYZ_ACCEL
 *  \n INV_XYZ_COMPASS
 *  @param[in]  sensors    Mask of sensors to wake.
 *  @return     0 if successful.
 */
void mpu_set_sensors(int8 sensors)
{
	int8 data;
	if (sensors & MPU_SENSOR_XYZ_GYRO)
        data = MPU_POWER_CLK_PLL;
    else if ( sensors )
        data = MPU_POWER_CLK_INTERNAL;
    else
        data = MPU_POWER_SLEEP;
	mpu_write( MPU_I2C_ADDR, MPU_REG_PWR_MGMT_1, data );
	
	data = 0;
    if (!(sensors & MPU_SENSOR_X_GYRO))
        data |= MPU_POWER_STBY_XG;
    if (!(sensors & MPU_SENSOR_Y_GYRO))
        data |= MPU_POWER_STBY_YG;
    if (!(sensors & MPU_SENSOR_Z_GYRO))
        data |= MPU_POWER_STBY_ZG;
    if (!(sensors & MPU_SENSOR_XYZ_ACCEL))
        data |= MPU_POWER_STBY_XYZA;
	mpu_write( MPU_I2C_ADDR, MPU_REG_PWR_MGMT_2, data );
	
	if (sensors && (sensors != MPU_SENSOR_XYZ_ACCEL))
        /* Latched interrupts only used in LP accel mode. */
        mpu_set_int_latched(0);

#if defined(AK89xx_SECONDARY)	
	int8 user_ctrl = mpu_read( MPU_I2C_ADDR, MPU_REG_USER_CTRL );

	/* Handle AKM power management. */
    if ( sensors & MPU_SENSOR_XYZ_COMPASS ) {
        data = MPU_AKM_SINGLE_MEASUREMENT;
        user_ctrl |= MPU_USER_AUX_IF_EN;
    } else {
        data = MPU_AKM_POWER_DOWN;
        user_ctrl &= ~MPU_USER_AUX_IF_EN;
    }
	mpu_write( MPU_I2C_ADDR, MPU_REG_S1_DO, data );
	
    if (mpu.dmp_on)
        user_ctrl |= MPU_USER_DMP_EN;
    else
        user_ctrl &= ~MPU_USER_DMP_EN;
	mpu_write( MPU_I2C_ADDR, MPU_REG_USER_CTRL, user_ctrl );
#endif
	
	mpu.sensors = sensors;
	
    delay_ms(50);
}


/**
 *  @brief      Enable latched interrupts.
 *  Any MPU register will clear the interrupt.
 *  @param[in]  enable  1 to enable, 0 to disable.
 *  @return     0 if successful.
 */
void mpu_set_int_latched(int1 enable)
{
	int8 data = enable ? (MPU_INT_PIN_LATCH_EN | MPU_INT_PIN_ANY_RD_CLR) : 0;
	
				//No bypass enabled & active high
	mpu_write( MPU_I2C_ADDR, MPU_REG_INT_PIN_CFG, data );
}



/**
 *  @brief      Set device to bypass mode.
 *  @param[in]  bypass_on   1 to enable bypass mode.
 *  @return     0 if successful.
 */
void mpu_set_bypass( int1 bypass_on )
{
	int8 data = mpu_read( MPU_I2C_ADDR, MPU_REG_USER_CTRL );
	if ( !bypass_on && (mpu.sensors & MPU_SENSOR_XYZ_COMPASS) )
		data |= MPU_USER_AUX_IF_EN;
	else
		data &= ~MPU_USER_AUX_IF_EN;
	mpu_write( MPU_I2C_ADDR, MPU_REG_USER_CTRL, data );
	
	delay_ms(3);
	
	data = bypass_on ? MPU_INT_PIN_BYPASS_EN : 0;
	if ( mpu.latched_int )
		data |= (MPU_INT_PIN_LATCH_EN | MPU_INT_PIN_ANY_RD_CLR);
	mpu_write( MPU_I2C_ADDR, MPU_REG_INT_PIN_CFG, data );
}


#ifdef AK89xx_SECONDARY
/* This initialization is similar to the one in ak8975.c. */
static void setup_compass()
{
    mpu_set_bypass(1);

    /* Find compass. Possible addresses range from 0x0C to 0x0F. */
	int8 akm_addr;
    for (akm_addr = 0x0C; akm_addr <= 0x0F; akm_addr++) {
        int8 data = mpu_read(akm_addr, MPU_AKM_REG_WHOAMI);
        if (data == MPU_AKM_WHOAMI)
            break;
    }

    if (akm_addr > 0x0F) {
        /* TODO: Handle this case in all compass-related functions. */
        return;
    }

	mpu.compass_addr = akm_addr;
	
	mpu_write( mpu.compass_addr, MPU_AKM_REG_CNTL, MPU_AKM_POWER_DOWN );
	delay_ms(1);
	
	mpu_write( mpu.compass_addr, MPU_AKM_REG_CNTL, MPU_AKM_FUSE_ROM_ACCESS );
	delay_ms(1);

		/* Get sensitivity adjustment data from fuse ROM. */
	mpu_read_begin( mpu.compass_addr, MPU_AKM_REG_ASAX );
	mpu.mag_sens_adj[0] = (int16)mpu_read_next() + 128;
	mpu.mag_sens_adj[1] = (int16)mpu_read_next() + 128;
	mpu.mag_sens_adj[2] = (int16)mpu_read_end() + 128;
  
	mpu_write( mpu.compass_addr, MPU_AKM_REG_CNTL, MPU_AKM_POWER_DOWN );
	delay_ms(1);
	
    mpu_set_bypass(0);

    /* Set up master mode, master clock, and ES bit. */
	mpu_write( MPU_I2C_ADDR, MPU_REG_I2C_MST, 0x40 );
	
	/* Slave 0 reads from AKM data registers. */
	mpu_write( MPU_I2C_ADDR, MPU_REG_S0_ADDR, 
				MPU_S_ADDR_I2C_READ | mpu.compass_addr );
	
    /* Compass reads start at this register. */
	mpu_write( MPU_I2C_ADDR, MPU_REG_S0_REG, MPU_AKM_REG_ST1 );
	
	/* Enable slave 0, 8-byte reads. */
	mpu_write( MPU_I2C_ADDR, MPU_REG_S0_CTRL, MPU_S_CTRL_SLAVE_EN | 8 );
   
	/* Slave 1 changes AKM measurement mode. */
	mpu_write( MPU_I2C_ADDR, MPU_REG_S1_ADDR, mpu.compass_addr );

	/* AKM measurement mode register. */
	mpu_write( MPU_I2C_ADDR, MPU_REG_S1_REG, MPU_AKM_REG_CNTL );

    /* Enable slave 1, 1-byte writes. */
	mpu_write( MPU_I2C_ADDR, MPU_REG_S1_CTRL, MPU_S_CTRL_SLAVE_EN | 1 );

    /* Set slave 1 data. */
	mpu_write( MPU_I2C_ADDR, MPU_REG_S1_DO, MPU_AKM_SINGLE_MEASUREMENT );

    /* Trigger slave 0 and slave 1 actions at each sample. */
	mpu_write( MPU_I2C_ADDR, MPU_REG_I2C_DELAY_CTRL, 0x03 );

#ifdef MPU9150
    /* For the MPU9150, the auxiliary I2C bus needs to be set to VDD. */
    data[0] = BIT_I2C_MST_VDDIO;
    if (i2c_write(st.hw->addr, st.reg->yg_offs_tc, 1, data))
        return -1;
#endif
}
#endif


void mpu_get_accel_reg( signed int16 *data, int32* timestamp )
{
	mpu_read_begin( MPU_I2C_ADDR, MPU_REG_RAW_ACCEL );
	
	signed int16 tmp;
	tmp = mpu_read_next();
	tmp <<= 8;
	tmp |= (signed int16)mpu_read_next();
	data[0] = tmp;
	
	tmp = mpu_read_next();
	data[1] = make16( tmp, mpu_read_next() );
	
	tmp = mpu_read_next();
	data[2] = make16( tmp, mpu_read_end() );
}


#define MPU_BANK_SIZE 256
/**
 *  @brief      Write to the DMP memory.
 *  This function prevents I2C writes past the bank boundaries. The DMP memory
 *  is only accessible when the chip is awake.
 *  @param[in]  mem_addr    Memory location (bank << 8 | start address)
 *  @param[in]  length      Number of bytes to write.
 *  @param[in]  data        Bytes to write to memory.
 *  @return     0 if successful.
 */
void mpu_write_mem(int16 mem_addr, int16 length, int8 *data)
{
    int8 tmp[2];

    tmp[0] = (int8)(mem_addr >> 8);
    tmp[1] = (int8)(mem_addr & 0xFF);

    /* Check bank boundaries. */
    if ((tmp[1] + length) > MPU_BANK_SIZE)
        return;
    
    mpu_write_begin( MPU_I2C_ADDR, MPU_REG_BANK_SEL );
    mpu_write_next( tmp[0] );
    mpu_write_next( tmp[1] );
    mpu_write_end();

    mpu_write_begin( MPU_I2C_ADDR, MPU_REG_MEM_R_W );
    for( int8 i=0; i<length; ++i ) {
        mpu_write_next( *data );
        ++data;
    }
    mpu_write_end();
}

/**
 *  @brief      Read from the DMP memory.
 *  This function prevents I2C reads past the bank boundaries. The DMP memory
 *  is only accessible when the chip is awake.
 *  @param[in]  mem_addr    Memory location (bank << 8 | start address)
 *  @param[in]  length      Number of bytes to read.
 *  @param[out] data        Bytes read from memory.
 *  @return     0 if successful.
 */
void mpu_read_mem(int16 mem_addr, int16 length, int8 *data)
{
    int8 tmp[2];

    tmp[0] = (int8)(mem_addr >> 8);
    tmp[1] = (int8)(mem_addr & 0xFF);

    /* Check bank boundaries. */
    if ((tmp[1] + length) > MPU_BANK_SIZE)
        return;
    
    mpu_write_begin( MPU_I2C_ADDR, MPU_REG_BANK_SEL );
    mpu_write_next( tmp[0] );
    mpu_write_next( tmp[1] );
    mpu_write_end();

    mpu_read_begin( MPU_I2C_ADDR, MPU_REG_MEM_R_W );
    int8 last=length-1;
    for( int8 i=0; i<last; ++i ) {
        *data = mpu_read_next();
        ++data;
    }
    *data = mpu_read_end();
}



/**
 *  @brief      Configure DMP image.
 *  @param[in]  start_addr  Starting address of DMP code memory.
 *  @param[in]  sample_rate Fixed sampling rate used when DMP is enabled.
 *  @return     0 if successful.
 */
void mpu_cnf_dmp_firmware( int16 start_addr, int16 sample_rate )
{
    /* Set program start address. */
    mpu_write_begin( MPU_I2C_ADDR, MPU_REG_PRGM_START_H );
    int8 tmp = start_addr >> 8;
    mpu_write_next( tmp );

    tmp = start_addr & 0xFF;
    mpu_write_next( tmp );
    
    mpu_write_end();
	
	mpu.dmp_sample_rate = sample_rate;
}


/**
 *  @brief      Set compass sampling rate.
 *  The compass on the auxiliary I2C bus is read by the MPU hardware at a
 *  maximum of 100Hz. The actual rate can be set to a fraction of the gyro
 *  sampling rate.
 *
 *  \n WARNING: The new rate may be different than what was requested. Call
 *  mpu_get_compass_sample_rate to check the actual setting.
 *  @param[in]  rate    Desired compass sampling rate (Hz).
 *  @return     0 if successful.
 */
void mpu_set_compass_sample_rate(int16 rate)
{
#ifdef AK89xx_SECONDARY
    if (!rate || (rate > mpu.sample_rate) || (rate > MAX_COMPASS_SAMPLE_RATE))
        return;

    int8 div = (mpu.sample_rate / rate) - 1;
	mpu_write( MPU_I2C_ADDR, MPU_REG_S4_CTRL, div );
#endif
}



/**
 *  @brief      Enable/disable DMP support.
 *  @param[in]  enable  1 to turn on the DMP.
 *  @return     0 if successful.
 */
void mpu_set_dmp_state( int1 enable )
{
    if ( mpu.dmp_on == enable )
        return;

    if (enable) {
        /* Disable data ready interrupt. */
        set_int_enable(0);
        /* Disable bypass mode. */
        mpu_set_bypass(0);
        /* Keep constant sample rate, FIFO rate controlled by DMP. */
        mpu_set_sample_rate( mpu.dmp_sample_rate );
        /* Remove FIFO elements. */
        mpu_write( MPU_I2C_ADDR, MPU_REG_FIFO_EN, 0 );
        mpu.dmp_on = 1;
        /* Enable DMP interrupt. */
        set_int_enable(1);
        mpu_reset_fifo();
    } 
	else {
        /* Disable DMP interrupt. */
        set_int_enable(0);
        /* Restore FIFO settings. */
        mpu_write( MPU_I2C_ADDR, MPU_REG_FIFO_EN, mpu.fifo_enable );
        mpu.dmp_on = 0;
        mpu_reset_fifo();
    }
}


/**
 *  @brief      Read from a single register.
 *  NOTE: The memory and FIFO read/write registers cannot be accessed.
 *  @param[in]  reg     Register address.
 *  @param[out] data    Register data.
 *  @return     0 if successful.
 */
inline int8 mpu_read_reg(int8 reg)
{
	return mpu_read( MPU_I2C_ADDR, reg );
}


int16 mpu_fifo_available()
{
	mpu_read_begin( MPU_I2C_ADDR, MPU_REG_FIFO_COUNTH );
	int8 fifoH = mpu_read_next();
	int8 fifoL = mpu_read_end();
	return make16( fifoH, fifoL );
}


/**
 *  @brief      Get one unparsed packet from the FIFO.
 *  This function should be used if the packet is to be parsed elsewhere.
 *  @param[in]  length  Length of one FIFO packet.
 *  @param[in]  data    FIFO packet.
 *  @param[in]  more    Number of remaining packets.
 */
int1 mpu_read_fifo_stream(int8 length, int8* data, int8* more )
{
    int16 fifo_count = mpu_fifo_available();

    if ( fifo_count < length ) {
        return 0;
    }
    if ( fifo_count > (MPU_MAX_FIFO/2) ) {
        /* FIFO is 50% full, better check overflow bit. */
		int8 status = mpu_read( MPU_I2C_ADDR, MPU_REG_INT_STATUS );
        if ( status & MPU_INT_STATUS_FIFO_OVERFLOW ) {
            mpu_reset_fifo();
            return 0;
        }
    }

	mpu_read_begin( MPU_I2C_ADDR, MPU_REG_FIFO_R_W );
	int8 i, last = length-1;
	for( i=0; i<last; ++i ) {
		*data = mpu_read_next();
		data++;
	}
	*data = mpu_read_end();
    more[0] = (fifo_count / length) - 1;
    return 1;
}


/**
 *  @brief      Register dump for testing.
 *  @return     0 if successful.
 */
void mpu_reg_dump()
{
    int8 ii;
	for (ii = 0; ii < (MPU_NUM_REG); ii++) {
        if (ii == MPU_REG_FIFO_R_W || ii == MPU_REG_MEM_R_W)
            continue;
        int8 data = mpu_read( MPU_I2C_ADDR, ii );
        dpm_log_i("%5x: %5x\r\n", ii, data);
    }
}


int8 mpu_who_am_i()
{
	return mpu_read( MPU_I2C_ADDR, MPU_REG_WHO_AM_I );
}