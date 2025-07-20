import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.components.dallas import DallasDevice
from esphome.const import CONF_ID

CODEOWNERS = ["@your-github-username"]

DEPENDENCIES = ["dallas", "text_sensor"]

dallas_key_reader_ns = cg.esphome_ns.namespace("dallas_key_reader")
DallasKeyReader = dallas_key_reader_ns.class_(
    "DallasKeyReader", cg.PollingComponent
)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(DallasKeyReader),
    cv.Required("bus"): cv.use_id(DallasDevice),
    cv.Required("key"): text_sensor.text_sensor_schema(),
}).extend(cv.polling_component_schema("60s"))

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    
    bus = await cg.get_variable(config["bus"])
    cg.add(var.set_bus(bus))
    
    key_sensor = await text_sensor.new_text_sensor(config["key"])
    cg.add(var.set_key_sensor(key_sensor))
