#ifndef FMRADIOPLUGIN_H
#define FMRADIOPLUGIN_H

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

#include "backends/rda5807m/RDA5807M.h"

//need libi2c-dev
extern "C" {
    #include <linux/i2c-dev.h>
    #include <i2c/smbus.h>

    #include <unistd.h>
    #include <fcntl.h>
    #include <sys/ioctl.h>
}

class FMRadioPlugin : public QObject, PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.viktorgino.headunit.fmradio" FILE "config.json")
    Q_INTERFACES(PluginInterface)

    Q_PROPERTY(QVariantMap ports_fm READ getPorts NOTIFY portsUpdated)
    Q_PROPERTY(int lfrequency READ getFreq NOTIFY freqUpdated)
  //Q_PROPERTY(CRadioController* RadioController READ radioController NOTIFY radioControllerChanged)
    //Q_PROPERTY(FMRadioPlugin* FMRadio READ FMradio NOTIFY FMradioChanged)
    Q_PROPERTY(int snr MEMBER snr NOTIFY snrChanged)
    Q_PROPERTY(int frequency MEMBER currentFrequency NOTIFY frequencyChanged)
    Q_PROPERTY(QString title MEMBER currentTitle NOTIFY titleChanged)
    Q_PROPERTY(QString text MEMBER currentText NOTIFY textChanged)
//    Q_PROPERTY(QVariantMap sensorModels READ getSensorModels NOTIFY sensorModelsUpdated)
//    Q_PROPERTY(int currentLight READ getCurrentLight NOTIFY currentLightUpdated)
public:
    explicit FMRadioPlugin(QObject *parent = nullptr);
    QObject *getContextProperty() override;
    void init() override;
    virtual void onSNR(int snr);

    Q_INVOKABLE void testNotification();
    Q_INVOKABLE void onSettingsPageDestroyed();
    FMRadioPlugin *FMradio();
public slots:
    //void buttonClicked(QString key);
    void openOverlay();
    void moveToFreq( int );

signals:
    void message(QString id, QVariant message);
    void portsUpdated();
    void freqUpdated();
    void snrChanged(int);
    void FMradioChanged();
    void titleChanged();
    void textChanged();
    void frequencyChanged();
    void changed(double);

private:
    RDA5807M *m_radioBackend = nullptr;
    QVariantMap m_ports;
    void updatePorts();
    void updatelFreq();
    void startTimer();
    void stopTimer();

    void getRDS();

    QTimer m_refreshTimer;

    float snr = 0;
    uint16_t currentFrequency;
    uint16_t previousFrequency;
    QString currentTitle;
    QString currentText;

    QVariantMap getPorts() {
        updatePorts();
        return m_ports;
    }
    int getFreq() {
        updatelFreq();
        return currentFrequency;
    }
/**/};

#endif // FMRADIOPLUGIN_H
