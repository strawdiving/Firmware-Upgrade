#include "VehicleManager.h"
#include "QuadApplication.h"
#include "linkmanager.h"

#include <QDebug>

VehicleManager::VehicleManager(QuadApplication *app)
    :ToolBase(app)
    , _linkMgr(NULL)
    , _activeVehicle(NULL)
    , _parameterReadyVehicleAvailable(false)
{

}

VehicleManager::~VehicleManager()
{

}

void VehicleManager::setToolbox(Toolbox* toolbox)
{
    ToolBase::setToolbox(toolbox);
    _linkMgr = _toolbox->linkManager();

    connect(_linkMgr,&LinkManager::vehicleHeartbeatInfo,this,&VehicleManager::_vehicleHeartbeatInfo);
}

void VehicleManager::_vehicleHeartbeatInfo(SerialLink* link, int vehicleId, int vehicleMavlinkVersion, int vehicleFirmwareType, int vehicleType)
{
    //qDebug()<<vehicleId<<vehicleMavlinkVersion<<vehicleFirmwareType<<vehicleType;

    Q_UNUSED(vehicleFirmwareType)
    if(vehicleId == 0) {
        return;
    }

   if(_activeVehicle && _activeVehicle->id() == vehicleId) {
       //qDebug()<<"Vehicle already exists.";
       return;
    }

     if(vehicleMavlinkVersion != MAVLINK_VERSION) {
       _app->showMessage(QString("The MAVLink protocol version on vehicle #%1 and QGroundControl differ! "
                                                                                                        "It is unsafe to use different MAVLink versions. "
                                                                                                        "QGroundControl therefore refuses to connect to vehicle #%1, which sends MAVLink version %2 (QGroundControl uses version %3).").arg(vehicleId).arg(vehicleMavlinkVersion).arg(MAVLINK_VERSION));
       return;
    }

    Vehicle* vehicle = new Vehicle(link,vehicleId,(MAV_TYPE)vehicleType);

    if(vehicle) {
        connect(vehicle,&Vehicle::linkDeleted,this,&VehicleManager::_deleteVehicle);
        _activeVehicle = vehicle;
    }

    if(_activeVehicle) {
         emit activeVehicleChanged(_activeVehicle);
        _activeVehicle->setActive(true);
    }
}

void VehicleManager::_deleteVehicle(Vehicle* vehicle)
{
    if(_activeVehicle == vehicle) {
        _activeVehicle->setActive(false);
        _activeVehicle = NULL;
    }
}
