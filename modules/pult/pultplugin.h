#ifndef PULTPLUGIN_H
#define PULTPLUGIN_H

#include <QObject>
#include <QStringList>
#include <QDebug>
#include <QSettings>
#include <QTimer>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCoreApplication>
#include <plugininterface.h>
#include <string>


//need libi2c-dev
extern "C" {
    #include <linux/i2c-dev.h>
    #include <i2c/smbus.h>

    #include <unistd.h>
    #include <fcntl.h>
    #include <sys/ioctl.h>
	
	#include <unistd.h>
	#include <sys/syscall.h>
    #include <linux/reboot.h>
	#include "wiringx.h"
}


class PultPlugin : public QObject, PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.viktorgino.headunit.pult" FILE "config.json")
    Q_INTERFACES(PluginInterface)
	
	Q_PROPERTY(QVariantMap ports_pult READ getPorts NOTIFY portsUpdated)
    Q_PROPERTY(QVariantMap sensorModels READ getSensorModels NOTIFY sensorModelsUpdated)
    Q_PROPERTY(int currentLight READ getCurrentLight NOTIFY currentLightUpdated)

public:
    explicit PultPlugin(QObject *parent = nullptr);

    void init() override;
	QObject *getContextProperty() override;


public slots:
    void eventMessage(QString id, QVariant message) override;
    void actionMessage(QString id, QVariant message) override;
    void onSettingsPageDestroyed();

signals:
    void message(QString id, QVariant message);
    void action(QString id, QVariant message);
    void portsUpdated();
    void sensorModelsUpdated();
    void sensorSettingsUpdated();
    void currentLightUpdated();

private slots:
    void settingsChanged(const QString &key, const QVariant &value);

private:
    QVariantMap m_ports;
    QVariantMap m_sensorModels;
    QVariantMap m_sensorSettings;

    unsigned int alsReadings[4];
    unsigned int alsReadingPtr = 0;
    bool startAnalyse;
    bool nightMode;

    unsigned int dayThreshold;
    unsigned int nightThreshold;

    QTimer m_refreshTimer;

    void updatePorts();
    void updateSensorModels();
    void loadSensorSettings(QString fileName);

    void startTimer();
    void stopTimer();

    void readI2C_status();
    void analyseResults();

    QVariantMap getPorts() {
        updatePorts();
        return m_ports;
    }

    QVariantMap getSensorModels() {
        updateSensorModels();
        return m_sensorModels;
    }

    unsigned int getCurrentLight() {
        return alsReadings[alsReadingPtr];
    }

};

#endif // PULTPLUGIN_H
