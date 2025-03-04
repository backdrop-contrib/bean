<?php

/**
 *  BlockBean extends Block
 *
 *  This class allows us to create Bean blocks on the fly.
 */
class BlockBean extends Block {

  protected $bean;

  /**
   * {@inheritdoc}
   */
  function __construct($plugin_name, array $data) {
    parent::__construct($plugin_name, $data);
    list(, $type, $bean_id) = explode('--', $this->delta);
    if (!empty($bean_id)) {
      $this->bean = bean_load($bean_id);
    }
    else {
      $this->bean = bean_create(array('type' => $type));
    }
    $this->bean->loadUp(bean_load_plugin_class($type));
    $this->settings += array(
      'bid' => $bean_id,
    );
  }

  /**
   *  Sets title text on draggable block panel in Layout builder.
   */
  function getAdminTitle() {
    $label = !empty($this->bean->getLabel()) ? $this->bean->getLabel() : t('Empty %type Bean block', array('%type' => $this->bean->type));
    return $label;
  }

  /**
   *  Sets block subject on block view.
   */
  function getTitle() {
    switch ($this->settings['title_display']) {
      case 'none':
        return '';
        break;
      case 'custom':
        return isset($this->settings['title']) ? check_plain($this->settings['title']) : '';
        break;
      default:
      case 'default':
        return $this->bean->getTitle();
        break;
    }
  }

  /**
   * Returns the rendered content of this block.
   *
   * @return string
   */
  function getContent() {
    $content = '';
    if (!empty($this->bean->id()) && entity_plus_access('view', 'bean', $this->bean)) {
      $content = $this->bean->view($this->bean->view_mode);
    }

    if (!empty($content)) {
      return render($content);
    }
  }

  /**
   *  Builds the block's configuration form.
   */
  function form(&$form, &$form_state) {
    form_load_include($form_state, 'inc', 'bean', 'includes/bean.pages');
    parent::form($form, $form_state);

    $form['#attached']['js'][] = backdrop_get_path('module', 'layout') . '/js/layout.admin.js';

    $form += bean_form_fields($this->bean);

    // Get the Bean type form.
    $form += $this->bean->getForm($form, $form_state);
    field_attach_form('bean', $this->bean, $form, $form_state);

    // We'll just display the title field in the form, and use it for the Bean.
    $form['title_display']['title_display']['#title'] = t('Title to display');
    $form['title_display']['title_display']['#options']['default'] = t('Default (Bean title)');
    $form['admin_label']['#access'] = FALSE;

    $form['title_display']['title'] = array(
      '#type' => 'textfield',
      '#title' => t('Custom title'),
      '#default_value' => $this->settings['title'],
      '#maxlength' => 255,
      '#states' => array(
        'visible' => array(
          ':input[name="title_display"]' => array('value' => 'custom'),
        ),
      ),
    );
    $form['bean'] = array(
      '#type' => 'value',
      '#value' => $this->bean,
    );

    $form['title']['#title'] = t('Bean title');
    $form['title']['#description'] = '';
    $form['title']['#states'] = array(
      'visible' => array(
        ':input[name="title_display"]' => array('value' => 'default'),
      ),
    );
  }

  /**
   * Validates the form settings.
   */
  function formValidate($form, &$form_state) {
    parent::formValidate($form, $form_state);
    $form_state['bean'] = $this->bean;
    bean_form_validate($form, $form_state);
  }

  /**
   * Submit handler to save the block-specific and form settings.
   */
  function formSubmit($form, &$form_state) {
    parent::formSubmit($form, $form_state);

    $this->bean->setValues($form_state['values']);
    $this->settings['title'] = 'asdfasdfasdfasd' . $form_state['values']['title'];
    $this->settings['title_display'] = $form_state['values']['title_display'];
    // @todo: If this starts working, it will save the Bean immediately, even
    // before the Layout is saved. This is not ideal.
    field_attach_submit('bean', $this->bean, $form, $form_state);
    $this->bean->save();
    $this->settings['bid'] = $this->bean->id();
    // Adjust the plugin for the new Bean id.
    if ($this->is_new && (@substr_compare($this->plugin, '--0', -strlen('--0')) == 0)) {
      $this->plugin = str_replace('--0', '--' . $this->bean->id(), $this->plugin);
    }
    // entity_form_submit_build_entity('bean', $this->bean, $form, $form_state);.
  }
}
