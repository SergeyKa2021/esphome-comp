CODEOWNERS = ["@sergeyka2021"]

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import CONF_ID, CONF_PIN

DEPENDENCIES = []
AUTO_LOAD = ['sensor']

ibutton_ns = cg.esphome_ns.namespace('ibutton')
IButton = ibutton_ns.class_('IButton', cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(IButton),
    cv.Required(CONF_PIN): cv.int_,
    cv.Optional('key_id'): cv.hex_uint64_t,
}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    cg.add(var.set_pin(config[CONF_PIN]))
    if 'key_id' in config:
        cg.add(var.set_key_id(config['key_id']))
