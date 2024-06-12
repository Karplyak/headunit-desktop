#include "pultplugin.h"

PultPlugin::PultPlugin(QObject *parent) : QObject (parent), m_refreshTimer(this)
{
	m_pluginSettings.eventListeners = QStringList() << "PultAuto::connected";
}

QObject *PultPlugin::getContextProperty() {
    return this;
}
/**/

void PultPlugin::init() {
	qDebug() << "PultPlugin init!!!!";
	alsReadingPtr = 0;
    startAnalyse = false;
    nightMode = false;
	
	wiringXSetup("rock4",NULL);
	pinMode(1, PINMODE_INPUT);
	
    updatePorts();
    updateSensorModels();
    dayThreshold = m_settings.value("day_threshold").toUInt();
    nightThreshold = m_settings.value("night_threshold").toUInt();

    connect(&m_refreshTimer, &QTimer::timeout, this, &PultPlugin::readI2C_status);
    startTimer();

}

void PultPlugin::updatePorts() {
    m_ports.clear();

    QDir dir("/dev");
    dir.setFilter(QDir::System);
    dir.setSorting(QDir::Name);

    dir.setNameFilters(QStringList("i2c-*"));

    for(const QFileInfo &info : dir.entryInfoList()) {
        m_ports.insert(info.filePath(), info.fileName());
		qDebug() << "PultPlugin updatePorts m_ports.insert" << info.fileName();
    }

    emit portsUpdated();
}

void PultPlugin::updateSensorModels() {
    m_sensorModels.clear();
    qDebug() << "PultPlugin Looking for Models in : " << QCoreApplication::applicationDirPath();

    QDir dir(QCoreApplication::applicationDirPath());
    dir.cd("../modules/pult/pults");
    dir.setSorting(QDir::Name);
    dir.setNameFilters(QStringList("*.json"));

    for (const QFileInfo &info : dir.entryInfoList()) {
        QFile file;
        file.setFileName(info.filePath());
        file.open(QIODevice::ReadOnly | QIODevice::Text);

qDebug() << "PultPlugin updatePorts m_ports.insert" << info.fileName();

        QString configFile = file.readAll();
        file.close();
        QJsonObject json = QJsonDocument::fromJson(configFile.toUtf8()).object();
        QString name;
        if (json.contains("name")) {
            name = json["name"].toString();
			qDebug() << "PultPlugin updatePorts m_ports.insert" << name;
        }
        else {
            name = info.fileName();
        }

        QDir appDir(QCoreApplication::applicationDirPath());
        QString relativePath = appDir.relativeFilePath(info.filePath());
        m_sensorModels.insert(relativePath, name);
    }/**/

    emit sensorModelsUpdated();

    loadSensorSettings(m_settings["sensor_model"].toString());
}

void PultPlugin::loadSensorSettings(QString fileName) {
    if (fileName.isEmpty()) {
        return;
    }

	qDebug() << "PultPlugin loadSensorSettings m_ports.name" << fileName;

    QFile file;
    file.setFileName(QCoreApplication::applicationDirPath() + "/" + fileName);
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QString configFile = file.readAll();
    file.close();
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(configFile.toUtf8(), &error);
    if (doc.isNull()) {
        qDebug() << "PultPlugin: JSON Parse error : " << error.errorString();
    }

    QJsonObject json = doc.object();

    m_sensorSettings.clear();

    for (auto it = json.constBegin(); it != json.constEnd(); it++) {
        m_sensorSettings.insert(it.key(), it.value());
		qDebug() << "PultPlugin loadSensorSettings m_ports.key" << it.key()<<" value " << it.value();
    }

    emit sensorSettingsUpdated();
    qDebug() << "PultPlugin: Loaded sensor settings : " << fileName;
}

void PultPlugin::startTimer() {
    int refreshInterval = m_settings.value("refresh_interval").toInt();
    qDebug() << "PultPlugin: Timer refresh interval:" << refreshInterval;

    if (refreshInterval > 0) {
        m_refreshTimer.start(refreshInterval * 1000); // Will restart timer if called again
    }
    else {
        stopTimer();
    }
}

