#include "backends/rda5807m/RDA5807M.h"

#define REG_CHIPID    0x00

#define REG_CONTROL   0x02
#define REG_CONTROL_TURNON_OUTPUT 0x8000
#define REG_CONTROL_UNMUTE 0x4000
#define REG_CONTROL_MUTE 0xBFFF
#define REG_CONTROL_MONO   0x2000
#define REG_CONTROL_BASS   0x1000
#define REG_CONTROL_SEEKUP 0x0200
#define REG_CONTROL_SEEKDOWN 0xDFFF
#define REG_CONTROL_SEEK_START   0x0100
#define REG_CONTROL_SEEK_STOP    0xFEFF
#define REG_CONTROL_RDS_ENABLE    0x0008
#define REG_CONTROL_NEW_DEMOD    0x0004 //New Demodulate Method Enable, can improve the receive sensitivity about 1dB.
#define REG_CONTROL_RESET  0x0002
#define REG_CONTROL_CHIP_ENABLE 0x0001

#define REG_CHANNEL_SETTING    0x03
#define REG_CHANNEL_SETTING_SPACE_100     0x0000
#define REG_CHANNEL_SETTING_BAND_FM       0x0000
#define REG_CHANNEL_SETTING_BAND_JP       0x0004
#define REG_CHANNEL_SETTING_BAND_WW       0x0008
#define REG_CHANNEL_SETTING_BAND_EE       0x000C
#define REG_CHANNEL_SETTING_TUNE   0x0010 // Tune is set the Fequency. diffent with seek is finfing the channel.
#define REG_CHANNEL_SETTING_UNTUNE   0xFFEF //

#define REG_CONTROL2   0x04
#define REG_CONTROL2_SOFTMUTE   0x0200
#define REG_CONTROL2_UNSOFTMUTE   0x0000


#define REG_VOLCONTROL     0x05
#define REG_VOLCONTROL_MAX   0x889F
#define REG_VOLCONTROL_MIN   0x8891
#define REG_VOLCONTROL_SET   0x8890


#define REG_STATUSA      0x0A
#define REG_STATUSA_RDS_READY       0x8000
#define REG_STATUSA_SEEK_DONE  0x4000
#define REG_STATUSA_TUNE_DONE  0x4000
#define REG_STATUSA_RDS_SYNC  0x1000
#define REG_STATUSA_STEREO    0x0400

#define REG_STATUSB           0x0B
#define REG_STATUSB_RSSIDATA_MASK   0xFE00
#define REG_STATUSB_FMTRUE   0x0100
#define REG_STATUSB_FMREADY  0x0080

#define CHIP_I2C_ADDRESS  0x10

#define BANDFM 0x00
#define BANDJP 0x01
#define BANDWW 0x10
#define BANDEE 0x11





bool RDA5807M::Radioinit (){
  bool result = false;
  char CheckDevice;
  qWarning("RDA5807M Radioinit") ;

  /*Wire.begin();
  Wire.beginTransmission(CHIP_I2C_ADDRESS);*/
  CheckDevice = 0;
  if (CheckDevice == 0)
  {//Chip found on line
    result = true;
    CHIPRegister[REG_CHIPID] = 0x5800; // factory ID base on datasheet
    CHIPRegister[1] = 0x0000;  // reg 01 NA
    CHIPRegister[REG_CONTROL]  = (REG_CONTROL_RESET | REG_CONTROL_CHIP_ENABLE | REG_CONTROL_NEW_DEMOD | REG_CONTROL_RDS_ENABLE); // include set reset
    CHIPRegister[REG_CHANNEL_SETTING] = (REG_CHANNEL_SETTING_SPACE_100 | REG_CHANNEL_SETTING_BAND_FM);
    CHIPRegister[REG_CONTROL2] = (REG_CONTROL2_UNSOFTMUTE);
    CHIPRegister[REG_VOLCONTROL] = (REG_VOLCONTROL_MIN);
    SaveRegisters();
    CHIPRegister[REG_CONTROL]=(REG_CONTROL_CHIP_ENABLE | REG_CONTROL_NEW_DEMOD | REG_CONTROL_RDS_ENABLE); //turn off reset
    SetBand(BANDFM); // already include Save Register so no need below save call
    //SaveRegisters();
  }
  return(result);
}

