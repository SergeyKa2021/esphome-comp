import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor, one_wire
from esphome.const import CONF_ID, CONF_ONE_WIRE_ID

CODEOWNERS = ["@your-github-username"]

dallas_key_reader_ns = cg.esphome_ns.namespace("dallas_key_reader")
DallasKeyReader = dallas_key_reader_ns.class_(
    "DallasKeyReader", cg.PollingComponent
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(DallasKeyReader),
            cv.GenerateID(CONF_ONE_WIRE_ID): cv.use_id(one_wire.OneWireComponent),
            cv.Required("key"): text_sensor.text_sensor_schema(),
        }
    )
    .extend(cv.polling_component_schema("60s"))
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    
    one_wire = await cg.get_variable(config[CONF_ONE_WIRE_ID])
    cg.add(var.set_one_wire(one_wire))
    
    key_sensor = await text_sensor.new_text_sensor(config["key"])
    cg.add(var.set_key_sensor(key_sensor))