void PultPlugin::stopTimer() {
    m_refreshTimer.stop();
}

void PultPlugin::eventMessage(QString id, QVariant message) {
    //When the phone connects send the current night mode status
    if (id == "AndroidAuto::connected") {
        if(message.toBool())
            emit action("SYSTEM::SetNightMode", nightMode);
    }
}

void PultPlugin::actionMessage(QString id, QVariant message) {
}

void PultPlugin::settingsChanged(const QString &key, const QVariant &) {
    if (key == "i2c_port") {
        //Will take effect next time the timer elapses
    }
    else if (key == "refresh_interval") {
        startTimer();
    }
    else if (key == "day_threshold") {
        dayThreshold = m_settings.value("day_threshold").toInt();
    }
    else if (key == "night_threshold") {
        nightThreshold = m_settings.value("night_threshold").toInt();
    }
    else if (key == "sensor_model") {
        loadSensorSettings(m_settings["sensor_model"].toString());
    }
}

void PultPlugin::onSettingsPageDestroyed() {
    qDebug() << "PultPlugin: settingsPageDestroyed()";

    dayThreshold = m_settings.value("day_threshold").toInt();
    nightThreshold = m_settings.value("night_threshold").toInt();
    loadSensorSettings(m_settings["sensor_model"].toString());
    startTimer();
}

