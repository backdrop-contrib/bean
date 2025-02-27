<?php

/**
 * @file
 * Bean Admin Page
 */

/**
 * Main page callback on the bean type
 */
function bean_admin_ui_admin_page() {
  $field_ui = module_exists('field_ui');
  $rows = array();
  $operations = array();
  $i = 0;

  foreach (bean_get_types() as $bean_type) {
    $row = array();
    $row['type'] = array(
      'data' => theme('label_machine_name__bean_type', array(
        'label' => check_plain($bean_type->getLabel()),
        'machine_name' => $bean_type->type,
      )),
    );
    $row['description'] = array(
      'data' => filter_xss_admin($bean_type->getDescription()),
    );
    $storage_status_label = (method_exists($bean_type, 'getStorageStatusLabel')) ? $bean_type->getStorageStatusLabel() : t('Unknown');
    $row['status'] = array('data' => $storage_status_label);
    // Edit and delete buttons.
    if ($bean_type->isEditable()) {
      $operations['edit'] = array(
        'title' => t('Edit'),
        'href' => 'admin/structure/block-types/manage/' . $bean_type->buildURL() . '/edit',
      );
      $storage_status_code = (method_exists($bean_type, 'getStorageStatus')) ? $bean_type->getStorageStatus() : BEAN_STORAGE_NORMAL;
      switch ($storage_status_code) {
        case BEAN_STORAGE_NORMAL:
          $operations['delete'] = array(
            'title' => t('Delete'),
            'href' => 'admin/structure/block-types/manage/' . $bean_type->buildURL() . '/delete',
          );
          break;
        case BEAN_STORAGE_OVERRIDE:
          $operations['revert'] = array(
            'title' => t('Revert'),
            'href' => 'admin/structure/block-types/manage/' . $bean_type->buildURL() . '/revert',
          );
          break;
        case BEAN_STORAGE_DEFAULT:
          break;
      }
    }

    if ($field_ui) {
      // Manage fields.
      $operations['manage_fields'] = array(
        'title' => t('Manage fields'),
        'href' => 'admin/structure/block-types/manage/' . $bean_type->buildURL() . '/fields',
      );

      // Display fields.
      $operations['manage_display'] = array(
        'title' => t('Manage display'),
        'href' => 'admin/structure/block-types/manage/' . $bean_type->buildURL() . '/display',
      );
    }
    $row['operations'] = array(
      'data' => array(
        '#type' => 'dropbutton',
        '#links' => $operations,
      ),
    );
    // Creative way to setup sorting rows; add number to prevent dual keys.
    $rows[str_replace(' ', '', $bean_type->getLabel()) . '_' . $i] = $row;
  }
  ksort($rows);

  $header = array(
    t('Name'),
    t('Description'),
    t('Status'),
    t('Operations'),
  );

  $build['bean_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('There are no Block types available.'),
  );
  return $build;
}

/**
 * Generates the bean type editing form.
 */
