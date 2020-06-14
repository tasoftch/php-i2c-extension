PHP_ARG_ENABLE(php_i2c, Whether to enable the I2C extension, [ --enable-i2c-php Enable I2C])

if test "$PHP_I2C" != "no"; then
    PHP_NEW_EXTENSION(php_i2c, i2c.c, $ext_shared)
fi