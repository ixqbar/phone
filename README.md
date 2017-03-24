
目前数据收录354522条记录

### functions
```php
array|bool phone(string $phone)
```

### install
```
git clone https://github.com/jonnywang/phone.git
cd phone
phpize
./configure
make
make install
```

### php.ini
```
extension=phone.so
phone.enable=1
phone.dict_path=/Users/xingqiba/data/softs/phone/phone.dat
```

### example
```
$result = phone('1367152');
Array
(
    [0] => 上海
    [1] => 上海
    [2] => 200000
    [3] => 021
    [4] => 1
)
```
 * 更新请参考example目录
 * 0 省份 1城市 2邮编 3区号 4号码类型（1移动 2联通 3电信 4电信虚拟运营商 5联通虚拟运营商 6移动虚拟运营商）

### contact
更多疑问请+qq群 233415606 or [website http://www.hnphper.com](http://www.hnphper.com)

### 其他
https://github.com/xluohome/phonedata
