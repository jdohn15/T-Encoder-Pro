from esphome.components import sensor
from esphome.const import CONF_ID

CONFIG_SCHEMA = sensor.sensor_schema().extend({
    cv.GenerateID(): cv.declare_id(CHSC5816TouchComponent),
})
