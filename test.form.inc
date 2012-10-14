<?php
// $Id$

/**
 * 节点添加/编辑表单
 */
function test_node_form($test = NULL) {
  $form = array(
    'fields' => array(), // 表单字段
    'settings' => array(), // 表单设置参数
  );

  $form['settings'] = array(
    '#ajax_validate' => 1, // 是否ajax验证基本内容，即非必填内容等
    '#validate' => array('test_node_form_validate'), // 调用验证函数
    '#redirect' => url('admin/test'), // 提交成功后跳转地址
  );

  // 使用 tid 来判断当前为新增还是编辑
  $form['fields']['tid'] = array(
    '#type' => 'hidden',
    '#default_value' => empty($test->tid) ? $test->tid : 0,
    '#constant' => 1, // 隐藏字段设置此属性，将不允许修改值，防止用户通过修改 html 源代码非法提交
  );
  
  $form['fields']['title'] = array(
    '#title' => t('test', '标题'),
    '#type' => 'textfield',
    '#default_value' => $test->title,
    '#required' => 1,
    '#weight' => -10,
    '#attributes' => array('size' => 100)
  );
  
  $form['fields']['file'] = array(
    '#type' => 'file',
    '#title' => '上传',
    '#multi' => 3,// 允许上传多个文件，若不允许，不设置此项
    '#call_function' => '',// 回调自定义函数处理文件，此选项为真，以下选项无效。
    '#filesave' => true,// 系统自动处理上传文件
    '#validate' => array(
      'savepath' => 'test',// 存储路径，相对于系统文件根目录，一般为 sites/files，留空则使用默认路径。
      //'no_insert' => true, // 为 true 则文件信息不写入数据库
      //'title' => '', // 文件名或标题，不大于 255 字符
      //'filename' => '', // 路径中的文件名，可包含扩展名，若不指定，则使用 $_FILES['name']
      //'extension' => '', // 也可单独指定扩展名
      'ext_id' => 0, // 扩展 Id
      'ext_id2' => 0, // 扩展 Id
      'ext_type' => '', // 扩展类型
      'status' => 1, // 文件状态，默认为 1，即正常
      'image' => array( // 验证图片
        'type' => array('jpg', 'png', 'gif'),
        'size' => '200x300',
      ),
      'filetype' => array('jpg', 'zip'),// 允许的文件类型
      'filesize' => '300',// 允许的最大文件
      'usersize' => true, // 验证用户上传文件总大小
      'call_function' => '',// 验证完成，并保存后，回调函数，传递表单字段名及文件对象
    ),
  );
  
  $form['fields']['body'] = array(
    '#title' => t('test', '内容'),
    '#type' => 'textarea',
    '#default_value' => $test->body,
    '#attributes' => array(
      'cols' => 80,
      'rows' => 20,
      'id' => 'body'
    ),
  );
  
  // filter_form_field 函数会返回当前用户允许的输入格式列表
  $form['fields']['format'] = filter_form_field((isset($test->format) ? $test->format : 2));
  
  // 调用一个字段组
  field_get_node_form('test', 'my_field', $test, $form, 1);
  
  $form['fields']['submit'] = array(
    '#type' => 'submit',
    '#submit' => 'test_node_form_submit', // 提交时回调函数
    '#weight' => 100
  );
  
  dd_get_editor('body', array(
    'toolbar' => 'medium', // 编辑器按钮，可选参数:full small
    'upload_image' => 1, // 是否允许上传图片
    'height' => 300, // 高度
  ));
  
  return $form;
}

/**
 * 验证函数
 * @param $form
 *  上面定义的表单
 * @param $v
 *  用户输入的参数，即 $_POST，其中 object $v['_dida_values'] 是经过处理的值
 */
function test_node_form_validate(&$form, &$v) {
  // 在这儿验证
  if (mb_strlen($v['title']) > 10) {
    $error['title']['#error'][] = '长度不大于10';
  }

  /**
   * $v['_dida_values'] 是经过系统预处理的用户提交值，格式为 object，亦即将 $_POST 转换为了 object
   * 转换为 object 的目的，是为了让提交值在各模式之间传递，方便引用修改，以及统一管理
   */

  // $test 如果文章在编辑状态，可传递当前文章对象 $test 
  if (empty($v['_dida_values']->tid)) {
    $test = test_load($v['_dida_values']->tid);
  } else {
    $test = NULL;
  }

  // 下面回调字段验证函数，必须正确指定模块名称和字段组名字
  // 必须传递一个对象
  // 返回错误信息 或 flase
  $error = field_get_node_validate('test', 'my_field', $v['_dida_values'], $test);
    
  // 若返回了错误信息，则验证不通过。反之则通过
  return $error;
}

// 表单提交回调函数
function test_node_form_submit(&$form, &$v) {
  if (!empty($v['_dida_values']->tid)) {
    // tid 不为空，表示更新记录
    $args = array('tid'); // 以 tid 为更新条件
  } else {
    // 新增记录
    $args = array();
    // 设置新增时间
    $v['_dida_values']->timestamp = $_SERVER['REQUEST_TIME'];
  }

  // 调用 db_write_record() 保存，也可直接使用 db_exec() 来保存
  if (db_write_record('test', $v['_dida_values'], $args)) {
    // 创建成功
    // db_write_recored() 会将插入的主键赋值给 $v['_dida_values']->tid
    // 为了增加 field 的通用性，field 均以 nid 做为节点主键，以此和节点关系。因此，给 nid 赋值
    $v['_dida_values']->nid = $v['_dida_values']->tid;
    // 保存数据，字段数据将与 nid 关联
    field_get_node_save('test', 'my_field', $v['_dida_values'], $test);
  }
}
