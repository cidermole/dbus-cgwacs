onReadCompleted()
handle device type, check selected "application" (front selector switch)

processAcquisitionData()
https://github.com/victronenergy/dbus-cgwacs/blob/93060bb5ded59018c7513d5a090fc0d1a6258b85/software/src/ac_sensor_updater.cpp#L714

startMeasurements()
- CheckSetup
    readRegisters(RegApplication, 2);
- Acquisition
    // send Em24Commands
    for cmd in Em24Commands:
        readRegisters(cmd->reg, maxOffset + 2);

processAcquisitionData()

// https://en.wikipedia.org/wiki/Modbus#Modbus_TCP_frame_format_(primarily_used_on_Ethernet_networks)


readRegisters(int16 startReg, int16 count)
    mModbus->readRegisters(ModbusRtu::ReadHoldingRegisters,
	                   mAcSensor->slaveAddress(), startReg, count);

// EasyModbusTCP.NET:

public int[] ReadHoldingRegisters(int startingAddress, int quantity)


