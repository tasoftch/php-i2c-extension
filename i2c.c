// include the PHP API itself
#include <php.h>
// then include the header of your extension
#include "i2c.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <inttypes.h>
#include <linux/i2c-dev.h>

#define PHP_I2C_ENDOCING_LITTLE_ENDIAN 1
#define PHP_I2C_ENDOCING_BIG_ENDIAN 2
#define PHP_I2C_DEV_RESOURCE "dev_i2c_stream"

ZEND_BEGIN_ARG_INFO_EX(arginfo_i2c_open, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, deviceName, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, encoding, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_i2c_resource, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, device, IS_RESOURCE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_i2c_select, 0, 0, 2)
    ZEND_ARG_TYPE_INFO(0, device, IS_RESOURCE, 0)
    ZEND_ARG_TYPE_INFO(0, address, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_i2c_write_reg, 0, 0, 2)
    ZEND_ARG_TYPE_INFO(0, device, IS_RESOURCE, 0)
    ZEND_ARG_TYPE_INFO(0, reg_num, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_i2c_write, 0, 0, 3)
    ZEND_ARG_TYPE_INFO(0, device, IS_RESOURCE, 0)
    ZEND_ARG_TYPE_INFO(0, reg_num, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, data, IS_LONG, 0)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_i2c_read_dep, 0, 0, 2)
    ZEND_ARG_TYPE_INFO(0, device, IS_RESOURCE, 0)
    ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_i2c_write_dep, 0, 0, 2)
    ZEND_ARG_TYPE_INFO(0, device, IS_RESOURCE, 0)
    ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 1)
ZEND_END_ARG_INFO()


// register our function to the PHP API
// so that PHP knows, which functions are in this module
zend_function_entry i2c_php_functions[] = {
    PHP_FE(i2c_open, arginfo_i2c_open)
    PHP_FE(i2c_close, arginfo_i2c_resource)

    PHP_FE(i2c_select, arginfo_i2c_select)

    PHP_FE(i2c_read_byte, arginfo_i2c_resource)
    PHP_FE(i2c_read_2_bytes, arginfo_i2c_resource)
    PHP_FE(i2c_read_3_bytes, arginfo_i2c_resource)
    PHP_FE(i2c_read_4_bytes, arginfo_i2c_resource)

    PHP_FE(i2c_write_register, arginfo_i2c_write_reg)
    PHP_FE(i2c_write_byte, arginfo_i2c_write)
    PHP_FE(i2c_write_2_bytes, arginfo_i2c_write)
    PHP_FE(i2c_write_3_bytes, arginfo_i2c_write)
    PHP_FE(i2c_write_4_bytes, arginfo_i2c_write)

    // Deprecated, but still implemented
    PHP_FE(i2c_read, arginfo_i2c_read_dep)
    PHP_FE(i2c_write, arginfo_i2c_write_dep)

    PHP_FE_END
};

struct i2c_bus {
    int fd;
    int state;
    int encoding;
};

// The resource management
static void _i2c_resource_destructor(zend_resource *rsrc)
{
    struct i2c_bus fd = *((struct i2c_bus *)rsrc->ptr);
    if(fd.state > 0)
        close(fd.fd);

    free(rsrc->ptr);
}

static int _i2c_mod_resource_id=0;

PHP_MINIT_FUNCTION( i2c_php ) {
    REGISTER_LONG_CONSTANT("PHP_I2C_ENDOCING_LITTLE_ENDIAN", PHP_I2C_ENDOCING_LITTLE_ENDIAN, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("PHP_I2C_ENDOCING_BIG_ENDIAN", PHP_I2C_ENDOCING_BIG_ENDIAN, CONST_CS | CONST_PERSISTENT);

    _i2c_mod_resource_id = zend_register_list_destructors_ex(_i2c_resource_destructor, NULL, PHP_I2C_DEV_RESOURCE, module_number);
}

// some pieces of information about our module
zend_module_entry i2c_php_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_I2C_EXTNAME,
    i2c_php_functions,
    PHP_MINIT(i2c_php),
    NULL,
    NULL,
    NULL,
    NULL,
    PHP_I2C_VERSION,
    STANDARD_MODULE_PROPERTIES
};

ZEND_GET_MODULE(i2c_php)


