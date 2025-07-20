import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_PIN

DEPENDENCIES = []
AUTO_LOAD = ['sensor']

dallas_key_ns = cg.esphome_ns.namespace('dallas_key')
DallasKeyComponent = dallas_key_ns.class_('DallasKeyComponent', cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(DallasKeyComponent),
    cv.Required(CONF_PIN): cv.int_,
}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    cg.add(var.set_pin(config[CONF_PIN]))
