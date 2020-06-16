<?php

namespace TASoft\Bus;

/**
 * Class I2C wraps the functions i2c_* into a class
 * @package TASoft\Bus
 */
class I2C
{
	/** @var resource */
	private $deviceStream;

	/**
	 * I2C constructor.
	 * @param int $address
	 * @param int $bus
	 */
	public function __construct(int $address, int $bus = 1)
	{
		$this->deviceStream = i2c_open("/dev/i2c-$bus");
		if($this->deviceStream)
			i2c_select($this->deviceStream, $address);
		if(!$this->deviceStream)
			throw new \RuntimeException("Could not open i2c bus");
	}

	/**
	 * Closes the i2c bus.
	 */
	public function close() {
		if($this->deviceStream)
			i2c_close($this->deviceStream);
		$this->deviceStream = NULL;
	}

	/**
	 * @inheritDoc
	 */
	public function __destruct()
	{
		$this->close();
	}

	/**
	 * Reads a specific length of bytes from bus
	 *
	 * @param $length
	 * @return array
	 */
	public function read($length) {
		return i2c_read($this->deviceStream, $length);
	}

	/**
	 * Reads one single byte from i2c bus
	 *
	 * @return int
	 */
	public function readByte() {
		return i2c_read($this->deviceStream, 1) [0] ?? 0;
	}

	/**
	 * Reads 2 bytes or 16 bit from i2c bus
	 *
	 * @return int
	 */
	public function read2Bytes() {
		@ list($b1, $b2) = i2c_read($this->deviceStream, 2);
		return ($b1 << 8) | $b2;
	}

	/**
	 * Reads 4 bytes or 32 bits from i2c bus
	 *
	 * @return int
	 */
	public function read4Bytes() {
		@ list($b1, $b2, $b3, $b4) = i2c_read($this->deviceStream, 4);
		return ($b1 << 24) | ($b2 << 16) | ($b3 << 8) | $b4;
	}

	/**
	 * Writes all bytes from an array into the specified register on the connected chip.
	 * If $waitForCompletion is true, it will wait for completion (as soon the register is set to 0x80)
	 *
	 * @param int $register
	 * @param array $bytes
	 * @param bool $waitForCompletion
	 * @return bool
	 */
	public function write(int $register, array $bytes, bool $waitForCompletion = true) {
		return i2c_write($this->deviceStream, $register, $bytes, $waitForCompletion);
	}

	/**
	 * Writes the register only without data
	 *
	 * @param int $register
	 * @return bool
	 */
	public function writeRegister(int $register) {
		return i2c_write($this->deviceStream, $register, [], false);
	}

	/**
	 * Writes 2 bytes to the specified register on the connected chip
	 *
	 * @param int $register
	 * @param int $bit16
	 * @param bool $waitForCompletion
	 * @return bool
	 */
	public function write16(int $register, int $bit16, bool $waitForCompletion = true) {
		return i2c_write($this->deviceStream, $register, [($bit16>>8) & 0xFF, $bit16 & 0xFF], $waitForCompletion);
	}

	/**
	 * Writes 4 bytes to the specified register on the connected chip.
	 *
	 * @param int $register
	 * @param int $bit32
	 * @param bool $waitForCompletion
	 * @return bool
	 */
	public function write32(int $register, int $bit32, bool $waitForCompletion = true) {
		return i2c_write($this->deviceStream, $register, [($bit32>>24) & 0xFF, ($bit32>>16) & 0xFF, ($bit32>>8) & 0xFF, $bit32 & 0xFF], $waitForCompletion);
	}
}