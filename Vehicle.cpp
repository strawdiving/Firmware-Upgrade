#include "Vehicle.h"

#include "QuadApplication.h"
#include "linkmanager.h"
#include <QDebug>

struct Modes2Name {
    uint8_t main_mode;
    uint8_t sub_mode;
    const char* name;
    bool canBeSet;
};

static const struct Modes2Name modes2name[] ={
{ PX4_CUSTOM_MAIN_MODE_MANUAL ,       0 , "Manual" ,                true},
{ PX4_CUSTOM_MAIN_MODE_STABILIZED ,  0 , "Stabilized" ,            true},
{ PX4_CUSTOM_MAIN_MODE_ALTCTL ,          0 , "Altitude Control" ,  true},
{ PX4_CUSTOM_MAIN_MODE_POSCTL ,        0 , "Position Control" , true},
{ PX4_CUSTOM_MAIN_MODE_OFFBOARD ,  0 , "Offboard Control" ,true},
{ PX4_CUSTOM_MAIN_MODE_AUTO ,             PX4_CUSTOM_SUB_MODE_AUTO_LOITER ,     "Loiter" ,                    true},
{ PX4_CUSTOM_MAIN_MODE_AUTO ,             PX4_CUSTOM_SUB_MODE_AUTO_RTL ,           "Return To Land" ,     true},

{ PX4_CUSTOM_MAIN_MODE_AUTO,        PX4_CUSTOM_SUB_MODE_AUTO_MISSION,   "Mission",       true},
{ PX4_CUSTOM_MAIN_MODE_AUTO,        PX4_CUSTOM_SUB_MODE_AUTO_LAND,      "Landing",       false},
{ PX4_CUSTOM_MAIN_MODE_AUTO,        PX4_CUSTOM_SUB_MODE_AUTO_READY,     "Ready",         false},
{ PX4_CUSTOM_MAIN_MODE_AUTO,        PX4_CUSTOM_SUB_MODE_AUTO_TAKEOFF,   "Takeoff",       false},
{ PX4_CUSTOM_MAIN_MODE_AUTO,        PX4_CUSTOM_SUB_MODE_AUTO_RTGS,      "Return, Link Loss",          false},
};


Vehicle::Vehicle(SerialLink* link, int vehicleId, MAV_TYPE vehicleType)
    : _vehicleId(vehicleId)
    , _vehicleType(vehicleType)
    , _link(NULL)
    , _active(false)
    ,  _armed(false)
    , _base_mode(0)
    , _custom_mode(0)
{
    //qDebug()<<"vehicle";
    _addLink(link);

    _linkMgr = qgcApp()->toolbox()->linkManager();
    connect(_linkMgr,&LinkManager::messageReceived,this,&Vehicle::_mavlinkMessageReceived);
}

Vehicle::~Vehicle()
{

}

 void Vehicle::_addLink(SerialLink* link)
 {
     if(!_link) {
         _link = link;
         connect(qgcApp()->toolbox()->linkManager(),&LinkManager::linkDisconnected,this,&Vehicle::_linkDeleted);
     }
 }

void Vehicle::_linkDeleted(SerialLink* link)
{
    Q_UNUSED(link)
    _link = NULL;
    qDebug()<<"Vehicle : link deleted";
    emit linkDeleted(this);
}

void Vehicle::setActive(bool active)
{
    _active = active;
}

void Vehicle::_mavlinkMessageReceived(SerialLink*link, mavlink_message_t &message)
{
    //qDebug()<<"vehicle: mavlinkMessageReceived";
    if(_link != link){
        _addLink(link);
    }

    if(!_heardFrom) {
        if(message.msgid == MAVLINK_MSG_ID_HEARTBEAT) {
            _heardFrom = true;
            _compID = message.compid;
        }
    }

    switch(message.msgid) {
    case MAVLINK_MSG_ID_HEARTBEAT:
        _handleHeartbeat(message);
        break;

    default:
        break;
    }
}

void Vehicle::_handleHeartbeat(mavlink_message_t& message)
{
    mavlink_heartbeat_t heartbeat;
    mavlink_msg_heartbeat_decode(&message, &heartbeat);

    bool modechanged = false;

    bool armed = heartbeat.base_mode&MAV_MODE_FLAG_SAFETY_ARMED;
    if(_armed != armed)
    {
        _armed = armed;
        emit armedChanged(_armed);
    }

    if (_base_mode != heartbeat.base_mode || _custom_mode != heartbeat.custom_mode)
    {
        modechanged = true;
        _base_mode = heartbeat.base_mode;
        _custom_mode = heartbeat.custom_mode;
        QString shortModeText = flightMode(_base_mode,_custom_mode);
        //qDebug()<<shortModeText;
        emit modeChanged(shortModeText);
    }
}

QString Vehicle::flightMode(uint8_t baseMode, uint32_t customMode)
{
    QString flightMode = "Unknown";
    if(baseMode & MAV_MODE_FLAG_CUSTOM_MODE_ENABLED){
        union px4_custom_mode px4_mode;
        px4_mode.data = customMode;

        bool found = false;
        for(size_t i = 0;i<sizeof(modes2name)/sizeof(modes2name[0]);i++)
        {
            const struct Modes2Name* pModes2Name = &modes2name[i];
            if(pModes2Name->main_mode == px4_mode.main_mode && pModes2Name->sub_mode == px4_mode.sub_mode){
                flightMode = pModes2Name->name;
                found = true;
                break;
            }
        }
        if(!found){
            qDebug()<<"Unknown flight mode:"<<customMode;
        }
    }else{
        qDebug()<<"PX4 Flight Stack flight mode without custom mode enabled?";
    }
   return flightMode;
}

QStringList Vehicle::flightModes()
{
    QStringList flightModes;
    for(size_t i = 0; i < sizeof(modes2name)/sizeof(modes2name[0]); i++)
    {
        const struct Modes2Name* pModes2Name = &modes2name[i];

        if(pModes2Name->canBeSet) {
            flightModes += pModes2Name->name;
        }
    }
    return flightModes;
}
