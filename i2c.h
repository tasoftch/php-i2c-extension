// we define Module constants
#define PHP_I2C_EXTNAME "php_i2c"
#define PHP_I2C_VERSION "0.8.0"

// then we declare the function to be exported
PHP_FUNCTION(i2c_open);
PHP_FUNCTION(i2c_close);
PHP_FUNCTION(i2c_select);
PHP_FUNCTION(i2c_read);
PHP_FUNCTION(i2c_write);
