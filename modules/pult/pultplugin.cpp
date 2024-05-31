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

    updatePorts();
    //updateSensorModels();
    dayThreshold = m_settings.value("day_threshold").toUInt();
    nightThreshold = m_settings.value("night_threshold").toUInt();

    //connect(&m_refreshTimer, &QTimer::timeout, this, &PultPlugin::readI2C);
    //startTimer();

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
    qDebug() << "PultPlugin Looking for Models in : ";

    /*QDir dir(QCoreApplication::applicationDirPath());
    dir.cd("modules/i2c-light-sensor/sensors");
    dir.setSorting(QDir::Name);
    dir.setNameFilters(QStringList("*.json"));

    for (const QFileInfo &info : dir.entryInfoList()) {
        QFile file;
        file.setFileName(info.filePath());
        file.open(QIODevice::ReadOnly | QIODevice::Text);

        QString configFile = file.readAll();
        file.close();
        QJsonObject json = QJsonDocument::fromJson(configFile.toUtf8()).object();
        QString name;
        if (json.contains("name")) {
            name = json["name"].toString();
        }
        else {
            name = info.fileName();
        }

        QDir appDir(QCoreApplication::applicationDirPath());
        QString relativePath = appDir.relativeFilePath(info.filePath());
        m_sensorModels.insert(relativePath, name);
    }*/

    emit sensorModelsUpdated();

    loadSensorSettings(m_settings["sensor_model"].toString());
}

void PultPlugin::loadSensorSettings(QString fileName) {
    if (fileName.isEmpty()) {
        return;
    }

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

void PultPlugin::readI2C() {
    int file;
    __u8 configReg = m_sensorSettings["configRegister"].toUInt();
    __u8 alsReg = m_sensorSettings["alsRegister"].toUInt();
    int configOnValue = m_sensorSettings["configOnValue"].toUInt();
    int result = 0;
    int deviceAddr = m_sensorSettings["deviceAddress"].toUInt();

    file = open(qPrintable(m_settings["i2c_port"].toString()), O_RDWR);
    if (file < 0) {
        qDebug() << "PultPlugin: Failed opening " << m_settings["i2c_port"].toString();
    }
    else {

        if (ioctl(file, I2C_SLAVE, deviceAddr) < 0) {
            qDebug() << "PultPlugin: Failed setting slave address " << deviceAddr;
        }
        else {
            //turn on device writing to the config addr
            if(m_sensorSettings["configRegisterWidth"].toInt() == 16)
                result = i2c_smbus_write_word_data(file, configReg, configOnValue);
            else
                result = i2c_smbus_write_byte_data(file, configReg, configOnValue);

            if (result < 0) {
                qDebug() << "PultPlugin: Failed writing " << configOnValue <<  " to address " << configReg;
                qDebug() << "PultPlugin: result " << result;
                close(file);
                return;
            }

            //read light value
            if (m_sensorSettings["alsRegisterWidth"].toInt() == 16)
                result = i2c_smbus_read_word_data(file, alsReg);
            else
                result = i2c_smbus_read_byte_data(file, alsReg);

            if (result >= 0) {
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

                if(startAnalyse)
                    analyseResults();
            }
            else {
                qDebug() << "PultPlugin: Error getting light value from register " << alsReg;
                qDebug() << "PultPlugin: result " << result;
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