PHP_FUNCTION(i2c_open) {
    char *devPath = NULL;
    size_t length = 0;

    int mode = PHP_I2C_ENDOCING_BIG_ENDIAN;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|l", &devPath, &length, &mode) != SUCCESS) {
          return;
      }

    int fd = open(devPath, O_RDWR);
    if (fd < 0) {
        RETURN_BOOL(0);
    }

    struct i2c_bus *_fd = calloc(sizeof(struct i2c_bus), 1);
    _fd->fd = fd;
    _fd->state = 1;
    _fd->encoding = mode;

    zend_resource *my_res  = zend_register_resource((void *)_fd, _i2c_mod_resource_id);
    RETURN_RES(my_res);
}


PHP_FUNCTION(i2c_close) {
    zval *rval;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &rval) != SUCCESS) {
       RETURN_BOOL(0);
    }

    ZEND_ASSERT( Z_TYPE_P(rval) == IS_RESOURCE );


    struct i2c_bus *fd = zend_fetch_resource(Z_RES_P(rval), PHP_I2C_DEV_RESOURCE, _i2c_mod_resource_id);

    if(fd->state > 0)
        close(fd->fd);
    RETURN_BOOL(1);
}


static struct i2c_bus *_fetch_i2c_bus(zend_execute_data *execute_data, zval *return_value) {
    zval *rval;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &rval) != SUCCESS) {
        RETVAL_BOOL(0);
        return NULL;
    }

    ZEND_ASSERT( Z_TYPE_P(rval) == IS_RESOURCE );

    struct i2c_bus *fd = zend_fetch_resource(Z_RES_P(rval), PHP_I2C_DEV_RESOURCE, _i2c_mod_resource_id);

    if(fd->state < 2) {
        zend_error(E_WARNING, "Device did not yet select an address on i2c bus");
        RETVAL_BOOL(0);
        return NULL;
    }
    return fd;
}


PHP_FUNCTION(i2c_select) {
    zval *rval;
    long address;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &rval, &address) != SUCCESS) {
       RETURN_BOOL(0);
    }

    ZEND_ASSERT( Z_TYPE_P(rval) == IS_RESOURCE );

    struct i2c_bus *fd = zend_fetch_resource(Z_RES_P(rval), PHP_I2C_DEV_RESOURCE, _i2c_mod_resource_id);

    if (ioctl(fd->fd, I2C_SLAVE, address) < 0) {
        RETURN_BOOL(0);
    }
    fd->state = 2;
    RETURN_BOOL(1);
}

PHP_FUNCTION(i2c_read_byte) {
    struct i2c_bus *fd = _fetch_i2c_bus(execute_data, return_value);

    char buffer[1];

    if (read(fd->fd, buffer, 1) != 1) {
        zend_error(E_WARNING, "Could not read from i2c bus");
        RETURN_BOOL(0);
    } else {
        RETURN_LONG((long) buffer[0] );
    }
}


PHP_FUNCTION(i2c_read_2_bytes) {
    struct i2c_bus *fd = _fetch_i2c_bus(execute_data, return_value);

    char buffer[2];

    if (read(fd->fd, buffer, 2) != 2) {
        zend_error(E_WARNING, "Could not read from i2c bus");
        RETURN_BOOL(0);
    } else {
        long n = 0;
        if(fd->encoding == PHP_I2C_ENDOCING_LITTLE_ENDIAN)
            n = ((long)buffer[1] << 8) | (long)buffer[0];
        else
            n = ((long)buffer[0] << 8) | (long)buffer[1];
        RETURN_LONG( n );
    }
}


PHP_FUNCTION(i2c_read_3_bytes) {
    struct i2c_bus *fd = _fetch_i2c_bus(execute_data, return_value);

    char buffer[3];

    if (read(fd->fd, buffer, 3) != 3) {
        zend_error(E_WARNING, "Could not read from i2c bus");
        RETURN_BOOL(0);
    } else {
        long n = 0;
        if(fd->encoding == PHP_I2C_ENDOCING_LITTLE_ENDIAN)
            n = ((long)buffer[2] << 16) | ((long)buffer[1] << 8) | (long)buffer[0];
        else
            n = ((long)buffer[0] << 16) | ((long)buffer[1] << 8) | (long)buffer[2];
        RETURN_LONG( n );
    }
}


