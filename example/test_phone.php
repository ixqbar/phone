<?php

$start = microtime(1);
$i = 0;
while ($i < 10000) {
  $result = phone(13671527966);
  $i++;
}

print_r($result);

echo (microtime(1) - $start) . PHP_EOL;