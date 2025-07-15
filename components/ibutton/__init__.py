import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import button
from esphome.const import CONF_ID, CONF_PIN, CONF_KEY_ID

DEPENDENCIES = []
AUTO_LOAD = ["sensor"]

ibutton_ns = cg.esphome_ns.namespace("ibutton")
IButton = ibutton_ns.class_("IButton", cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(IButton),
    cv.Required(CONF_PIN): cv.int_,
    cv.Optional(CONF_KEY_ID): cv.uint64_t,
    cv.Optional("write_button"): cv.use_id(button.Button),
}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    cg.add(var.set_pin(config[CONF_PIN]))
    
    if CONF_KEY_ID in config:
        cg.add(var.set_key_id(config[CONF_KEY_ID]))
    
    if "write_button" in config:
        button_var = yield cg.get_variable(config["write_button"])
        cg.add(var.set_write_button(button_var))