void PultPlugin::readI2C_status() {
    int file;
    __u8 configReg = m_sensorSettings["configRegister"].toUInt();
    //__u8 alsReg = m_sensorSettings["alsRegister"].toUInt();
	__u8 alsReg = 0x01;
    int configOnValue = m_sensorSettings["configOnValue"].toUInt();
    __u8 result = 0;
    int deviceAddr = m_sensorSettings["deviceAddress"].toUInt();
	/*int deviceAddr = 0x3d;
	QString qs = m_sensorSettings["Key_Mute"].toString();
	std::string test333 = qs.toStdString();
	QString qstr = QString::fromStdString(test333.substr(0, 4));
	qDebug() << "PultPlugin: view m_sensorSettings configOnValue " << m_sensorSettings["configOnValue"].toUInt();
	qDebug() << "PultPlugin: view m_sensorSettings Key_Mute " << m_sensorSettings["Key_Mute"].toUInt();
*/

    //file = open(qPrintable(m_settings["i2c_port"].toString()), O_RDWR);
	file = open("/dev/i2c-7",O_RDWR);
    if (file < 0) {
        qDebug() << "PultPlugin: Failed opening " << m_settings["i2c_port"].toString();
    }
    else {

        if (ioctl(file, I2C_SLAVE, deviceAddr) < 0) {
            qDebug() << "PultPlugin: Failed setting slave address " << deviceAddr;
        }
        else {
            //turn on device writing to the config addr
            /*if(m_sensorSettings["configRegisterWidth"].toInt() == 16)
                result = i2c_smbus_write_word_data(file, configReg, configOnValue);
            else
                result = i2c_smbus_write_byte_data(file, configReg, configOnValue);

            if (result < 0) {
                qDebug() << "PultPlugin: Failed writing " << configOnValue <<  " to address " << configReg;
                qDebug() << "PultPlugin: result " << result;
                close(file);
                return;
            }*/
			
			
			if (digitalRead(1)) {
				qDebug() << "PultPlugin: result pin " <<  digitalRead(1);
            //read light value
            if (m_sensorSettings["alsRegisterWidth"].toInt() == 16)
                result = i2c_smbus_read_word_data(file, alsReg);
            else
                result = i2c_smbus_read_byte_data(file, alsReg);

				qDebug() << "PultPlugin: result " << result;

			

            if (result != 255) {
				if (result==m_sensorSettings["Key_Powr"].toUInt())
				{
					qDebug() << "PultPlugin: shutdown result " << result;
					//reboot(LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2, LINUX_REBOOT_CMD_POWER_OFF, 0);
					system("/usr/bin/poweroff");
				}	
/*				else if (result==m_sensorSettings["Key_Sekp"].toUInt())
				{
					emit message("MediaInput", "Next");
				}
				else if (result==m_sensorSettings["Key_Sekm"].toUInt())
				{
					emit message("MediaInput", "Previous");
				}*/
				else if (result==m_sensorSettings["Key_Disp"].toUInt())
				{
					//emit message("MediaInput", "Previous");
					emit action("SYSTEM::SetNightMode", false);
				}
				else if (result==m_sensorSettings["Key_Volu"].toUInt())
				{
					emit action("AudioProcessorPlugin::VolumeUp", 0);
				}
				else if (result==m_sensorSettings["Key_Vold"].toUInt())
				{
					emit action("AudioProcessorPlugin::VolumeDown", 0);
				}
				else if (result==m_sensorSettings["Key_Sekm"].toUInt())
				{
					emit action("AudioProcessorPlugin::TuneDown", 0);
				}
				else if (result==m_sensorSettings["Key_Sekp"].toUInt())
				{
					emit action("AudioProcessorPlugin::TuneUp", 0);
				}
				else if(result==m_sensorSettings["Key_Audo"].toUInt())
				{
					emit action("AudioProcessorPlugin::Sound", 0);
				}
				else if(result==m_sensorSettings["Key_Mute"].toUInt())
				{
					emit action("AudioProcessorPlugin::Mute", 0);
				}
				
				/*switch (result) {
					case m_sensorSettings["Key_Sekp"].toUInt():
						emit message("MediaInput", "Next");
						break;
					case m_sensorSettings["Key_Sekm"].toUInt():
						emit message("MediaInput", "Previous");
						break;
					case 0x4a:
						emit action("AudioProcessorPlugin::VolumeUp", 0);
						break;
					case 0x02:
						emit action("AudioProcessorPlugin::VolumeDown", 0);
						break;
					case Key_Sound:
						emit action("AudioProcessorPlugin::Sound", 0);
						break;
					case Key_TuneDown:
						emit action("AudioProcessorPlugin::TuneDown", 0);
						break;
					case Key_TuneUp:
						emit action("AudioProcessorPlugin::TuneUp", 0);
						break;
					case m_sensorSettings["Key_Mute"].toUInt():
						emit action("AudioProcessorPlugin::TuneDown", 0);
						break;
					default:
						break;
					}*/
				
				
				
				
				
				
				
				
				
				
                if ((alsReadingPtr + 1) < sizeof(alsReadings) / sizeof(alsReadings[0])) {
                    alsReadingPtr++;
                }
                else {
                    alsReadingPtr = 0;
                    //Now we can start analysing samples
                    startAnalyse = true;
                }

                alsReadings[alsReadingPtr] = result;
                emit currentLightUpdated();

               /* if(startAnalyse)
                    analyseResults();*/
            }
            else {
                qDebug() << "PultPlugin: Error getting light value from register " << alsReg;
                qDebug() << "PultPlugin: result " << result;
            }
			}
        }
        close(file);
    }
}

void PultPlugin::analyseResults() {
    bool isNight = true;
    bool isDay = true;

    for (unsigned int i = 0; i < sizeof(alsReadings) / sizeof(alsReadings[0]); i++) {
        isNight = (isNight && (alsReadings[i] <= nightThreshold));
        isDay = (isDay && (alsReadings[i] >= dayThreshold));
    }

    if (isNight && isDay) {
        //do nothing
    }
    else if (isDay && nightMode) {
        emit action("SYSTEM::SetNightMode", false);
        nightMode = false;
    }
    else if(isNight && !nightMode) {
        emit action("SYSTEM::SetNightMode", true);
        nightMode = true;
    }
}
/*
void PultPlugin::testNotification(){
    emit message("GUI::Notification", "{\"image\":\"qrc:/qml/icons/alert.png\", \"title\":\"Test notification\", \"description\":\"This is a test notification\"}");
}


void PultPlugin::onSettingsPageDestroyed() {
    qDebug() << "Sample plugin destroyed";
}
*/