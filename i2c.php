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