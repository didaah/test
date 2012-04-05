<?php
// $Id$

function test_page($test) {
  dd_set_title(array('我是网页标题1', '我是网页标题2'));
  return theme('test', $test);
}
