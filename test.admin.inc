<?php
// $Id$

/**
 * 字段管理
 */
function test_admin_fields() {
  /**
   * 注册 tabs
   */
  dd_set_tabs(array(
    l(t('test', '字段管理'), 'admin/test/fields'),
    l(t('test', '添加字段'), 'admin/test/fields/add'),
    l(t('test', '文章管理'), 'admin/test'),
  ));
  
  $args = arg();
  switch ($args[3]) {
    case 'add':
      /**
       * 在这里根据注册的字段组信息回调表单，完成字段的添加/编辑
       */
      $field->type = 'my_field'; // 字段组系统名称，必须与注册一致
      $field->module = 'test'; // 模块名，必须
      $field->redirect = url('admin/test/fields'); // 添加/编辑后完返回的地址
      $output = dd_get_form('field_form', $field); // 回调字段表单
    break;
    default:
      // 这里即回调由 test 模块定义的 my_field 字段组
      $output = field_lists('test', 'my_field');
  }
  
  
  return $output;
}

/**
 * 文章管理
 */
function test_admin_page() {
  dd_set_tabs(array(
    l(t('test', '字段管理'), 'admin/test/fields'),
    l(t('test', '添加字段'), 'admin/test/fields/add'),
    l(t('test', '文章管理'), 'admin/test'),
  ));

  if ($fetch = pager_query('SELECT * FROM {test}', 10)) {
    foreach ($fetch as $o) {
      $table[] = array(l($o->title, 'test/'.$o->tid), format_date($o->timestamp), l(t('test', '编辑文章'), 'admin/test/edit/' . $o->tid));
    }
    $output = theme('table', array(t('test', '标题'), t('test', '时间')), $table);
    $output .= pager_view(); // 调用分页
  }
  return $output;
}

