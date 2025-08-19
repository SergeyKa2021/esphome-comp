CODEOWNERS = ["@Sergeyka2021"]

from esphome.components.sensor import Sensor
from esphome import automation
from esphome.automation import maybe_simple_id

DS1990KeySensor = Sensor.class_('DS1990KeySensor', Sensor)

# Automation to trigger key reading
@automation.register_action(
    "ds1990_key.read",
    maybe_simple_id(
        {
            cv.GenerateID(): cv.use_id(DS1990KeySensor),
        }
    ),
)
async def ds1990_key_read_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    return var
