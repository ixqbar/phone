PHP_ARG_ENABLE(phone, whether to enable phone support,
Make sure that the comment is aligned:
[  --enable-phone           Enable phone support])

if test "$PHP_PHONE" != "no"; then  
  PHP_SUBST(PHONE_SHARED_LIBADD)

  PHP_NEW_EXTENSION(phone, phone.c, $ext_shared)
fi
