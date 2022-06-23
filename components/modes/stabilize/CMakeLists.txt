idf_component_register(SRCS "stabilize.c"
                    INCLUDE_DIRS "include"
                    REQUIRE "config" "mpu" "motor_driver")
