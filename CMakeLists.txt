# SPDX-License-Identifier: Apache-2.0

if(CONFIG_INTERNAL_LED_DRIVER)
  zephyr_include_directories(.)

  zephyr_library()
  zephyr_library_sources(
    internal_led.c
    )
endif()
