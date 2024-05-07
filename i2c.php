<?php
/**
 * MIT License
 *
 * Copyright (c) 2020 TASoft Applications
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*
 * This file describes the extension for php editors and documentation utilities.
 * You must not include this file in your application because the extension defines this functions.
 */

const PHP_I2C_ENDOCING_LITTLE_ENDIAN = 1;
const PHP_I2C_ENDOCING_BIG_ENDIAN = 2;

/**
 * Opens the device
 * Usually "/dev/i2c-0", "/dev/i2c-1" or "/dev/i2c-2"
 *
 * @param string $deviceName
 * @param int $encoding
 * @return bool|resource
 */
function i2c_open(string $deviceName, int $encoding = PHP_I2C_ENDOCING_BIG_ENDIAN) {}

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
function i2c_select($device, int $address) {}

/**
 * Reads a single byte from the device
 *
 * @param resource $device
 * @return int
 */
function i2c_read_byte($device): int {}

/**
 * Reads two bytes from the device
 *
 * @param resource $device
 * @return int
 */
function i2c_read_2_bytes($device): int {}

/**
 * Reads three bytes byte from the device
 *
 * @param resource $device
 * @return int
 */
function i2c_read_3_bytes($device): int {}

/**
 * Reads four bytes from the device
 *
 * @param resource $device
 * @return int
 */
function i2c_read_4_bytes($device): int {}

/**
 * Writes one byte to the bus.
 * Please note that the $data is truncated to 0 - 255.
 *
 * @param resource $device
 * @param int $register
 * @return bool
 */
function i2c_write_register($device, int $register): bool {}

/**
 * Writes one byte to the bus.
 * Please note that the $data is truncated to 0 - 255.
 *
 * @param resource $device
 * @param int $register
 * @param int $data
 * @return bool
 */
function i2c_write_byte($device, int $register, int $data): bool {}

/**
 * Writes one byte to the bus.
 * Please note that the $data is truncated to 0 - 65535.
 *
 * @param resource $device
 * @param int $register
 * @param int $data
 * @return bool
 */
function i2c_write_2_bytes($device, int $register, int $data): bool {}

/**
 * Writes one byte to the bus.
 * Please note that the $data is truncated to 0 - 16777215.
 *
 * @param resource $device
 * @param int $register
 * @param int $data
 * @return bool
 */
function i2c_write_3_bytes($device, int $register, int $data): bool {}

/**
 * Writes one byte to the bus.
 * Please note that the $data is truncated to 0 - 4294967295.
 *
 * @param resource $device
 * @param int $register
 * @param int $data
 * @return bool
 */
function i2c_write_4_bytes($device, int $register, int $data): bool {}

/**
 * Writes the $bytes into the register.
 * If no bytes are specified, it just writes the register.
 *
 * @param resource $device
 * @param int $register
 * @param array|null $bytes
 * @return bool
 */
function i2c_write($device, int $register, array $bytes = NULL) {}

/**
 * Reads $length bytes from i2c bus
 *
 * @param resource $device
 * @param int $length
 * @return array
 */
function i2c_read($device, int $length) {}