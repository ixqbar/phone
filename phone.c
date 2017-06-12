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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_phone.h"

ZEND_DECLARE_MODULE_GLOBALS(phone)

static int le_phone;

ZEND_BEGIN_ARG_INFO_EX(arg_info_phone, 0, 0, 1)
	ZEND_ARG_INFO(0, phone)
ZEND_END_ARG_INFO()

const zend_function_entry phone_functions[] = {
	PHP_FE(phone, arg_info_phone)
	PHP_FE_END
};

/* {{{ phone_module_entry
 */
zend_module_entry phone_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"phone",
	phone_functions,
	PHP_MINIT(phone),
	PHP_MSHUTDOWN(phone),
	PHP_RINIT(phone),
	PHP_RSHUTDOWN(phone),
	PHP_MINFO(phone),
#if ZEND_MODULE_API_NO >= 20010901
	PHP_PHONE_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_PHONE
ZEND_GET_MODULE(phone)
#endif

PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("phone.enable",    "0", PHP_INI_SYSTEM, OnUpdateBool,   enable,    zend_phone_globals, phone_globals)
	STD_PHP_INI_ENTRY("phone.dict_path", "",  PHP_INI_SYSTEM, OnUpdateString, dict_path, zend_phone_globals, phone_globals)
PHP_INI_END()


static void php_phone_init_globals(zend_phone_globals *phone_globals)
{
	phone_globals->enable = 0;
	phone_globals->dict_path = NULL;
	phone_globals->phone = NULL;
}

static int phone_init()
{
	if (PHONE_G(enable) == 0) {
		return FAILURE;
	}

	if (PHONE_G(dict_path) == "" || PHONE_G(dict_path) == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Please init your phone dict path in php.ini");
		return FAILURE;
	}

	FILE *fp = fopen(PHONE_G(dict_path), "r");
	if (!fp) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Please init your phone dict path in php.ini");
		return FAILURE;
	}

	fseek(fp, 0L, SEEK_END);
	size_t size = ftell(fp);

	fseek(fp, 4L, SEEK_SET);

	char buf[1024];
	size_t rn;

	//索引偏移量
	int tel_index = 0;
	rn = fread(&tel_index, 1, 4, fp);
	if (rn != 4 || tel_index == 0) {
		fclose(fp);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Please init your phone dict path in php.ini");
		return FAILURE;
	}

	//读取
	int i;
	int tel;
	char type;
	int data_index;
	char *token;

	while (tel_index < size) {
		fseek(fp, tel_index, SEEK_SET);

		tel = 0;
		rn = fread(&tel, 1, 4, fp);
		if (rn != 4 || tel == 0) {
			fclose(fp);
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Please init your phone dict path in php.ini");
			return FAILURE;
		}

		data_index = 0;
		rn = fread(&data_index, 1, 4, fp);
		if (rn != 4 || data_index == 0) {
			fclose(fp);
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Please init your phone dict path in php.ini");
			return FAILURE;
		}

		//type=1 移动 type=2 联通 type=3 电信 type=4 电信虚拟运营商 type=5 联通虚拟运营商 type=6 移动虚拟运营商
		type = -1;
		rn = fread(&type, 1, 1, fp);
		if (rn <= 0 || type == -1) {
			fclose(fp);
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Please init your phone dict path in php.ini");
			return FAILURE;
		}

		fseek(fp, data_index, SEEK_SET);
		rn = fread(buf, 1, 1024, fp);
		if (rn <= 0) {
			fclose(fp);
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Please init your phone dict path in php.ini");
			return FAILURE;
		}
		buf[rn] = 0;

		struct PHONE *phone;
		phone = malloc(sizeof(struct PHONE));
		phone->tel = tel;
		phone->type = type;
		//<省份>|<城市>|<邮编>|<长途区号>
		token = strtok(buf, "|");
		i = 0;
		while (token != NULL) {
			switch (i) {
			case 0:
				strncpy(phone->province, token, 20);
				break;
			case 1:
				strncpy(phone->city, token, 20);
				break;
			case 2:
				strncpy(phone->zip, token, 10);
				break;
			case 3:
				strncpy(phone->zone, token, 10);
				break;
			}
			i++;
			token = strtok(NULL, "|");
		}

		//构建hash
		HASH_ADD_INT(PHONE_G(phone), tel, phone);

		tel_index += 9;
	}

	fclose(fp);

	return SUCCESS;
}

static void phone_deinit()
{
	if (PHONE_G(enable) == 1) {
		struct PHONE *current, *tmp;
		HASH_ITER(hh, PHONE_G(phone), current, tmp) {
			HASH_DEL(PHONE_G(phone), current);
			free(current);
		}
	}
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(phone)
{
	ZEND_INIT_MODULE_GLOBALS(phone, php_phone_init_globals, NULL);

	REGISTER_INI_ENTRIES();

	if (PHONE_G(enable) == 1) {
		return phone_init();
	}

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(phone)
{
	phone_deinit();

	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(phone)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(phone)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(phone)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "phone support", "enabled");
	php_info_print_table_row(2, "version", PHP_PHONE_VERSION);
	php_info_print_table_row(2, "author", "QQ:174171262");
	php_info_print_table_end();
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */


PHP_FUNCTION(phone)
{
	char *tel_str = NULL;
#if PHP_MAJOR_VERSION >= 7
	size_t tel_len = 0;
#else
	int tel_len = 0;
#endif
	char t;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &tel_str, &tel_len) == FAILURE
		|| tel_len < 7) {
		RETURN_FALSE;
	}

	t = tel_str[7];
	tel_str[7] = 0;

	int tel = atoi(tel_str);

	tel_str[7] = t;

	struct PHONE *result;
	HASH_FIND_INT(PHONE_G(phone), &tel, result);
	if (!result) {
		RETURN_FALSE;
	}

	array_init(return_value);
#if PHP_MAJOR_VERSION >= 7
	add_next_index_string(return_value, result->province);
	add_next_index_string(return_value, result->city);
	add_next_index_string(return_value, result->zip);
	add_next_index_string(return_value, result->zone);
	add_next_index_long(return_value, result->type);
#else
	add_next_index_string(return_value, result->province, 1);
	add_next_index_string(return_value, result->city, 1);
	add_next_index_string(return_value, result->zip, 1);
	add_next_index_string(return_value, result->zone, 1);
	add_next_index_long(return_value, result->type);
#endif
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
