import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor, one_wire
from esphome.const import CONF_ADDRESS, CONF_ID

DEPENDENCIES = ['one_wire']
AUTO_LOAD = ['text_sensor']

ds1990_key_ns = cg.esphome_ns.namespace('ds1990_key')
DS1990KeySensor = ds1990_key_ns.class_('DS1990KeySensor', text_sensor.TextSensor, cg.PollingComponent)

CONFIG_SCHEMA = (
    text_sensor.text_sensor_schema())
    .extend({
    cv.GenerateID(): cv.declare_id(DS1990KeySensor),
    cv.Required(CONF_ADDRESS): cv.hex_uint64_t,
    })
    .extend(one_wire.one_wire_device_schema())
    .extend(cv.polling_component_schema("5s"))
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await text_sensor.register_text_sensor(var, config)
    await one_wire.register_one_wire_device(var, config)
    
    cg.add(var.set_address(config[CONF_ADDRESS]))
    
