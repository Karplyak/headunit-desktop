#include "TDA7418.h"

#define DEBUG_MODE

TDA7418::TDA7418() {
	
	qWarning("TDA7418 Init ");
    for(int i = 0; i <= EQ_Band_Treble; i++){
        setEqBandLevel((Audio_EQ_Bands) i, 0);
    }
    for(int i = 0; i <= Subwoofer; i++){
        setOutputChannelLevel((Audio_Output_Channels) i, 100);
    }
    setMute(false);
    //setVolume(90);
    setInput(Input_3);
    setInputGain(0);
	writeByte(TDA7419_SPECTRUM, 0b11001010);
    writeByte(TDA7419_MIX_G_EFF, 0b11110000);
}

void TDA7418::setInput(Audio_Input_Channels channel) {
	qWarning("TDA7418 setInput");
    m_inputSelectorData &= 0b11111000;
    switch (channel)
    {
    case Input_1:
        m_inputSelectorData |= TDA7418_Input_SE1;
        break;
    case Input_2:
        m_inputSelectorData |= TDA7418_Input_SE2;
        break;
    case Input_3:
        m_inputSelectorData |= TDA7418_Input_SE3;
        break;
    case Input_4:
        m_inputSelectorData |= TDA7418_Input_PD;
        break;

    default:
        return;
    }
	
    writeByte(TDA7419_INPUT2, 0b00000101);
    //qWarning("TDA7418 setInput %d",m_inputSelectorData);
	//qWarning("Error opening I2C device %s : %s", device, strerror(errno));
    return writeByte(TDA7418_Register_Input_Selector, m_inputSelectorData);

}
void TDA7418::setVolume(int volume) {
	qWarning("TDA7418 setVolume");
    if(volume > 115 || volume < 0){
        qWarning("Invalid volume level");
        return;
    }

    double ratio = 0.8;
    int vol = 0;
    if (volume < 100){
        vol = 96 - (volume * ratio);
    } else {
        vol = volume - 100;
    }
	qWarning("TDA7418 setVolume %d",vol);
	
    return writeByte((char)TDA7418_Register_Volume, 0b10000000 | vol);
}

void TDA7418::setMute(bool mute) {
    unsigned char muteSettings = 0b01111100 | !mute;
	qWarning("TDA7418 setMute %d",muteSettings);
    return writeByte(TDA7418_Register_Soft_Mute, muteSettings);
}

void TDA7418::setInputGain(int level) {
	
    if (level > 15 || level < 0) {
		return;
	}

    m_inputSelectorData &= 0b10000111;
    m_inputSelectorData |= level << 3;
	//m_inputSelectorData |= 0x80;
	qWarning("TDA7418 setInputGain %d",m_inputSelectorData);
    return writeByte(TDA7418_Register_Input_Selector, m_inputSelectorData);

}
void TDA7418::setOutputChannelLevel(Audio_Output_Channels channel, int level) {
    qWarning("TDA7418 setOutputChannelLevel1 %02d",level);
    int i2cRegister = 0;

    switch (channel)
    {
    case Front_Left:
        i2cRegister = TDA7418_Register_Attenuator_FL;
        break;
    case Front_Right:
        i2cRegister = TDA7418_Register_Attenuator_FR;
        break;
    case Rear_Left:
        i2cRegister = TDA7418_Register_Attenuator_RL;
        break;
    case Rear_Right:
        i2cRegister = TDA7418_Register_Attenuator_RR;
        break;
    case Subwoofer:
        i2cRegister = TDA7418_Register_Attenuator_SUB;
        break;

    default:
        return;
    }

    if(level > 115 || level < 0){
        qWarning() << "Invalid channel level" << level;
        return;
    }

    double ratio = 0.8;
    int vol = 0;
    if (level < 100){
        vol = 96 - (level * ratio);
        qWarning() << "Invalid2"<<vol;
    } else {
        vol = level * ratio - 80;
        qWarning() << "Invalid1"<<vol;
    }/**/



    qWarning("TDA7418 setOutputChannelLevel %x",  vol);
    return writeByte((char)i2cRegister, 0b10000000 | vol);

}
void TDA7418::setEqBandLevel(Audio_EQ_Bands band, int level) {
	qWarning("TDA7418 setEqBandLevel");
    int i2cRegister = 0;
    char register_data = 0b11100000;

    switch (band)
    {
    case EQ_Band_Bass:
        i2cRegister = TDA7418_Register_Bass;
        break;
    case EQ_Band_Middle:
        i2cRegister = TDA7418_Register_Middle;
        break;
    case EQ_Band_Treble:
        i2cRegister = TDA7418_Register_Treble;
        break;
    
    default:
        return;
    }

	if (level < -15 || level > 15) {
        qWarning("Invalid EQ level");
		return;
	}

    if (level >= 0) {
        register_data |= 16 + level;
        qWarning("Invalid2 %d",level + 16);
    }
    else {
        register_data |= level + 30;
    }
    qWarning("TDA7418 setEqBandLevel 0x%02x",register_data);
    return writeByte((char)i2cRegister, register_data);

}

void TDA7418::writeBytes(char registerAddress, int len, unsigned char* byteBuffer)
{
    int fd;
	qWarning("TDA7418 writeBytes");
    const char *device = "/dev/i2c-7";

    fd = open(device, O_RDWR);
    if (fd < 0)
    {
        qWarning("Error opening I2C device %s : %s", device, strerror(errno));
        return;
    }

	if (ioctl(fd, I2C_SLAVE, TDA7418_I2C_ADDRESS) < 0) {
        qWarning("Could not set address to 0x%02x %s", TDA7418_I2C_ADDRESS, strerror(errno));
		return;
	}

    if (i2c_smbus_write_block_data(fd, registerAddress, len, (unsigned char*) byteBuffer) < 0)
    {
        qWarning("Unable to write register data block to 0x%02x::0x%02x : %s", TDA7418_I2C_ADDRESS, registerAddress, strerror(errno));
        return;
    }
    close(fd);
}

void TDA7418::writeByte(char registerAddress, unsigned char byteBuffer)
{
    int fd;
	qWarning("TDA7418 writeByte");
    const char *device = "/dev/i2c-7";

    fd = open(device, O_RDWR);
    if (fd < 0)
    {
        qDebug("Error opening I2C device %s : %s", device, strerror(errno));
        return;
    }

	if (ioctl(fd, I2C_SLAVE, TDA7418_I2C_ADDRESS) < 0) {
        qWarning("Could not set address to 0x%02x : %s", TDA7418_I2C_ADDRESS, strerror(errno));
		return;
	}

    if (i2c_smbus_write_byte_data(fd, registerAddress, byteBuffer) < 0)
    {
        qWarning("Unable to write register data byte 0x%02x::0x%02x : %s", TDA7418_I2C_ADDRESS, registerAddress, strerror(errno));
        return;
    }
    close(fd);
}
