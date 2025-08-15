import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.const import CONF_ID
from esphome.components.one_wire import OneWireComponent

CODEOWNERS = ["@SergeyKa2021"]
DEPENDENCIES = ["one_wire", "text_sensor"]

dallas_key_reader_ns = cg.esphome_ns.namespace("dallas_key_reader")
DallasKeyReader = dallas_key_reader_ns.class_("DallasKeyReader", cg.PollingComponent)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(DallasKeyReader),
    cv.Required("one_wire_id"): cv.use_id(OneWireComponent),
    cv.Required("key"): text_sensor.text_sensor_schema(),
}).extend(cv.polling_component_schema("60s"))

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    
    one_wire = await cg.get_variable(config["one_wire_id"])
    cg.add(var.set_one_wire(one_wire))
    
    key_sensor = await text_sensor.new_text_sensor(config["key"])
    cg.add(var.set_key_sensor(key_sensor))
