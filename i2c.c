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
#include <inttypes.h>
#include <linux/i2c-dev.h>


// register our function to the PHP API
// so that PHP knows, which functions are in this module
zend_function_entry i2c_php_functions[] = {
    PHP_FE(i2c_open, NULL)
    PHP_FE(i2c_close, NULL)
    PHP_FE(i2c_select, NULL)
    PHP_FE(i2c_read, NULL)
    PHP_FE(i2c_write, NULL)

    {NULL, NULL, NULL}
};

// some pieces of information about our module
zend_module_entry i2c_php_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_I2C_EXTNAME,
    i2c_php_functions,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    PHP_I2C_VERSION,
    STANDARD_MODULE_PROPERTIES
};

ZEND_GET_MODULE(i2c_php)

struct i2c_bus {
	int fd;
	int state;
};

// The resource management
static void _i2c_resource_destructor(zend_resource *rsrc)
{
	struct i2c_bus fd = *((struct i2c_bus *)rsrc->ptr);
    if(fd.state > 0)
    	close(fd.fd);

    free(rsrc->ptr);
}



// Finally, we implement our "Hello World" function
// this function will be made available to PHP
// and prints to PHP stdout using printf
PHP_FUNCTION(i2c_open) {
    char *devPath;
    int length;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &devPath, &length) != SUCCESS) {
          return;
      }

	int fd = open(devPath, O_RDWR);
	if (fd < 0) {
		RETURN_BOOL(0);
	}

	struct i2c_bus *_fd = calloc(sizeof(struct i2c_bus), 1);
	_fd->fd = fd;
	_fd->state = 1;

	int res_num = zend_register_list_destructors_ex(_i2c_resource_destructor, NULL, "dev_stream", 0x01);
	zend_resource *my_res  = zend_register_resource((void *)_fd, res_num);
	RETURN_RES(my_res);
}

// Finally, we implement our "Hello World" function
// this function will be made available to PHP
// and prints to PHP stdout using printf
PHP_FUNCTION(i2c_close) {
    zval *rval;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &rval) != SUCCESS) {
       RETURN_BOOL(0);
    }
	zend_resource *resource = Z_REF_P(rval);
    struct i2c_bus fd = *((struct i2c_bus *)resource->ptr);
    if(fd.state > 0)
    	close(fd.fd);
    RETURN_BOOL(1);
}


PHP_FUNCTION(i2c_select) {
    zval *rval;
    long address;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &rval, &address) != SUCCESS) {
	   RETURN_BOOL(0);
	}
	zend_resource *resource = Z_REF_P(rval);
	struct i2c_bus *fd = (struct i2c_bus *) resource->ptr;


	if (ioctl(fd->fd, I2C_SLAVE, address) < 0) {
		RETURN_BOOL(0);
	}
	fd->state = 2;
	RETURN_BOOL(1);
}


PHP_FUNCTION(i2c_read) {
    zval *rval;
        long length = 1;

    	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|l", &rval, &length) != SUCCESS) {
    	   RETURN_BOOL(0);
    	}


    	zend_resource *resource = Z_REF_P(rval);
    	struct i2c_bus fd = *((struct i2c_bus *)resource->ptr);
        if(fd.state < 2) {
        	zend_error(E_WARNING, "Device did not yet select an address on i2c bus");
			RETURN_BOOL(0);
        }
		char buffer[length];

    	if (read(fd.fd, buffer, length) != length) {
            zend_error(E_WARNING, "Could not read from i2c bus");
            			RETURN_BOOL(0);
          } else {
          	array_init(return_value);

			for(int e = 0;e<length;e++) {
				add_next_index_long(return_value, buffer[e]);
			}
          }
}


PHP_FUNCTION(i2c_write) {
    zval *rval, *bytes;
	long reg_id;
	int waitForComplete = 0;


	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rla|b", &rval, &reg_id, &bytes, &waitForComplete) != SUCCESS) {
	   RETURN_BOOL(0);
	}
	zend_resource *resource = Z_REF_P(rval);
	struct i2c_bus fd = *((struct i2c_bus *)resource->ptr);

	if(reg_id < 0 || reg_id > 255) {
		zend_error(E_WARNING, "I2C register must be an integer value between 0 and 255");
        RETURN_BOOL(0);
	}

	HashTable *arr_hash;
	HashPosition pointer;
	int array_count;

	arr_hash = Z_ARRVAL_P(bytes);
	array_count = zend_hash_num_elements(arr_hash);

	uint8_t buffer[array_count+2];
	int bufCount = 0;
	buffer[bufCount++] = reg_id;

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

   if(fd.state < 2) {
           	zend_error(E_WARNING, "Device did not yet select an address on i2c bus");
   			RETURN_BOOL(0);
           }

    if(write(fd.fd, buffer, bufCount) != bufCount) {
    	zend_error(E_WARNING, "Could not write to i2c bus");
        RETURN_BOOL(0);
    }

    if(waitForComplete) {
    	do {
              if (read(fd.fd, buffer, 2) != 2)
                {
                zend_error(E_WARNING, "Could not read from i2c bus for completion");
                RETURN_FALSE;
                }
              } while (!(buffer[0] & 0x80));
    }

    RETURN_TRUE;
}

