<?php

namespace TASoft\Bus;

use RuntimeException;

/**
 * Class I2C wraps the functions i2c_* into a class
 * @package TASoft\Bus
 */
class I2C
{
	const LITTLE_ENDIAN_ENCODING = 1;
	const BIG_ENDIAN_ENCODING = 2;

	/** @var resource */
	private $deviceStream;
	/** @var int */
	private $address;

	private $byteEncoding = self::BIG_ENDIAN_ENCODING;

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
			throw new RuntimeException("Could not open i2c bus");
		$this->address = $address;
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

	protected function checkDev() {
		if(!$this->deviceStream)
			throw new RuntimeException("Can not read/write to i2c bus. Probably closed or not opened correctly");
	}

	/**
	 * Reads a specific length of bytes from bus
	 *
	 * @param $length
	 * @return array
	 */
	public function read($length) {
		$this->checkDev();
		return i2c_read($this->deviceStream, $length);
	}

	/**
	 * Reads one single byte from i2c bus
	 *
	 * @return int
	 */
	public function readByte() {
		$this->checkDev();
		return i2c_read($this->deviceStream, 1) [0] ?? 0;
	}

	/**
	 * Reads 2 bytes or 16 bit from i2c bus
	 *
	 * @return int
	 */
	public function read2Bytes() {
		$this->checkDev();
		if($this->byteEncoding == self::LITTLE_ENDIAN_ENCODING)
			@ list($b2, $b1) = i2c_read($this->deviceStream, 2);
		else
			@ list($b1, $b2) = i2c_read($this->deviceStream, 2);
		return ($b1 << 8) | $b2;
	}

	/**
	 * Reads 3 bytes or 24 bits from i2c bus
	 *
	 * @return int
	 */
	public function read3Bytes() {
		$this->checkDev();
		if($this->byteEncoding == self::LITTLE_ENDIAN_ENCODING)
			@ list($b3, $b2, $b1) = i2c_read($this->deviceStream, 3);
		else
			@ list($b1, $b2, $b3) = i2c_read($this->deviceStream, 3);
		return ($b1 << 16) | ($b2 << 8) | $b3;
	}

	/**
	 * Reads 4 bytes or 32 bits from i2c bus
	 *
	 * @return int
	 */
	public function read4Bytes() {
		$this->checkDev();
		if($this->byteEncoding == self::LITTLE_ENDIAN_ENCODING)
			@ list($b4, $b3, $b2, $b1) = i2c_read($this->deviceStream, 4);
		else
			@ list($b1, $b2, $b3, $b4) = i2c_read($this->deviceStream, 4);
		return ($b1 << 24) | ($b2 << 16) | ($b3 << 8) | $b4;
	}

	/**
	 * Writes all bytes from an array into the specified register on the connected chip.
	 *
	 * @param int $register
	 * @param array $bytes
	 * @return bool
	 */
	public function write(int $register, array $bytes) {
		$this->checkDev();
		return i2c_write($this->deviceStream, $register, $bytes);
	}

	/**
	 * Writes the register only without data
	 *
	 * @param int $register
	 * @return bool
	 */
	public function writeRegister(int $register) {
		return i2c_write($this->deviceStream, $register);
	}

	/**
	 * Writes one byte to a specified register
	 *
	 * @param int $register
	 * @param int $byte
	 */
	public function writeByte(int $register, int $byte) {
		$this->write($register, [$byte]);
	}

	/**
	 * Writes 2 bytes to the specified register on the connected chip
	 *
	 * @param int $register
	 * @param int $bit16
	 * @return bool
	 */
	public function write2Bytes(int $register, int $bit16) {
		if($this->byteEncoding == self::LITTLE_ENDIAN_ENCODING)
			return i2c_write($this->deviceStream, $register, [$bit16 & 0xFF, ($bit16>>8) & 0xFF]);
		return i2c_write($this->deviceStream, $register, [($bit16>>8) & 0xFF, $bit16 & 0xFF]);
	}

	/**
	 * Writes 4 bytes to the specified register on the connected chip.
	 *
	 * @param int $register
	 * @param int $bit32
	 * @return bool
	 */
	public function write4Bytes(int $register, int $bit32) {
		if($this->byteEncoding == self::LITTLE_ENDIAN_ENCODING)
			return i2c_write($this->deviceStream, $register, [$bit32 & 0xFF, ($bit32>>8) & 0xFF, ($bit32>>16) & 0xFF, ($bit32>>24) & 0xFF]);
		return i2c_write($this->deviceStream, $register, [($bit32>>24) & 0xFF, ($bit32>>16) & 0xFF, ($bit32>>8) & 0xFF, $bit32 & 0xFF]);
	}

	/**
	 * Reads one byte from a specified register
	 *
	 * @param int $register
	 * @return int
	 */
	public function readRegister(int $register): int {
		$this->writeRegister($register);
		return $this->readByte();
	}

	/**
	 * Reads two bytes from a specified register
	 *
	 * @param int $register
	 * @return int
	 */
	public function readRegister16(int $register): int {
		$this->writeRegister($register);
		return $this->read2Bytes();
	}

	/**
	 * Reads three bytes from a specified register
	 *
	 * @param int $register
	 * @return int
	 */
	public function readRegister24(int $register): int {
		$this->writeRegister($register);
		return $this->read3Bytes();
	}

	/**
	 * Reads four bytes from a specified register
	 *
	 * @param int $register
	 * @return int
	 */
	public function readRegister32(int $register): int {
		$this->writeRegister($register);
		return $this->read4Bytes();
	}

	/**
	 * Interprets a given integer as signed integer using a specific bit depth
	 *
	 * @param int $integer
	 * @param int $bits
	 * @return int
	 */
	public static function convertToSignedInteger(int $integer, int $bits = 16): int {
		$div = 1<<($bits-1);
		$mask = $div-1;
		return ($integer > $mask) ? -$div+($integer&$mask) : $integer;
	}

	/**
	 * @return int
	 */
	public function getAddress(): int
	{
		return $this->address;
	}

	/**
	 * @return int
	 */
	public function getByteEncoding(): int
	{
		return $this->byteEncoding;
	}

	/**
	 * @param int $byteEncoding
	 * @return I2C
	 */
	public function setByteEncoding(int $byteEncoding): I2C
	{
		$this->byteEncoding = $byteEncoding;
		return $this;
	}
}