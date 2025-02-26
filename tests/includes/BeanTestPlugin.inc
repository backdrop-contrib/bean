<?php

class BeanTestPlugin extends BeanPlugin {
  public function values() {
    return array(
      'test_boolean' => TRUE,
      'test_string' => t('String'),
      'test_array' => array(
        'test_array_1' => 'test_array_1',
      ),
    );
  }

  public function form($bean, $form, &$form_state) {
    $form = array();
    $form['test_boolean'] = array(
      '#type' => 'textfield',
      '#title' => t('String'),
      '#default_value' => $bean->test_string,
    );

    $form['test_boolean'] = array(
      '#type' => 'checkbox',
      '#title' => t('Boolean'),
      '#default_value' => $bean->test_boolean,
    );

    $form['test_array'] = array(
      '#type' => 'string',
      '#title' => t('Array'),
      '#default_value' => $bean->test_array['test_array_1'],
    );

    return $form;
  }
}

class BeanPluginWrong { }