void RDA5807M::SetBand(char BankSet)
{
//Band Select.
//00 = 87–108 MHz (US/Europe)
//01 = 76–91 MHz (Japan)
//10 = 76–108 MHz (world wide)
//11 = 65 –76 MHz （East Europe） or 50-65MHz
switch(BankSet)
{
    case BANDFM:
    {
        MaxFreq=10800;
        MinFreq=8700;
        CHIPRegister[REG_CHANNEL_SETTING] = (REG_CHANNEL_SETTING_SPACE_100 | REG_CHANNEL_SETTING_BAND_FM );
        SaveRegisters();
        break;
    }
    case BANDJP:
    {
        MaxFreq=9100;
        MinFreq=7600;
        CHIPRegister[REG_CHANNEL_SETTING] = (REG_CHANNEL_SETTING_SPACE_100 | REG_CHANNEL_SETTING_BAND_JP);
        SaveRegisters();
        break;
    }
    case BANDWW:
    {
        MaxFreq=10800;
        MinFreq=7600;
        CHIPRegister[REG_CHANNEL_SETTING] = (REG_CHANNEL_SETTING_SPACE_100 | REG_CHANNEL_SETTING_BAND_WW);
        SaveRegisters();
        break;
    }
    case BANDEE:
    {
        MaxFreq=7600;
        MinFreq=6500;
        CHIPRegister[REG_CHANNEL_SETTING] = (REG_CHANNEL_SETTING_SPACE_100 | REG_CHANNEL_SETTING_BAND_EE);
        SaveRegisters();
        break;
    }






}


}



void RDA5807M::SetFrequency(unsigned int FreqSet,char VolumeSet) {
  unsigned int FreqData;
  SetBand(BANDFM);
  if (FreqSet < MinFreq) FreqSet = MinFreq;
  if (FreqSet > MaxFreq) FreqSet = MaxFreq;
  FreqData = (FreqSet - MinFreq) / 10;
  CHIPRegister[REG_CHANNEL_SETTING] |= REG_CHANNEL_SETTING_TUNE; // start tune
  CHIPRegister[REG_CHANNEL_SETTING] |= FreqData << 6;

  //Serial.println(CHIPRegister[REG_CHANNEL_SETTING]);
  qWarning("RDA5807M REG_CHANNEL_SETTING %d", REG_CHANNEL_SETTING);

  CHIPRegister[REG_CONTROL] |= (REG_CONTROL_TURNON_OUTPUT | REG_CONTROL_UNMUTE | REG_CONTROL_CHIP_ENABLE); //   play
  CHIPRegister[REG_VOLCONTROL]=REG_VOLCONTROL_SET;
  CHIPRegister[REG_VOLCONTROL] |=VolumeSet;
  SaveRegisters();
  CHIPRegister[REG_CHANNEL_SETTING] &= REG_CHANNEL_SETTING_UNTUNE; // disable turn in arduino registry
   /* */
}

unsigned int RDA5807M::GetFrequency(void)
{
    unsigned int DataRead=0x0000;
    //Wire.setWireTimeout(100000,false); //Wire.setWireTimeout(timeout by us 32bit, Reset module when timeout). Default is no timeout and requestFrom will loop untill read enough byte.
    /*Wire.requestFrom(CHIP_I2C_ADDRESS, 2,true);
    if(Wire.available()==2)
    {
        DataRead=Wire.read();
        DataRead=DataRead<<8;
        DataRead=DataRead|Wire.read();
    }
    DataRead &= 0x03FF;  // get last 10 bit
    DataRead = DataRead*10 + MinFreq;
*/
    return DataRead;
}

char RDA5807M::GetRSSI_FMInfo(void)
{
  char DataReturn=0x00;
  char DataRead;
  char RSSIData;

  unsigned char byteBuffer[3];

  int fd;
  qWarning("TDA7418 writeBytes");
  const char *device = "/dev/i2c-7";

  fd = open(device, O_RDWR);
  if (fd < 0)
  {
      qWarning("Error opening I2C device %s : %s", device, strerror(errno));
      return 0;
  }

  if (ioctl(fd, I2C_SLAVE, CHIP_I2C_ADDRESS) < 0) {
      qWarning("Could not set address to 0x%02x %s", CHIP_I2C_ADDRESS, strerror(errno));
      return 0;
  }

  if (i2c_smbus_read_i2c_block_data(fd, CHIP_I2C_ADDRESS, 3, (unsigned char*) byteBuffer) < 0)
  {
      qWarning("Unable to write register data block to 0x%02x: %s", CHIP_I2C_ADDRESS, strerror(errno));
      return 0;
  }
  close(fd);
  qWarning("Unable to write register data block to 0x%02x:%02x:%02x", byteBuffer[0],byteBuffer[1],byteBuffer[2]);

  /*Wire.requestFrom(CHIP_I2C_ADDRESS, 3,true);
    if(Wire.available()==3)
    {
        DataRead=Wire.read();
        DataRead=Wire.read(); // skip 2 byte as this is Fequency data
        DataRead=Wire.read();
        DataReturn=DataRead&0x01; // get last bit is FM station found
        DataReturn=DataReturn<<7;
        DataReturn=DataReturn&0x80;
        RSSIData=DataRead>>1;
        RSSIData=RSSIData&0x7F; //get 7 bit RSSI
        DataReturn=DataReturn|RSSIData;

    }
*/
    return DataReturn;
}

