import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, one_wire
from esphome.const import CONF_ADDRESS, CONF_ID

ds1990_key_ns = cg.esphome_ns.namespace('ds1990_key')
DS1990KeySensor = ds1990_key_ns.class_('DS1990KeySensor', sensor.Sensor, cg.Component)

CONFIG_SCHEMA = sensor.sensor_schema(
    DS1990KeySensor,
    accuracy_decimals=0,
    state_class=sensor.STATE_CLASS_MEASUREMENT,
).extend({
    cv.GenerateID(): cv.declare_id(DS1990KeySensor),
    cv.Required(CONF_ADDRESS): cv.hex_uint64_t,
    cv.Optional(one_wire.CONF_ONE_WIRE_ID): cv.use_id(one_wire.OneWire),
})

async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)
    
    cg.add(var.set_address(config[CONF_ADDRESS]))
    
    if one_wire_id := config.get(one_wire.CONF_ONE_WIRE_ID):
        one_wire_bus = await cg.get_variable(one_wire_id)
        cg.add(var.set_one_wire(one_wire_bus))
