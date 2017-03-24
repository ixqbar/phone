/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_PHONE_H
#define PHP_PHONE_H

extern zend_module_entry phone_module_entry;
#define phpext_phone_ptr &phone_module_entry

#define PHP_PHONE_VERSION "0.0.1"

#ifdef PHP_WIN32
#	define PHP_PHONE_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_PHONE_API __attribute__ ((visibility("default")))
#else
#	define PHP_PHONE_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#include "uthash.h"

struct PHONE {
	int tel;
	char province[20];
	char city[20];
	char zip[10];
	char zone[10];
	char type;
	UT_hash_handle hh;
};

PHP_MINIT_FUNCTION(phone);
PHP_MSHUTDOWN_FUNCTION(phone);
PHP_RINIT_FUNCTION(phone);
PHP_RSHUTDOWN_FUNCTION(phone);
PHP_MINFO_FUNCTION(phone);

PHP_FUNCTION(phone);

ZEND_BEGIN_MODULE_GLOBALS(phone)
	zend_bool enable;
	char *dict_path;
	struct PHONE *phone;
ZEND_END_MODULE_GLOBALS(phone)

#ifdef ZTS
#define PHONE_G(v) TSRMG(phone_globals_id, zend_phone_globals *, v)
#else
#define PHONE_G(v) (phone_globals.v)
#endif

#endif


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
