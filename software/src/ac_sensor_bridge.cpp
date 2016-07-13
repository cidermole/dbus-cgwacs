#include <cmath>
#include <QCoreApplication>
#include <QsLog.h>
#include <velib/vecan/products.h>
#include "ac_sensor.h"
#include "ac_sensor_bridge.h"
#include "ac_sensor_settings.h"
#include "power_info.h"

AcSensorBridge::AcSensorBridge(AcSensor *acSensor, AcSensorSettings *settings,
							   bool isSecundary, QObject *parent) :
	DBusBridge(getServiceName(acSensor, settings, isSecundary), true, parent)
{
	Q_ASSERT(acSensor != 0);
	Q_ASSERT(settings != 0);
	connect(acSensor, SIGNAL(destroyed()), this, SLOT(deleteLater()));
	connect(settings, SIGNAL(destroyed()), this, SLOT(deleteLater()));
	// Changes in QT properties will not be propagated to the D-Bus at once, but
	// in 2500ms invervals.
	setUpdateInterval(1000);

	produce(acSensor, "connectionState", "/Connected");
	produce(acSensor, "errorCode", "/ErrorCode");

	bool isGridmeter =
		(isSecundary ? settings->l2ServiceType() : settings->serviceType()) == "grid";

	producePowerInfo(acSensor->meanPowerInfo(), "/Ac", isGridmeter);
	producePowerInfo(acSensor->l1PowerInfo(), "/Ac/L1", isGridmeter);
	producePowerInfo(acSensor->l2PowerInfo(), "/Ac/L2", isGridmeter);
	producePowerInfo(acSensor->l3PowerInfo(), "/Ac/L3", isGridmeter);

	if (isSecundary || settings->serviceType() == "pvinverter")
		produce(settings, isSecundary ? "l2Position" : "position", "/Position");
	produce(settings, isSecundary ? "l2ProductName" : "productName", "/ProductName");
	produce(settings, isSecundary ? "l2EffectiveCustomName" : "effectiveCustomName", "/CustomName");

	QString processName = QCoreApplication::applicationName();
	// The values of the items below will not change after creation, so we don't
	// need an update mechanism.
	produce("/Mgmt/ProcessName", processName);
	produce("/Mgmt/ProcessVersion", QCoreApplication::applicationVersion());
	produce("/FirmwareVersion", acSensor->firmwareVersion());
	produce("/ProductId", isSecundary ? VE_PROD_ID_CARLO_GAVAZZI_PIGGY_BACK :
										VE_PROD_ID_CARLO_GAVAZZI_EM);
	produce("/DeviceType", acSensor->deviceType());
	produce("/Mgmt/Connection", acSensor->portName());

	int deviceInstance = isSecundary ?
		settings->l2DeviceInstance() :
		settings->deviceInstance();
	produce("/DeviceInstance", deviceInstance);
	produce("/Serial", acSensor->serial());

	registerService();
}

bool AcSensorBridge::toDBus(const QString &path, QVariant &value)
{
	if (path == "/Connected") {
		value = QVariant(value.value<ConnectionState>() == Connected ? 1 : 0);
	} else if (path == "/Position") {
		value = QVariant(static_cast<int>(value.value<Position>()));
	}
	if (value.type() == QVariant::Double && !qIsFinite(value.toDouble()))
		value = QVariant();
	return true;
}

bool AcSensorBridge::fromDBus(const QString &path, QVariant &value)
{
	Q_UNUSED(value)
	if (path == "/CustomName")
		return true;
	return false;
}

QString AcSensorBridge::toText(const QString &path, const QVariant &value, const QString &unit,
							   int precision)
{
	if (path == "/ProductId")
		return QString::number(value.toInt(), 16);
	return DBusBridge::toText(path, value, unit, precision);
}

QString AcSensorBridge::getServiceName(AcSensor *acSensor, AcSensorSettings *settings,
									   bool isSecundary)
{
	QString serviceType = isSecundary ?
		settings->l2ServiceType() :
		settings->serviceType();
	QString portId = acSensor->portName().
			replace("/dev/", "").
			replace("/", "_");
	int deviceInstance = isSecundary ?
		settings->l2DeviceInstance() :
		settings->deviceInstance();
	QString serviceName = QString("pub/com.victronenergy.%1.cgwacs_%2_di%3_mb%4").
			arg(serviceType).
			arg(portId).
			arg(deviceInstance).
			arg(acSensor->slaveAddress());
	if (isSecundary)
		serviceName += "_l2";
	return serviceName;
}

void AcSensorBridge::producePowerInfo(PowerInfo *pi, const QString &path, bool isGridmeter)
{
	produce(pi, "current", path + "/Current", "A", 1);
	produce(pi, "voltage", path + "/Voltage", "V", 0);
	produce(pi, "power", path + "/Power", "W", 0);
	produce(pi, "energyForward", path + "/Energy/Forward", "kWh", 1);
	if (isGridmeter)
		produce(pi, "energyReverse", path + "/Energy/Reverse", "kWh", 1);
}
