#ifndef ACSENSORMEDIATOR_H
#define ACSENSORMEDIATOR_H

#include <QObject>
#include "defines.h"

class AcSensor;
class AcSensorSettings;
class ModbusRtu;
class Settings;
class VeQItem;

class AcSensorMediator : public QObject
{
	Q_OBJECT
public:
	AcSensorMediator(const QString &portName, bool isZigbee, VeQItem *settingsRoot,
					 QObject *parent = 0);

signals:
	void gridMeterChanged();

	void serialEvent(const char *);

	void connectionLost();

private slots:
	void onDeviceFound();

	void onDeviceSettingsInitialized();

	void onDeviceInitialized();

	void onConnectionLost();

	void onConnectionStateChanged();

	void onServiceTypeChanged();

private:
	void publishSensor(AcSensor *acSensor, AcSensor *pvSensor, AcSensorSettings *acSensorSettings);

	void registerDevice(const QString &serial);

	int getDeviceInstance(const QString &serial, bool isSecundary) const;

	QList<AcSensor *> mAcSensors;
	ModbusRtu *mModbus;
	VeQItem *mDeviceIds;
};

#endif // ACSENSORMEDIATOR_H