void RDA5807M::SetVolume(char VolumeSet)
{

    CHIPRegister[REG_VOLCONTROL]=REG_VOLCONTROL_SET;
    CHIPRegister[REG_VOLCONTROL] |=VolumeSet;
    CHIPRegister[REG_CONTROL] |= REG_CONTROL_UNMUTE;
    SaveRegisters();

}

void RDA5807M::ResetChip()
{
    CHIPRegister[REG_CONTROL]  = (REG_CONTROL_RESET | REG_CONTROL_CHIP_ENABLE | REG_CONTROL_NEW_DEMOD | REG_CONTROL_RDS_ENABLE); // include set reset
    SaveRegisters();
    CHIPRegister[REG_CONTROL]=(REG_CONTROL_CHIP_ENABLE | REG_CONTROL_NEW_DEMOD | REG_CONTROL_RDS_ENABLE); //turn off reset
    SetBand(BANDFM); // already include Save Register so no need below save call

}

void RDA5807M::SaveRegisters()
{
  /*Wire.beginTransmission(CHIP_I2C_ADDRESS);
  for (int i = 2; i < 6; i++)
  I2Cwrite16(CHIPRegister[i]);
  Wire.endTransmission();
  */
  int fd;
  qWarning("RDA5807M SaveRegisters");
  const char *device = "/dev/i2c-7";

  fd = open(device, O_RDWR);
  if (fd < 0)
  {
      qDebug("Error opening I2C device %s : %s", device, strerror(errno));
      return;
  }

  if (ioctl(fd, I2C_SLAVE, CHIP_I2C_ADDRESS) < 0) {
      qWarning("Could not set address to 0x%02x : %s", CHIP_I2C_ADDRESS, strerror(errno));
      return;
  }

  for (int i = 2; i < 6; i++)
  {
      if (i2c_smbus_write_byte(fd, CHIPRegister[i]>> 8) < 0)
      {
          qWarning("Unable to write register data byte 0x%02x: %s", CHIP_I2C_ADDRESS,  strerror(errno));
          return;
      }
        //byte temm = HIPRegister[i]&0xFF;
      if (i2c_smbus_write_byte(fd,   CHIPRegister[i]&0xFF) < 0)
      {
          qWarning("Unable to write register data byte 0x%02x: %s", CHIP_I2C_ADDRESS,  strerror(errno));
          return;
      }

  }
  close(fd);
}
void RDA5807M::SetMute(bool OnOff)
{
    if(OnOff==true) CHIPRegister[REG_CONTROL] &= REG_CONTROL_MUTE;
    else CHIPRegister[REG_CONTROL] |= REG_CONTROL_UNMUTE;
    SaveRegisters();
}

void RDA5807M::SeekUp()
{
    CHIPRegister[REG_CONTROL] |= (REG_CONTROL_SEEKUP | REG_CONTROL_SEEK_START);
    SaveRegisters();
    CHIPRegister[REG_CONTROL] &= REG_CONTROL_SEEK_STOP; // disable seek register in arduino.
}

void RDA5807M::I2Cwrite16(unsigned int DataOut)
{
  //Wire.write(DataOut >> 8); Wire.write(DataOut & 0xFF);
}
/*
void RDA5807M::writeBytes(char registerAddress, int len, unsigned char* byteBuffer)
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

    if (ioctl(fd, I2C_SLAVE, CHIP_I2C_ADDRESS) < 0) {
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

void RDA5807M::writeByte(char registerAddress, unsigned char byteBuffer)
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

    if (ioctl(fd, I2C_SLAVE, CHIP_I2C_ADDRESS) < 0) {
        qWarning("Could not set address to 0x%02x : %s", TDA7418_I2C_ADDRESS, strerror(errno));
        return;
    }

    if (i2c_smbus_write_byte_data(fd, registerAddress, byteBuffer) < 0)
    {
        qWarning("Unable to write register data byte 0x%02x::0x%02x : %s", TDA7418_I2C_ADDRESS, registerAddress, strerror(errno));
        return;
    }
    close(fd);
}*/

