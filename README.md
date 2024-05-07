# The PHP i2c Extension

I've created this extension to get access to the i2c bus on my raspberry pi.

## Note

This module will not work with PHP 8.0 or above.

## Prerequisites

- The development module for your version of PHP, i.e, _php7.4-dev_.

## Installation

Change the configure file on line 5 (_php-config7.4_), to match your installed version of PHP.
Then, run the following commands to install the extension.

```bin
$ cd ~
$ git clone https://github.com/tasoftch/php-i2c-extension.git
$ cd php-i2c-extension
$ phpize
$ ./configure --with-php-config=/usr/bin/php-config7.4 --enable-php-i2c
$ make
$ sudo make install
```

Next, find the location of PHP's INI files on your computer by running the following command.

```bin
$ php --ini | grep -i "Configuration File.*Path"
```

**Note:** If you're using Microsoft Windows, then run `php --ini` and look for the value of "Configuration File (php.ini) Path:".

You should see a directory path such as _/etc/php/7.4/cli_.
In that directory, create a new INI file named _20-i2c.ini_ with the following contents

```ini
; configuration for php i2c module
extension=php_i2c
```

Then, with the file created, test that the I2C extension is loaded by running the following command:

```bash
php --ri php_i2c
```

You should see the following output:

```bash

php_i2c

Version => 0.8.0
```

## Usage

The extension adds five function to the global scope:
1. ```i2c_open```  
    This opens the device bus.
1. ```i2c_select```  
    This selects an address of a connected chip.
1. ```i2c_read``` ```i2c_read_byte``` ```i2c_read_2_bytes``` ```i2c_read_3_bytes``` ```i2c_read_4_bytes```      
    Reads data from the i2c bus.
1. ```i2c_write``` ```i2c_write_byte``` ```i2c_write_2_bytes``` ```i2c_write_3_bytes``` ```i2c_write_4_bytes```  
   Writes data to the i2c bus
1. ```i2c_close```  
    Closes the bus.
    
### Example

I've tested with a Raspberry Pi Model B 3+ and the Adafruit ADS1115 analog to digital converter.
It's default i2c address is 0x48.

```php
<?php
$fd = i2c_open("/dev/i2c-1");
i2c_select($fd, 0x48);

for($e=0;$e<30;$e++) {
    // Read for 30 times the value between channel AIN_0 and GND, 4.096 V, 128 samples/s
    i2c_write($fd, 1, [0xc3, 0x85]);
    // or
    // i2c_write_2_bytes( 0x01c385 );
    
    // Wait for conversion completed
    usleep(9000);
    i2c_write($fd, 0);
    $data = i2c_read($fd, 2);

    $value = $data[0]*256 + $data[1];
    printf("Hex: 0x%02x%02x - Int: %d - Float, converted: %f V\n",
        $data[0], $data[1], $value, (float)$value*4.096/32768.0);

    usleep(500000);
}

i2c_close($fd);
```

## Usage PHP

The package also contains a php wrapper class for i2c.

```bin
$ composer require tasoft/php-i2c-extension
```

Please note that the composer installation does not compile the extension!
For compilation use the installation guide described before.

Now the same example can be rewritten as:

```php
<?php
use TASoft\Bus\I2C;

$i2c = new I2C(0x48, 1);
for($e=0;$e<30;$e++) {
    // Read for 30 times the value between channel AIN_0 and GND, 4.096 V, 128 samples/s
    $i2c->write16(1, 0xC385);
    // Wait for conversion completed
    usleep(9000);
    $value = $i2c->readRegister16(1);

    printf("Hex: 0x%04x - Int: %d - Float, converted: %f V\n",
        $value, $value, (float)$value*4.096/32768.0);

    usleep(500000);
}
```
