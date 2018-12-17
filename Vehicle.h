#ifndef VEHICLE_H
#define VEHICLE_H

#include <QObject>
#include <common/mavlink.h>
#include "SerialCommunication.h"
#include <QVariant>
#include <mavlink_types.h>
#include <QDebug>

class Vehicle;
class LinkManager;

class Vehicle : public QObject
{
    Q_OBJECT
public:
    explicit Vehicle(SerialLink* link, int vehicleId, MAV_TYPE vehicleType);
    ~Vehicle();

    LinkManager* linkManager(void) { return _linkMgr;}

    int id(void) { return _vehicleId;}

    void setActive(bool active);    

    QString flightMode(uint8_t baseMode, uint32_t customMode);

    /// Returns the list of available flight modes
    QStringList flightModes();

    bool armed(void) {return _armed;}
    bool active(void) {return _active;}    

    static const int maxRCChannelsCount = 18;

signals:
    void linkDeleted(Vehicle* vehicle);
    void vehicleAdded(Vehicle* vehicle);

    void modeChanged(QString shortModeText);
    void armedChanged(bool armed);
    void telemetryChanged(uint8_t rssi);

    void rcChannelsChanged(int channelCount,int pwmValues[maxRCChannelsCount]);

    void parameterReadyVehicleAvailableChanged(bool parameterReadyVehicleAvailable);
    //void activeVehicleChanged(Vehicle* activeVehicle);

public slots:
    void _linkDeleted(SerialLink* link);
    void _mavlinkMessageReceived(SerialLink*link, mavlink_message_t &message);

private:
    void _addLink(SerialLink* link);
    void _handleHeartbeat(mavlink_message_t& message);

    SerialLink* _link;
    int _vehicleId;
    MAV_TYPE _vehicleType;
    LinkManager * _linkMgr;

    uint8_t             _compID;
    bool                _heardFrom;
    bool _active;

    //flight modes
    uint8_t     _base_mode;
    uint32_t   _custom_mode;
    bool         _armed;

    static const char* manualFMode;
    static const char* stabilizedFMode;
    static const char* altCtlFMode;                                       //ALT_CTRL
    static const char* posCtlFMode;                                    //POS_CTRL
    static const char* offboardFMode;                                 //OFFBOARD
    static const char* rtlFMode;                                            //AUTO_RTL
    static const char* loiterFMode;                                       //AUTO_LOITER    

};

#endif // VEHICLE_H
