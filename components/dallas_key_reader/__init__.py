import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor, dallas
from esphome.const import CONF_ID

CODEOWNERS = ["@your-github-username"]

DEPENDENCIES = ["dallas", "text_sensor"]

dallas_key_reader_ns = cg.esphome_ns.namespace("dallas_key_reader")
DallasKeyReader = dallas_key_reader_ns.class_(
    "DallasKeyReader", cg.PollingComponent
)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(DallasKeyReader),
    cv.Required("dallas"): cv.use_id(dallas.DallasComponent),
    cv.Required("key"): text_sensor.text_sensor_schema(),
}).extend(cv.polling_component_schema("60s"))

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    
    dallas_hub = await cg.get_variable(config["dallas"])
    cg.add(var.set_dallas(dallas_hub))
    
    key_sensor = await text_sensor.new_text_sensor(config["key"])
    cg.add(var.set_key_sensor(key_sensor))