PHP_FUNCTION(i2c_read_4_bytes) {
    struct i2c_bus *fd = _fetch_i2c_bus(execute_data, return_value);

    char buffer[4];

    if (read(fd->fd, buffer, 4) != 4) {
        zend_error(E_WARNING, "Could not read from i2c bus");
        RETURN_BOOL(0);
    } else {
        long n = 0;
        if(fd->encoding == PHP_I2C_ENDOCING_LITTLE_ENDIAN)
            n = ((long)buffer[3] << 24) | ((long)buffer[2] << 16) | ((long)buffer[1] << 8) | (long)buffer[0];
        else
            n = ((long)buffer[0] << 24) | ((long)buffer[1] << 16) | ((long)buffer[2] << 8) | (long)buffer[3];
        RETURN_LONG( n );
    }
}


PHP_FUNCTION(i2c_read) {
    zval *rval;
    long length = 1;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|l", &rval, &length) != SUCCESS) {
       RETURN_BOOL(0);
    }

    ZEND_ASSERT( Z_TYPE_P(rval) == IS_RESOURCE );

    struct i2c_bus *fd = zend_fetch_resource(Z_RES_P(rval), PHP_I2C_DEV_RESOURCE, _i2c_mod_resource_id);

    if(fd->state < 2) {
        zend_error(E_WARNING, "Device did not yet select an address on i2c bus");
        RETURN_BOOL(0);
    }
    char buffer[length];

    if (read(fd->fd, buffer, length) != length) {
        zend_error(E_WARNING, "Could not read from i2c bus");
        RETURN_BOOL(0);
    } else {
        array_init(return_value);

        for(int e = 0;e<length;e++) {
                add_next_index_long(return_value, buffer[e]);
        }
    }
}


PHP_FUNCTION(i2c_write_register) {
    long reg_id = 0;
    zval *rval = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &rval, &reg_id) != SUCCESS) {
       RETURN_BOOL(0);
    }

    ZEND_ASSERT( Z_TYPE_P(rval) == IS_RESOURCE );

    struct i2c_bus *fd = zend_fetch_resource(Z_RES_P(rval), PHP_I2C_DEV_RESOURCE, _i2c_mod_resource_id);

    uint8_t buffer[] = {reg_id & 0xFF};

    if(write(fd->fd, buffer, 1) != 1) {
        zend_error(E_WARNING, "Could not write to i2c bus");
        RETURN_BOOL(0);
    }
    RETURN_BOOL(1);
}


PHP_FUNCTION(i2c_write_byte) {
    long data = 0, reg_id = 0;
    zval *rval = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "rll", &rval, &reg_id, &data) != SUCCESS) {
       RETURN_BOOL(0);
    }

    ZEND_ASSERT( Z_TYPE_P(rval) == IS_RESOURCE );

    struct i2c_bus *fd = zend_fetch_resource(Z_RES_P(rval), PHP_I2C_DEV_RESOURCE, _i2c_mod_resource_id);

    uint8_t buffer[] = {reg_id & 0xFF, data & 0xFF};

    if(write(fd->fd, buffer, 2) != 2) {
        zend_error(E_WARNING, "Could not write to i2c bus");
        RETURN_BOOL(0);
    }
    RETURN_BOOL(1);
}

PHP_FUNCTION(i2c_write_2_bytes) {
    long data = 0, reg_id = 0;
    zval *rval = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "rll", &rval, &reg_id, &data) != SUCCESS) {
       RETURN_BOOL(0);
    }

    ZEND_ASSERT( Z_TYPE_P(rval) == IS_RESOURCE );

    struct i2c_bus *fd = zend_fetch_resource(Z_RES_P(rval), PHP_I2C_DEV_RESOURCE, _i2c_mod_resource_id);

    uint8_t buffer[] = {reg_id & 0xFF, 0,0};
    if(fd->encoding == PHP_I2C_ENDOCING_LITTLE_ENDIAN) {
        buffer[1] = data & 0xFF;
        buffer[2] = (data >> 8) & 0xFF;
    } else {
        buffer[2] = data & 0xFF;
        buffer[1] = (data >> 8) & 0xFF;
    }

    if(write(fd->fd, buffer, 3) != 3) {
        zend_error(E_WARNING, "Could not write to i2c bus");
        RETURN_BOOL(0);
    }
    RETURN_BOOL(1);
}


