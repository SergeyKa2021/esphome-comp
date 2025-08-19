import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor, one_wire
from esphome.const import CONF_ADDRESS, CONF_ID

DEPENDENCIES = ['one_wire']
AUTO_LOAD = ['text_sensor']

ds1990_key_ns = cg.esphome_ns.namespace('ds1990_key')
DS1990KeyTextSensor = ds1990_key_ns.class_('DS1990KeyTextSensor', text_sensor.TextSensor, cg.Component)

CONFIG_SCHEMA = text_sensor.text_sensor_schema().extend({
    cv.GenerateID(): cv.declare_id(DS1990KeyTextSensor),
    cv.Required(CONF_ADDRESS): cv.hex_uint64_t,
    cv.Optional(one_wire.CONF_ONE_WIRE_ID): cv.use_id(one_wire.OneWire),
})

async def to_code(config):
    var = await text_sensor.new_text_sensor(config)
    await cg.register_component(var, config)
    
    cg.add(var.set_address(config[CONF_ADDRESS]))
    
    if one_wire.CONF_ONE_WIRE_ID in config:
        one_wire_bus = await cg.get_variable(config[one_wire.CONF_ONE_WIRE_ID])
        cg.add(var.set_one_wire(one_wire_bus))
