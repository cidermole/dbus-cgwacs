#ifndef LOGGER_H
#define LOGGER_H

#include <QCoreApplication>
#include <QElapsedTimer>

class ModbusRtu;
class QTextStream;

/*!
 * Logs all available modbus RTU registers for a EM24 AC sensor.
 */
class Logger : public QCoreApplication
{
	Q_OBJECT
public:
	Logger(int &argc, char **argv);

private slots:
	void onErrorReceived(int errorType, int exception);

	void onReadCompleted(int function, const QList<quint16> &values);

	void onWriteCompleted(int function, quint16 address, quint16 value);

private:
	void startNext();

	QTextStream *mCsvOut;
	QElapsedTimer mStopwatch;
	ModbusRtu *mModbus;

	int mIndex;
};

#endif // LOGGER_H