PHP_FUNCTION(i2c_write_3_bytes) {
    long data = 0, reg_id = 0;
    zval *rval = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "rll", &rval, &reg_id, &data) != SUCCESS) {
       RETURN_BOOL(0);
    }

    ZEND_ASSERT( Z_TYPE_P(rval) == IS_RESOURCE );

    struct i2c_bus *fd = zend_fetch_resource(Z_RES_P(rval), PHP_I2C_DEV_RESOURCE, _i2c_mod_resource_id);

    uint8_t buffer[] = {reg_id & 0xFF, 0,0, 0};
    if(fd->encoding == PHP_I2C_ENDOCING_LITTLE_ENDIAN) {
        buffer[1] = data & 0xFF;
        buffer[2] = (data >> 8) & 0xFF;
        buffer[3] = (data >> 16) & 0xFF;
    } else {
        buffer[3] = data & 0xFF;
        buffer[2] = (data >> 8) & 0xFF;
        buffer[1] = (data >> 16) & 0xFF;
    }

    if(write(fd->fd, buffer, 4) != 4) {
        zend_error(E_WARNING, "Could not write to i2c bus");
        RETURN_BOOL(0);
    }
    RETURN_BOOL(1);
}


PHP_FUNCTION(i2c_write_4_bytes) {
    long data = 0, reg_id = 0;
    zval *rval = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "rll", &rval, &reg_id, &data) != SUCCESS) {
       RETURN_BOOL(0);
    }

    ZEND_ASSERT( Z_TYPE_P(rval) == IS_RESOURCE );

    struct i2c_bus *fd = zend_fetch_resource(Z_RES_P(rval), PHP_I2C_DEV_RESOURCE, _i2c_mod_resource_id);

    uint8_t buffer[] = {reg_id & 0xFF, 0, 0, 0, 0};
    if(fd->encoding == PHP_I2C_ENDOCING_LITTLE_ENDIAN) {
        buffer[1] = data & 0xFF;
        buffer[2] = (data >> 8) & 0xFF;
        buffer[3] = (data >> 16) & 0xFF;
        buffer[4] = (data >> 24) & 0xFF;
    } else {
        buffer[4] = data & 0xFF;
        buffer[3] = (data >> 8) & 0xFF;
        buffer[2] = (data >> 16) & 0xFF;
        buffer[1] = (data >> 24) & 0xFF;
    }

    if(write(fd->fd, buffer, 5) != 5) {
        zend_error(E_WARNING, "Could not write to i2c bus");
        RETURN_BOOL(0);
    }
    RETURN_BOOL(1);
}


PHP_FUNCTION(i2c_write) {
    zval *rval, *bytes = NULL;
    long reg_id;
    HashTable *arr_hash;
    int array_count;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl|a", &rval, &reg_id, &bytes) != SUCCESS) {
       RETURN_BOOL(0);
    }

    ZEND_ASSERT( Z_TYPE_P(rval) == IS_RESOURCE );

    struct i2c_bus *fd = zend_fetch_resource(Z_RES_P(rval), PHP_I2C_DEV_RESOURCE, _i2c_mod_resource_id);

    if(fd->state < 2) {
        zend_error(E_WARNING, "Device did not yet select an address on i2c bus");
        RETURN_FALSE;
    }

    if(reg_id < 0 || reg_id > 255) {
        zend_error(E_WARNING, "I2C register must be an integer value between 0 and 255");
        RETURN_FALSE;
    }

    if(bytes) {
        arr_hash = Z_ARRVAL_P(bytes);
        array_count = zend_hash_num_elements(arr_hash);
    } else
        array_count = 0;

    uint8_t buffer[array_count+1];
    int bufCount = 0;
    buffer[bufCount++] = reg_id;

    if(bytes) {
        for(int idx = 0;idx < array_count;idx++) {
            zval *data = data = zend_hash_index_find(arr_hash, idx);
            if (Z_TYPE_P(data) == IS_LONG) {
                long value = Z_LVAL_P(data);
                if(value >= 0 && value <=255) {
                    buffer[bufCount++] = value;
                } else {
                    zend_error(E_WARNING, "i2c_write expects an array containing integer values between 0 and 255");
                    RETURN_BOOL(0);
                }
            } else {
                zend_error(E_WARNING, "i2c_write expects an array containing integer values between 0 and 255");
                RETURN_BOOL(0);
            }
        }
    }

    if(write(fd->fd, buffer, bufCount) != bufCount) {
        zend_error(E_WARNING, "Could not write to i2c bus");
        RETURN_BOOL(0);
    }

    RETURN_TRUE;
}