function bean_admin_ui_type_form($form, &$form_state, $bean_type = NULL) {
  $form['new'] = array(
    '#type' => 'value',
    '#value' => TRUE,
  );
  // If bean_type is null then load an empty one.
  if (is_null($bean_type)) {
    $plugin_info = _bean_admin_default_plugin();
    $plugin_info['name'] = '';

    $bean_type = new BeanCustom($plugin_info);
  }
  elseif (!method_exists($bean_type, 'getStorageStatus') || $bean_type->getStorageStatus() == 'Normal') {
    $form['new'] = array(
      '#type' => 'value',
      '#value' => FALSE,
    );
  }

  $disabled = !$bean_type->isEditable();

  if ($disabled) {
    backdrop_set_message(t('This Block Type can not be edited'));
  }
  $form['bean_type'] = array(
    '#type' => 'value',
    '#value' => $bean_type,
  );

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $bean_type->getLabel(),
    '#description' => t('The human-readable name of this block type.'),
    '#required' => TRUE,
    '#size' => 30,
    '#disabled' => $disabled,
  );

  $form['description'] = array(
    '#title' => t('Description'),
    '#type' => 'textarea',
    '#default_value' => $bean_type->getDescription(),
    '#description' => t('The description of this block type.'),
    '#disabled' => $disabled,
  );

  // Machine-readable type name.
  $form['name'] = array(
    '#type' => 'machine_name',
    '#default_value' => isset($bean_type->type) ? $bean_type->type : '',
    '#maxlength' => 32,
    '#machine_name' => array(
      'exists' => 'bean_type_load',
      'source' => array('label'),
    ),
    '#description' => t('A unique machine-readable name for this block type. It must only contain lowercase letters, numbers, and underscores.'),
    '#disabled' => $disabled,
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Block type'),
    '#weight' => 40,
    '#disabled' => $disabled,
  );

  // This is a new bean type.
  if (!empty($bean_type->type)) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#name' => 'delete',
      '#value' => t('Delete Block type'),
      '#weight' => 45,
      '#limit_validation_errors' => array(),
      '#disabled' => $disabled,
    );

    if (method_exists($bean_type, 'getStorageStatus')) {
      if ($bean_type->getStorageStatus() == 'Overridden') {
        $form['actions']['delete']['#name'] = 'revert';
        $form['actions']['delete']['#value'] = t('Revert to defaults');
      }
      else {
        unset($form['actions']['delete']);
      }

    }
  }
  return $form;
}

/**
 * Form API submit callback for the type form.
 */
function bean_admin_ui_type_form_submit(&$form, &$form_state) {
  $op = $form_state['clicked_button']['#name'];
  if (in_array($op, array('revert', 'delete'))) {
    $form_state['redirect'] = 'admin/structure/block-types/manage/' . $form['bean_type']['#value']->type . '/' . $op;
  }
  else {
    $form_state['redirect'] = 'admin/structure/block-types';
    backdrop_set_message(t('The block type has been saved.'));
  }
  $config = config('bean.type.' . $form_state['values']['name']);
  $config->set('name', $form_state['values']['name']);
  $config->set('label', $form_state['values']['label']);
  $config->set('description', $form_state['values']['description']);
  $config->save();

  // Clear the field and entity caches so that the new "manage fields" and
  // "display fields" pages for the new bean type work.
  bean_reset();
}

/**
 * Menu callback; Form builder to confirm delete/revert of a bean type.
 */
function bean_admin_ui_type_op_confirm($form, &$form_state, $op, $type) {
  $form['op'] = array(
    '#type' => 'value',
    '#value' => $op,
  );
  $form['type'] = array(
    '#type' => 'value',
    '#value' => $type,
  );
  $form['name'] = array(
    '#type' => 'value',
    '#value' => $type->getLabel(),
  );
  $caption = '';
  if ($op == 'delete') {
    $message = t('Are you sure you want to delete the block type %type?', array('%type' => $type->getLabel()));
    $num_beans = db_query("SELECT COUNT(*) FROM {bean} WHERE type = :type", array(':type' => $type->type))->fetchField();
    if ($num_beans) {
      $caption .= '<p>' . format_plural($num_beans, '%type is used by 1 block on your site, which will be broken when this is removed.', '%type is used by @count blocks on your site, which will be broken when this is removed', array('%type' => $type->getLabel())) . '</p>';
    }
    $action = t('Delete');
  }
  else {
    $message = t('Are you sure you want to revert the block type %type?', array('%type' => $type->getLabel()));
    $action = t('Revert');
  }
  $caption .= '<p>' . t('This action cannot be undone.') . '</p>';

  return confirm_form($form, $message, 'admin/structure/block-types', $caption, $action);
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function bean_admin_ui_type_op_confirm_submit($form, &$form_state) {
  $form_state['values']['type']->{$form['op']['#value']}();
  $form_state['redirect'] = 'admin/structure/block-types';
}
