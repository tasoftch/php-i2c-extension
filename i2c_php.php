<?php

/**
 * Opens the device
 * Usually "/dev/i2c-0", "/dev/i2c-1" or "/dev/i2c-2"
 *
 * @param string $deviceName
 * @return bool|resource
 */
function i2c_open($deviceName) {}

/**
 * Closes the device pointer
 *
 * @param resource $device
 * @return bool
 */
function i2c_close($device) {}

/**
 * Selects the slave address on the i2c bus.
 *
 * @param resource $device
 * @param int $address
 * @return bool
 */
function i2c_select($device, $address) {}

/**
 * Reads $length bytes from i2c bus
 *
 * @param resource $device
 * @param int $length
 * @return array
 */
function i2c_read($device, $length) {}

/**
 * Writes the $bytes into the register.
 * If no bytes are specified, it just writes the register.
 *
 * @param resource $device
 * @param int $register
 * @param array|null $bytes
 * @return bool
 */
function i2c_write($device, $register, array $bytes = NULL) {}